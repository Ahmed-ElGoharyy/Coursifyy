#include "stdafx.h"
#include "Coursify.h"

Coursify::Coursify(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    ui.stackedWidget->setCurrentWidget(ui.Login);   //to start at login page "Awl haga"


    connect(ui.signup_button, &QPushButton::clicked, this, [=]() {         //Register transition
        ui.stackedWidget->setCurrentWidget(ui.Register);                  
        });

    connect(ui.back_button, &QPushButton::clicked, this, [=]() {           //Back transition
        ui.stackedWidget->setCurrentWidget(ui.Login); 
        });

}

Coursify::~Coursify()
{}
