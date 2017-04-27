#include <attestate/validate.h>

#include <attestate/serialize.h>
#include <attestate/student.h>
#include <attestate/grades.h>

namespace attestate {
namespace validation {

boost::optional<StudentErrors> validate(const Class& c, Class::Index studentIdx)
{
    using namespace cfg::tags;

    const Student& s = c.student(studentIdx);
    if (s.state() == State::Deleted) {
        return boost::none;
    }

    PropertyErrors v;
    if (s.familyName().isEmpty()) {
        v.emplace(property::FAMILY_NAME, ValueError::Empty);
    }
    if (s.name().isEmpty()) {
        v.emplace(property::NAME, ValueError::Empty);
    }
    if (s.parentalName().isEmpty()) {
        v.emplace(property::PARENTAL_NAME, ValueError::Empty);
    }

    if (s.attestateId().isEmpty()) {
        v.emplace(property::ATTESTATE_ID, ValueError::Empty);
    }

    if (s.birthDate().isNull() || !s.birthDate().isValid()) {
        v.emplace(property::BIRTH_DATE, ValueError::Invalid);
    }

    if (!s.issueDate()) {
        if (!c.issueDate()) {
            v.emplace(property::ISSUE_DATE, ValueError::Invalid);
        }
    } else if (s.issueDate()->isNull() || !s.issueDate()->isValid()) {
        v.emplace(property::ISSUE_DATE, ValueError::Invalid);
    }

    if (!s.graduationYear()) {
        if (!c.graduationYear()) {
            v.emplace(property::GRADUATION_YEAR, ValueError::Empty);
        }
    }

    GradeErrors ge;
    const auto& sp = c.subjectsPlan();
    for (size_t i = 0; sp && i < sp->subjectsCount(); ++i) {
        const auto subjId = sp->at(i).id();
        auto gv = s.grades().value(subjId);
        if (!gv || gv->isEmpty()) {
            ge.emplace(subjId, ValueError::Empty);
        } else if (!grades::isValid(*gv)) {
            ge.emplace(subjId, ValueError::Invalid);
        }
    }

    if (v.empty() && ge.empty()) {
        return boost::none;
    }

    return StudentErrors{std::move(v), std::move(ge)};
}

boost::optional<ClassErrors> validate(const Class& c)
{
    PropertyErrors v;
    if (!c.issueDate() || !c.issueDate()->isValid()) {
        v.emplace(cfg::tags::property::ISSUE_DATE, ValueError::Invalid);
    }

    if (!c.graduationYear()) {
        v.emplace(cfg::tags::property::GRADUATION_YEAR, ValueError::Empty);
    }

    StudentErrorsMap se;
    for (size_t i = 0 ; i < c.studentsCount(); ++i) {
        auto sres = validate(c, i);
        if (sres) {
            se.emplace(c.student(i).id(), std::move(*sres));
        }
    }

    if (v.empty() && se.empty()) {
        return boost::none;
    }

    return ClassErrors{std::move(v), std::move(se)};
}

} // namespace validation
} // namespace attestate
