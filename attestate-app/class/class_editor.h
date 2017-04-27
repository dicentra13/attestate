#pragma once

#include "class_model.h"
#include "class_view.h"

#include <QDialog>

QT_BEGIN_NAMESPACE
class QDialogButtonBox;
class QPushButton;
QT_END_NAMESPACE

class ClassEditor : public QWidget
{
    Q_OBJECT

public:
    explicit ClassEditor(QWidget* parent = 0);
    explicit ClassEditor(const QString& csvpath, QWidget* parent = 0);

    cls::Model* model() { return model_; }

private slots:
    void submit();
    void generate();

private:
    QPushButton* submitButton;
    QPushButton* generateButton;
    QPushButton* revertButton;
    QPushButton* quitButton;
    QDialogButtonBox* buttonBox;

    cls::Model* model_;
    cls::View* view_;
};
