#pragma once

#include "class/class_editor.h"
#include "class/class_common_widget.h"

#include <QLayout>

namespace cls {

class Widget : public QWidget {
public:
    explicit Widget(QWidget* parent = 0)
        : QWidget(parent)
    {
        QVBoxLayout* l = new QVBoxLayout(this);
        common_ = new cls::CommonWidget(this);
        l->addWidget(common_);
        editor_ = new ClassEditor(this);
        l->addWidget(editor_);
        common_->setModel(editor_->model());

//        fileMenu_ = menuBar()->addMenu(tr("&File"));
//        fileMenu_->addAction(newAct_);
//        fileMenu_->addAction(openAct_);
//        fileMenu_->addAction(saveAct_);
    }

private:
    cls::CommonWidget* common_;
    ClassEditor* editor_;
};

} // namespace cls
