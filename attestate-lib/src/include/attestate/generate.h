#pragma once

#include <attestate/class.h>
#include <doctpl/template.h>

namespace attestate {

namespace cfg {

QString attestateDateFormat();

namespace tags {
namespace fields {

// text

const QString ATTESTATE_ID = QString::fromUtf8("Номер_аттестата");
const QString FAMILY_NAME_1 = QString::fromUtf8("Фамилия_1");
const QString FAMILY_NAME_2 = QString::fromUtf8("Фамилия_2");
const QString NAME_PNAME_1 = QString::fromUtf8("Имя_Отчество_1");
const QString NAME_PNAME_2 = QString::fromUtf8("Имя_Отчество_2");
const QString BIRTH_DATE = QString::fromUtf8("Дата_рождения");
const QString ISSUE_DATE_1 = QString::fromUtf8("Дата_выдачи_1");
const QString ISSUE_DATE_2 = QString::fromUtf8("Дата_выдачи_2");
const QString GRADUATION_YEAR = QString::fromUtf8("Год_выдачи");

// table

const QString GRADES_1 = QString::fromUtf8("Оценки_1");
const QString GRADES_2 = QString::fromUtf8("Оценки_2");

const QString AUX = QString::fromUtf8("Дополнительные");

} // namespace fields
} // namespace tags
} // namespace cfg

namespace gen {

void fillTemplate(
    const Class& cls,
    Class::Index studentIndex,
    doctpl::Template& doc);

} // namespace gen
} // namespace attestate
