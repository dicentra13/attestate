#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QDateEdit>

namespace cls {

class Model;

class CommonWidget : public QWidget {

    Q_OBJECT

public:
    explicit CommonWidget(QWidget* parent = 0);

    void setModel(Model* model);

private slots:
    void onClassIdChanged(QString);
    void onGraduationYearChanged(int);
    void onIssueDateChanged(QDate);

private:
    QLineEdit* classId_;
    QSpinBox* graduationYear_;
    QDateEdit* issueDate_;

    Model* model_;
};

} // namespace cls
