#include <attestate/class.h>

#include "helpers.h"
#include "unique_vector.h"

#include <attestate/exception.h>

#include <vector>

namespace attestate {

namespace {

typedef std::vector<Class::StudentPtr> StudentPtrVector;

struct Data {
    ClassId classId;
    OptionalYear graduationYear;
    OptionalDate issueDate;
    ID subjectsPlanId;
};

typedef std::unique_ptr<Data> DataPtr;

struct IsModified {
    explicit IsModified(bool v)
        : classId(v)
        , graduationYear(v)
        , issueDate(v)
        , subjectsPlan(v)
    {}

    bool operator () () const
    {
        return classId || graduationYear || issueDate || subjectsPlan;
    }

    void reset(bool v)
    {
        classId = graduationYear = issueDate = subjectsPlan = v;
    }

    bool classId;
    bool graduationYear;
    bool issueDate;
    bool subjectsPlan;
};


struct StudentKey {
    explicit StudentKey(const Class::StudentPtr& s) : id_(s->id()) {}

    const ID& operator () () const { return id_; }

    bool operator < (const StudentKey& o) const { return id_ < o.id_; }

private:
    ID id_;
};

std::ostream& operator << (std::ostream& os, const StudentKey& k)
{
    os << k();
    return os;
}

typedef UniqueVector<Class::StudentPtr, StudentKey> StudentsVector;

struct StudentsDiff {
    StudentsDiff() {}

    explicit StudentsDiff(const StudentsVector& students)
    {
        for (size_t i = 0; i < students.size(); ++i) {
            original.insert(students.at(i)->id());
        }
    }

    void processAdded(const ID& id)
    {
        deleted.erase(id);
        if (!original.count(id)) {
            added.insert(id);
        }
    }

    void processDeleted(const ID& id)
    {
        added.erase(id);
        if (original.count(id)) {
            deleted.insert(id);
        }
    }

    bool empty() const { return added.empty() && deleted.empty(); }

    IDSet original;
    IDSet added;
    IDSet deleted;
};

} // namespace

// impl

class Class::Impl {
public:
    explicit Impl(const ID& id)
        : id(id)
        , data(new Data{"", boost::none, boost::none, ID::emptyID()})
        , originalData(nullptr)
        , subjectsPlan(nullptr)
        , isDeleted(false)
        , isModified_(true)
    {}

    Impl(
            const ID& id,
            const ClassId& classId,
            OptionalYear graduationYear,
            OptionalDate issueDate,
            StudentPtrVector studentsV,
            const SubjectsPlanPtr& subjectsPlan)
        : id(id)
        , data(new Data{classId, graduationYear, issueDate, ID::emptyID()})
        , originalData(new Data(*data))
        , subjectsPlan(subjectsPlan)
        , isDeleted(false)
        , isModified_(false)
    {
        for (const auto& sp : studentsV) {
            ATT_ASSERT(sp);
            ATT_REQUIRE(sp->state() == State::Existing, "Student " << sp->id() << " state is not allowed");
        }
        students = StudentsVector(std::move(studentsV));
        if (subjectsPlan) {
            data->subjectsPlanId = subjectsPlan->id();
            originalData->subjectsPlanId = subjectsPlan->id();
        }
        studentsDiff = StudentsDiff(students);
    }

    Impl(const Impl&) = delete;
    Impl& operator = (const Impl&) = delete;

    Impl(Impl&& o) : id(o.id), isModified_(o.isModified_) { *this = std::move(o); }

    Impl& operator = (Impl&& o)
    {
        id = o.id;
        data = std::move(o.data);
        originalData = std::move(o.originalData);
        subjectsPlan = o.subjectsPlan;
        students = std::move(o.students);
        studentsDiff = std::move(o.studentsDiff);
        isDeleted = o.isDeleted;
        isModified_ = o.isModified_;
        return *this;
    }

    void calcModifiedClassId()
    {
        isModified_.classId = !originalData ||
            originalData->classId != data->classId;
    }

    void calcModifiedGraduationYear()
    {
        isModified_.graduationYear = !originalData ||
            originalData->graduationYear != data->graduationYear;
    }

    void calcModifiedIssueDate()
    {
        isModified_.issueDate = !originalData ||
            data->issueDate != originalData->issueDate;
    }

    void calcModifiedSubjectsPlan()
    {
        isModified_.subjectsPlan = !originalData ||
            originalData->subjectsPlanId != data->subjectsPlanId;
    }

    bool areStudentsModified() const
    {
        if (!studentsDiff.empty()) {
            return true;
        }
        for (size_t i = 0; i < students.size(); ++i) {
            if (students.at(i)->state() != State::Existing) {
                return true;
            }
        }
        return false;
    }

    // own data
    const IsModified& isModified() const { return isModified_; }

    // including students
    bool isModifiedState() const
    {
        return isModified_() || areStudentsModified();
    }

    void resetModified() { isModified_.reset(false); }

    void resetStudentsDiff() { studentsDiff = StudentsDiff(students); }

    ID id;
    DataPtr data;
    DataPtr originalData;

    // denormalized data

    SubjectsPlanPtr subjectsPlan;

    StudentsVector students;
    StudentsDiff studentsDiff;

    bool isDeleted;

private:
    IsModified isModified_;
};

// Class

Class::Class(const ID& id)
    : impl_(new Impl(id))
{}

Class::Class(
        const ID& id,
        const ClassId& classId,
        OptionalYear graduationYear,
        const OptionalDate& issueDate,
        StudentPtrVector students,
        const SubjectsPlanPtr& subjectsPlan)
    : impl_(new Impl(
        id,
        classId,
        graduationYear,
        issueDate,
        std::move(students),
        subjectsPlan))
{}

Class::Class(Class&&) = default;
Class& Class::operator = (Class&&) = default;

Class::~Class()
{}

const ID& Class::id() const { return impl_->id; }

void Class::setDBID(const DBID& dbid)
{
    impl_->id = ID::setDBID(impl_->id, dbid);
}

State Class::state() const
{
    if (impl_->isDeleted) {
        return State::Deleted;
    }
    if (!impl_->originalData) {
        return State::New;
    }

    return impl_->isModifiedState() ? State::Modified : State::Existing;
}

void Class::setDeleted(bool isDeleted)
{
    impl_->isDeleted = isDeleted;
}

bool Class::isModified() const { return state() == State::Modified; }

// class data

const ClassId& Class::classId() const { return impl_->data->classId; }

void Class::setClassId(const ClassId& id)
{
    if (id == impl_->data->classId) {
        return;
    }
    impl_->data->classId = id;
    impl_->calcModifiedClassId();
}

bool Class::isClassIdModified() const { return impl_->isModified().classId; }

// graduation year

OptionalYear Class::graduationYear() const
{
    return impl_->data->graduationYear;
}

void Class::setGraduationYear(const OptionalYear& year)
{
    if (year == impl_->data->graduationYear) {
        return;
    }
    impl_->data->graduationYear = year;
    impl_->calcModifiedGraduationYear();
}

bool Class::isGraduationYearModified() const
{
    return impl_->isModified().graduationYear;
}

// attestate info

const OptionalDate& Class::issueDate() const { return impl_->data->issueDate; }

void Class::setIssueDate(const OptionalDate& issueDate)
{
    if (issueDate == impl_->data->issueDate) {
        return;
    }
    impl_->data->issueDate = issueDate;
    impl_->calcModifiedIssueDate();
}

bool Class::isIssueDateModified() const
{
    return impl_->isModified().issueDate;
}


// students access

const Student& Class::student(Index at) const
{
    const auto& ptr = impl_->students.at(at);
    ATT_ASSERT(ptr);
    return *ptr;
}

Student& Class::student(Index at)
{
    const auto& ptr = impl_->students.at(at);
    ATT_ASSERT(ptr);
    return *ptr;
}

Class::ConstStudentWeakPtrList Class::studentsList() const
{
    ConstStudentWeakPtrList result;
    size_t size = impl_->students.size();
    for (size_t i = 0; i < size; ++i) {
        result.push_back(impl_->students.at(i).get());
    }
    return result;
}

Class::StudentWeakPtrList Class::studentsList()
{
    StudentWeakPtrList result;
    size_t size = impl_->students.size();
    for (size_t i = 0; i < size; ++i) {
        result.push_back(impl_->students.at(i).get());
    }
    return result;
}

void Class::insert(StudentPtr student, Index at)
{
    ATT_ASSERT(student);
    const ID id = student->id();
    ATT_REQUIRE(student->state() != State::Deleted, "Student " << id << " is deleted");
    impl_->students.insert(std::move(student), at);
    impl_->studentsDiff.processAdded(id);
}

void Class::insert(std::map<Index, StudentPtr> students)
{
    IDSet ids;
    for (const auto& p : students) {
        ATT_ASSERT(p.second);
        const ID id = p.second->id();
        ATT_REQUIRE(p.second->state() != State::Deleted, "Student " << id << " is deleted");
        ids.insert(id);
    }
    impl_->students.insert(std::move(students));
    for (const auto& id : ids) {
        impl_->studentsDiff.processAdded(id);
    }
}

void Class::append(StudentPtr student)
{
    ATT_ASSERT(student);
    const ID id = student->id();
    ATT_REQUIRE(student->state() != State::Deleted, "Student " << id << " is deleted");
    impl_->students.append(std::move(student));
    impl_->studentsDiff.processAdded(id);
}

Class::StudentPtr Class::erase(Index at)
{
    auto ptr = impl_->students.remove(at);
    impl_->studentsDiff.processDeleted(ptr->id());
    return ptr;
}

std::map<Class::Index, Class::StudentPtr> Class::erase(const std::set<Index>& at)
{
    auto res = impl_->students.remove(at);
    for (const auto& p : res) {
        impl_->studentsDiff.processDeleted(p.second->id());
    }
    return res;
}

size_t Class::studentsCount() const
{
    return impl_->students.size();
}

bool Class::areStudentsModified() const
{
    return impl_->areStudentsModified();
}

// subjects plan

const SubjectsPlanPtr& Class::subjectsPlan() const
{
    return impl_->subjectsPlan;
}

void Class::setSubjectsPlan(const SubjectsPlanPtr& subjectsPlan)
{
    if (byPointerCmp(subjectsPlan, impl_->subjectsPlan)) {
        return;
    }
    impl_->subjectsPlan = subjectsPlan;
    impl_->data->subjectsPlanId = subjectsPlan
        ? subjectsPlan->id()
        : ID::emptyID();
    impl_->calcModifiedSubjectsPlan();
}

bool Class::isSubjectsPlanModified() const
{
    return impl_->isModified().subjectsPlan;
}


// TODO test saving all students must have Existing state
void Class::save()
{
    ATT_REQUIRE(!impl_->isDeleted, "Cannot save deleted class, id " << impl_->id);
    impl_->originalData.reset(new Data(*impl_->data));

    const size_t size = impl_->students.size();
    for (size_t i = 0; i < size; ++i) {
        auto& s = student(i);
        s.save();
    }
    impl_->resetStudentsDiff();
    impl_->resetModified();
}

} // namespace attestate

