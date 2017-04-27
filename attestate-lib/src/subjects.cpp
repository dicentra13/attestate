#include "include/attestate/subjects.h"

#include "unique_vector.h"
#include "diff.h"

#include <vector>
#include <algorithm>

namespace attestate {

// Subject

namespace {

struct SubjectData {
    DataString name;
    DataString shortenedName;
};

typedef std::unique_ptr<SubjectData> SubjectDataPtr;

} // namespace

class Subject::Impl {
public:
    explicit Impl(const ID& id)
        : id(id)
        , data(new SubjectData)
        , originalData (nullptr)
        , isModified(true)
        , isDeleted(false)
    {}

    Impl(
            const ID& id,
            const DataString& name,
            const DataString& shortenedName)
        : id(id)
        , data(new SubjectData{name, shortenedName})
        , originalData (new SubjectData(*data))
        , isModified(false)
        , isDeleted(false)
    {}

    ID id;
    SubjectDataPtr data;
    SubjectDataPtr originalData;

    struct IsModified {
        explicit IsModified(bool val)
            : name(val)
            , shortenedName(val)
        {}

        void reset(bool val)
        {
            name = val;
            shortenedName = val;
        }

        bool operator () () const { return name || shortenedName; }

        bool name;
        bool shortenedName;
    };

    IsModified isModified;
    bool isDeleted;
};


Subject::Subject(const ID& id)
    : impl_(new Impl(id))
{}

Subject::Subject(
        const ID& id,
        const DataString& name,
        const DataString& shortenedName)
    : impl_(new Impl(id, name, shortenedName))
{}

Subject::Subject(Subject&&) = default;
Subject& Subject::operator = (Subject&&) = default;

Subject::~Subject() {}

const ID& Subject::id() const { return impl_->id; }

void Subject::setDBID(const DBID& dbid)
{
    impl_->id = ID::setDBID(impl_->id, dbid);
}

State Subject::state() const
{
    if (impl_->isDeleted) {
        return State::Deleted;
    }
    if (!impl_->originalData) {
        return State::New;
    }

    return impl_->isModified() ? State::Modified : State::Existing;
}

void Subject::setDeleted(bool isDeleted) { impl_->isDeleted = isDeleted; }

const DataString& Subject::name() const { return impl_->data->name; }

void Subject::setName(const DataString& name)
{
    if (name != impl_->data->name) {
        impl_->data->name = name;
        impl_->isModified.name =
            !impl_->originalData || name != impl_->originalData->name;
    }
}

bool Subject::isNameModified() const
{
    return impl_->isModified.name;
}

const DataString&
Subject::shortenedName() const { return impl_->data->shortenedName; }

void Subject::setShortenedName(const DataString& name)
{
    if (name != impl_->data->shortenedName) {
        impl_->data->shortenedName = name;
        impl_->isModified.shortenedName =
            !impl_->originalData || name != impl_->originalData->shortenedName;
    }
}

bool Subject::isShortenedNameModified() const
{
    return impl_->isModified.shortenedName;
}

void Subject::save()
{
    ATT_REQUIRE(!impl_->isDeleted, "Cannot save deleted subject, id " << impl_->id);
    impl_->originalData.reset(new SubjectData(*impl_->data));
    impl_->isModified.reset(false);
}


// SubjectsPlan

// Impl

namespace {

struct SubjectID {
    explicit SubjectID(const SubjectPtr& s) : id_(s->id()) {}
    explicit SubjectID(const ID& id) : id_(id) {}

    const ID& operator () () const { return id_; }

    bool operator < (const SubjectID& o) const { return id_ < o.id_; }

private:
    ID id_;
};

std::ostream& operator << (std::ostream& os, const SubjectID& k)
{
    os << k();
    return os;
}

typedef UniqueVector<SubjectPtr, SubjectID> SubjectsVector;

struct SubjectsPlanData {
    DataString name;
    SubjectsVector subjects;
};

typedef std::unique_ptr<SubjectsPlanData> SubjectsPlanDataPtr;


typedef attestate::Diff<
    SubjectPtr,
    SubjectsPlan::Index,
    SubjectsPlan::SubjectPtrCompare>
DiffT;

DiffT::ValuesType buildValues(const SubjectsVector& vec)
{
    DiffT::ValuesType v;
    const auto size = vec.size();
    for (size_t i = 0; i < size; ++i) {
        v[vec.at(i)] = i;
    }
    return v;
}

DiffT::DiffType buildDiff(const SubjectsVector& v, const SubjectsVector& vo)
{
    return DiffT::compute(buildValues(v), buildValues(vo));
}

} // namespace

class SubjectsPlan::Impl {
public:
    explicit Impl(const ID& id)
        : id(id)
        , data(new SubjectsPlanData{"", {}})
        , originalData(nullptr)
        , isNameModified(true)
        , isDeleted(false)
    {}

    Impl(
            const ID& id,
            const DataString& name,
            SubjectPtrVector subjects)
        : id(id)
        , data(new SubjectsPlanData{name, {}})
        , originalData(new SubjectsPlanData(*data))
        , isNameModified(false)
        , isDeleted(false)
    {
        for (const auto& s : subjects) {
            ATT_ASSERT(s);
        }
        data->subjects = SubjectsVector(std::move(subjects));
        originalData->subjects = data->subjects;
    }

    bool areSubjectsModified() const
    {
        return !originalData ||
            !buildDiff(data->subjects, originalData->subjects).empty();
    }

    bool isModified() const { return isNameModified || areSubjectsModified(); }

    ID id;
    SubjectsPlanDataPtr data;
    SubjectsPlanDataPtr originalData;

    bool isNameModified;
    bool isDeleted;
};


SubjectsPlan::SubjectsPlan(const ID& id)
    : impl_(new Impl(id))
{}

SubjectsPlan::SubjectsPlan(
        const ID& id,
        const DataString& name,
        SubjectPtrVector subjects)
    : impl_(new Impl(id, name, std::move(subjects)))
{}

SubjectsPlan::SubjectsPlan(SubjectsPlan&&) = default;
SubjectsPlan& SubjectsPlan::operator = (SubjectsPlan&&) = default;

SubjectsPlan::~SubjectsPlan()
{}

const ID& SubjectsPlan::id() const { return impl_->id; }

void SubjectsPlan::setDBID(const DBID& dbid)
{
    impl_->id = ID::setDBID(impl_->id, dbid);
}

State SubjectsPlan::state() const
{
    if (impl_->isDeleted) {
        return State::Deleted;
    }
    if (!impl_->originalData) {
        return State::New;
    }

    return impl_->isModified() ? State::Modified : State::Existing;
}

void SubjectsPlan::setDeleted(bool isDeleted) { impl_->isDeleted = isDeleted; }

const DataString& SubjectsPlan::name() const { return impl_->data->name; }

void SubjectsPlan::setName(const DataString& name)
{
    if (name != impl_->data->name) {
        impl_->data->name = name;
        impl_->isNameModified = !impl_->originalData ||
            impl_->originalData->name != name;
    }
}

bool SubjectsPlan::isNameModified() const
{
    return impl_->isNameModified;
}

void SubjectsPlan::insert(const SubjectPtr& subject, Index at)
{
    ATT_ASSERT(subject);
    impl_->data->subjects.insert(subject, at);
}

void SubjectsPlan::append(const SubjectPtr& subject)
{
    ATT_ASSERT(subject);
    impl_->data->subjects.append(subject);
}

SubjectPtr SubjectsPlan::erase(Index at)
{
    return impl_->data->subjects.remove(at);
}

std::map<SubjectsPlan::Index, SubjectPtr>
SubjectsPlan::erase(const std::set<Index>& at)
{
    return impl_->data->subjects.remove(at);
}

void SubjectsPlan::move(Index from, Index to)
{
    impl_->data->subjects.move(from, to);
}

bool SubjectsPlan::areSubjectsModified() const
{
    return impl_->areSubjectsModified();
}

bool SubjectsPlan::isModified() const { return state() == State::Modified; }

const Subject& SubjectsPlan::at(Index at) const
{
    const auto& ptr = impl_->data->subjects.at(at);
    ATT_ASSERT(ptr);
    return *ptr;
}

bool SubjectsPlan::hasSubject(const ID& id) const
{
    return impl_->data->subjects.contains(SubjectID(id));
}

size_t SubjectsPlan::subjectsCount() const
{
    return impl_->data->subjects.size();
}

SubjectsPlan::SubjectIdVector SubjectsPlan::subjectIds() const
{
    SubjectIdVector res;
    const auto size = impl_->data->subjects.size();
    res.reserve(size);
    for (Index i = 0; i < size; ++i) {
        res.push_back(impl_->data->subjects.at(i)->id());
    }
    return res;
}

bool SubjectsPlan::operator == (const SubjectsPlan& other) const
{
    return impl_->id == other.impl_->id;
}

void SubjectsPlan::save()
{
    ATT_REQUIRE(!impl_->isDeleted, "Cannot save deleted subjects plan, id " << impl_->id);
    impl_->originalData.reset(new SubjectsPlanData(*impl_->data));
    impl_->isNameModified = false;
}


bool SubjectsPlan::SubjectPtrCompare::operator () (
    const SubjectPtr& p1, const SubjectPtr& p2) const
{
    ATT_ASSERT(p1);
    ATT_ASSERT(p2);
    return p1->id() < p2->id();
}

SubjectsPlan::Diff SubjectsPlan::diff(const SubjectsPlan& o) const
{
    return buildDiff(impl_->data->subjects, o.impl_->data->subjects);
}

void SubjectsPlan::applyDiff(const Diff& diff)
{
    auto v = buildValues(impl_->data->subjects);
    for (const auto& p : diff) {
        ATT_ASSERT(p.first);
    }
    DiffT::apply(v, diff);
    std::map<Index, SubjectPtr> rv;
    for (const auto& p : v) {
        rv.insert({p.second, p.first});
    }
    size_t i = 0;
    SubjectsVector newSubjects;
    newSubjects.reserve(rv.size());
    for (const auto& p : rv) {
        ATT_REQUIRE(p.first == i, "Unexpected index " << size_t(p.first) << ", expected " << i);
        ++i;
        newSubjects.append(p.second);
    }
    impl_->data->subjects = std::move(newSubjects);
}

SubjectsPlan::Diff SubjectsPlan::reverseDiff(const SubjectsPlan::Diff& diff)
{
    return DiffT::reverse(diff);
}

} // namespace attestate

