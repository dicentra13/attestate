#pragma once

#include "common.h"

#include <QString>
#include <QChar>

#include <boost/optional.hpp>

#include <set>
#include <map>

namespace attestate {

namespace grades {

typedef QString Value;
typedef boost::optional<Value> OptionalValue;

typedef std::set<Value> ValuesSet;

ValuesSet validValues();
bool isValid(const Value& value);

enum class Type {HasRepresentation, Auxilliary, None};

Type type(const Value& value);

typedef QString Representation;

const Representation& representation(const Value& value);

} // namespace grades

class SubjectsGrades {
public:
    SubjectsGrades();
    SubjectsGrades(const std::map<ID, grades::Value>& values); // subject id -> grade value

    SubjectsGrades(const SubjectsGrades&);
    SubjectsGrades& operator = (const SubjectsGrades&);

    SubjectsGrades(SubjectsGrades&&);
    SubjectsGrades& operator = (SubjectsGrades&&);

    ~SubjectsGrades();


    // none if there is no grade for subject
    grades::OptionalValue value(const ID& subjectId) const;

    // setting empty value will remove subject
    void setValue(
        const ID& subjectId, const grades::OptionalValue& value);


    // subject ID -> {this grade, other grade}
    typedef std::map<ID, std::pair<grades::OptionalValue, grades::OptionalValue>>
        Diff;

    Diff diff(const SubjectsGrades& otherGrades) const;

    // checks that it is a valid diff of stored grades
    void applyDiff(const Diff& diff);

    // grades list according to subjects plan
    std::list<grades::OptionalValue> values(const std::list<ID>& subjectIds) const;

private:
    class Impl;

    std::unique_ptr<Impl> impl_;
}; // class SubjectsGrades


namespace grades {

SubjectsGrades::Diff reverseDiff(const SubjectsGrades::Diff& diff);

} // namespace grades

} // namespace attestate

