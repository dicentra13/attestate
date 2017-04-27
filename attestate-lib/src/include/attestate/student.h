#pragma once

#include <attestate/common.h>

#include <boost/optional.hpp>

#include <memory>

namespace attestate {

class SubjectsGrades;

class Student {
public:
    // create new
    explicit Student(const ID& id);

    // load existing
    Student(
        const ID& id,
        const DataString& familyName,
        const DataString& name,
        const DataString& parentalName,
        const QDate& birthDate,
        const SubjectsGrades& grades,
        OptionalYear graduationYear,
        const AttestateId& attestateId,
        const OptionalDate& issueDate);

    Student(Student&&);
    Student& operator = (Student&&);

    ~Student();

    const ID& id() const;
    void setDBID(const DBID& dbid); // if saved to DB

    // returns Modified if any data needs saving, including grades
    State state() const;

    void setDeleted(bool isDeleted);

    // personal information

    const DataString& familyName() const;
    void setFamilyName(const DataString& familyName);
    bool isFamilyNameModified() const;

    const DataString& name() const;
    void setName(const DataString& name);
    bool isNameModified() const;

    const DataString& parentalName() const;
    void setParentalName(const DataString& parentalName);
    bool isParentalNameModified() const;

    const QDate& birthDate() const;
    void setBirthDate(const QDate& birthDate);
    bool isBirthDateModified() const;

    bool isPersonalInfoModified() const;

    // subject grades

    const SubjectsGrades& grades() const;
    SubjectsGrades& grades();
    bool areGradesModified() const;

    // graduation year

    OptionalYear graduationYear() const;
    void setGraduationYear(OptionalYear graduationYear);
    bool isGraduationYearModified() const;

    // attestate data

    const AttestateId& attestateId() const;
    void setAttestateId(const AttestateId& attestateId);
    bool isAttestateIdModified() const;

    const OptionalDate& issueDate() const;
    void setIssueDate(const OptionalDate& issueDate);
    bool isIssueDateModified() const;

    bool isModified() const;

    // set current state as original and discard cached changes
    void save();

private:
    class Impl;

    std::unique_ptr<Impl> impl_;
};

} // namespace attestate

