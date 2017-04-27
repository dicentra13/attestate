#pragma once

#include <attestate/common.h>
#include <attestate/student.h>
#include <attestate/subjects.h>

#include <QString>

#include <list>
#include <set>

namespace attestate {

typedef DataString ClassId;

class Class {
public:
    typedef std::unique_ptr<Student> StudentPtr;

    // create new
    explicit Class(const ID& id);

    // load existing
    Class(
        const ID& id,
        const ClassId& classId,
        OptionalYear graduationYear,
        const OptionalDate& issueDate,
        std::vector<StudentPtr> students,
        const SubjectsPlanPtr& subjectsPlan);

    Class(Class&&);
    Class& operator = (Class&&);

    ~Class();

    const ID& id() const;
    void setDBID(const DBID& dbid); // if saved to DB

    State state() const;
    void setDeleted(bool isDeleted);

    bool isModified() const;

    // class data

    const ClassId& classId() const;
    void setClassId(const ClassId& id);
    bool isClassIdModified() const;

    // graduation year

    OptionalYear graduationYear() const;
    void setGraduationYear(const OptionalYear& year);
    bool isGraduationYearModified() const;

    // issue date

    const OptionalDate& issueDate() const;
    void setIssueDate(const OptionalDate& issueDate);
    bool isIssueDateModified() const;

    // students access

    typedef Student* StudentWeakPtr;
    typedef std::list<StudentWeakPtr> StudentWeakPtrList;
    typedef const Student* ConstStudentWeakPtr;
    typedef std::list<ConstStudentWeakPtr> ConstStudentWeakPtrList;

    typedef uint8_t Index;

    const Student& student(Index at) const;
    Student& student(Index at);

    ConstStudentWeakPtrList studentsList() const;
    StudentWeakPtrList studentsList();

    // check students are not marked as deleted
    void insert(StudentPtr student, Index at);
    void insert(std::map<Index, StudentPtr> students);
    void append(StudentPtr student);

    // returns deleted student
    StudentPtr erase(Index at);
    // returns map {index in vector before erase -> student}
    std::map<Index, StudentPtr> erase(const std::set<Index>& at);

    size_t studentsCount() const;

    bool areStudentsModified() const;

    // subjects plan
    // shared with other classes

    const SubjectsPlanPtr& subjectsPlan() const;
    bool isSubjectsPlanModified() const;

    void setSubjectsPlan(const SubjectsPlanPtr& subjectsPlan);

    // set current state as original and discard cached changes
    // in class itself and students
    void save();

private:
    class Impl;

    std::unique_ptr<Impl> impl_;
};

} // namespace attestate

