#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_NO_MAIN

#include <boost/test/unit_test.hpp>

#include <attestate/student.h>
#include <attestate/grades.h>
#include <attestate/exception.h>

#include "../src/helpers.h"

using namespace attestate;

BOOST_AUTO_TEST_SUITE(student_tests)

BOOST_AUTO_TEST_CASE(test_construction_new)
{
    const ID id = ID::gen();
    Student s(id);
    BOOST_CHECK(s.state() == State::New);
    BOOST_CHECK(s.id() == id);
    BOOST_CHECK(s.familyName().isEmpty() && s.isFamilyNameModified());
    BOOST_CHECK(s.name().isEmpty() && s.isNameModified());
    BOOST_CHECK(s.parentalName().isEmpty() && s.isParentalNameModified());
    BOOST_CHECK(s.birthDate() == QDate() && s.isBirthDateModified());
    BOOST_CHECK(s.isPersonalInfoModified());
    BOOST_CHECK(s.graduationYear() == boost::none && s.isGraduationYearModified());
    BOOST_CHECK(s.attestateId().isEmpty() && s.isAttestateIdModified());
    BOOST_CHECK(s.issueDate() == boost::none && s.isIssueDateModified());
    BOOST_CHECK(s.grades().diff(SubjectsGrades()).empty());
    BOOST_CHECK(s.areGradesModified());
}

const ID ID_1 = ID::gen();
const DataString FNAME_1 = "Ivanov";
const DataString NAME_1 = "Vasya";
const DataString PNAME_1 = "Petrovich";
const QDate BDATE_1 = QDate(1995, 1, 1);

const ID SUBJ_ID_1 = ID::gen();
const ID SUBJ_ID_2 = ID::gen();
const grades::Value G_V_1 = "5";
const grades::Value G_V_2 = QString::fromUtf8("д");
const SubjectsGrades GRADES_1({{SUBJ_ID_1, G_V_1}, {SUBJ_ID_2, G_V_2}});

const Year YEAR_1 = 2016;
const AttestateId ATT_ID_1 = "000";

const boost::optional<std::string> TEMPLATE_PATH_1(".");
const OptionalDate ISSUE_DATE_1(QDate(2016, 6, 15));

Student createStudent1()
{
    return Student(
        ID_1,
        FNAME_1, NAME_1, PNAME_1,
        BDATE_1,
        GRADES_1,
        YEAR_1,
        ATT_ID_1,
        ISSUE_DATE_1);
}

BOOST_AUTO_TEST_CASE(test_construction_existing)
{
    Student s(createStudent1());

    BOOST_CHECK(s.state() == State::Existing && !s.isModified());
    BOOST_CHECK(s.id() == ID_1);
    BOOST_CHECK(s.familyName() == FNAME_1 && !s.isFamilyNameModified());
    BOOST_CHECK(s.name() == NAME_1 && !s.isNameModified());
    BOOST_CHECK(s.parentalName() == PNAME_1 && !s.isParentalNameModified());
    BOOST_CHECK(s.birthDate() == BDATE_1 && !s.isBirthDateModified());
    BOOST_CHECK(!s.isPersonalInfoModified());
    BOOST_CHECK(s.graduationYear() == YEAR_1 && !s.isGraduationYearModified());
    BOOST_CHECK(s.attestateId() == ATT_ID_1 && !s.isAttestateIdModified());
    BOOST_CHECK(s.issueDate() == ISSUE_DATE_1 && !s.isIssueDateModified());

    const auto& grades = s.grades();
    BOOST_CHECK(
        grades.value(SUBJ_ID_1) == G_V_1 &&
        grades.value(SUBJ_ID_2) == G_V_2);
    BOOST_CHECK(!s.areGradesModified());
}

const ID ID_2 = ID::gen();
const DataString FNAME_2 = "Petrov";
const DataString NAME_2 = "Ivan";
const DataString PNAME_2 = "Vasilyevich";
const QDate BDATE_2 = QDate(1995, 1, 2);

const ID SUBJ_ID_3 = ID::gen();
const grades::Value G_V_3 = "5";
const grades::Value G_V_2_2 = "3";

const Year YEAR_2 = 2015;
const AttestateId ATT_ID_2 = "111";

const boost::optional<std::string> TEMPLATE_PATH_2("..");
const OptionalDate ISSUE_DATE_2(QDate(2016, 6, 10));

const DataString EMPTY_STR = "";

BOOST_AUTO_TEST_CASE(test_edit_family_name)
{
    {
        Student s(createStudent1());
        s.setFamilyName(FNAME_2);
        BOOST_CHECK(s.familyName() == FNAME_2);
        BOOST_CHECK(s.isFamilyNameModified());
        BOOST_CHECK(s.state() == State::Modified && s.isModified());
        BOOST_CHECK(s.isPersonalInfoModified());
        s.setFamilyName(FNAME_1);
        BOOST_CHECK(s.familyName() == FNAME_1);
        BOOST_CHECK(!s.isFamilyNameModified());
        BOOST_CHECK(s.state() == State::Existing && !s.isModified());
        BOOST_CHECK(!s.isPersonalInfoModified());
    }
    {
        Student s(ID::gen());
        for (const auto& v : {FNAME_2, EMPTY_STR}) {
            s.setFamilyName(v);
            BOOST_CHECK(s.familyName() == v);
            BOOST_CHECK(s.isFamilyNameModified());
            BOOST_CHECK(s.isPersonalInfoModified());
            BOOST_CHECK(s.state() == State::New);
        }
    }
}

BOOST_AUTO_TEST_CASE(test_edit_name)
{
    {
        Student s(createStudent1());
        s.setName(NAME_2);
        BOOST_CHECK(s.name() == NAME_2);
        BOOST_CHECK(s.isNameModified());
        BOOST_CHECK(s.isPersonalInfoModified());
        BOOST_CHECK(s.state() == State::Modified && s.isModified());
        s.setName(NAME_1);
        BOOST_CHECK(s.name() == NAME_1);
        BOOST_CHECK(!s.isNameModified());
        BOOST_CHECK(!s.isPersonalInfoModified());
        BOOST_CHECK(s.state() == State::Existing && !s.isModified());
    }
    {
        Student s(ID::gen());
        for (const auto& v : {NAME_2, EMPTY_STR}) {
            s.setName(v);
            BOOST_CHECK(s.name() == v);
            BOOST_CHECK(s.isNameModified());
            BOOST_CHECK(s.isPersonalInfoModified());
            BOOST_CHECK(s.state() == State::New);
        }
    }
}

BOOST_AUTO_TEST_CASE(test_edit_parental_name)
{
    {
        Student s(createStudent1());
        s.setParentalName(PNAME_2);
        BOOST_CHECK(s.parentalName() == PNAME_2);
        BOOST_CHECK(s.isParentalNameModified());
        BOOST_CHECK(s.isPersonalInfoModified());
        BOOST_CHECK(s.state() == State::Modified && s.isModified());
        s.setParentalName(PNAME_1);
        BOOST_CHECK(s.parentalName() == PNAME_1);
        BOOST_CHECK(!s.isParentalNameModified());
        BOOST_CHECK(!s.isPersonalInfoModified());
        BOOST_CHECK(s.state() == State::Existing && !s.isModified());
    }
    {
        Student s(ID::gen());
        for (const auto& v : {PNAME_2, EMPTY_STR}) {
            s.setParentalName(v);
            BOOST_CHECK(s.parentalName() == v);
            BOOST_CHECK(s.isParentalNameModified());
            BOOST_CHECK(s.isPersonalInfoModified());
            BOOST_CHECK(s.state() == State::New);
        }
    }
}

BOOST_AUTO_TEST_CASE(test_edit_bdate)
{
    {
        Student s(createStudent1());
        s.setBirthDate(BDATE_2);
        BOOST_CHECK(s.birthDate() == BDATE_2);
        BOOST_CHECK(s.isBirthDateModified());
        BOOST_CHECK(s.isPersonalInfoModified());
        BOOST_CHECK(s.state() == State::Modified && s.isModified());
        s.setBirthDate(BDATE_1);
        BOOST_CHECK(s.birthDate() == BDATE_1);
        BOOST_CHECK(!s.isBirthDateModified());
        BOOST_CHECK(!s.isPersonalInfoModified());
        BOOST_CHECK(s.state() == State::Existing && !s.isModified());
    }
    {
        Student s(ID::gen());
        for (const auto& v : {BDATE_2, QDate()}) {
            s.setBirthDate(v);
            BOOST_CHECK(s.birthDate() == v);
            BOOST_CHECK(s.isBirthDateModified());
            BOOST_CHECK(s.isPersonalInfoModified());
            BOOST_CHECK(s.state() == State::New);
        }
    }
}

BOOST_AUTO_TEST_CASE(test_edit_grades)
{
    {
        Student s(createStudent1());
        auto& grades = s.grades();
        grades.setValue(SUBJ_ID_2, G_V_2_2);
        grades.setValue(SUBJ_ID_3, G_V_3);
        BOOST_CHECK(s.state() == State::Modified && s.isModified());
        BOOST_CHECK(s.areGradesModified());
        grades.setValue(SUBJ_ID_2, G_V_2);
        grades.setValue(SUBJ_ID_3, boost::none);
        BOOST_CHECK(s.state() == State::Existing && !s.isModified());
        BOOST_CHECK(!s.areGradesModified());
    }
    {
        Student s(ID::gen());
        auto& grades = s.grades();
        grades.setValue(SUBJ_ID_2, G_V_2_2);
        BOOST_CHECK(s.areGradesModified());
        BOOST_CHECK(s.state() == State::New);
        grades.setValue(SUBJ_ID_2, G_V_2);
        BOOST_CHECK(s.areGradesModified());
        BOOST_CHECK(s.state() == State::New);
    }
}

BOOST_AUTO_TEST_CASE(test_edit_graduation_year)
{
    {
        Student s(createStudent1());
        s.setGraduationYear(YEAR_2);
        BOOST_CHECK(s.graduationYear() == YEAR_2);
        BOOST_CHECK(s.isGraduationYearModified());
        BOOST_CHECK(s.state() == State::Modified && s.isModified());
        s.setGraduationYear(YEAR_1);
        BOOST_CHECK(s.graduationYear() == YEAR_1);
        BOOST_CHECK(!s.isGraduationYearModified());
        BOOST_CHECK(s.state() == State::Existing && !s.isModified());
    }
    {
        Student s(ID::gen());
        for (const auto& v : {OptionalYear(YEAR_2), OptionalYear()}) {
            s.setGraduationYear(v);
            BOOST_CHECK(s.graduationYear() == v);
            BOOST_CHECK(s.isGraduationYearModified());
            BOOST_CHECK(s.state() == State::New);
        }
    }
}

BOOST_AUTO_TEST_CASE(test_edit_attestate_id)
{
    {
        Student s(createStudent1());
        s.setAttestateId(ATT_ID_2);
        BOOST_CHECK(s.attestateId() == ATT_ID_2);
        BOOST_CHECK(s.isAttestateIdModified());
        BOOST_CHECK(s.state() == State::Modified && s.isModified());
        s.setAttestateId(ATT_ID_1);
        BOOST_CHECK(s.attestateId() == ATT_ID_1);
        BOOST_CHECK(!s.isAttestateIdModified());
        BOOST_CHECK(s.state() == State::Existing && !s.isModified());
    }
    {
        Student s(ID::gen());
        for (const auto& v : {ATT_ID_2, EMPTY_STR}) {
            s.setAttestateId(v);
            BOOST_CHECK(s.attestateId() == v);
            BOOST_CHECK(s.isAttestateIdModified());
            BOOST_CHECK(s.state() == State::New);
        }
    }
}

BOOST_AUTO_TEST_CASE(test_edit_issue_date)
{
    {
        Student s(createStudent1());

        for (OptionalDate date : {ISSUE_DATE_2, OptionalDate()}) {
            s.setIssueDate(date);
            BOOST_CHECK(s.issueDate() == date);
            BOOST_CHECK(s.isIssueDateModified());
            BOOST_CHECK(s.state() == State::Modified && s.isModified());
            s.setIssueDate(ISSUE_DATE_1);
            BOOST_CHECK(s.issueDate() == ISSUE_DATE_1);
            BOOST_CHECK(!s.isIssueDateModified());
            BOOST_CHECK(s.state() == State::Existing && !s.isModified());
        }
    }
    {
        Student s(ID::gen());
        for (const auto& v : {ISSUE_DATE_1, ISSUE_DATE_2, OptionalDate()}) {
            s.setIssueDate(v);
            BOOST_CHECK(s.issueDate() == v);
            BOOST_CHECK(s.isIssueDateModified());
            BOOST_CHECK(s.state() == State::New);
        }

    }
}

BOOST_AUTO_TEST_CASE(test_save_new)
{
    Student s(ID_2);

    s.setFamilyName(FNAME_2);
    s.setName(NAME_2);
    s.setParentalName(PNAME_2);
    s.setBirthDate(BDATE_2);
    s.setGraduationYear(YEAR_2);
    s.setAttestateId(ATT_ID_2);
    s.setIssueDate(ISSUE_DATE_2);
    auto& grades = s.grades();
    grades.setValue(SUBJ_ID_2, G_V_2_2);

    BOOST_CHECK(s.state() == State::New);

    s.save();

    BOOST_CHECK(s.state() == State::Existing && !s.isModified());
    BOOST_CHECK(s.familyName() == FNAME_2 && !s.isFamilyNameModified());
    BOOST_CHECK(s.name() == NAME_2 && !s.isNameModified());
    BOOST_CHECK(s.parentalName() == PNAME_2 && !s.isParentalNameModified());
    BOOST_CHECK(s.birthDate() == BDATE_2 && !s.isBirthDateModified());
    BOOST_CHECK(!s.isPersonalInfoModified());
    BOOST_CHECK(!s.areGradesModified());
    BOOST_CHECK(s.graduationYear() == YEAR_2 && !s.isGraduationYearModified());
    BOOST_CHECK(s.attestateId() == ATT_ID_2 && !s.isAttestateIdModified());
    BOOST_CHECK(s.issueDate() == ISSUE_DATE_2 && !s.isIssueDateModified());
}

BOOST_AUTO_TEST_CASE(test_save_existing)
{
    Student s(createStudent1());

    s.setFamilyName(FNAME_2);
    s.setName(NAME_2);
    s.setParentalName(PNAME_2);
    s.setBirthDate(BDATE_2);
    s.setGraduationYear(YEAR_2);
    s.setAttestateId(ATT_ID_2);
    s.setIssueDate(ISSUE_DATE_2);
    auto& grades = s.grades();
    grades.setValue(SUBJ_ID_2, G_V_2_2);

    BOOST_CHECK(s.state() == State::Modified && s.isModified());

    s.save();

    BOOST_CHECK(s.state() == State::Existing && !s.isModified());
    BOOST_CHECK(s.familyName() == FNAME_2 && !s.isFamilyNameModified());
    BOOST_CHECK(s.name() == NAME_2 && !s.isNameModified());
    BOOST_CHECK(s.parentalName() == PNAME_2 && !s.isParentalNameModified());
    BOOST_CHECK(s.birthDate() == BDATE_2 && !s.isBirthDateModified());
    BOOST_CHECK(!s.isPersonalInfoModified());
    BOOST_CHECK(!s.areGradesModified());
    BOOST_CHECK(s.graduationYear() == YEAR_2 && !s.isGraduationYearModified());
    BOOST_CHECK(s.attestateId() == ATT_ID_2 && !s.isAttestateIdModified());
    BOOST_CHECK(s.issueDate() == ISSUE_DATE_2 && !s.isIssueDateModified());
}

BOOST_AUTO_TEST_CASE(test_delete)
{
    {
        Student s(ID::gen());

        s.setDeleted(true);
        BOOST_CHECK(s.state() == State::Deleted);
        BOOST_CHECK_THROW(s.save(), Exception);
        s.setDeleted(false);
        BOOST_CHECK(s.state() == State::New);
    }

    {
        Student s(createStudent1());

        s.setDeleted(true);
        BOOST_CHECK(s.state() == State::Deleted);
        BOOST_CHECK_THROW(s.save(), Exception);
        s.setDeleted(false);
        BOOST_CHECK(s.state() == State::Existing);
    }
}

BOOST_AUTO_TEST_SUITE_END()
