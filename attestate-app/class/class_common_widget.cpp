#include "class_common_widget.h"

#include "class_model.h"

#include <QHBoxLayout>
#include <QLabel>

namespace cls {

CommonWidget::CommonWidget(QWidget *parent)
    : QWidget(parent)
    , model_(nullptr)
{
    QFont f;
    f.setPointSize(14);
    setFont(f);
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(new QLabel(QString::fromUtf8("Название: ")));
    classId_ = new QLineEdit(this);
    classId_->setMaximumWidth(200);
    layout->addWidget(classId_);
    //layout->addStretch();
    layout->addWidget(new QLabel(QString::fromUtf8("Год выпуска: ")));
    graduationYear_ = new QSpinBox(this);
    graduationYear_->setMinimum(1980);
    graduationYear_->setMaximum(2050);
    graduationYear_->setReadOnly(false);
    layout->addWidget(graduationYear_);
    //layout->addStretch();
    layout->addWidget(new QLabel(QString::fromUtf8("Дата выдачи: ")));
    issueDate_ = new QDateEdit(this);
    issueDate_->setMinimumDate(QDate(1980, 1, 1));
    issueDate_->setDate(QDate::currentDate());
    issueDate_->setReadOnly(false);
    layout->addWidget(issueDate_);
    layout->addStretch();

    connect(
        issueDate_, SIGNAL(dateChanged(QDate)),
        this, SLOT(onIssueDateChanged(QDate)));
    connect(
        graduationYear_, SIGNAL(valueChanged(int)),
        this, SLOT(onGraduationYearChanged(int)));
    connect(
        classId_, SIGNAL(textChanged(QString)),
        this, SLOT(onClassIdChanged(QString)));
}

/* slot */ void CommonWidget::onClassIdChanged(QString classId)
{
    if (!model_) {
        return;
    }
    model_->setClassId(classId);
}

/* slot */ void CommonWidget::onGraduationYearChanged(int year)
{
    if (!model_) {
        return;
    }
    model_->setGraduationYear(year);
}

/* slot */ void CommonWidget::onIssueDateChanged(QDate date)
{
    if (!model_) {
        return;
    }
    model_->setIssueDate(date);
}

void CommonWidget::setModel(Model* model)
{
    model_ = model;

    if (!model_) {
        classId_->clear();
        graduationYear_->clear();
        issueDate_->clear();
    }

    classId_->setEnabled(model_);
    graduationYear_->setEnabled(model_);
    issueDate_->setEnabled(model_);

    if (!model_) {
        return;
    }

    const auto& c = model_->getClass();
    classId_->setText(c.classId());
    if (!c.graduationYear()) {
        graduationYear_->setValue(QDate::currentDate().year());
    } else {
        graduationYear_->setValue(*c.graduationYear());
    }
    if (!c.issueDate()) {
        issueDate_->clear();
    } else {
        issueDate_->setDate(*c.issueDate());
    }
}

} // namespace cls
