#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Coursify.h"
#include "courseSystem.h"
#include "Utilities.h"

class Coursify : public QMainWindow
{
    Q_OBJECT

public:
    Coursify(QWidget* parent = nullptr);
    ~Coursify();

private:
    Ui::CoursifyClass ui;

private:
    courseSystem* m_system; // Your course system instance

};
