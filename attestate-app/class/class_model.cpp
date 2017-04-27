#include "class_model.h"

#include <attestate/exception.h>
#include <attestate/grades.h>
#include <attestate/serialize.h>

namespace cls {

namespace {

const HeaderData::Index COMMON_SECTIONS = 6;


} // namespace

Model::Model(const QString& csvfile, QObject* parent)
    : QAbstractTableModel(parent)
{
    class_ = attestate::csv::read(
        csvfile,
        attestate::csv::Params{';', QString("dd.MM.yyyy")});

    initHeaderData();

    emit dataChanged(
        index(0, 0),
        index(rowCount() - 1, columnCount() - 1));
}

Model::~Model()
{}

void Model::initHeaderData()
{
    for (size_t i = 0; i < COMMON_SECTIONS; ++i) {
        headerData_.append();
        headerData_.setData(
            headerData_.count() - 1,
            QVariant(Qt::AlignHCenter | Qt::AlignBottom),
            Qt::TextAlignmentRole);
        headerData_.setData(
            headerData_.count() - 1,
            Qt::Horizontal,
            Qt::UserRole);
    }
    headerData_.setData(0, QString::fromUtf8("№ аттестата"));
    headerData_.setData(1, QString::fromUtf8("Дата выдачи"));
    headerData_.setData(2, QString::fromUtf8("Фамилия"));
    headerData_.setData(3, QString::fromUtf8("Имя"));
    headerData_.setData(4, QString::fromUtf8("Отчество"));
    headerData_.setData(5, QString::fromUtf8("Дата рождения"));

    const auto& subjectsPlan = class_->subjectsPlan();
    for (size_t i = 0; subjectsPlan && i < subjectsPlan->subjectsCount(); ++i) {
        headerData_.append();
        headerData_.setData(
            headerData_.count() - 1,
            subjectsPlan->at(i).name());
        headerData_.setData(
            headerData_.count() - 1,
            QVariant(Qt::AlignLeft | Qt::AlignVCenter),
            Qt::TextAlignmentRole);
        headerData_.setData(
            headerData_.count() - 1,
            Qt::Vertical,
            Qt::UserRole);
    }

    emit headerDataChanged(Qt::Horizontal, 0, columnCount() - 1);
}

int Model::rowCount(const QModelIndex& /*parent*/) const
{
    return class_->studentsCount();
}

int Model::columnCount(const QModelIndex& /*parent*/) const
{
    return COMMON_SECTIONS + (class_->subjectsPlan()
        ? class_->subjectsPlan()->subjectsCount()
        : 0);
}

QVariant Model::data(const QModelIndex& index, int role) const
{
    checkIndexIsValid(index);
    QVariant result;
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        const attestate::Student& s = class_->student(index.row());
        if (index.column() < COMMON_SECTIONS) {
            std::map<int, std::function<QVariant(void)>> dataMapper = {
                {0, [&s] () { return QVariant(s.attestateId()); }},
                {1, [&s, this] ()
                    {
                        return s.issueDate()
                            ? QVariant(*s.issueDate())
                            : class_->issueDate()
                                ? QVariant(*class_->issueDate())
                                : QVariant();
                    }
                },
                {2, [&s] () { return QVariant(s.familyName()); }},
                {3, [&s] () { return QVariant(s.name()); }},
                {4, [&s] () { return QVariant(s.parentalName()); }},
                {5, [&s] () { return QVariant(s.birthDate()); }}
            };
            result = dataMapper.at(index.column())();
        } else {
            auto grade = s.grades().value(
                class_->subjectsPlan()->at(index.column() - COMMON_SECTIONS).id());
            if (grade) {
                result = QVariant(*grade);
            }
        }
    } else if (role == Qt::TextAlignmentRole) {
        result = (index.column() >= COMMON_SECTIONS)
            ? QVariant(Qt::AlignHCenter | Qt::AlignVCenter)
            : QVariant(Qt::AlignLeft | Qt::AlignVCenter);
    } /*else if (role == Qt::BackgroundRole) {
        result = QBrush(Qt::red);
    }*/
    return result;
}

QVariant Model::headerData(
    int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        return headerData_.data(section, role);
    } else {
        QVariant res;
        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            res.setValue(section + 1);
        }
        return res;
    }
}

Qt::ItemFlags Model::flags(const QModelIndex& /*index*/) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

bool Model::setData(
    const QModelIndex& index, const QVariant& data, int role)
{
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        checkIndexIsValid(index);

        attestate::Student& s = class_->student(index.row());
        if (index.column() < COMMON_SECTIONS) {
            std::map<int, std::function<void(void)>> dataMapper = {
                {0, [&s, &data] () { s.setAttestateId(data.toString()); }},
                {1, [&s, this, &data] ()
                    {
                        QDate issueDate = qvariant_cast<QDate>(data);
                        if (!issueDate.isValid()) {
                            return;
                        }
                        if (attestate::OptionalDate(issueDate) == class_->issueDate()) {
                            s.setIssueDate(boost::none);
                        } else {
                            s.setIssueDate(issueDate);
                        }
                    }
                },
                {2, [&s, &data] () { s.setFamilyName(data.toString()); }},
                {3, [&s, &data] () { s.setName(data.toString()); }},
                {4, [&s, &data] () { s.setParentalName(data.toString()); }},
                {5, [&s, &data] () { s.setBirthDate(data.toDate()); }}
            };
            dataMapper.at(index.column())();
        } else {
            auto subjectId = class_->subjectsPlan()->at(index.column() - COMMON_SECTIONS).id();
            QString value = data.toString();
            if (value.isEmpty()) {
                s.grades().setValue(subjectId, boost::none);
            } else {
                ATT_REQUIRE(attestate::grades::isValid(value), "Invalid grade value");
                s.grades().setValue(subjectId, value);
            }
        }

        emit dataChanged(index, index);
        return true;
    }
    return false;
}

bool Model::insertRows(int row, int count, const QModelIndex& parent)
{

}

bool Model::removeRows(int row, int count, const QModelIndex& parent)
{

}

void Model::setClassId(const QString& classId) { class_->setClassId(classId); }

void Model::setIssueDate(const QDate& date)
{
    if (attestate::OptionalDate(date) != class_->issueDate()) {
        class_->setIssueDate(date);
        emit dataChanged(
            index(0, 0),
            index(rowCount() - 1, columnCount() - 1));
    }
}

void Model::setGraduationYear(int year) { class_->setGraduationYear(year); }

void Model::checkIndexIsValid(const QModelIndex& index) const
{
    ATT_REQUIRE(
        0 <= index.row() && index.row() < rowCount(),
        "Class model: row index out of range: " << index.row());
    ATT_REQUIRE(
        0 <= index.column() && index.column() < columnCount(),
        "Class model: column index out of range: " << index.column());
}

} // namespace cls
