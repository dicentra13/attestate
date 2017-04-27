#include "horizontal_header.h"

namespace cls {

HorizontalHeader::HorizontalHeader(QWidget* parent)
    : HeaderView(Qt::Horizontal, parent)
    , textMargin_(0)
{
    HeaderView::setSectionsClickable(true);
    setHighlightSections(true);
    QHeaderView::setSectionResizeMode(QHeaderView::Interactive);
    setTextElideMode(Qt::ElideNone);
}

void HorizontalHeader::setTextMargin(int margin)
{
    textMargin_ = margin;
}

namespace {

const int MAX_VERTICAL_TEXT_WIDTH = 150;

} // namespace

void HorizontalHeader::paintSection(
    QPainter* painter, const QRect& rect, int logicalIndex) const
{
    QVariant userData =
        model()->headerData(logicalIndex, Qt::Horizontal, Qt::UserRole);
    Qt::Orientation textOrient = Qt::Horizontal;
    if (userData.type() == QVariant::Int && userData.toInt() == Qt::Vertical) {
        textOrient = Qt::Vertical;
    }

    QStyleOptionHeader opt;
    initStyleOption(&opt);
    opt.state |= state(logicalIndex);
    if (isSortIndicatorShown() && sortIndicatorSection() == logicalIndex) {
        opt.sortIndicator = (sortIndicatorOrder() == Qt::AscendingOrder)
            ? QStyleOptionHeader::SortDown
            : QStyleOptionHeader::SortUp;
    }
    QVariant textAlignment = model()->headerData(
        logicalIndex, Qt::Horizontal, Qt::TextAlignmentRole);
    opt.rect = textOrient == Qt::Horizontal
        ? rect
        : QRect(rect.left(), rect.top(),
            std::min(rect.width(), MAX_VERTICAL_TEXT_WIDTH), rect.height());
    opt.section = logicalIndex;
    opt.textAlignment = Qt::Alignment(textAlignment.isValid()
        ? Qt::Alignment(textAlignment.toInt())
        : defaultAlignment());
    opt.iconAlignment = Qt::AlignVCenter;
    QString text = model()->headerData(
        logicalIndex, Qt::Horizontal, Qt::DisplayRole).toString();
    if (textElideMode() != Qt::ElideNone) {
        text = opt.fontMetrics.elidedText(
            text, textElideMode() , rect.width() - 4);
    }
    opt.text = "";
    QVariant variant = model()->headerData(logicalIndex, Qt::Horizontal,
        Qt::DecorationRole);
    opt.icon = qvariant_cast<QIcon>(variant);
    if (opt.icon.isNull()) {
        opt.icon = qvariant_cast<QPixmap>(variant);
    }
    QVariant foregroundBrush = model()->headerData(
        logicalIndex, Qt::Horizontal, Qt::ForegroundRole);
    if (foregroundBrush.canConvert<QBrush>()) {
        opt.palette.setBrush(QPalette::ButtonText,
            qvariant_cast<QBrush>(foregroundBrush));
    }

    QVariant backgroundBrush = model()->headerData(
        logicalIndex, Qt::Horizontal, Qt::BackgroundRole);
    if (backgroundBrush.canConvert<QBrush>()) {
        opt.palette.setBrush(QPalette::Button,
            qvariant_cast<QBrush>(backgroundBrush));
        opt.palette.setBrush(QPalette::Window,
            qvariant_cast<QBrush>(backgroundBrush));
        painter->setBrushOrigin(opt.rect.topLeft());
    }
    int visual = visualIndex(logicalIndex);
    Q_ASSERT(visual != -1);
    opt.position = position(visual);
    opt.selectedPosition = selectedPosition(visual);

    painter->save();
    style()->drawControl(QStyle::CE_Header, &opt, painter, this);
    painter->restore();

    painter->save();
    if (textOrient == Qt::Vertical) {
        painter->translate(rect.left(), rect.bottom());
        painter->rotate(270);

        painter->setBrush(Qt::NoBrush);
        painter->drawText(
            QRect(textMargin_, 0, rect.height(), rect.width()),
            text,
            QTextOption(opt.textAlignment));
    } else {
        painter->translate(QPoint(opt.rect.left(), opt.rect.top()));
        painter->setBrush(Qt::NoBrush);
        painter->drawText(
            QRect(0, -textMargin_, opt.rect.width(), opt.rect.height()),
            text, QTextOption(opt.textAlignment));
    }
    painter->restore();
}
bool HorizontalHeader::sectionIntersectsSelection(int logicalIndex) const
{
    return selectionModel()->columnIntersectsSelection(
        logicalIndex, rootIndex());
}

bool HorizontalHeader::sectionSelected(int logicalIndex) const
{
    return selectionModel()->isColumnSelected(logicalIndex, rootIndex());
}

bool HorizontalHeader::isPreviousSelected(int visualIndex) const
{
    return selectionModel()->isColumnSelected(
        this->logicalIndex(visualIndex - 1), rootIndex());
}

bool HorizontalHeader::isNextSelected(int visualIndex) const
{
    return selectionModel()->isColumnSelected(
        this->logicalIndex(visualIndex + 1), rootIndex());
}

void HorizontalHeader::drawText(
    int logicalIndex,
    const QString& text,
    const QStyleOptionHeader& opt,
    QPainter* painter) const
{

}

QSize HorizontalHeader::sectionSizeFromContents(int logicalIndex) const
{
    QVariant userData =
        model()->headerData(logicalIndex, Qt::Horizontal, Qt::UserRole);
    QFontMetrics metrics(fontMetrics());
    QString text = model()->headerData(logicalIndex, Qt::Horizontal).toString();
    QSize size(
        metrics.width(text) + 2 * textMargin_,
        metrics.height() + 2 * textMargin_);

    if (userData.type() == QVariant::Int && userData.toInt() == Qt::Vertical) {
        double w = 0.0;
        size_t f = 0;
        do {
            ++f;
            w += MAX_VERTICAL_TEXT_WIDTH;
        } while (w < size.width());
        return QSize(
            f * size.height(),
            std::min(size.width(), MAX_VERTICAL_TEXT_WIDTH));
    }
    return size;
}

} // namespace cls
