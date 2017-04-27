#include "main_window.h"

#include "class/class_editor.h"

#include <QFileDialog>
#include <QFileInfo>

MainWindow::MainWindow()
{
    central_ = new CentralWidget(this);
    setCentralWidget(central_);

    fileMenu_ = menuBar()->addMenu(tr("&File"));

    openAct_ = new QAction(tr("&Open"), this);
    fileMenu_->addAction(openAct_);
    connect(openAct_, SIGNAL(triggered()), this, SLOT(open()));

    saveAct_ = new QAction(tr("&Save"), this);
    fileMenu_->addAction(saveAct_);
    connect(saveAct_, SIGNAL(triggered()), this, SLOT(save()));
}

void MainWindow::open()
{
    auto filename = QFileDialog::getOpenFileName(this, "Open csv file", "", "*.csv");
    ClassEditor* editor = new ClassEditor(filename, this);
    central_->common->setModel(editor->model());
    QFileInfo fi(filename);
    central_->classTab->addTab(editor, fi.fileName());
    central_->classTab->setTabToolTip(0, fi.absoluteFilePath());
    central_->classTab->setTabsClosable(true);
}

void MainWindow::save()
{}
