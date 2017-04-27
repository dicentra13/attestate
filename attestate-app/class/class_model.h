#pragma once

#include "class_header_data.h"
#include "colored_cell_delegate.h"

#include <attestate/class.h>

#include <QtCore>

#include <memory>

namespace cls {

class Model : public QAbstractTableModel {

    Q_OBJECT

public:

    explicit Model(const QString& csvfile, QObject* parent = 0);


    virtual ~Model();

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

    virtual QVariant data(
        const QModelIndex& index,
        int role = Qt::DisplayRole) const;

    virtual QVariant headerData(
        int section,
        Qt::Orientation orientation,
        int role = Qt::DisplayRole) const;

    virtual Qt::ItemFlags flags(const QModelIndex& index) const;

    virtual bool setData(
        const QModelIndex& index,
        const QVariant& data,
        int role = Qt::EditRole);

    virtual bool insertRows(
        int row,
        int count,
        const QModelIndex& parent = QModelIndex());

    virtual bool removeRows(
        int row,
        int count,
        const QModelIndex& parent = QModelIndex());

    ColoredCellDelegate* delegate()
    {
        return new ColoredCellDelegate(this, this);
    }

    // called from slots of CommonWidget

    void setClassId(const QString& classId);
    void setIssueDate(const QDate& date);
    void setGraduationYear(int year);

    // const class data access

    const attestate::Class& getClass() const { return *class_; }

private:
    void initHeaderData();
    void checkIndexIsValid(const QModelIndex& index) const;

    HeaderData headerData_;

    std::unique_ptr<attestate::Class> class_;
};

} // namespace cls
