#include "class_header_data.h"

#include <attestate/exception.h>

namespace cls {

void HeaderData::setData(Index section, const QVariant& data, int role)
{
    ATT_REQUIRE(
        section < count(),
        "Class header: section index out of range: " << section);

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        texts_[section] = data;
    } else if (role == Qt::TextAlignmentRole) {
        alignments_[section] = data;
    } else if (role == Qt::UserRole) {
        orientations_[section] = data;
    } else {
        ATT_ERROR("Unknown role: " << role);
    }
}

QVariant HeaderData::data(Index section, int role) const
{
    ATT_REQUIRE(
        section < count(),
        "Class header: section index out of range: " << section);

    QVariant res;
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        res = texts_[section];
    } else if (role == Qt::TextAlignmentRole) {
        res = alignments_[section];
    } else if (role == Qt::UserRole) {
        res = orientations_[section];
    }
    return res;
}

void HeaderData::insert(Index section)
{
    ATT_REQUIRE(
        section <= count(),
        "Class header: section index out of range: " << section);

    texts_.insert(texts_.begin() + section, QString());
    alignments_.insert(alignments_.begin() + section, 0);
    orientations_.insert(orientations_.begin() + section, 0);
}

void HeaderData::append()
{
    insert(count());
}

void HeaderData::remove(Index section)
{
    ATT_REQUIRE(
        section < count(),
        "Class header: section index out of range: " << section);

    texts_.erase(texts_.begin() + section);
    alignments_.erase(alignments_.begin() + section);
    orientations_.erase(orientations_.begin() + section);
}

} // namespace cls
