#pragma once

#include "class/class_widget.h"

#include <QMainWindow>
#include <QTabWidget>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QLayout>
#include <QObject>

class MainWindow : public QMainWindow {

    Q_OBJECT

public:
    MainWindow();

private slots:
    // file slots
    void open();
    void save();

private:
    class CentralWidget : public QWidget {
    public:
        explicit CentralWidget(MainWindow* mw)
            : QWidget(mw)
        {
            common = new cls::CommonWidget(this);
            common->setModel(nullptr);
            classTab = new QTabWidget(this);
            QVBoxLayout* l = new QVBoxLayout(this);
            l->addWidget(common);
            l->addWidget(classTab);
        }

        cls::CommonWidget* common;
        QTabWidget* classTab;
    };

    CentralWidget* central_;

    QMenu* fileMenu_;

    // file actions
    QAction* openAct_;
    QAction* saveAct_;
};
