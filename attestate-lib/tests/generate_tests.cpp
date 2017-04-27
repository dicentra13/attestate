#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_NO_MAIN

#include <boost/test/unit_test.hpp>

#include <attestate/generate.h>
#include <attestate/serialize.h>
#include <attestate/student.h>
#include <attestate/subjects.h>
#include <attestate/grades.h>
#include <attestate/class.h>
#include <attestate/exception.h>

#include <doctpl/template.h>
#include <doctpl/serialize.h>

#include "helpers.h"

#include <initializer_list>

using namespace attestate;

BOOST_FIXTURE_TEST_SUITE(generation_tests, AppFixture)

using namespace attestate::cfg::tags;

void checkReprGrades(
    const doctpl::Template& doc, const Student& s, const SubjectsPlanPtr& sp)
{
    QStringList reprSubjectsD, reprGradesD;
    QStringList reprSubjectsS, reprGradesS;

    const auto& g = s.grades();
    for (size_t i = 0; sp && i < sp->subjectsCount(); ++i) {
        auto v = g.value(sp->at(i).id());
        if (v
            && grades::isValid(*v)
            && grades::type(*v) == grades::Type::HasRepresentation)
        {
            reprSubjectsS.push_back(sp->at(i).name());
            reprGradesS.push_back(grades::representation(*v));
        }
    }

    auto gf1 = doc.fields()->as<doctpl::TableField>()->find(fields::GRADES_1);
    auto gf2 = doc.fields()->as<doctpl::TableField>()->find(fields::GRADES_2);

    size_t i = 0;
    for ( ; i < gf1->rowsCount() && !gf1->text(i, 0).isEmpty(); ++i) {
        reprSubjectsD.push_back(gf1->text(i, 0));
        reprGradesD.push_back(gf1->text(i, 1));
    }
    for (size_t i0 = i; i0 < gf1->rowsCount(); ++i0) {
        BOOST_CHECK(gf1->text(i0, 0).isEmpty());
        BOOST_CHECK(gf1->text(i0, 1).isEmpty());
    }

    size_t j = 0;
    if (i == gf1->rowsCount()) {
        for ( ; j < gf2->rowsCount() && !gf2->text(j, 0).isEmpty(); ++j) {
            reprSubjectsD.push_back(gf2->text(j, 0));
            reprGradesD.push_back(gf2->text(j, 1));
        }
    }
    for (size_t j0 = j; j0 < gf2->rowsCount(); ++j0) {
        BOOST_CHECK(gf2->text(j0, 0).isEmpty());
        BOOST_CHECK(gf2->text(j0, 1).isEmpty());
    }

    BOOST_REQUIRE(reprSubjectsD.size() == reprSubjectsS.size());
    BOOST_CHECK(
        std::equal(
            reprSubjectsD.begin(), reprSubjectsD.end(),
            reprSubjectsS.begin()));
    BOOST_CHECK(
        std::equal(
            reprGradesD.begin(), reprGradesD.end(),
            reprGradesS.begin()));
}

void checkAuxSubjects(
    const doctpl::Template& doc, const Student& s, const SubjectsPlanPtr& sp)
{
    QStringList auxSubjectsD;
    QStringList auxSubjectsS;

    const auto& g = s.grades();
    for (size_t i = 0; sp && i < sp->subjectsCount(); ++i) {
        auto v = g.value(sp->at(i).id());
        if (v && grades::isValid(*v) && grades::type(*v) == grades::Type::Auxilliary) {
            auxSubjectsS.push_back(sp->at(i).name());
        }
    }

    auto aux = doc.fields()->as<doctpl::TableField>()->find(fields::AUX);

    size_t i = 0;
    for ( ; i < aux->rowsCount() && !aux->text(i, 0).isEmpty(); ++i) {
        auxSubjectsD.push_back(aux->text(i, 0));
    }
    for (size_t i0 = i; i0 < aux->rowsCount(); ++i0) {
        BOOST_CHECK(aux->text(i0, 0).isEmpty());
    }

    BOOST_REQUIRE(auxSubjectsD.size() == auxSubjectsS.size());
    BOOST_CHECK(
        std::equal(
            auxSubjectsD.begin(), auxSubjectsD.end(),
            auxSubjectsS.begin()));
}

void check(const doctpl::Template& doc, const Student& s, const Class& c)
{
    auto tf = doc.fields()->as<doctpl::TextField>();
    BOOST_CHECK(tf->find(fields::FAMILY_NAME_1)->text() == s.familyName());
    BOOST_CHECK(tf->find(fields::FAMILY_NAME_2)->text() == s.familyName());

    QString n = s.name() + " " + s.parentalName();
    BOOST_CHECK(tf->find(fields::NAME_PNAME_1)->text() == n);
    BOOST_CHECK(tf->find(fields::NAME_PNAME_2)->text() == n);

    BOOST_CHECK(tf->find(fields::BIRTH_DATE)->text()
        == s.birthDate().toString(cfg::attestateDateFormat()));

    OptionalDate issueDate = s.issueDate();
    if (!issueDate) {
        issueDate = c.issueDate();
    }
    const auto& issueDateStr = tf->find(fields::ISSUE_DATE_1)->text();
    BOOST_CHECK(issueDateStr == tf->find(fields::ISSUE_DATE_2)->text());
    BOOST_CHECK((!issueDate && issueDateStr.isEmpty())
        || (issueDate && issueDate->toString(cfg::attestateDateFormat()) == issueDateStr));

    OptionalYear gradYear = s.graduationYear();
    if (!gradYear) {
        gradYear = c.graduationYear();
    }
    const auto& gradYearStr = tf->find(fields::GRADUATION_YEAR)->text();
    BOOST_CHECK((!gradYear && gradYearStr.isEmpty())
        || (gradYear && gradYearStr.toInt() == *gradYear));

    BOOST_CHECK(s.attestateId() == tf->find(fields::ATTESTATE_ID)->text());

    checkReprGrades(doc, s, c.subjectsPlan());
    checkAuxSubjects(doc, s, c.subjectsPlan());
}

/*
const grades::Value G_3 = "3";
const grades::Value G_4 = "4";
const grades::Value G_5 = "5";
const grades::Value G_T_1 = QString::fromUtf8("д");
const grades::Value G_T_2 = QString::fromUtf8("Д");
const grades::Value G_T_3 = QString::fromUtf8("+");
const grades::Value G_N_1 = QString::fromUtf8("н");
const grades::Value G_N_2 = QString::fromUtf8("Н");
const grades::Value G_N_3 = QString::fromUtf8("-");

void fill()
{
    auto c = csv::read(
        "../../tests/data/generate_data.csv",
        csv::Params{';', "dd.MM.yyyy"});

    std::vector<grades::Value> v0 = {G_3, G_4, G_5, G_T_1, G_T_2, G_T_3, G_N_1, G_N_2, G_N_3};
    std::vector<grades::Value> v1 = {G_3, G_4, G_5};
    std::vector<grades::Value> v2 = {G_T_1, G_T_2, G_T_3, G_N_1, G_N_2, G_N_3};

    for (size_t i = 0; i < c->studentsCount(); ++i) {
        std::vector<grades::Value> v;
        auto sc = c->subjectsPlan()->subjectsCount();
        for (size_t j = 0; j < sc; ++j) {
            if ((i % 3 == 1 && j < sc - 3) || (i % 3 == 2)) {
                v.push_back(v1.at(j % v1.size()));
            } else if (i % 3 == 1 && j >= sc - 3) {
                v.push_back(v2.at(j % v2.size()));
            } else {
                v.push_back(v0.at(j % v0.size()));
            }
        }
        std::random_shuffle(v.begin(), v.end());
        auto& s = c->student(i);
        for (size_t j = 0; j < sc; ++j) {
            s.grades().setValue(c->subjectsPlan()->at(j).id(), v.at(j));
        }
    }
    csv::write(
        *c,
        "../../tests/data/generate_data.csv",
        csv::Params{';', "dd.MM.yyyy"});
}
*/

BOOST_AUTO_TEST_CASE(test)
{
    std::unique_ptr<doctpl::Template> doc =
        doctpl::xml::read("../../../doctpl-lib/tests/data/11kl_2016.xml");

    auto c = csv::read(
        "../../tests/data/generate_data.csv",
        csv::Params{';', "dd.MM.yyyy"});

    auto sc =c->subjectsPlan()->subjectsCount();
    BOOST_REQUIRE(sc > 3);

    auto gf1 = doc->fields()->as<doctpl::TableField>()->find(fields::GRADES_1);
    while (gf1->rowsCount() > sc - 3) {
        gf1->deleteRow(gf1->rowsCount() - 1);
    }
    auto gf2 = doc->fields()->as<doctpl::TableField>()->find(fields::GRADES_2);
    while (gf2->rowsCount() > sc - 3) {
        gf2->deleteRow(gf2->rowsCount() - 1);
    }

    for (size_t i = 0; i < c->studentsCount(); ++i) {
        gen::fillTemplate(*c, i, *doc);
        check(*doc, c->student(i), *c);
    }
}

BOOST_AUTO_TEST_SUITE_END()
