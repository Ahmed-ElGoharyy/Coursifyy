#include "stdafx.h"
#include "Coursify.h"
#include "Utilities.h"
#include "courseSystem.h"


courseSystem Sys;             //lazem yeb2o global
user* currentUser = nullptr;       //lazem yeb2o global




Coursify::Coursify(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    ui.stackedWidget->setCurrentWidget(ui.Login);   //to start at login page "Awl haga"


    connect(ui.signup_button, &QPushButton::clicked, this, [=]() {         //  Register transition
        ui.stackedWidget->setCurrentWidget(ui.Register);
        });


    connect(ui.back_button, &QPushButton::clicked, this, [=]() {          //  Back transition in Register panel  
        ui.stackedWidget->setCurrentWidget(ui.Login);
        });


    connect(ui.back_button_2, &QPushButton::clicked, this, [=]() {          //  Back transition in admin panel
        ui.stackedWidget->setCurrentWidget(ui.Login);
        });

    connect(ui.back_button_3, &QPushButton::clicked, this, [=]() {          //  Back transition in Student panel    
        ui.stackedWidget->setCurrentWidget(ui.Login);
        });

    connect(ui.Register_signup, &QPushButton::clicked, this, [=]() {          // Register function for Admin and Student  

        if (ui.radioButtonS->isChecked()) {
            if (Sys.registerStudent(ui.Register_S1, ui.Register_S2, ui.Register_S3, ui.Register_S4)) {
                QMessageBox::information(this, " Success", "\n Student registered successfully \n ");
            }
          
         
        }

        else if (ui.radioButtonA->isChecked()){
            if (Sys.registerAdmin(ui.Register_A1, ui.Register_A2, ui.Register_A3, ui.Register_A4, ui.Register_A5)) {
                QMessageBox::information(this, " Success", "\n Student registered successfully \n ");
            }
            
        }

        else {
            QMessageBox::critical(this, " Error", "\n Must choose to register as Student or admin \n ");
        }


        });



    connect(ui.login_button, &QPushButton::clicked, this, [=]() {          //  Login Function    

        char choice = Sys.authenticateUser(ui.login_username, ui.login_password, currentUser);

        if (choice == 'S') {
            ui.stackedWidget->setCurrentWidget(ui.Student_Panel);
        }
        else if (choice == 'A') {
            ui.stackedWidget->setCurrentWidget(ui.Admin_Panel);
        }
        else if (choice =='F'){
            QMessageBox::critical(this, " Wrong Credentials ", "\n Username or Password are incorrect! \n ");
        }

        });


    setupRadioButtonLogic(ui.radioButtonA, ui.radioButtonS,
        ui.Register_S1, ui.Register_S2,
        ui.Register_S3, ui.Register_S4,
        ui.Register_A1, ui.Register_A2,
        ui.Register_A3, ui.Register_A4, ui.Register_A5);




    m_system = new courseSystem();
    if (!m_system->loadData()) {
        QMessageBox::warning(this, "Data Loading Error",
            "Could not load existing data. Starting with empty data.");
    }
    else { cout << "Success"; }
}

    Coursify::~Coursify()
    {
        delete m_system;
    }
