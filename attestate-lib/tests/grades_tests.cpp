#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_NO_MAIN

#include <boost/test/unit_test.hpp>

#include <attestate/grades.h>
#include <attestate/exception.h>

#include "helpers.h"

using namespace attestate;

const grades::OptionalValue NO_GRADE = boost::none;

void checkGradeValues(
    const SubjectsGrades& g,
    const std::list<ID>& subjectIds,
    const std::list<grades::OptionalValue>& exp)
{
    auto recv = g.values(subjectIds);

    BOOST_REQUIRE(
        exp.size() == subjectIds.size() &&
        recv.size() == subjectIds.size());

    auto ite = exp.cbegin();
    auto itr = recv.cbegin();
    auto its = subjectIds.cbegin();

    for ( ; ite != exp.cend(); ++ite, ++itr, ++its) {
        BOOST_CHECK_MESSAGE(
            *ite == *itr,
            "Grade values mismatch, subject id: " << *its
                << " expected: " << *ite
                << " received: " << *itr);
    }
}

BOOST_AUTO_TEST_SUITE(grades_tests)

const grades::Value G_3 = "3";
const grades::Value G_4 = "4";
const grades::Value G_5 = "5";
const grades::Value G_T = QString::fromUtf8("д");

BOOST_AUTO_TEST_CASE(test_construction_and_access)
{
    SubjectsGrades g;

    // populate and check values

    const ID ID_1 = ID::gen();
    g.setValue(ID_1, G_5);
    BOOST_CHECK(g.value(ID_1) == G_5);

    const ID ID_2 = ID::gen();
    g.setValue(ID_2, G_4);
    BOOST_CHECK(g.value(ID_1) == G_5);
    BOOST_CHECK(g.value(ID_2) == G_4);

    const ID ID_3 = ID::gen();
    g.setValue(ID_3, G_3);
    BOOST_CHECK(g.value(ID_1) == G_5);
    BOOST_CHECK(g.value(ID_2) == G_4);
    BOOST_CHECK(g.value(ID_3) == G_3);

    const std::list<ID> PLAN_1 = {ID_3, ID::emptyID(), ID_1, ID_2};

    const ID ID_4 = ID::gen();
    const std::list<ID> PLAN_2 = {ID_1, ID_2, ID_3, ID_4};

    checkGradeValues(g, PLAN_1, {G_3, NO_GRADE, G_5, G_4});
    checkGradeValues(g, PLAN_2, {G_5, G_4, G_3, NO_GRADE});

    g.setValue(ID_4, G_T);
    BOOST_CHECK(g.value(ID_1) == G_5);
    BOOST_CHECK(g.value(ID_2) == G_4);
    BOOST_CHECK(g.value(ID_3) == G_3);
    BOOST_CHECK(g.value(ID_4) == G_T);

    checkGradeValues(g, PLAN_2, {G_5, G_4, G_3, G_T});

    // construct with data

    SubjectsGrades g2({{ID_1, G_5}, {ID_2, G_4}, {ID_3, G_3}, {ID_4, G_T}});
    BOOST_CHECK(g2.value(ID_1) == G_5);
    BOOST_CHECK(g2.value(ID_2) == G_4);
    BOOST_CHECK(g2.value(ID_3) == G_3);
    BOOST_CHECK(g2.value(ID_4) == G_T);

    checkGradeValues(g2, PLAN_2, {G_5, G_4, G_3, G_T});

    // edit

    g.setValue(ID_2, G_T);
    BOOST_CHECK(g.value(ID_1) == G_5);
    BOOST_CHECK(g.value(ID_2) == G_T);
    BOOST_CHECK(g.value(ID_3) == G_3);
    BOOST_CHECK(g.value(ID_4) == G_T);
    checkGradeValues(g, PLAN_2, {G_5, G_T, G_3, G_T});

    // remove

    g.setValue(ID_3, NO_GRADE);
    BOOST_CHECK(g.value(ID_1) == G_5);
    BOOST_CHECK(g.value(ID_2) == G_T);
    BOOST_CHECK(g.value(ID_3) == NO_GRADE);
    BOOST_CHECK(g.value(ID_4) == G_T);
    checkGradeValues(g, PLAN_2, {G_5, G_T, NO_GRADE, G_T});

    g.setValue(ID_2, NO_GRADE);
    BOOST_CHECK(g.value(ID_1) == G_5);
    BOOST_CHECK(g.value(ID_2) == NO_GRADE);
    BOOST_CHECK(g.value(ID_3) == NO_GRADE);
    BOOST_CHECK(g.value(ID_4) == G_T);
    checkGradeValues(g, PLAN_2, {G_5, NO_GRADE, NO_GRADE, G_T});

    g.setValue(ID_4, NO_GRADE);
    BOOST_CHECK(g.value(ID_1) == G_5);
    BOOST_CHECK(g.value(ID_2) == NO_GRADE);
    BOOST_CHECK(g.value(ID_3) == NO_GRADE);
    BOOST_CHECK(g.value(ID_4) == NO_GRADE);
    checkGradeValues(g, PLAN_2, {G_5, NO_GRADE, NO_GRADE, NO_GRADE});

    g.setValue(ID_1, NO_GRADE);
    BOOST_CHECK(g.value(ID_1) == NO_GRADE);
    BOOST_CHECK(g.value(ID_2) == NO_GRADE);
    BOOST_CHECK(g.value(ID_3) == NO_GRADE);
    BOOST_CHECK(g.value(ID_4) == NO_GRADE);
    checkGradeValues(g, PLAN_2, {NO_GRADE, NO_GRADE, NO_GRADE, NO_GRADE});
}

BOOST_AUTO_TEST_CASE(test_diff)
{
    const ID ID_1 = ID::gen();
    const ID ID_2 = ID::gen();
    const ID ID_3 = ID::gen();
    const ID ID_4 = ID::gen();

    SubjectsGrades g1({{ID_1, G_5}, {ID_2, G_4}, {ID_3, G_3}});

    SubjectsGrades g2({{ID_1, G_5}, {ID_2, G_5}, {ID_4, G_5}});

    auto diff = g1.diff(g2);
    BOOST_REQUIRE(diff.size() == 3);
    BOOST_CHECK(diff.at(ID_2).first == G_4 && diff.at(ID_2).second == G_5);
    BOOST_CHECK(diff.at(ID_3).first == G_3 && diff.at(ID_3).second == NO_GRADE);
    BOOST_CHECK(diff.at(ID_4).first == NO_GRADE && diff.at(ID_4).second == G_5);
    auto rdiff = grades::reverseDiff(diff);
    BOOST_REQUIRE(rdiff.size() == 3);
    BOOST_CHECK(rdiff.at(ID_2).first == G_5 && rdiff.at(ID_2).second == G_4);
    BOOST_CHECK(rdiff.at(ID_3).first == NO_GRADE && rdiff.at(ID_3).second == G_3);
    BOOST_CHECK(rdiff.at(ID_4).first == G_5 && rdiff.at(ID_4).second == NO_GRADE);

    g1.applyDiff(diff);
    BOOST_CHECK(g1.value(ID_1) == G_5);
    BOOST_CHECK(g1.value(ID_2) == G_5);
    BOOST_CHECK(g1.value(ID_3) == NO_GRADE);
    BOOST_CHECK(g1.value(ID_4) == G_5);

    g1.applyDiff(rdiff);
    BOOST_CHECK(g1.value(ID_1) == G_5);
    BOOST_CHECK(g1.value(ID_2) == G_4);
    BOOST_CHECK(g1.value(ID_3) == G_3);
    BOOST_CHECK(g1.value(ID_4) == NO_GRADE);
}

BOOST_AUTO_TEST_CASE(test_erase_nonexistent_grade)
{
    const ID ID_1 = ID::gen();
    const ID ID_2 = ID::gen();
    const ID ID_3 = ID::gen();
    const ID ID_4 = ID::gen();
    SubjectsGrades g({{ID_1, G_5}, {ID_2, G_4}, {ID_3, G_3}});
    BOOST_CHECK_THROW(g.setValue(ID_4, boost::none), Exception);
}

BOOST_AUTO_TEST_CASE(test_valid)
{
    for (const grades::Value& r : grades::validValues()) {
        BOOST_CHECK(grades::isValid(r));
    }
    BOOST_CHECK(!grades::isValid(""));
}

BOOST_AUTO_TEST_CASE(test_types)
{
    const grades::ValuesSet reprValues = {"5", "4", "3"};
    for (const grades::Value& r : grades::validValues()) {
        BOOST_CHECK(
            !reprValues.count(r) || grades::type(r) == grades::Type::HasRepresentation);
        BOOST_CHECK(
            reprValues.count(r) || grades::type(r) != grades::Type::HasRepresentation);
    }

    const grades::ValuesSet auxValues = {
        QString::fromUtf8("Д"),
        QString::fromUtf8("д"),
        QString::fromUtf8("+")
    };

    for (const grades::Value& v : grades::validValues()) {
        BOOST_CHECK(!auxValues.count(v) || grades::type(v) == grades::Type::Auxilliary);
        BOOST_CHECK(auxValues.count(v) || grades::type(v) != grades::Type::Auxilliary);
    }

    const grades::ValuesSet noneValues = {
        QString::fromUtf8("Н"),
        QString::fromUtf8("н"),
        QString::fromUtf8("-")
    };

    for (const grades::Value& v : grades::validValues()) {
        BOOST_CHECK(!noneValues.count(v) || grades::type(v) == grades::Type::None);
        BOOST_CHECK(noneValues.count(v) || grades::type(v) != grades::Type::None);
    }
}

BOOST_AUTO_TEST_SUITE_END()
