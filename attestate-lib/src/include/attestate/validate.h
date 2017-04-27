#pragma once

#include <attestate/common.h>
#include <attestate/class.h>

#include <boost/optional.hpp>

#include <map>

namespace attestate {

namespace cfg {
namespace tags {
namespace property {

const QString FAMILY_NAME = "FAMILY_NAME";
const QString NAME = "NAME";
const QString PARENTAL_NAME = "PARENTAL_NAME";
const QString BIRTH_DATE = "BIRTH_DATE";

const QString ATTESTATE_ID = "ATTESTATE_ID";

const QString ISSUE_DATE = "ISSUE_DATE";
const QString GRADUATION_YEAR = "GRADUATION_YEAR";

} // namespace property
} // namespace tags
} // namespace cfg

namespace validation {

enum class ValueError { Empty, Invalid };

typedef std::map<QString, ValueError> PropertyErrors; // property tag -> error

typedef std::map<ID, ValueError> GradeErrors; // subject id -> error

struct StudentErrors {
    PropertyErrors propertyErrors;
    GradeErrors gradeErrors;
};

typedef std::map<ID, StudentErrors> StudentErrorsMap;

struct ClassErrors {
    PropertyErrors propertyErrors; // issue date, graduation year
    StudentErrorsMap studentErrors;
};


// if student is deleted boost::none will be returned
boost::optional<StudentErrors> validate(const Class& c, Class::Index studentIdx);

// deleted students are omitted
boost::optional<ClassErrors> validate(const Class& c);

} // namespace validation
} // namespace attestate
