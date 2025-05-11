#include "stdafx.h"
#include "Coursify.h"
#include "Utilities.h"
#include "courseSystem.h"


Coursify::Coursify(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    ui.stackedWidget->setCurrentWidget(ui.Login);   //to start at login page "Awl haga"


    connect(ui.signup_button, &QPushButton::clicked, this, [=]() {         //  Register transition
        ui.stackedWidget->setCurrentWidget(ui.Register);
        });

    connect(ui.back_button, &QPushButton::clicked, this, [=]() {          //  Back transition in Register    
        ui.stackedWidget->setCurrentWidget(ui.Login);
     
        });

    connect(ui.login_button, &QPushButton::clicked, this, [=]() {          //  Back transition in Register    

        courseSystem Sys;             //lazem yeb2o global
        user* currentUser = nullptr;       //lazem yeb2o global

        char choice = Sys.authenticateUser(ui.login_username, ui.login_password, currentUser);

        if (choice == 'S') {
            ui.stackedWidget->setCurrentWidget(ui.Student_Panel);
        }
        else if (choice == 'A') {
            ui.stackedWidget->setCurrentWidget(ui.Admin_Panel);
        }
        else if (choice =='F'){
            QMessageBox::critical(this, "Error", " Username or Password is incorrect! ");
        }

        });


    setupRadioButtonLogic(ui.radioButtonA, ui.radioButtonS,
        ui.Register_S1, ui.Register_S2,
        ui.Register_S3, ui.Register_S4,
        ui.Register_A1, ui.Register_A2,
        ui.Register_A3, ui.Register_A4);




    m_system = new courseSystem();
    if (!m_system->loadData()) {
        QMessageBox::warning(this, "Data Loading Error",
            "Could not load existing data. Starting with empty data.");
    }
    else { cout << "Sucess"; }
}

    Coursify::~Coursify()
    {
        delete m_system;
    }
