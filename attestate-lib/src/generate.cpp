#include <attestate/generate.h>

#include "magic_strings.h"

#include <attestate/student.h>
#include <attestate/grades.h>

#include <doctpl/table_field.h>
#include <doctpl/text_field.h>

#include <sstream>

namespace attestate {

namespace cfg {

QString attestateDateFormat() { return QString::fromUtf8("dd  MMMM  yyyy  года"); }

} // namespace cfg

namespace gen {

namespace {

QString formatYear(int year)
{
    std::ostringstream os;
    os << year;
    return QString(os.str().c_str());
}

QString formatDate(const QDate& date)
{
    return date.toString(cfg::attestateDateFormat());
}

void fillCommonInfo(const Student& s, const Class& c, doctpl::Template& doc)
{
    auto tf = doc.fields()->as<doctpl::TextField>();

    tf->find(cfg::tags::fields::FAMILY_NAME_1)->setText(s.familyName());
    tf->find(cfg::tags::fields::FAMILY_NAME_2)->setText(s.familyName());
    QString n = s.name() + " " + s.parentalName();
    tf->find(cfg::tags::fields::NAME_PNAME_1)->setText(n);
    tf->find(cfg::tags::fields::NAME_PNAME_2)->setText(n);
    tf->find(cfg::tags::fields::BIRTH_DATE)->setText(formatDate(s.birthDate()));

    QString gradYear = s.graduationYear()
        ? formatYear(*s.graduationYear())
        : c.graduationYear() ? formatYear(*c.graduationYear()) : QString();

    tf->find(cfg::tags::fields::GRADUATION_YEAR)->setText(gradYear);

    QString issueDate = s.issueDate()
        ? formatDate(*s.issueDate())
        : c.issueDate() ? formatDate(*c.issueDate()) : QString();

    tf->find(cfg::tags::fields::ISSUE_DATE_1)->setText(issueDate);
    tf->find(cfg::tags::fields::ISSUE_DATE_2)->setText(issueDate);

    tf->find(cfg::tags::fields::ATTESTATE_ID)->setText(s.attestateId());
}

void fillGrades(const Student& s, const Class& c, doctpl::Template& doc)
{
    std::string n = (s.familyName() + " " + s.name()).toStdString();

    auto tbf = doc.fields()->as<doctpl::TableField>();

    auto marked1 = tbf->find(cfg::tags::fields::GRADES_1);
    auto marked2 = tbf->find(cfg::tags::fields::GRADES_2);

    auto aux = tbf->find(cfg::tags::fields::AUX);

    marked1->clear();
    marked2->clear();
    aux->clear();

    size_t markedCounter = 0;
    size_t auxCounter = 0;

    auto markedIt = [=] (size_t markedCounter)
        -> std::pair<doctpl::TableField*, size_t> // field, row
    {
        auto c = markedCounter;
        for (auto f : {marked1, marked2}) {
            if (c < f->rowsCount()) {
                return {f, c};
            }
            c -= f->rowsCount();
        }
        ATT_ERROR("Too many marked subjects count: " << markedCounter);
    };

    const auto& sp = c.subjectsPlan();

    for (size_t j = 0; sp && j < sp->subjectsCount(); ++j) {
        const auto& subj = sp->at(j);
        auto gv = s.grades().value(subj.id());
        if (!gv || !grades::isValid(*gv)) {
            continue;
        }
        //ATT_REQUIRE(
        //    gv,
        //    "Grade not set for subject: " << subj.name().toStdString() << ", student: " << n);
        auto value = *gv;
        //ATT_REQUIRE(
        //    grades::isValid(value),
        //    "Invalid grade value: " << value.toStdString()
        //        << ", subject: " << subj.name().toStdString()
        //        << ", student: " << n);

        if (grades::type(value) == grades::Type::HasRepresentation) {
            auto it = markedIt(markedCounter);
            it.first->setText(it.second, 0, subj.name());
            it.first->setText(it.second, 1, grades::representation(value));
            ++markedCounter;
        } else if (grades::type(value) == grades::Type::Auxilliary) {
            ATT_REQUIRE(
                auxCounter < aux->rowsCount(),
                "Too many auxilliary subjects count: " << auxCounter);
            aux->setText(auxCounter++, 0, subj.name());
        }
    }
}

} // namespace

void fillTemplate(
    const Class& cls,
    Class::Index studentIndex,
    doctpl::Template& doc)
{
    const auto& s = cls.student(studentIndex);

    fillCommonInfo(s, cls, doc);
    fillGrades(s, cls, doc);
}

} // namespace gen
} // namespace attestate
