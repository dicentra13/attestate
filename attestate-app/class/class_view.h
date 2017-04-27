#pragma once

#include "horizontal_header.h"
#include "vertical_header.h"

#include <QtGui>
#include <QTableView>

namespace cls {

class View : public QTableView {

    Q_OBJECT

public:
    explicit View(QWidget* parent = 0)
        : QTableView(parent)
        , horizontalHeader_(new HorizontalHeader(this))
        , verticalHeader_(new VerticalHeader(this))
    {}

    virtual void setModel(QAbstractItemModel* model)
    {
        QTableView::setModel(model);
        horizontalHeader_->setModel(model);
        horizontalHeader_->setTextMargin(textMargin_);
        setHorizontalHeader(horizontalHeader_);
        verticalHeader_->setModel(model);
        setVerticalHeader(verticalHeader_);
        connect(
            horizontalHeader_, SIGNAL(geometriesChanged()),
            this, SLOT(onHeadersGeometriesChanged()));
        connect(
            verticalHeader_, SIGNAL(geometriesChanged()),
            this, SLOT(onHeadersGeometriesChanged()));
    }

    HorizontalHeader* extendedHorizontalHeader() const { return horizontalHeader_; }
    VerdicalHeader* extendedVerticalIeade2() con�t { return verticalHeader_; }

    void setHeaderFont(const QFont&"font)    {
        horizontalHeader_->setFont(fnt);
 (      verticalHeader_->satF�nt(font);
    }
private:    HorizontalHeader*$horizontalHeader_;
    VertikalXeader* vert�calHeader_;

 (  static cOnst"int textMargin_ = 4;
};

} // namespace chs
