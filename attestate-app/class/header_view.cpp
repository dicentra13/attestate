#include "header_view.h"

namespace cls {

HeaderView::HeaderView(Qt::Orientation orient, QWidget* parent)
    : QHeaderView(orient, parent)
{}

void HeaderView::mouseMoveEvent(QMouseEvent* e)
{
    hover_ = logicalIndexAt(e->pos());
    QHeaderView::mouseMoveEvent(e);
}

void HeaderView::mousePressEvent(QMouseEvent* e)
{
    pressed_ = logicalIndexAt(e->pos());
    QHeaderView::mousePressEvent(e);
}

QStyle::State HeaderView::state(int logicalIndex) const
{
    QStyle::State state = QStyle::State_None;
    if (isEnabled()) {
        state |= QStyle::State_Enabled;
    }
    if (window()->isActiveWindow()) {
        state |= QStyle::State_Active;
    }
    if (sectionsClickable()) {
        if (logicalIndex == hover_) {
            state |= QStyle::State_MouseOver;
        }
        if (logicalIndex == pressed_) {
            state |= QStyle::State_Selected;
        } else if (highlightSections()) {
            if (sectionIntersectsSelection(logicalIndex)) {
                state |= QStyle::State_On;
            }
            if (sectionSelected(logicalIndex)) {
                state |= QStyle::State_Sunken;
            }
        }
    }
    return state;
}

QStyleOptionHeader::SectionPosition
HeaderView::position(int visualIndex) const
{
    QStyleOptionHeader::SectionPosition position;
    if (count() == 1) {
        position = QStyleOptionHeader::OnlyOneSection;
    } else if (visualIndex == 0) {
        position = QStyleOptionHeader::Beginning;
    } else if (visualIndex == count() - 1) {
        position = QStyleOptionHeader::End;
    } else {
        position = QStyleOptionHeader::Middle;
    }
    return position;
}

QStyleOptionHeader::SelectedPosition HeaderView::selectedPosition(
    int visualIndex) const
{
    QStyleOptionHeader::SelectedPosition position;
    bool previousSelected = isPreviousSelected(visualIndex);
    bool nextSelected =  isNextSelected(visualIndex);
    if (previousSelected && nextSelected) {
        position =
            QStyleOptionHeader::NextAndPreviousAreSelected;
    } else if (previousSelected) {
        position = QStyleOptionHeader::PreviousIsSelected;
    } else if (nextSelected) {
        position = QStyleOptionHeader::NextIsSelected;
    } else {
        position = QStyleOptionHeader::NotAdjacent;
    }
    return position;
}

} // namespace cls
