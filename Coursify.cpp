#include "stdafx.h"
#include "Coursify.h"
#include "Utilities.h"


Coursify::Coursify(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    ui.stackedWidget->setCurrentWidget(ui.Login);   //to start at login page "Awl haga"


    connect(ui.signup_button, &QPushButton::clicked, this, [=]() {         //Register transition
        ui.stackedWidget->setCurrentWidget(ui.Register);                  
        });

    connect(ui.back_button, &QPushButton::clicked, this, [=]() {           //Back transition in Register
        ui.stackedWidget->setCurrentWidget(ui.Login); 
        });

    setupRadioButtonLogic(ui.radioButtonA, ui.radioButtonS,
        ui.Register_S1, ui.Register_S2,
        ui.Register_S3, ui.Register_S4,
        ui.Register_A1, ui.Register_A2,
        ui.Register_A3, ui.Register_A4 );

}

Coursify::~Coursify()
{}
