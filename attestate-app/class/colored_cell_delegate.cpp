#include "colored_cell_delegate.h"

#include "class_model.h"

#include <attestate/exception.h>

namespace cls {

namespace {

const int COMMON_SECTIONS = 6;

} // namespace

void ColoredCellDelegate::paint(
    QPainter* painter,
    const QStyleOptionViewItem& option,
    const QModelIndex& index) const
{
    if (!model_) {
        return;
    }

    checkIndexIsValid(index);

    painter->save();

    const attestate::Student& s = model_->getClass().student(index.row());
    attestate::State state = attestate::State::Existing;
    if (s.state() == attestate::State::New) {
        state = attestate::State::New;
    } else if (index.column() < COMMON_SECTIONS) {
        std::map<int, std::function<bool(void)>> isModified = {
            {0, [&s] () { return s.isAttestateIdModified(); }},
            {1, [&s] () { return s.isIssueDateModified(); }},
            {2, [&s] () { return s.isFamilyNameModified(); }},
            {3, [&s] () { return s.isNameModified(); }},
            {4, [&s] () { return s.isParentalNameModified(); }},
            {5, [&s] () { return s.isBirthDateModified(); }}
        };
        if (isModified.at(index.column())()) {
            state = attestate::State::Modified;
        }
    } else {
//        auto grade = s.grades().value(
//            class_->subjectsPlan()->at(index.column() - COMMON_SECTIONS).id());
//        if (grade) {
//            result = QVariant(*grade);
//        }
    }

    QColor brushColor;

    switch (state) {
    case attestate::State::Existing : brushColor = Qt::white;
        break;
    case attestate::State::Modified : brushColor = Qt::yellow;
        break;
    case attestate::State::New : brushColor = Qt::green;
        break;
    }

    brushColor.setAlpha(127);
    painter->fillRect(option.rect, QBrush(brushColor));
    painter->restore();
    QStyleOptionViewItem newOption(option);
    newOption.palette.setBrush(QPalette::Window, Qt::NoBrush);
    newOption.palette.setBrush(QPalette::Base, Qt::NoBrush);

    if (index.column() == 1) {
        // issue date
        if (!s.issueDate()) {
            newOption.palette.setColor(QPalette::Text, Qt::gray);
        }
    }

    QItemDelegate::paint(painter, newOption, index);
}

void ColoredCellDelegate::checkIndexIsValid(const QModelIndex& index) const
{
    ATT_REQUIRE(
        0 <= index.row() && index.row() < model_->rowCount(),
        "Class model: row index out of range: " << index.row());
    ATT_REQUIRE(
        0 <= index.column() && index.column() < model_->columnCount(),
        "Class model: column index out of range: " << index.column());
}

} // namespace cls
