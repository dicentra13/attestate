#include "class_editor.h"

#include <attestate/grades.h>
#include <attestate/generate.h>
#include <doctpl/template.h>
#include <doctpl/serialize.h>

#include <QtGui>
#include <QPushButton>
#include <QDateEdit>
#include <QDialogButtonBox>
#include <QHBoxLayout>
//#include <QPrintDialog>
#include <QFileDialog>
#include <QObject>

#include <memory>
#include <sstream>

ClassEditor::ClassEditor(QWidget* parent)
    : QWidget(parent)
{
    model_ = new cls::Model(QString::fromUtf8("/home/dicentra/att_2015/Топлакалцян.csv"), this);
    view_ = new cls::View(this);

    QFont f;
    f.setPointSize(14);
    view_->setFont(f);
    view_->setHeaderFont(f);
    view_->setModel(model_);
    view_->setItemDelegate(model_->delegate());
    view_->resizeColumnsToContents();
    view_->resizeRowsToContents();

    submitButton = new QPushButton(tr("Submit"));
    submitButton->setDefault(true);
    generateButton = new QPushButton(tr("&Generate"));
    revertButton = new QPushButton(tr("&Revert"));
    quitButton = new QPushButton(tr("Quit"));

    buttonBox = new QDialogButtonBox(Qt::Vertical);
    buttonBox->addButton(submitButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(generateButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(revertButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

    connect(submitButton, SIGNAL(clicked()), this, SLOT(submit()));
    connect(generateButton, SIGNAL(clicked()), this, SLOT(generate()));
    connect(revertButton, SIGNAL(clicked()), model_, SLOT(revertAll()));
    connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));

    QHBoxLayout* mainLayout = new QHBoxLayout;
    mainLayout->addWidget(view_);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    setWindowTitle(tr("Attestate editor"));

    show();
}

ClassEditor::ClassEditor(const QString& csvpath, QWidget* parent)
    : QWidget(parent)
{
    model_ = new cls::Model(csvpath, this);
    view_ = new cls::View(this);

    QFont f;
    f.setPointSize(14);
    view_->setFont(f);
    view_->setHeaderFont(f);
    view_->setModel(model_);
    view_->setItemDelegate(model_->delegate());
    view_->resizeColumnsToContents();
    view_->resizeRowsToContents();

    submitButton = new QPushButton(tr("Submit"));
    submitButton->setDefault(true);
    generateButton = new QPushButton(tr("&Generate"));
    revertButton = new QPushButton(tr("&Revert"));
    quitButton = new QPushButton(tr("Quit"));

    buttonBox = new QDialogButtonBox(Qt::Vertical);
    buttonBox->addButton(submitButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(generateButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(revertButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

    connect(submitButton, SIGNAL(clicked()), this, SLOT(submit()));
    connect(generateButton, SIGNAL(clicked()), this, SLOT(generate()));
    connect(revertButton, SIGNAL(clicked()), model_, SLOT(revertAll()));
    connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));

    QHBoxLayout* mainLayout = new QHBoxLayout;
    mainLayout->addWidget(view_);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    //setWindowTitle(tr("Attestate editor"));

    show();
}

void ClassEditor::submit() {}

void ClassEditor::generate()
{
    auto templatePath = QFileDialog::getOpenFileName(
        this,
        QString::fromUtf8("Шаблон аттестата"),
        "",
        "*.xml");

    auto saveDir = QFileDialog::getExistingDirectory(
        this,
        QString::fromUtf8("Путь для сохранения PDF файлов"));


    std::unique_ptr<doctpl::Template> doc = doctpl::xml::read(templatePath);

    const auto& c = model_->getClass();

    for (size_t i = 0; i < c.studentsCount(); ++i) {
        attestate::gen::fillTemplate(c, i, *doc);
        const auto& s = c.student(i);
        doc->print(saveDir + "/" + s.familyName() + " " + s.name() + ".pdf");
    }
}
