#pragma once

#include <attestate/class.h>

#include <QString>

#include <memory>

namespace attestate {

namespace cfg {
namespace header {

size_t minSectionsCount();

size_t sectionPos(const QString& sectionName);

namespace tags {

const QString ATTESTATE_ID = QString::fromUtf8("Номер аттестата");
const QString FAMILY_NAME = QString::fromUtf8("Фамилия");
const QString NAME = QString::fromUtf8("Имя");
const QString PARENTAL_NAME = QString::fromUtf8("Отчество");
const QString BIRTH_DATE = QString::fromUtf8("Дата рождения");
const QString ISSUE_DATE = QString::fromUtf8("Дата выдачи");

} // namespace tags

} // namespace header
} // namespace cfg

namespace csv {

struct Params {
    char delimiter;
    QString dateFormat;
};

std::unique_ptr<Class> read(const QString& filename, const Params& params);

void write(const Class& cls, const QString& filename, const Params& params);

} // namespace csv
} // namespace attestate
