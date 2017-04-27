#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_NO_MAIN

#include <boost/test/unit_test.hpp>

#include <attestate/subjects.h>

#include "helpers.h"
#include "../src/diff.h"

using namespace attestate;

const DataString EMPTY_STR = "";

// tests of class Subject

BOOST_AUTO_TEST_SUITE(subject_tests)

BOOST_AUTO_TEST_CASE(test_create_new)
{
    const ID id = ID::gen();
    Subject s(id);
    BOOST_CHECK(s.id() == id);
    BOOST_CHECK(s.state() == State::New);
    BOOST_CHECK(s.name().isEmpty() && s.isNameModified());
    BOOST_CHECK(s.shortenedName().isEmpty() && s.isShortenedNameModified());
}

const ID ID_1 = ID::gen();
const DataString NAME_1 = "Russian language";
const DataString SNAME_1 = "Russian";

Subject createSubject1()
{
    return Subject(ID_1, NAME_1, SNAME_1);
}

BOOST_AUTO_TEST_CASE(test_create_existing)
{
    Subject s(createSubject1());
    BOOST_CHECK(s.id() == ID_1);
    BOOST_CHECK(s.state() == State::Existing);
    BOOST_CHECK(s.name() == NAME_1 && !s.isNameModified());
    BOOST_CHECK(s.shortenedName() == SNAME_1 && !s.isShortenedNameModified());
}


const DataString NAME_2 = "Language (rus)";
const DataString SNAME_2 = "Rus";

const DataString EMPTY_STR = "";

BOOST_AUTO_TEST_CASE(test_edit_name)
{
    {
        Subject s(createSubject1());
        s.setName(NAME_2);
        BOOST_CHECK(s.name() == NAME_2);
        BOOST_CHECK(s.isNameModified());
        BOOST_CHECK(s.state() == State::Modified);
        s.setName(NAME_1);
        BOOST_CHECK(s.name() == NAME_1);
        BOOST_CHECK(!s.isNameModified());
        BOOST_CHECK(s.state() == State::Existing);
    }
    {
        Subject s(ID::gen());
        for (const auto& v : {NAME_2, EMPTY_STR}) {
            s.setName(v);
            BOOST_CHECK(s.name() == v);
            BOOST_CHECK(s.isNameModified());
            BOOST_CHECK(s.state() == State::New);
        }
    }
}

BOOST_AUTO_TEST_CASE(test_edit_shortened_name)
{
    {
        Subject s(createSubject1());
        s.setShortenedName(SNAME_2);
        BOOST_CHECK(s.shortenedName() == SNAME_2);
        BOOST_CHECK(s.isShortenedNameModified());
        BOOST_CHECK(s.state() == State::Modified);
        s.setShortenedName(SNAME_1);
        BOOST_CHECK(s.shortenedName() == SNAME_1);
        BOOST_CHECK(!s.isShortenedNameModified());
        BOOST_CHECK(s.state() == State::Existing);
    }
    {
        Subject s(ID::gen());
        for (const auto& v : {SNAME_2, EMPTY_STR}) {
            s.setShortenedName(v);
            BOOST_CHECK(s.shortenedName() == v);
            BOOST_CHECK(s.isShortenedNameModified());
            BOOST_CHECK(s.state() == State::New);
        }
    }
}

BOOST_AUTO_TEST_CASE(test_save_new)
{
    Subject s(ID_1);

    s.setName(NAME_1);
    s.setShortenedName(SNAME_1);

    BOOST_CHECK(s.state() == State::New);

    s.save();

    BOOST_CHECK(s.name() == NAME_1 && !s.isNameModified());
    BOOST_CHECK(s.shortenedName() == SNAME_1 && !s.isShortenedNameModified());
    BOOST_CHECK(s.state() == State::Existing);
}

BOOST_AUTO_TEST_CASE(test_save_existing)
{
    Subject s(ID_1, NAME_1, SNAME_1);

    s.setName(NAME_2);
    s.setShortenedName(SNAME_2);

    BOOST_CHECK(s.state() == State::Modified);

    s.save();

    BOOST_CHECK(s.name() == NAME_2 && !s.isNameModified());
    BOOST_CHECK(s.shortenedName() == SNAME_2 && !s.isShortenedNameModified());
    BOOST_CHECK(s.state() == State::Existing);
}

BOOST_AUTO_TEST_CASE(test_delete)
{
    {
        Subject s(ID::gen());

        s.setDeleted(true);
        BOOST_CHECK(s.state() == State::Deleted);
        BOOST_CHECK_THROW(s.save(), Exception);
        s.setDeleted(false);
        BOOST_CHECK(s.state() == State::New);
    }

    {
        Subject s(createSubject1());

        s.setDeleted(true);
        BOOST_CHECK(s.state() == State::Deleted);
        BOOST_CHECK_THROW(s.save(), Exception);
        s.setDeleted(false);
        BOOST_CHECK(s.state() == State::Existing);
    }
}

BOOST_AUTO_TEST_SUITE_END()


// tests of class SubjectsPlan

BOOST_AUTO_TEST_SUITE(subjects_plan_tests)

BOOST_AUTO_TEST_CASE(test_create_new)
{
    const ID id = ID::gen();
    SubjectsPlan p(id);
    BOOST_CHECK(p.id() == id);
    BOOST_CHECK(p.state() == State::New);
    BOOST_CHECK(p.name().isEmpty() && p.isNameModified());
    BOOST_CHECK(p.subjectIds().empty() && p.areSubjectsModified());
    BOOST_CHECK(p.subjectsCount() == 0);
}

const ID ID_1 = ID::gen();
const DataString S_NAME_1 = "Subject 1";
const SubjectPtr SUBJECT_1 = std::make_shared<Subject>(ID_1, S_NAME_1);

const ID ID_2 = ID::gen();
const DataString S_NAME_2 = "Subject 2";
const SubjectPtr SUBJECT_2 = std::make_shared<Subject>(ID_2, S_NAME_2);

const ID ID_3 = ID::gen();
const DataString S_NAME_3 = "Subject 3";
const SubjectPtr SUBJECT_3 = std::make_shared<Subject>(ID_3, S_NAME_3);

const ID ID_4 = ID::gen();
const DataString S_NAME_4 = "Subject 4";
const SubjectPtr SUBJECT_4 = std::make_shared<Subject>(ID_4, S_NAME_4);

const ID ID_5 = ID::gen();
const DataString S_NAME_5 = "Subject 5";
const SubjectPtr SUBJECT_5 = std::make_shared<Subject>(ID_5, S_NAME_5);

const ID ID_6 = ID::gen();
const DataString S_NAME_6 = "Subject 6";
const SubjectPtr SUBJECT_6 = std::make_shared<Subject>(ID_6, S_NAME_6);


const SubjectPtrVector LIST_1 = {SUBJECT_1, SUBJECT_2, SUBJECT_3, SUBJECT_4};

const ID PLAN_ID_1 = ID::gen();
const DataString NAME_1 = "Common";
const DataString NAME_2 = "Common_2";

SubjectsPlan createPlan()
{
    return SubjectsPlan(PLAN_ID_1, NAME_1, LIST_1);
}

void checkSubjects(const SubjectsPlan& plan, std::initializer_list<ID> ids)
{
    BOOST_REQUIRE(plan.subjectsCount() == ids.size());
    const auto& idsr = plan.subjectIds();
    BOOST_REQUIRE(idsr.size() == ids.size());
    auto it = idsr.begin();
    size_t i = 0;
    for (const auto& id : ids) {
        BOOST_CHECK(*it++ == id);
        BOOST_CHECK(plan.at(i++).id() == id);
        BOOST_CHECK(plan.hasSubject(id));
    }
}

BOOST_AUTO_TEST_CASE(test_create_existing)
{
    SubjectsPlan p = createPlan();
    BOOST_CHECK(p.id() == PLAN_ID_1);
    BOOST_CHECK(p.state() == State::Existing && !p.isModified());
    BOOST_CHECK(p.name() == NAME_1 && !p.isNameModified());
    BOOST_CHECK(!p.areSubjectsModified());
    checkSubjects(p, {ID_1, ID_2, ID_3, ID_4});
}

BOOST_AUTO_TEST_CASE(test_create_errors)
{
    {
        // empty ptr
        SubjectPtrVector subjects{SUBJECT_1, nullptr, SUBJECT_2};
        std::unique_ptr<SubjectsPlan> p;
        BOOST_CHECK_THROW(
            p.reset(new SubjectsPlan(PLAN_ID_1, NAME_1, std::move(subjects))),
            Exception);
    }
    {
        // duplicate id
        SubjectPtrVector subjects{SUBJECT_1, SUBJECT_2, SUBJECT_3, SUBJECT_1};
        std::unique_ptr<SubjectsPlan> p;
        BOOST_CHECK_THROW(
            p.reset(new SubjectsPlan(PLAN_ID_1, NAME_1, std::move(subjects))),
            Exception);
    }
}

BOOST_AUTO_TEST_CASE(test_edit_name)
{
    {
        SubjectsPlan p(createPlan());
        p.setName(NAME_2);
        BOOST_CHECK(p.name() == NAME_2);
        BOOST_CHECK(p.isNameModified());
        BOOST_CHECK(p.state() == State::Modified && p.isModified());
        p.setName(NAME_1);
        BOOST_CHECK(p.name() == NAME_1);
        BOOST_CHECK(!p.isNameModified());
        BOOST_CHECK(p.state() == State::Existing && !p.isModified());
    }
    {
        SubjectsPlan p(ID::gen());
        for (const auto& v : {NAME_2, EMPTY_STR}) {
            p.setName(v);
            BOOST_CHECK(p.name() == v);
            BOOST_CHECK(p.isNameModified());
            BOOST_CHECK(p.state() == State::New);
        }
    }
}

void checkDiff(const SubjectsPlan& p1, const SubjectsPlan& p2,
    const SubjectsPlan::Diff& ediff)
{
    auto rdiff = p1.diff(p2);
    BOOST_REQUIRE(rdiff.size() == ediff.size());
    auto ite = ediff.begin();
    auto itr = rdiff.begin();
    for ( ; ite != ediff.end(); ++ite, ++itr) {
        BOOST_CHECK(ite->first == itr->first);
        BOOST_CHECK(equals(ite->second.first, itr->second.first));
        BOOST_CHECK(equals(ite->second.second, itr->second.second));
    }
}

BOOST_AUTO_TEST_CASE(test_insert_errors)
{
    {
        // empty ptr
        SubjectsPlan p(createPlan());
        BOOST_CHECK_THROW(p.insert(nullptr, 0), Exception);
        BOOST_CHECK(!p.areSubjectsModified() && !p.isModified());
        BOOST_CHECK_THROW(p.append(nullptr), Exception);
        BOOST_CHECK(!p.areSubjectsModified() && !p.isModified());
    }
    {
        // duplicate id
        SubjectsPlan p(createPlan());
        BOOST_CHECK_THROW(p.insert(SUBJECT_1, 0), Exception);
        BOOST_CHECK(!p.areSubjectsModified() && !p.isModified());
        BOOST_CHECK_THROW(p.append(SUBJECT_1), Exception);
        BOOST_CHECK(!p.areSubjectsModified() && !p.isModified());
    }
    {
        // invalid index
        SubjectsPlan p(createPlan());
        BOOST_CHECK_THROW(p.insert(SUBJECT_6, 8), Exception);
        BOOST_CHECK(!p.areSubjectsModified() && !p.isModified());
    }
}

BOOST_AUTO_TEST_CASE(test_insert_erase)
{
    {
        SubjectsPlan p(createPlan());
        SubjectsPlan p2(createPlan());
        p.insert(SUBJECT_5, 1);
        checkSubjects(p, {ID_1, ID_5, ID_2, ID_3, ID_4});
        checkDiff(p, p2, SubjectsPlan::Diff{
            {SUBJECT_5, {1, boost::none}},
            {SUBJECT_2, {2, 1}},
            {SUBJECT_3, {3, 2}},
            {SUBJECT_4, {4, 3}}
        });
        BOOST_CHECK(p.areSubjectsModified() &&
            p.isModified() && p.state() == State::Modified);
        p.erase(1);
        checkSubjects(p, {ID_1, ID_2, ID_3, ID_4});
        checkDiff(p, p2, {});
        BOOST_CHECK(!p.areSubjectsModified() &&
            !p.isModified() && p.state() == State::Existing);
    }
    {
        SubjectsPlan p(ID::gen());
        SubjectsPlan p2(ID::gen());
        p.insert(SUBJECT_5, 0);
        checkSubjects(p, {ID_5});
        checkDiff(p, p2, SubjectsPlan::Diff{{SUBJECT_5, {0, boost::none}}});
        BOOST_CHECK(p.areSubjectsModified() && p.state() == State::New);
        p.erase(0);
        checkSubjects(p, {});
        checkDiff(p, p2, {});
        BOOST_CHECK(p.areSubjectsModified() && p.state() == State::New);
    }
}

BOOST_AUTO_TEST_CASE(test_append_erase)
{
    {
        SubjectsPlan p(createPlan());
        SubjectsPlan p2(createPlan());
        p.append(SUBJECT_5);
        checkSubjects(p, {ID_1, ID_2, ID_3, ID_4, ID_5});
        checkDiff(p, p2, SubjectsPlan::Diff{{SUBJECT_5, {4, boost::none}}});
        BOOST_CHECK(p.areSubjectsModified() &&
            p.isModified() && p.state() == State::Modified);
        BOOST_CHECK(p.erase(4)->id() == ID_5);
        checkSubjects(p, {ID_1, ID_2, ID_3, ID_4});
        checkDiff(p, p2, {});
        BOOST_CHECK(!p.areSubjectsModified() &&
            !p.isModified() && p.state() == State::Existing);
    }
    {
        SubjectsPlan p(ID::gen());
        SubjectsPlan p2(ID::gen());
        p.append(SUBJECT_5);
        checkSubjects(p, {ID_5});
        checkDiff(p, p2, SubjectsPlan::Diff{{SUBJECT_5, {0, boost::none}}});
        BOOST_CHECK(p.areSubjectsModified() && p.state() == State::New);
        BOOST_CHECK(p.erase(0)->id() == ID_5);
        checkSubjects(p, {});
        checkDiff(p, p2, {});
        BOOST_CHECK(p.areSubjectsModified() && p.state() == State::New);
    }
}

BOOST_AUTO_TEST_CASE(test_erase_multi)
{
    SubjectsPlan p(createPlan());
    SubjectsPlan p2(createPlan());
    p.append(SUBJECT_5);
    p.append(SUBJECT_6);
    checkSubjects(p, {ID_1, ID_2, ID_3, ID_4, ID_5, ID_6});
    checkDiff(p, p2, SubjectsPlan::Diff{
        {SUBJECT_5, {4, boost::none}},
        {SUBJECT_6, {5, boost::none}}
    });
    auto m = p.erase({0, 2, 3, 5});
    checkDiff(p, p2, SubjectsPlan::Diff{
        {SUBJECT_1, {boost::none, 0}},
        {SUBJECT_2, {0, 1}},
        {SUBJECT_3, {boost::none, 2}},
        {SUBJECT_4, {boost::none, 3}},
        {SUBJECT_5, {1, boost::none}}
    });
    BOOST_CHECK(p.areSubjectsModified() &&
        p.isModified() && p.state() == State::Modified);
    checkSubjects(p, {ID_2, ID_5});
    BOOST_REQUIRE(m.size() == 4);
    decltype(m) m2 = {{0, SUBJECT_1}, {2, SUBJECT_3}, {3, SUBJECT_4}, {5, SUBJECT_6}};
    auto it = m.begin();
    auto it2 = m2.begin();
    for ( ; it != m.end(); ++it, ++it2) {
        BOOST_CHECK(
            it->first == it2->first &&
            it->second && it2->second && it->second->id() == it2->second->id());
    }
}

BOOST_AUTO_TEST_CASE(test_erase_errors)
{
    {
        // invalid index
        SubjectsPlan p(createPlan());
        BOOST_CHECK_THROW(p.erase(8), Exception);
        BOOST_CHECK(!p.areSubjectsModified() && !p.isModified());
    }
    {
        // invalid index in multiple erase
        SubjectsPlan p(createPlan());
        BOOST_CHECK_THROW(p.erase({0, 1, 8}), Exception);
        BOOST_CHECK(!p.areSubjectsModified() && !p.isModified());
        checkSubjects(p, {ID_1, ID_2, ID_3, ID_4});
    }
}

void checkMove(
    SubjectsPlan& p,
    SubjectsPlan::Index from, SubjectsPlan::Index to,
    std::initializer_list<ID> idsFromTo,
    std::initializer_list<ID> idsToFrom)
{
    auto state = p.state();
    p.move(from, to);
    checkSubjects(p, idsFromTo);
    BOOST_CHECK(
        (state == State::New && p.state() == State::New) ||
        (state == State::Existing &&
            p.state() == State::Modified && p.isModified() && p.areSubjectsModified()));
    p.move(to, from);
    checkSubjects(p, idsToFrom);
    BOOST_CHECK(state == p.state());
    BOOST_CHECK(
        (state == State::New && p.areSubjectsModified()) ||
        (state == State::Existing && !p.isModified() && !p.areSubjectsModified()));
}

BOOST_AUTO_TEST_CASE(test_move)
{
    SubjectsPlan p(createPlan());
    checkMove(p, 0, 3, {ID_2, ID_3, ID_4, ID_1}, {ID_1, ID_2, ID_3, ID_4});
    checkMove(p, 0, 2, {ID_2, ID_3, ID_1, ID_4}, {ID_1, ID_2, ID_3, ID_4});
    checkMove(p, 0, 1, {ID_2, ID_1, ID_3, ID_4}, {ID_1, ID_2, ID_3, ID_4});
    checkMove(p, 1, 3, {ID_1, ID_3, ID_4, ID_2}, {ID_1, ID_2, ID_3, ID_4});
    checkMove(p, 1, 2, {ID_1, ID_3, ID_2, ID_4}, {ID_1, ID_2, ID_3, ID_4});
    checkMove(p, 2, 3, {ID_1, ID_2, ID_4, ID_3}, {ID_1, ID_2, ID_3, ID_4});
    checkMove(p, 3, 0, {ID_4, ID_1, ID_2, ID_3}, {ID_1, ID_2, ID_3, ID_4});
    checkMove(p, 2, 0, {ID_3, ID_1, ID_2, ID_4}, {ID_1, ID_2, ID_3, ID_4});
    checkMove(p, 1, 0, {ID_2, ID_1, ID_3, ID_4}, {ID_1, ID_2, ID_3, ID_4});
    checkMove(p, 3, 1, {ID_1, ID_4, ID_2, ID_3}, {ID_1, ID_2, ID_3, ID_4});
    checkMove(p, 2, 1, {ID_1, ID_3, ID_2, ID_4}, {ID_1, ID_2, ID_3, ID_4});
    checkMove(p, 3, 2, {ID_1, ID_2, ID_4, ID_3}, {ID_1, ID_2, ID_3, ID_4});
}

BOOST_AUTO_TEST_CASE(test_move_errors)
{
    SubjectsPlan p(createPlan());
    BOOST_CHECK_THROW(p.move(0, 4), Exception);
    BOOST_CHECK(!p.areSubjectsModified() && !p.isModified());
    checkSubjects(p, {ID_1, ID_2, ID_3, ID_4});
    BOOST_CHECK_THROW(p.move(4, 0), Exception);
    BOOST_CHECK(!p.areSubjectsModified() && !p.isModified());
    checkSubjects(p, {ID_1, ID_2, ID_3, ID_4});
}

BOOST_AUTO_TEST_CASE(test_diff)
{
    SubjectsPlan p1(createPlan());
    SubjectsPlan p2(createPlan());
    SubjectsPlan p(createPlan());

    p1.erase(1);
    p1.append(SUBJECT_5);
    checkDiff(p1, p2, SubjectsPlan::Diff{
        {SUBJECT_2, {boost::none, 1}},
        {SUBJECT_3, {1, 2}},
        {SUBJECT_4, {2, 3}},
        {SUBJECT_5, {3, boost::none}}
    });

    auto diff = p1.diff(p2);
    auto rdiff = SubjectsPlan::reverseDiff(diff);
    p2.applyDiff(rdiff);
    checkDiff(p1, p2, {});
    p1.applyDiff(diff);
    checkDiff(p1, p2, SubjectsPlan::Diff{
        {SUBJECT_2, {1, boost::none}},
        {SUBJECT_3, {2, 1}},
        {SUBJECT_4, {3, 2}},
        {SUBJECT_5, {boost::none, 3}}
    });
    p1.applyDiff(rdiff);
    checkDiff(p1, p2, {});
}

BOOST_AUTO_TEST_CASE(test_diff_errors)
{
    {
        // no subject
        SubjectsPlan p(createPlan());
        BOOST_CHECK_THROW(p.applyDiff({
                {SUBJECT_5, {1, boost::none}}
            }), Exception);
        BOOST_CHECK(!p.areSubjectsModified() && !p.isModified());
        checkSubjects(p, {ID_1, ID_2, ID_3, ID_4});
    }
    {
        // value mismatch
        {
            SubjectsPlan p(createPlan());
            BOOST_CHECK_THROW(p.applyDiff({
                    {SUBJECT_1, {boost::none, 1}}
                }), Exception);
            BOOST_CHECK(!p.areSubjectsModified() && !p.isModified());
            checkSubjects(p, {ID_1, ID_2, ID_3, ID_4});
        }
        {
            SubjectsPlan p(createPlan());
            BOOST_CHECK_THROW(p.applyDiff({
                    {SUBJECT_1, {1, 2}}
                }), Exception);
            BOOST_CHECK(!p.areSubjectsModified() && !p.isModified());
            checkSubjects(p, {ID_1, ID_2, ID_3, ID_4});
        }
    }
    {
        // equal values
        {
            SubjectsPlan p(createPlan());
            BOOST_CHECK_THROW(p.applyDiff({
                    {SUBJECT_1, {0, 0}}
                }), Exception);
            BOOST_CHECK(!p.areSubjectsModified() && !p.isModified());
            checkSubjects(p, {ID_1, ID_2, ID_3, ID_4});
        }
        {
            SubjectsPlan p(createPlan());
            BOOST_CHECK_THROW(p.applyDiff({
                    {SUBJECT_5, {boost::none, boost::none}}
                }), Exception);
            BOOST_CHECK(!p.areSubjectsModified() && !p.isModified());
            checkSubjects(p, {ID_1, ID_2, ID_3, ID_4});
        }
    }
}

BOOST_AUTO_TEST_CASE(test_diff_empty_ptr)
{
    SubjectsPlan p(createPlan());
    BOOST_CHECK_THROW(p.applyDiff({
            {SUBJECT_2, {1, boost::none}},
            {nullptr, {boost::none, 0}}
        }), Exception);
    BOOST_CHECK(!p.areSubjectsModified() && !p.isModified());
    checkSubjects(p, {ID_1, ID_2, ID_3, ID_4});
}

BOOST_AUTO_TEST_CASE(test_delete)
{
    {
        SubjectsPlan p(ID::gen());

        p.setDeleted(true);
        BOOST_CHECK(p.state() == State::Deleted);
        BOOST_CHECK_THROW(p.save(), Exception);
        p.setDeleted(false);
        BOOST_CHECK(p.state() == State::New);
    }

    {
        SubjectsPlan p(createPlan());

        p.setDeleted(true);
        BOOST_CHECK(p.state() == State::Deleted);
        BOOST_CHECK_THROW(p.save(), Exception);
        p.setDeleted(false);
        BOOST_CHECK(p.state() == State::Existing);
    }
}

BOOST_AUTO_TEST_CASE(test_save_new)
{
    SubjectsPlan p(PLAN_ID_1);

    p.setName(NAME_1);
    p.append(SUBJECT_1);
    p.append(SUBJECT_2);

    BOOST_CHECK(p.state() == State::New);

    p.save();

    BOOST_CHECK(!p.isNameModified());
    BOOST_CHECK(!p.areSubjectsModified());
    BOOST_CHECK(!p.isModified() && p.state() == State::Existing);
}

BOOST_AUTO_TEST_CASE(test_save_existing)
{
    SubjectsPlan p(createPlan());

    p.setName(NAME_2);
    p.erase(0);

    BOOST_CHECK(p.state() == State::Modified && p.isModified());

    p.save();

    BOOST_CHECK(!p.isNameModified());
    BOOST_CHECK(!p.areSubjectsModified());
    BOOST_CHECK(!p.isModified() && p.state() == State::Existing);
}

BOOST_AUTO_TEST_SUITE_END()
