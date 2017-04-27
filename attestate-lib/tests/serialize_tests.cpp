#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_NO_MAIN

#include <boost/test/unit_test.hpp>

#include <attestate/serialize.h>
#include <attestate/student.h>
#include <attestate/subjects.h>
#include <attestate/grades.h>
#include <attestate/class.h>
#include <attestate/exception.h>

#include "../src/helpers.h"

#include <initializer_list>

using namespace attestate;

BOOST_AUTO_TEST_SUITE(serialization_tests)

const SubjectPtr SUBJECT_1 = std::make_shared<Subject>(ID::gen(), QString::fromUtf8("Русский язык"));
const SubjectPtr SUBJECT_2 = std::make_shared<Subject>(ID::gen(), QString::fromUtf8("Алгебра"));
const SubjectPtr SUBJECT_3 = std::make_shared<Subject>(ID::gen(), QString::fromUtf8("Информатика и ИКТ"));
const SubjectPtr SUBJECT_4 = std::make_shared<Subject>(ID::gen(), QString::fromUtf8("Искусство"));
const SubjectPtr SUBJECT_5 = std::make_shared<Subject>(ID::gen(), QString::fromUtf8("Обж"));

const SubjectsPlanPtr SUBJECTS_PLAN = std::make_shared<SubjectsPlan>(
    ID::gen(),
    QString::fromUtf8("Учебный план"),
    SubjectPtrVector{ SUBJECT_1, SUBJECT_2, SUBJECT_3, SUBJECT_4, SUBJECT_5 }
);

const grades::Value G_3 = "3";
const grades::Value G_4 = "4";
const grades::Value G_5 = "5";
const grades::Value G_T_1 = QString::fromUtf8("д");
const grades::Value G_T_2 = QString::fromUtf8("Д");
const grades::Value G_T_3 = QString::fromUtf8("+");
const grades::Value G_N_1 = QString::fromUtf8("н");
const grades::Value G_N_2 = QString::fromUtf8("Н");
const grades::Value G_N_3 = QString::fromUtf8("-");

Class::StudentPtr createStudent1()
{
    return Class::StudentPtr(new Student(
        ID::gen(),
        QString::fromUtf8("Иванов"),
        QString::fromUtf8("Иван"),
        QString::fromUtf8("Иванович"),
        QDate(2000, 2, 1),
        {{
            {SUBJECT_1->id(), G_3},
            {SUBJECT_2->id(), G_4},
            {SUBJECT_3->id(), G_T_1},
            {SUBJECT_4->id(), G_T_3},
            {SUBJECT_5->id(), G_N_2}
        }},
        boost::none,
        "001",
        QDate::currentDate()
    ));
}

Class::StudentPtr createStudent2()
{
    return Class::StudentPtr(new Student(
         ID::gen(),
         QString::fromUtf8("Петров"),
         QString::fromUtf8("Петр"),
         QString::fromUtf8("Петрович"),
         QDate(2001, 12, 14),
         {{
             {SUBJECT_1->id(), G_5},
             {SUBJECT_2->id(), G_4},
             {SUBJECT_3->id(), G_N_1},
             {SUBJECT_4->id(), G_T_2},
             {SUBJECT_5->id(), G_N_3}
         }},
         boost::none,
         "002",
         QDate::currentDate()
     ));
}

Class::StudentPtr createStudent3()
{
    return Class::StudentPtr(new Student(
         ID::gen(),
         QString::fromUtf8("Сидоров"),
         QString::fromUtf8("Сидор"),
         QString::fromUtf8("Сидорович"),
         QDate(2002, 11, 4),
         {{
             {SUBJECT_1->id(), G_3},
             {SUBJECT_2->id(), G_5},
             {SUBJECT_3->id(), G_T_1},
             {SUBJECT_4->id(), G_4},
             {SUBJECT_5->id(), G_T_2}
         }},
         boost::none,
         "003",
         QDate::currentDate()
     ));
}

std::unique_ptr<Class> createClass()
{
    std::vector<Class::StudentPtr> students;
    students.push_back(createStudent1());
    students.push_back(createStudent2());
    students.push_back(createStudent3());

    return std::unique_ptr<Class>(new Class{
        ID::gen(),
        QString::fromUtf8("Класс"),
        OptionalYear(),
        QDate::currentDate(),
        std::move(students),
        SUBJECTS_PLAN
    });
}

void checkSubjectsPlan(const SubjectsPlanPtr& pe, const SubjectsPlanPtr& pr)
{
    if ((!pe && !pr)
        || (!pe && pr->subjectsCount() == 0)
        || (!pr && pe->subjectsCount() == 0))
    {
        return;
    }
    BOOST_REQUIRE(pe && pr);
    BOOST_REQUIRE(pe->subjectsCount() == pr->subjectsCount());

    for (size_t i = 0; i< pe->subjectsCount(); ++i) {
        BOOST_CHECK(pe->at(i).name() == pr->at(i).name());
    }
}

void checkStudent(const Student& se, const Student& sr)
{
    BOOST_CHECK(se.familyName() == sr.familyName());
    BOOST_CHECK(se.name() == sr.name());
    BOOST_CHECK(se.parentalName() == sr.parentalName());
    BOOST_CHECK(se.birthDate() == sr.birthDate());
    BOOST_CHECK(se.graduationYear() == sr.graduationYear());
    BOOST_CHECK(se.attestateId() == sr.attestateId());
    // issue dates is set must be equal
    BOOST_CHECK(!se.issueDate() || !sr.issueDate()
        || *se.issueDate() == *sr.issueDate());
}

// without issue dates
void checkClass(const Class& ce, const Class& cr)
{
    const auto& pe = ce.subjectsPlan();
    const auto& pr = cr.subjectsPlan();
    checkSubjectsPlan(pe, pr);

    BOOST_REQUIRE(ce.studentsCount() == cr.studentsCount());
    for (size_t i = 0; i < ce.studentsCount(); ++i) {
        const auto& se = ce.student(i);
        const auto& sr = cr.student(i);
        checkStudent(se, sr);

        // grades
        if (!pe || !pr) {
            continue;
        }
        for (size_t i = 0; i < pe->subjectsCount(); ++i) {
            BOOST_CHECK(se.grades().value(pe->at(i).id()) == sr.grades().value(pr->at(i).id()));
        }
    }
}

// students have equal issue dates

BOOST_AUTO_TEST_CASE(test_with_subjects_plan)
{
    auto cls = createClass();
    csv::Params params{';', QString("dd.MM.yyyy")};
    csv::write(*cls, "test.csv", params);
    auto rCls = csv::read("test.csv", params);

    checkClass(*cls, *rCls);
}

BOOST_AUTO_TEST_CASE(test_with_empty_subjects_plan)
{
    {
        auto cls = createClass();
        cls->setSubjectsPlan(std::make_shared<SubjectsPlan>(ID::gen()));
        csv::Params params{';', QString("dd.MM.yyyy")};
        csv::write(*cls, "test.csv", params);
        auto rCls = csv::read("test.csv", params);

        checkClass(*cls, *rCls);
    }
    {
        auto cls = createClass();
        cls->setSubjectsPlan(nullptr);
        csv::Params params{';', QString("dd.MM.yyyy")};
        csv::write(*cls, "test.csv", params);
        auto rCls = csv::read("test.csv", params);

        checkClass(*cls, *rCls);
    }
}

BOOST_AUTO_TEST_CASE(test_with_empty_student_issue_dates)
{
    auto cls = createClass();
    for (size_t i = 0; i < cls->studentsCount(); ++i) {
        cls->student(i).setIssueDate(boost::none);
    }
    csv::Params params{';', QString("dd.MM.yyyy")};
    csv::write(*cls, "test.csv", params);
    auto rCls = csv::read("test.csv", params);

    checkClass(*cls, *rCls);

    BOOST_CHECK(rCls->issueDate() == QDate::currentDate());
    for (size_t i = 0; i < rCls->studentsCount(); ++i) {
        BOOST_CHECK(!rCls->student(i).issueDate()); // equal to class
    }
}

BOOST_AUTO_TEST_CASE(test_with_empty_class_equal_student_issue_dates)
{
    auto cls = createClass();
    cls->setIssueDate(boost::none);
    csv::Params params{';', QString("dd.MM.yyyy")};
    csv::write(*cls, "test.csv", params);
    auto rCls = csv::read("test.csv", params);

    checkClass(*cls, *rCls);

    BOOST_CHECK(rCls->issueDate() == QDate::currentDate());
    for (size_t i = 0; i < rCls->studentsCount(); ++i) {
        BOOST_CHECK(!rCls->student(i).issueDate()); // equal to class
    }
}

BOOST_AUTO_TEST_CASE(test_with_all_empty_issue_dates)
{
    auto cls = createClass();
    for (size_t i = 0; i < cls->studentsCount(); ++i) {
        cls->student(i).setIssueDate(boost::none);
    }
    cls->setIssueDate(boost::none);
    csv::Params params{';', QString("dd.MM.yyyy")};
    csv::write(*cls, "test.csv", params);
    auto rCls = csv::read("test.csv", params);

    checkClass(*cls, *rCls);

    BOOST_CHECK(!rCls->issueDate());
    for (size_t i = 0; i < rCls->studentsCount(); ++i) {
        BOOST_CHECK(!rCls->student(i).issueDate()); // none
    }
}

BOOST_AUTO_TEST_CASE(test_with_students_different_issue_dates)
{
    for (OptionalDate d : {
        OptionalDate(QDate::currentDate()),
        OptionalDate(QDate::currentDate().addMonths(-1)),
        OptionalDate()})
    {
        auto cls = createClass();
        for (size_t i = 0; i < cls->studentsCount() - 1; ++i) {
            cls->student(i).setIssueDate(QDate::currentDate().addMonths(-1));
        }
        cls->setIssueDate(d);
        csv::Params params{';', QString("dd.MM.yyyy")};
        csv::write(*cls, "test.csv", params);
        auto rCls = csv::read("test.csv", params);

        checkClass(*cls, *rCls);

        // dependent only on students issue dates after read
        BOOST_CHECK(rCls->issueDate() == QDate::currentDate().addMonths(-1));

        for (size_t i = 0; i < rCls->studentsCount() - 1; ++i) {
            BOOST_CHECK(!rCls->student(i).issueDate()); // equal to class
        }
        BOOST_CHECK(rCls->student(rCls->studentsCount() - 1).issueDate()
            == QDate::currentDate());
    }
}

BOOST_AUTO_TEST_SUITE_END()
