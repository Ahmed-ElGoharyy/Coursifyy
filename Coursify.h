#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Coursify.h"

class Coursify : public QMainWindow
{
    Q_OBJECT

public:
    Coursify(QWidget *parent = nullptr);
    ~Coursify();

private:
    Ui::CoursifyClass ui;

};
