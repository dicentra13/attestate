#include <attestate/serialize.h>

#include "magic_strings.h"

#include <attestate/subjects.h>
#include <attestate/student.h>
#include <attestate/grades.h>

#include <QFile>
#include <QTextStream>

#include <map>

namespace attestate {

namespace cfg {
namespace header {

size_t minSectionsCount() { return 6; }

size_t sectionPos(const QString& sectionName)
{
    static const std::map<QString, size_t> s_sectMap = {
        {tags::ATTESTATE_ID, 0},
        {tags::ISSUE_DATE, 1},
        {tags::FAMILY_NAME, 2},
        {tags::NAME, 3},
        {tags::PARENTAL_NAME, 4},
        {tags::BIRTH_DATE, 5}
    };

    auto it = s_sectMap.find(sectionName);
    ATT_REQUIRE(
        it != s_sectMap.end(),
        "Section not found: " << sectionName.toStdString());

    return it->second;
}

} // namespace header
} // namespace cfg

using namespace cfg::header;

namespace csv {

namespace {

SubjectsPlanPtr parseHeader(
    const QString& header, const Params& params, const char* fn)
{
    QStringList separatedHeader = header.split(params.delimiter);
    ATT_REQUIRE(
        (size_t)separatedHeader.size() >= minSectionsCount(),
        "Too few columns in header: " << separatedHeader.size()
            << " in csv file: " << fn);

    SubjectsPlanPtr subjectsPlan = std::make_shared<SubjectsPlan>(ID::gen());
    auto it = separatedHeader.begin();
    std::advance(it, minSectionsCount());
    for ( ; it != separatedHeader.end(); ++it) {
        SubjectPtr s = std::make_shared<Subject>(ID::gen(), *it);
        subjectsPlan->append(s);
    }

    return subjectsPlan;
}

Class::StudentPtr parseStudent(
    const QString& line, const Params& params,
    const char* fn, size_t lineNo,
    const SubjectsPlanPtr& subjectsPlan)
{
    QStringList separated = line.split(params.delimiter);
    ATT_REQUIRE(
        (size_t)separated.size() == minSectionsCount() + subjectsPlan->subjectsCount(),
        "Columns count mismatch: " << lineNo << " line in csv file: " << fn);

    for (auto& s : separated) {
        s = s.trimmed();
    }

    AttestateId id = separated.at(sectionPos(tags::ATTESTATE_ID));
    OptionalDate issueDate = QDate::fromString(
        separated.at(sectionPos(tags::ISSUE_DATE)),
        params.dateFormat);
    if (!issueDate->isValid()) {
        issueDate.reset();
    }
    QDate birthDate = QDate::fromString(
        separated.at(sectionPos(tags::BIRTH_DATE)),
        params.dateFormat);

    QStringList::const_iterator markIt = separated.begin();
    std::advance(markIt, minSectionsCount());
    SubjectsGrades grades;
    for (size_t subj = 0; markIt != separated.end(); ++subj, ++markIt) {
        grades.setValue(subjectsPlan->at(subj).id(), *markIt);
    }

    return Class::StudentPtr(new Student(
        ID::gen(),
        separated.at(sectionPos(tags::FAMILY_NAME)),
        separated.at(sectionPos(tags::NAME)),
        separated.at(sectionPos(tags::PARENTAL_NAME)),
        birthDate,
        grades,
        /* graduationYear */ boost::none,
        id,
        issueDate));
}

} // namespace

std::unique_ptr<Class> read(const QString& filename, const Params& params)
{
    const char* fn = filename.toStdString().c_str();
    QFile classData(filename);
    ATT_REQUIRE(
        classData.open(QIODevice::ReadOnly),
        "Could not open file " << fn);

    QTextStream stream(&classData);

    QString line = stream.readLine();
    ATT_REQUIRE(!line.isNull(), "No header in csv file: " << fn);

    SubjectsPlanPtr subjectsPlan = parseHeader(line, params, fn);

    std::vector<Class::StudentPtr> students;
    typedef std::map<QDate, size_t> Dates;
    Dates dates;
    size_t lineNo = 1;
    while (!(line = stream.readLine()).isNull()) {
        students.push_back(parseStudent(line, params, fn, lineNo, subjectsPlan));
        if (students.back()->issueDate()) {
            ++dates[*students.back()->issueDate()];
        }
        ++lineNo;
    }

    typedef Dates::value_type Dp;
    auto dateIt = std::max_element(
        dates.begin(), dates.end(),
        [] (const Dp& l, const Dp& r) { return l.second < r.second; });

    OptionalDate issueDate = dateIt == dates.end()
        ? OptionalDate()
        : OptionalDate(dateIt->first);
    if (issueDate) {
        for (auto& sp : students) {
            if (sp->issueDate() == issueDate) {
                sp->setIssueDate(OptionalDate());
                sp->save();
            }
        }
    }

    return std::unique_ptr<Class>(new Class(
        ID::gen(),
        "",
        OptionalYear(),
        issueDate,
        std::move(students),
        subjectsPlan));
}

namespace {

void writeHeader(QTextStream& stream, const SubjectsPlanPtr& subjectsPlan,
    const Params& params)
{
    stream << tags::ATTESTATE_ID
        << params.delimiter << tags::ISSUE_DATE
        << params.delimiter << tags::FAMILY_NAME
        << params.delimiter << tags::NAME
        << params.delimiter << tags::PARENTAL_NAME
        << params.delimiter << tags::BIRTH_DATE;

    for (size_t i = 0; subjectsPlan && i < subjectsPlan->subjectsCount(); ++i) {
        stream << params.delimiter << subjectsPlan->at(i).name();
    }

    stream << "\n";
}

void writeStudent(
    QTextStream& stream,
    const Class& c, Class::Index si,
    const Params& params)
{
    const Student& s = c.student(si);
    stream << s.attestateId()
        << params.delimiter
        << (s.issueDate()
            ? s.issueDate()->toString(params.dateFormat)
            : c.issueDate()
                ? c.issueDate()->toString(params.dateFormat)
                : QString())
        << params.delimiter << s.familyName()
        << params.delimiter << s.name()
        << params.delimiter << s.parentalName()
        << params.delimiter << s.birthDate().toString(params.dateFormat);

    const auto& sp = c.subjectsPlan();
    const auto& grades = s.grades();

    for (size_t i = 0; sp && i < sp->subjectsCount(); ++i) {
        const auto& v = grades.value(sp->at(i).id());
        stream << params.delimiter << (v ? *v : QString());
    }

    stream << "\n";
}

} // namespace

void write(const Class& cls, const QString& filename, const Params& params)
{
    const char* fn = filename.toStdString().c_str();
    QFile classData(filename);
    ATT_REQUIRE(
        classData.open(QIODevice::WriteOnly),
        "Could not open file " << fn);

    QTextStream stream(&classData);

    writeHeader(stream, cls.subjectsPlan(), params);
    for (size_t i = 0; i < cls.studentsCount(); ++i) {
        writeStudent(stream, cls, i, params);
    }

    classData.close();
}

} // namespace csv
} // namespace attestate
