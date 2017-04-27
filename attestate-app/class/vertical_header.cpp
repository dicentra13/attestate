#include "vertical_header.h"

namespace cls {

VerticalHeader::VerticalHeader(QWidget* parent)
    : HeaderView(Qt::Vertical, parent)
{
    setSectionsClickable(true);
    setHighlightSections(true);
    setSortIndicatorShown(false);
}

void VerticalHeader::setTextMargin(int margin)
{
    textMargin_ = margin;
}

void VerticalHeader::setModel(QAbstractItemModel* model)
{
    QHeaderView::setModel(model);

    for (int i = 0; i < model->rowCount(); ++i) {
        model->setHeaderData(
            i,
            Qt::Vertical,
            sectionSizeFromContents(i),
            Qt::SizeHintRole);
    }
}

void VerticalHeader::onRowsInserted(
    const QModelIndex& /*parent*/, int start, int /*end*/)
{
    emit headerDataChanged(Qt::Vertical, start, model()->rowCount() - 1);
}

void VerticalHeader::onRowsRemoved(
    const QModelIndex& /*parent*/, int start, int /*end*/)
{
    emit headerDataChanged(Qt::Vertical, start, model()->rowCount() - 1);
}

void VerticalHeader::onSectionResized(
    int /*logicalIndex*/, int /*oldSize*/, int /*newSize*/)
{}

bool VerticalHeader::sectionIntersectsSelection(int logicalIndex) const
{
    return selectionModel()->rowIntersectsSelection(
        logicalIndex, rootIndex());
}

bool VerticalHeader::sectionSelected(int logicalIndex) const
{
    return selectionModel()->isRowSelected(logicalIndex, rootIndex());
}

bool VerticalHeader::isPreviousSelected(int visualIndex) const
{
    return selectionModel()->isRowSelected(
        this->logicalIndex(visualIndex - 1), rootIndex());
}

bool VerticalHeader::isNextSelected(int visualIndex) const
{
    return selectionModel()->isRowSelected(
        this->logicalIndex(visualIndex + 1), rootIndex());
}

void VerticalHeader::paintSection(
    QPainter* painter, const QRect& rect, int logicalIndex) const
{
    QStyleOptionHeader opt;
    initStyleOption(&opt);
    opt.state = state(logicalIndex);

    opt.rect = rect;
    opt.section = logicalIndex;
    opt.textAlignment = Qt::AlignCenter;
    opt.iconAlignment = Qt::AlignVCenter;
    QString text = model()->headerData(
        logicalIndex, Qt::Vertical, Qt::DisplayRole).toString();
    if (textElideMode() != Qt::ElideNone) {
        text = opt.fontMetrics.elidedText(
            opt.text, textElideMode() , rect.width() - 4);
    }
    opt.text = text;
    QVariant variant = model()->headerData(logicalIndex, Qt::Vertical,
        Qt::DecorationRole);
    opt.icon = qvariant_cast<QIcon>(variant);
    if (opt.icon.isNull()) {
        opt.icon = qvariant_cast<QPixmap>(variant);
    }
    QVariant foregroundBrush = model()->headerData(
        logicalIndex, Qt::Vertical, Qt::ForegroundRole);
    if (foregroundBrush.canConvert<QBrush>()) {
        opt.palette.setBrush(QPalette::ButtonText,
            qvariant_cast<QBrush>(foregroundBrush));
    }
    QPointF oldBO = painter->brushOrigin();
    QVariant backgroundBrush = model()->headerData(
        logicalIndex, Qt::Vertical, Qt::BackgroundRole);
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
    opt.orientation = Qt::Vertical;

    opt.selectedPosition = selectedPosition(visual);
    style()->drawControl(QStyle::CE_Header, &opt, painter, this);
    painter->setBrushOrigin(oldBO);
}

void VerticalHeader::drawText(int /*logicalIndex*/, const QString& /*text*/,
        const QStyleOptionHeader& /*opt*/, QPainter* /*painter*/) const
{}

QSize VerticalHeader::sizeHint() const
{
    QSize s;
    s.setHeight(height());
    for (int i = 0; i < model()->rowCount(); ++i) {
        s.setWidth(std::max(s.width(), sectionSizeFromContents(i).width()));
    }
    return s;
}

} // namespace cls
