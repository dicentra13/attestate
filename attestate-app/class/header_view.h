#pragma once

#include <QtGui>
#include <QHeaderView>

namespace cls {

class HeaderView : public QHeaderView {

    Q_OBJECT

public:
    explicit HeaderView(Qt::Orientation orient, QWidget* parent = 0);

protected:
    virtual void mouseMoveEvent(QMouseEvent* e);
    virtual void mousePressEvent(QMouseEvent* e);

    virtual QStyle::State state(int logicalIndex) const;

    virtual QStyleOptionHeader::SectionPosition
    position(int visualIndex) const;

    virtual QStyleOptionHeader::SelectedPosition
    selectedPosition(int visualIndex) const;

    virtual bool sectionIntersectsSelection(int logicalIndex) const = 0;
    virtual bool sectionSelected(int logicalIndex) const = 0;
    virtual bool isPreviousSelected(int visualIndex) const = 0;
    virtual bool isNextSelected(int visualIndex) const = 0;

    virtual void drawText(
        int logicalIndex,
        const QString& text,
        const QStyleOptionHeader& opt, QPainter* painter) const = 0;

    int hover_;
    int pressed_;
};

} // namespace cls
