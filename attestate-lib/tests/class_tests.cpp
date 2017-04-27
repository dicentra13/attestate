#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_NO_MAIN

#include <boost/test/unit_test.hpp>

#include <attestate/student.h>
#include <attestate/subjects.h>
#include <attestate/grades.h>
#include <attestate/class.h>
#include <attestate/exception.h>

#include "../src/helpers.h"

#include <initializer_list>

using namespace attestate;

BOOST_AUTO_TEST_SUITE(class_tests)

BOOST_AUTO_TEST_CASE(test_create_new)
{
    const ID id = ID::gen();
    Class c(id);
    BOOST_CHECK(c.state() == State::New);
    BOOST_CHECK(c.id() == id);
    BOOST_CHECK(c.classId().isEmpty() && c.isClassIdModified());
    BOOST_CHECK(!c.graduationYear() && c.isGraduationYearModified());
    BOOST_CHECK(!c.issueDate() && c.isIssueDateModified());
    BOOST_CHECK(!c.subjectsPlan() && c.isSubjectsPlanModified());
    BOOST_CHECK(c.studentsCount() == 0 && c.studentsList().empty());
}

const ID STUDENT_ID_1 = ID::gen();
const DataString FNAME_1 = "Ivanov";
const DataString NAME_1 = "Vasya";
const DataString PNAME_1 = "Petrovich";
const QDate BDATE_1 = QDate(1995, 1, 1);

const ID SUBJ_ID_1 = ID::gen();
const DataString SUBJ_NAME_1 = "Subject 1";
const SubjectPtr SUBJ_1 = std::make_shared<Subject>(SUBJ_ID_1, SUBJ_NAME_1);

const ID SUBJ_ID_2 = ID::gen();
const DataString SUBJ_NAME_2 = "Subject 2";
const SubjectPtr SUBJ_2 = std::make_shared<Subject>(SUBJ_ID_2, SUBJ_NAME_2);

const grades::Value G_V_1 = "5";
const grades::Value G_V_2 = QString::fromUtf8("д");
const SubjectsGrades GRADES_1({{SUBJ_ID_1, G_V_1}, {SUBJ_ID_2, G_V_2}});

const Year YEAR_1 = 2016;
const AttestateId ATT_ID_1 = "000";

const OptionalDate ISSUE_DATE_1(QDate(2016, 6, 15));

Class::StudentPtr createStudent1()
{
    return Class::StudentPtr(new Student(
        STUDENT_ID_1,
        FNAME_1, NAME_1, PNAME_1,
        BDATE_1,
        GRADES_1,
        YEAR_1,
        ATT_ID_1,
        ISSUE_DATE_1));
}

const ID STUDENT_ID_2 = ID::gen();
const DataString FNAME_2 = "Petrov";
const DataString NAME_2 = "Ivan";
const DataString PNAME_2 = "Vasilyevich";
const QDate BDATE_2 = QDate(1995, 1, 2);

const ID SUBJ_ID_3 = ID::gen();
const DataString SUBJ_NAME_3 = "Subject 3";
const SubjectPtr SUBJ_3 = std::make_shared<Subject>(SUBJ_ID_3, SUBJ_NAME_3);

const grades::Value G_V_3 = "5";
const grades::Value G_V_2_2 = "3";
const SubjectsGrades GRADES_2({{SUBJ_ID_2, G_V_2_2}, {SUBJ_ID_3, G_V_3}});

const Year YEAR_2 = 2015;
const AttestateId ATT_ID_2 = "111";

const OptionalDate ISSUE_DATE_2(QDate(2016, 6, 10));

Class::StudentPtr createStudent2()
{
    return Class::StudentPtr(new Student(
        STUDENT_ID_2,
        FNAME_2, NAME_2, PNAME_2,
        BDATE_2,
        GRADES_2,
        YEAR_2,
        ATT_ID_2,
        ISSUE_DATE_2));
}

Class::StudentPtr createExistingStudent()
{
    return Class::StudentPtr(new Student(
        ID::gen(),
        FNAME_2, NAME_2, PNAME_2,
        BDATE_2,
        GRADES_2,
        YEAR_2,
        ATT_ID_2,
        ISSUE_DATE_2));
}

const ID PLAN_ID_1 = ID::gen();
const DataString PLAN_NAME_1 = "Plan 1";

SubjectsPlanPtr createSubjectsPlan1()
{
    return std::make_shared<SubjectsPlan>(
        PLAN_ID_1,
        PLAN_NAME_1,
        SubjectPtrVector{SUBJ_1, SUBJ_2, SUBJ_3});
}

const ID PLAN_ID_2 = ID::gen();
const DataString PLAN_NAME_2 = "Plan 2";

SubjectsPlanPtr createSubjectsPlan2()
{
    return std::make_shared<SubjectsPlan>(
        PLAN_ID_2,
        PLAN_NAME_2,
        SubjectPtrVector{SUBJ_3, SUBJ_2, SUBJ_1});
}

const ID CLASS_ID = ID::gen();
const DataString CLASS_CLASS_ID_1 = "Class";
const DataString CLASS_CLASS_ID_2 = "Class 2";

Class createClass()
{
    std::vector<Class::StudentPtr> s;
    s.push_back(createStudent1());
    s.push_back(createStudent2());

    return Class(
        CLASS_ID, CLASS_CLASS_ID_1, YEAR_1, ISSUE_DATE_1,
        std::move(s),
        createSubjectsPlan1());
}

void checkStudentsList(const Class& c, std::initializer_list<ID> ids)
{
    const auto& sl = c.studentsList();
    BOOST_REQUIRE(sl.size() == ids.size());
    BOOST_REQUIRE(c.studentsCount() == ids.size());
    size_t i = 0;
    auto it = sl.begin();
    for (const auto& id : ids) {
        BOOST_CHECK(c.student(i++).id() == id);
        BOOST_CHECK((*it++)->id() == id);
    }
}

BOOST_AUTO_TEST_CASE(test_create_existing)
{
    Class c = createClass();
    BOOST_CHECK(c.state() == State::Existing && !c.isModified());
    BOOST_CHECK(c.id() == CLASS_ID);
    BOOST_CHECK(c.classId() == CLASS_CLASS_ID_1 && !c.isClassIdModified());
    BOOST_CHECK(c.graduationYear() == YEAR_1 && !c.isGraduationYearModified());
    BOOST_CHECK(c.issueDate() == ISSUE_DATE_1 && !c.isIssueDateModified());
    BOOST_CHECK(c.subjectsPlan() && !c.isSubjectsPlanModified());
    BOOST_CHECK_NO_THROW(c.subjectsPlan()->id() == PLAN_ID_1);
    BOOST_CHECK(!c.areStudentsModified());
    checkStudentsList(c, {STUDENT_ID_1, STUDENT_ID_2});
}

BOOST_AUTO_TEST_CASE(test_create_errors)
{
    auto createClassFromStudents = [] (std::vector<Class::StudentPtr>&& s)
    {
        return Class(
            CLASS_ID, CLASS_CLASS_ID_1, YEAR_1, ISSUE_DATE_1,
            std::move(s),
            createSubjectsPlan1());
    };

    // empty ptr
    {
        std::unique_ptr<Class> c;
        std::vector<Class::StudentPtr> s;
        s.push_back(createStudent1());
        s.push_back(Class::StudentPtr());
        BOOST_CHECK_THROW(
            c.reset(new Class(createClassFromStudents(std::move(s)))),
            Exception);
    }
    // new student
    {
        std::unique_ptr<Class> c;
        std::vector<Class::StudentPtr> s;
        s.push_back(createStudent1());
        s.push_back(Class::StudentPtr(new Student(ID::gen())));
        BOOST_CHECK_THROW(
            c.reset(new Class(createClassFromStudents(std::move(s)))),
            Exception);
    }
    // modified student
    {
        std::unique_ptr<Class> c;
        std::vector<Class::StudentPtr> s;
        s.push_back(createStudent1());
        s.push_back(createStudent2());
        s.back()->setName(NAME_1);
        BOOST_CHECK_THROW(
            c.reset(new Class(createClassFromStudents(std::move(s)))),
            Exception);
    }
    // deleted student
    {
        std::unique_ptr<Class> c;
        std::vector<Class::StudentPtr> s;
        s.push_back(createStudent1());
        s.push_back(createStudent2());
        s.back()->setDeleted(true);
        BOOST_CHECK_THROW(
            c.reset(new Class(createClassFromStudents(std::move(s)))),
            Exception);
    }
}

const DataString EMPTY_STR = "";

BOOST_AUTO_TEST_CASE(test_edit_class_id)
{
    {
        Class c(createClass());
        c.setClassId(CLASS_CLASS_ID_2);
        BOOST_CHECK(c.classId() == CLASS_CLASS_ID_2);
        BOOST_CHECK(c.isClassIdModified());
        BOOST_CHECK(c.state() == State::Modified && c.isModified());
        c.setClassId(CLASS_CLASS_ID_1);
        BOOST_CHECK(c.classId() == CLASS_CLASS_ID_1);
        BOOST_CHECK(!c.isClassIdModified());
        BOOST_CHECK(c.state() == State::Existing && !c.isModified());
    }
    {
        Class c(ID::gen());
        for (const auto& v : {CLASS_CLASS_ID_2, EMPTY_STR}) {
            c.setClassId(v);
            BOOST_CHECK(c.classId() == v);
            BOOST_CHECK(c.isClassIdModified());
            BOOST_CHECK(c.state() == State::New);
        }
    }
}

BOOST_AUTO_TEST_CASE(test_edit_graduation_year)
{
    {
        Class c(createClass());
        c.setGraduationYear(YEAR_2);
        BOOST_CHECK(c.graduationYear() == YEAR_2);
        BOOST_CHECK(c.isGraduationYearModified());
        BOOST_CHECK(c.state() == State::Modified && c.isModified());
        c.setGraduationYear(YEAR_1);
        BOOST_CHECK(c.graduationYear() == YEAR_1);
        BOOST_CHECK(!c.isGraduationYearModified());
        BOOST_CHECK(c.state() == State::Existing && !c.isModified());
    }
    {
        Class c(ID::gen());
        for (const auto& v : {OptionalYear(YEAR_2), OptionalYear()}) {
            c.setGraduationYear(v);
            BOOST_CHECK(c.graduationYear() == v);
            BOOST_CHECK(c.isGraduationYearModified());
            BOOST_CHECK(c.state() == State::New);
        }
    }
}

BOOST_AUTO_TEST_CASE(test_edit_attestate_info)
{
    {
        Class c(createClass());
        c.setIssueDate(ISSUE_DATE_2);
        BOOST_CHECK(c.issueDate() == ISSUE_DATE_2);
        BOOST_CHECK(c.isIssueDateModified());
        BOOST_CHECK(c.state() == State::Modified && c.isModified());
        c.setIssueDate(ISSUE_DATE_1);
        BOOST_CHECK(c.issueDate() == ISSUE_DATE_1);
        BOOST_CHECK(!c.isIssueDateModified());
        BOOST_CHECK(c.state() == State::Existing && !c.isModified());
    }
    {
        Class c(ID::gen());
        for (const auto& v : {ISSUE_DATE_1, ISSUE_DATE_2, OptionalDate()}) {
            c.setIssueDate(v);
            BOOST_CHECK(c.issueDate() == v);
            BOOST_CHECK(c.isIssueDateModified());
            BOOST_CHECK(c.state() == State::New);
        }
    }
}

BOOST_AUTO_TEST_CASE(test_edit_subjects_plan)
{
    {
        Class c(createClass());
        c.setSubjectsPlan(createSubjectsPlan2());
        BOOST_CHECK(c.subjectsPlan() && c.subjectsPlan()->id() == PLAN_ID_2);
        BOOST_CHECK(c.isSubjectsPlanModified());
        BOOST_CHECK(c.state() == State::Modified && c.isModified());
        c.setSubjectsPlan(createSubjectsPlan1());
        BOOST_CHECK(c.subjectsPlan() && c.subjectsPlan()->id() == PLAN_ID_1);
        BOOST_CHECK(!c.isSubjectsPlanModified());
        BOOST_CHECK(c.state() == State::Existing && !c.isModified());
    }
    {
        Class c(ID::gen());
        for (const auto& v : {createSubjectsPlan2(), SubjectsPlanPtr()}) {
            c.setSubjectsPlan(v);
            BOOST_CHECK(
                (!v && !c.subjectsPlan())
                || (v && c.subjectsPlan() && c.subjectsPlan()->id() == v->id()));
            BOOST_CHECK(c.isSubjectsPlanModified());
            BOOST_CHECK(c.state() == State::New);
        }
    }
}

BOOST_AUTO_TEST_CASE(test_students_access)
{
    Class c(createClass());
    checkStudentsList(c, {STUDENT_ID_1, STUDENT_ID_2});
    BOOST_CHECK_THROW(c.student(2), Exception);
}


typedef std::map<Class::Index, Class::StudentPtr> InsertMap;

void checkStudentsUnmodified(const Class& c)
{
    BOOST_CHECK(!c.areStudentsModified());
    checkStudentsList(c, {STUDENT_ID_1, STUDENT_ID_2});
}

BOOST_AUTO_TEST_CASE(test_students_insert)
{
    {
        Class c(createClass());
        auto s = createExistingStudent();
        const ID id = s->id();
        c.append(std::move(s));
        BOOST_CHECK(c.areStudentsModified() && c.isModified());
        checkStudentsList(c, {STUDENT_ID_1, STUDENT_ID_2, id});
    }
    {
        Class c(createClass());
        auto s = createExistingStudent();
        const ID id = s->id();
        c.insert(std::move(s), 0);
        BOOST_CHECK(c.areStudentsModified() && c.isModified());
        checkStudentsList(c, {id, STUDENT_ID_1, STUDENT_ID_2});
    }
    {
        Class c(createClass());
        auto s = createExistingStudent();
        const ID id = s->id();
        c.insert(std::move(s), 1);
        BOOST_CHECK(c.areStudentsModified() && c.isModified());
        checkStudentsList(c, {STUDENT_ID_1, id, STUDENT_ID_2});
    }
    {
        Class c(createClass());
        auto s = createExistingStudent();
        const ID id = s->id();
        c.insert(std::move(s), 2);
        BOOST_CHECK(c.areStudentsModified() && c.isModified());
        checkStudentsList(c, {STUDENT_ID_1, STUDENT_ID_2, id});
    }

    {
        Class c(createClass());
        auto s0 = createExistingStudent();
        const ID id0 = s0->id();
        auto s2 = createExistingStudent();
        const ID id2 = s2->id();
        auto s4 = createExistingStudent();
        const ID id4 = s4->id();
        InsertMap m;
        m.emplace(0, std::move(s0));
        m.emplace(2, std::move(s2));
        m.emplace(4, std::move(s4));
        c.insert(std::move(m));
        BOOST_CHECK(c.areStudentsModified() && c.isModified());
        checkStudentsList(c, {id0, STUDENT_ID_1, id2, STUDENT_ID_2, id4});
    }
}

BOOST_AUTO_TEST_CASE(test_students_insert_errors)
{
    auto createDeletedStudent = [] ()
    {
        Class::StudentPtr s(new Student(ID::gen()));
        s->setDeleted(true);
        return s;
    };

    {
        // empty ptr
        Class c(createClass());
        BOOST_CHECK_THROW(c.append(Class::StudentPtr()), Exception);
        checkStudentsUnmodified(c);
        BOOST_CHECK_THROW(c.insert(Class::StudentPtr(), 0), Exception);
        checkStudentsUnmodified(c);
        InsertMap m;
        m.emplace(0, Class::StudentPtr(new Student(ID::gen())));
        m.emplace(1, Class::StudentPtr());
        BOOST_CHECK_THROW(c.insert(std::move(m)), Exception);
        checkStudentsUnmodified(c);
    }
    {
        // deleted student
        Class c(createClass());
        BOOST_CHECK_THROW(c.append(createDeletedStudent()), Exception);
        checkStudentsUnmodified(c);
        BOOST_CHECK_THROW(c.insert(createDeletedStudent(), 0), Exception);
        checkStudentsUnmodified(c);
        InsertMap m;
        m.emplace(0, Class::StudentPtr(new Student(ID::gen())));
        m.emplace(1, createDeletedStudent());
        BOOST_CHECK_THROW(c.insert(std::move(m)), Exception);
        checkStudentsUnmodified(c);
    }
    {
        // duplicate key
        Class c(createClass());
        BOOST_CHECK_THROW(
            c.append(Class::StudentPtr(new Student(STUDENT_ID_1))),
            Exception);
        checkStudentsUnmodified(c);
        BOOST_CHECK_THROW(
            c.insert(Class::StudentPtr(new Student(STUDENT_ID_1)), 0),
            Exception);
        checkStudentsUnmodified(c);
        {
            InsertMap m;
            m.emplace(0, Class::StudentPtr(new Student(ID::gen())));
            m.emplace(1, createStudent1());
            BOOST_CHECK_THROW(c.insert(std::move(m)), Exception);
            checkStudentsUnmodified(c);
        }
        {
            const ID STUDENT_ID_3 = ID::gen();
            InsertMap m;
            m.emplace(0, Class::StudentPtr(new Student(STUDENT_ID_3)));
            m.emplace(1, Class::StudentPtr(new Student(STUDENT_ID_3)));
            BOOST_CHECK_THROW(c.insert(std::move(m)), Exception);
            checkStudentsUnmodified(c);
        }
    }
    {
        // invalid index
        Class c(createClass());
        BOOST_CHECK_THROW(
            c.insert(Class::StudentPtr(new Student(ID::gen())), 3),
            Exception);
        checkStudentsUnmodified(c);
        InsertMap m;
        m.emplace(0, Class::StudentPtr(new Student(ID::gen())));
        m.emplace(8, Class::StudentPtr(new Student(ID::gen())));
        BOOST_CHECK_THROW(c.insert(std::move(m)), Exception);
        checkStudentsUnmodified(c);
    }
}

BOOST_AUTO_TEST_CASE(test_students_erase)
{
    {
        Class c(createClass());
        auto s = c.erase(0);
        BOOST_CHECK(s->id() == STUDENT_ID_1);
        BOOST_CHECK(c.areStudentsModified() && c.isModified());
        checkStudentsList(c, {STUDENT_ID_2});
    }
    {
        Class c(createClass());
        auto s = c.erase(1);
        BOOST_CHECK(s->id() == STUDENT_ID_2);
        BOOST_CHECK(c.areStudentsModified() && c.isModified());
        checkStudentsList(c, {STUDENT_ID_1});
    }
    {
        Class c(createClass());
        auto s = c.erase({0, 1});
        auto it = s.begin();
        BOOST_CHECK(it->first == 0);
        BOOST_CHECK(it++->second->id() == STUDENT_ID_1);
        BOOST_CHECK(it->first == 1);
        BOOST_CHECK(it++->second->id() == STUDENT_ID_2);
        BOOST_CHECK(c.areStudentsModified() && c.isModified());
        checkStudentsList(c, {});
    }
    {
        // invalid index
        Class c(createClass());
        BOOST_CHECK_THROW(c.erase(8), Exception);
        checkStudentsUnmodified(c);
        BOOST_CHECK_THROW(c.erase({0, 8}), Exception);
        checkStudentsUnmodified(c);
    }
}

BOOST_AUTO_TEST_CASE(test_students_modified)
{
    {
        Class c(createClass());
        c.student(0).setName(NAME_2);
        BOOST_CHECK(c.areStudentsModified() && c.isModified());
    }
    {
        Class c(createClass());
        c.append(createExistingStudent());
        BOOST_CHECK(c.areStudentsModified() && c.isModified());
        c.erase(c.studentsCount() - 1);
        BOOST_CHECK(!c.areStudentsModified() && !c.isModified());
    }
    {
        Class c(createClass());
        InsertMap m;
        m.emplace(0, createExistingStudent());
        m.emplace(2, createExistingStudent());
        m.emplace(4, createExistingStudent());
        c.insert(std::move(m));
        BOOST_CHECK(c.areStudentsModified() && c.isModified());
        c.erase({0, 2, 4});
        BOOST_CHECK(!c.areStudentsModified() && !c.isModified());
    }
    {
        Class c(createClass());
        auto s = c.erase(0);
        BOOST_CHECK(c.areStudentsModified() && c.isModified());
        c.insert(std::move(s), 0);
        BOOST_CHECK(!c.areStudentsModified() && !c.isModified());
    }
    {
        Class c(createClass());
        auto s = c.erase({0, 1});
        BOOST_CHECK(c.areStudentsModified() && c.isModified());
        c.insert(std::move(s));
        BOOST_CHECK(!c.areStudentsModified() && !c.isModified());
    }
    {
        Class c(createClass());
        auto s = c.erase({0, 1});
        BOOST_CHECK(c.areStudentsModified() && c.isModified());
        s.begin()->second->setName(NAME_2);
        c.insert(std::move(s));
        BOOST_CHECK(c.areStudentsModified() && c.isModified());
    }
}

BOOST_AUTO_TEST_CASE(test_deleted)
{
    {
        Class c(ID::gen());

        c.setDeleted(true);
        BOOST_CHECK(c.state() == State::Deleted);
        BOOST_CHECK_THROW(c.save(), Exception);
        c.setDeleted(false);
        BOOST_CHECK(c.state() == State::New);
    }

    {
        Class c(createClass());

        c.setDeleted(true);
        BOOST_CHECK(c.state() == State::Deleted);
        BOOST_CHECK_THROW(c.save(), Exception);
        c.setDeleted(false);
        BOOST_CHECK(c.state() == State::Existing);
    }
}

BOOST_AUTO_TEST_CASE(test_save)
{
    {
        Class c(CLASS_ID);
        c.setClassId(CLASS_CLASS_ID_1);
        c.setGraduationYear(YEAR_1);
        c.setIssueDate(ISSUE_DATE_1);
        c.setSubjectsPlan(createSubjectsPlan1());
        c.append(createStudent1());
        c.append(createStudent2());

        BOOST_CHECK(c.state() == State::New);

        c.save();

        BOOST_CHECK(c.state() == State::Existing && !c.isModified());
        BOOST_CHECK(c.id() == CLASS_ID);
        BOOST_CHECK(c.classId() == CLASS_CLASS_ID_1 && !c.isClassIdModified());
        BOOST_CHECK(c.graduationYear() == YEAR_1 && !c.isGraduationYearModified());
        BOOST_CHECK(c.issueDate() == ISSUE_DATE_1 && !c.isIssueDateModified());
        BOOST_CHECK(c.subjectsPlan() && !c.isSubjectsPlanModified());
        BOOST_CHECK_NO_THROW(c.subjectsPlan()->id() == PLAN_ID_1);
        BOOST_CHECK(!c.areStudentsModified());
        checkStudentsList(c, {STUDENT_ID_1, STUDENT_ID_2});
        for (auto sp : c.studentsList()) {
            BOOST_CHECK(!sp->isModified());
        }
    }
    {
        Class c(createClass());
        c.setClassId(CLASS_CLASS_ID_2);
        c.setGraduationYear(YEAR_2);
        c.setIssueDate(ISSUE_DATE_2);
        c.setSubjectsPlan(createSubjectsPlan2());
        const ID STUDENT_ID_3 = ID::gen();
        c.append(Class::StudentPtr(new Student(STUDENT_ID_3)));
        c.erase(1);

        BOOST_CHECK(c.state() == State::Modified && c.isModified());

        c.save();

        BOOST_CHECK(c.state() == State::Existing && !c.isModified());
        BOOST_CHECK(c.id() == CLASS_ID);
        BOOST_CHECK(c.classId() == CLASS_CLASS_ID_2 && !c.isClassIdModified());
        BOOST_CHECK(c.graduationYear() == YEAR_2 && !c.isGraduationYearModified());
        BOOST_CHECK(c.issueDate() == ISSUE_DATE_2 && !c.isIssueDateModified());
        BOOST_CHECK(c.subjectsPlan() && !c.isSubjectsPlanModified());
        BOOST_CHECK_NO_THROW(c.subjectsPlan()->id() == PLAN_ID_2);
        BOOST_CHECK(!c.areStudentsModified());
        checkStudentsList(c, {STUDENT_ID_1, STUDENT_ID_3});
        for (auto sp : c.studentsList()) {
            BOOST_CHECK(!sp->isModified() && sp->state() == State::Existing);
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
