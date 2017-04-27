#pragma once

#include <QItemDelegate>
#include <QPainter>

namespace cls {

class Model;

class ColoredCellDelegate : public QItemDelegate {
public:
    ColoredCellDelegate(const Model* model, QObject* parent)
        : QItemDelegate(parent)
        , model_(model)
    {}

    inline void setData(const Model* model) { model_ = model; }

    virtual void paint(
        QPainter* painter,
        const QStyleOptionViewItem& option,
        const QModelIndex& index) const;

protected:
    void checkIndexIsValid(const QModelIndex& index) const;

    const Model* model_;
};

} // namespace cls
