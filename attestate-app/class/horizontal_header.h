#pragma once

#include "header_view.h"

#include <QtGui>
#include <QObject>

namespace cls {

class HorizontalHeader : public HeaderView {

    Q_OBJECT

public:
    explicit HorizontalHeader(QWidget* parent = 0);

    void setTextMargin(int margin);

protected:
    virtual void paintSection(
        QPainter* painter, const QRect& rect, int logicalIndex) const;

    virtual QSize sectionSizeFromContents(int logicalIndex) const;

protected:
    virtual bool sectionIntersectsSelection(int logicalIndex) const;
    virtual bool sectionSelected(int logicalIndex) const;
    virtual bool isPreviousSelected(int visualIndex) const;
    virtual bool isNextSelected(int visualIndex) const;
    virtual void drawText(
        int logicalIndex,
        const QString& text,
        const QStyleOptionHeader& opt, QPainter* painter) const;

private:
    int textMargin_;
};

} // namespace cls
