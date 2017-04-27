#include <attestate/student.h>

#include "helpers.h"

#include <attestate/grades.h>
#include <attestate/exception.h>

namespace attestate {

// Impl

namespace {

struct Data {
    DataString familyName;
    DataString name;
    DataString parentalName;
    QDate birthDate;
    SubjectsGrades grades;
    OptionalYear graduationYear;
    AttestateId attestateId;
    OptionalDate issueDate;
};

struct IsModified {
    explicit IsModified(bool val)
        : familyName(val)
        , name(val)
        , parentalName(val)
        , birthDate(val)
        , graduationYear(val)
        , attestateId(val)
        , issueDate(val)
    {}

    bool operator () () const
    {
        return familyName || name || parentalName ||
            birthDate || graduationYear ||
            attestateId || issueDate;
    }

    bool familyName;
    bool name;
    bool parentalName;
    bool birthDate;
    bool graduationYear;
    bool attestateId;
    bool issueDate;
};

typedef std::unique_ptr<Data> DataPtr;

} // namespace

class Student::Impl {
public:
    explicit Impl(const ID& id)
        : id(id)
        , data(new Data{"", "", "", QDate(), std::move(SubjectsGrades()), boost::none, "", nullptr})
        , originalData(nullptr)
        , isDeleted(false)
        , isModified_(true)
    {}

    Impl(
            const ID& id,
            const DataString& fName,
            const DataString& name,
            const DataString& pName,
            const QDate& birthDate = {},
            const SubjectsGrades& grades = {},
            OptionalYear graduationYear = boost::none,
            const AttestateId& attestateId = {},
            const OptionalDate& issueDate = boost::none)
        : id(id)
        , data(new Data{fName, name, pName, birthDate, grades, graduationYear,
            attestateId, issueDate})
        , originalData(new Data(*data))
        , isDeleted(false)
        , isModified_(false)
    {}

    void calcModifiedFamilyName()
    {
        isModified_.familyName = !originalData ||
            data->familyName != originalData->familyName;
    }

    void calcModifiedName()
    {
        isModified_.name = !originalData ||
            data->name != originalData->name;
    }

    void calcModifiedParentalName()
    {
        isModified_.parentalName = !originalData ||
            data->parentalName != originalData->parentalName;
    }

    void calcModifiedBirthDate()
    {
        isModified_.birthDate = !originalData ||
            data->birthDate != originalData->birthDate;
    }

    void calcModifiedGraduationYear()
    {
        isModified_.graduationYear = !originalData ||
            data->graduationYear != originalData->graduationYear;
    }

    void calcModifiedAttestateId()
    {
        isModified_.attestateId = !originalData ||
            data->attestateId != originalData->attestateId;
    }

    void calcModifiedIssueDate()
    {
        isModified_.issueDate = !originalData ||
            data->issueDate != originalData->issueDate;
    }

    bool areGradesModified() const
    {
        return !originalData || !data->grades.diff(originalData->grades).empty();
    }

    const IsModified& isModified() const { return isModified_; }

    bool isModifiedState() const
    {
        return isModified_() || areGradesModified();
    }

    void resetModified() { isModified_ = IsModified(originalData ? false : true); }

    ID id;
    DataPtr data;
    DataPtr originalData;
    bool isDeleted;

private:
    IsModified isModified_;
};


// Student

Student::Student(const ID& id)
    : impl_(new Impl(id))
{}

Student::Student(
        const ID& id,
        const DataString& fName,
        const DataString& name,
        const DataString& pName,
        const QDate& birthDate,
        const SubjectsGrades& grades,
        OptionalYear graduationYear,
        const AttestateId& attestateId,
        const OptionalDate& issueDate)
    : impl_(new Impl(id, fName, name, pName, birthDate, grades, graduationYear,
        attestateId, issueDate))
{}

Student::Student(Student&&) = default;
Student& Student::operator = (Student&&) = default;

Student::~Student()
{}

const ID& Student::id() const { return impl_->id; }

void Student::setDBID(const DBID& dbid)
{
    impl_->id = ID::setDBID(impl_->id, dbid);
}

State Student::state() const
{
    if (impl_->isDeleted) {
        return State::Deleted;
    }
    if (!impl_->originalData) {
        return State::New;
    }

    return impl_->isModifiedState() ? State::Modified : State::Existing;
}

void Student::setDeleted(bool isDeleted)
{
    impl_->isDeleted = isDeleted;
}


const DataString& Student::familyName() const { return impl_->data->familyName; }

void Student::setFamilyName(const DataString& familyName)
{
    impl_->data->familyName = familyName;
    impl_->calcModifiedFamilyName();
}

bool Student::isFamilyNameModified() const { return impl_->isModified().familyName; }


const DataString& Student::name() const { return impl_->data->name; }

void Student::setName(const DataString& name)
{
    impl_->data->name = name;
    impl_->calcModifiedName();
}

bool Student::isNameModified() const { return impl_->isModified().name; }


const DataString& Student::parentalName() const { return impl_->data->parentalName; }

void Student::setParentalName(const DataString& parentalName)
{
    impl_->data->parentalName = parentalName;
    impl_->calcModifiedParentalName();
}

bool Student::isParentalNameModified() const { return impl_->isModified().parentalName; }


const QDate& Student::birthDate() const { return impl_->data->birthDate; }

void Student::setBirthDate(const QDate& birthDate)
{
    impl_->data->birthDate = birthDate;
    impl_->calcModifiedBirthDate();
}

bool Student::isBirthDateModified() const { return impl_->isModified().birthDate; }

bool Student::isPersonalInfoModified() const
{
    return isFamilyNameModified() ||
        isNameModified() ||
        isParentalNameModified() ||
        isBirthDateModified();
}

// subject grades

const SubjectsGrades& Student::grades() const { return impl_->data->grades; }

SubjectsGrades& Student::grades() { return impl_->data->grades; }

bool Student::areGradesModified() const
{
    return impl_->areGradesModified();
}

// graduation year

OptionalYear Student::graduationYear() const
{
    return impl_->data->graduationYear;
}

void Student::setGraduationYear(OptionalYear graduationYear)
{
    impl_->data->graduationYear = graduationYear;
    impl_->calcModifiedGraduationYear();
}

bool Student::isGraduationYearModified() const
{
    return impl_->isModified().graduationYear;
}


// attestate data

const AttestateId& Student::attestateId() const { return impl_->data->attestateId; }

void Student::setAttestateId(const AttestateId& attestateId)
{
    impl_->data->attestateId = attestateId;
    impl_->calcModifiedAttestateId();
}

bool Student::isAttestateIdModified() const {return impl_->isModified().attestateId; }

const OptionalDate& Student::issueDate() const
{
    return impl_->data->issueDate;
}

void Student::setIssueDate(const OptionalDate& issueDate)
{
    impl_->data->issueDate = issueDate;
    impl_->calcModifiedIssueDate();
}

bool Student::isIssueDateModified() const { return impl_->isModified().issueDate; }

bool Student::isModified() const { return state() == State::Modified; }

void Student::save()
{
    ATT_REQUIRE(!impl_->isDeleted, "Cannot save deleted student, id " << impl_->id);
    impl_->originalData.reset(new Data(*impl_->data));
    impl_->resetModified();
}

} // namespace attestate

