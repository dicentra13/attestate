#include <attestate/grades.h>

#include "diff.h"

#include <attestate/exception.h>

#include <map>
#include <set>
#include <algorithm>

namespace attestate {

namespace grades {

namespace {

const ValuesSet s_reprValues = { "5", "4", "3" };

const ValuesSet s_auxValues = {
    QString::fromUtf8("д"), QString::fromUtf8("Д"), "+"
};

const ValuesSet s_noneValues = {
    QString::fromUtf8("н"), QString::fromUtf8("Н"), "-"
};

} // namespace

// TODO maybe customized ? DB?
ValuesSet validValues()
{
    ValuesSet all(s_reprValues.begin(), s_reprValues.end());
    all.insert(s_auxValues.begin(), s_auxValues.end());
    all.insert(s_noneValues.begin(), s_noneValues.end());
    return all;
}

bool isValid(const Value& value)
{
    return validValues().count(value) != 0;
}

//FIXME tests

namespace {

typedef std::map<Value, Representation> RepresentationMap;

const RepresentationMap& representationsImpl()
{
    static const RepresentationMap s_repr = {
        {"5", QString::fromUtf8("5 (отлично)")},
        {"4", QString::fromUtf8("4 (хорошо)")},
        {"3", QString::fromUtf8("3 (удовл.)")}
    };

    return s_repr;
}

} // namespace

const Representation& representation(const Value& value)
{
    return representationsImpl().at(value);
}

Type type(const Value& value)
{
    Type res;
    if (s_reprValues.count(value)) {
        res = Type::HasRepresentation;
    } else if (s_auxValues.count(value)) {
        res = Type::Auxilliary;
    } else if (s_noneValues.count(value)) {
        res =  Type::None;
    } else {
        ATT_ERROR("Invalid grade value: " << value.toStdString());
    }
    return res;
}

} // namespace grades


// class SubjectsGrades

class SubjectsGrades::Impl {
public:
    std::map<ID, grades::Value> values; // subject id -> grade value
};


SubjectsGrades::SubjectsGrades()
    : impl_(new Impl())
{}

SubjectsGrades::SubjectsGrades(const std::map<ID, grades::Value>& values)
    : impl_(new Impl{values})
{}

SubjectsGrades::SubjectsGrades(const SubjectsGrades& o)
    : impl_(new Impl(*o.impl_))
{}

SubjectsGrades& SubjectsGrades::operator = (const SubjectsGrades& o)
{
    *impl_ = *o.impl_;
    return *this;
}

SubjectsGrades::SubjectsGrades(SubjectsGrades&& o)
    : impl_(std::move(o.impl_))
{}

SubjectsGrades& SubjectsGrades::operator = (SubjectsGrades&& o)
{
    impl_ = std::move(o.impl_);
    return *this;
}

SubjectsGrades::~SubjectsGrades()
{}


grades::OptionalValue SubjectsGrades::value(const ID& subjectId) const
{
    auto it = impl_->values.find(subjectId);
    if (it == impl_->values.end()) {
        return boost::none;
    }
    return it->second;
}

void SubjectsGrades::setValue(
    const ID& subjectId, const grades::OptionalValue& value)
{
    if (!value || value->isEmpty()) {
        auto it = impl_->values.find(subjectId);
        ATT_REQUIRE(it != impl_->values.end(), "No subject with id " << subjectId); // TODO test
        impl_->values.erase(it);
        return;
    }
    auto res = impl_->values.insert({subjectId, *value});
    if (!res.second) {
        res.first->second = *value;
    }
}

SubjectsGrades::Diff
SubjectsGrades::diff(const SubjectsGrades& otherGrades) const
{
    return attestate::Diff<ID, grades::Value>::compute(
        impl_->values, otherGrades.impl_->values);
}

void SubjectsGrades::applyDiff(const Diff& diff)
{
    attestate::Diff<ID, grades::Value>::apply(impl_->values, diff);
}

std::list<grades::OptionalValue>
SubjectsGrades::values(const std::list<ID>& subjectIds) const
{
    std::list<grades::OptionalValue> res;
    for (auto id : subjectIds) {
        auto it = impl_->values.find(id);
        if (it == impl_->values.end()) {
            res.push_back(boost::none);
        } else {
            res.push_back(it->second);
        }
    }
    return res;
}

namespace grades {

SubjectsGrades::Diff reverseDiff(const SubjectsGrades::Diff& diff)
{
    return attestate::Diff<ID, grades::Value>::reverse(diff);
}

} // namespace grades

} // namespace attestate

