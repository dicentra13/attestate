#pragma once

#include "header_view.h"

#include <QtGui>
#include <QObject>

namespace cls {

class VerticalHeader : public HeaderView {

    Q_OBJECT

public:
    explicit VerticalHeader(QWidget* parent = 0);

    void setTextMargin(int margin);

    virtual void setModel(QAbstractItemModel* model);

public slots:
    void onRowsInserted(const QModelIndex& /*parent*/, int start, int end);
    void onRowsRemoved(const QModelIndex& /*parent*/, int start, int end);
    void onSectionResized(int logicalIndex, int /*oldSize*/, int newSize);

protected:
    virtual void paintSection(
        QPainter* painter, const QRect& rect, int logicalIndex) const;

    virtual QSize sizeHint() const;

    virtual bool sectionIntersectsSelection(int logicalIndex) const;
    virtual bool sectionSelected(int logicalIndex) const;
    virtual bool isPreviousSelected(int visualIndex) const;
    virtual bool isNextSelected(int visualIndex) const;

    virtual void drawText(
        int logicalIndex,
        const QString& text,
        const QStyleOptionHeader& opt,
        QPainter* painter) const;

    bool hasSelectedSections() const;

signals:
    void sectionStateChanged(int logicalIndex, int state);

private:
    int textMargin_;
};

} // namespace cls
