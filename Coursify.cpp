#include "stdafx.h"
#include "Coursify.h"
#include "Utilities.h"
#include "courseSystem.h"

// Global variables
courseSystem Sys;             // Global course system instance
user* currentUser = nullptr;  // Current logged in user

Coursify::Coursify(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    ui.stackedWidget->setCurrentWidget(ui.Login);   // Start at login page

    // Set up navigation connections
    connect(ui.signup_button, &QPushButton::clicked, this, [=]() {
        ui.stackedWidget->setCurrentWidget(ui.Register);
        });
    connect(ui.back_button, &QPushButton::clicked, this, [=]() {
        ui.stackedWidget->setCurrentWidget(ui.Login);
        });
    connect(ui.back_button_2, &QPushButton::clicked, this, [=]() {
        ui.stackedWidget->setCurrentWidget(ui.Login);
        });
    connect(ui.back_button_3, &QPushButton::clicked, this, [=]() {
        ui.stackedWidget->setCurrentWidget(ui.Login);
        });

    // Registration functionality
    connect(ui.Register_signup, &QPushButton::clicked, this, [=]() {
        if (ui.radioButtonS->isChecked()) {
            if (Sys.registerStudent(ui.Register_S1, ui.Register_S2, ui.Register_S3, ui.Register_S4)) {
                QMessageBox::information(this, " Success", "\n Student registered successfully \n ");
                ui.stackedWidget->setCurrentWidget(ui.Login);
            }
        }
        else if (ui.radioButtonA->isChecked()) {
            if (Sys.registerAdmin(ui.Register_A1, ui.Register_A2, ui.Register_A3, ui.Register_A4, ui.Register_A5)) {
                QMessageBox::information(this, " Success", "\n Administrator registered successfully \n\n You knew the admin secret key :) \n ");
                ui.stackedWidget->setCurrentWidget(ui.Login);
            }
        }
        else {
            QMessageBox::critical(this, " Error", "\n Must choose to register as Student or admin \n ");
        }
        });

    // FIX: Use the global Sys instance for importing CSV files
    connect(ui.pushButton_uploadDescription, &QPushButton::clicked, this, [this]() {
        Sys.importCoursesFromFile(this);  // Use global Sys instance instead of m_system
        });

    // Login functionality
    connect(ui.login_button, &QPushButton::clicked, this, [=]() {
        char choice = Sys.authenticateUser(ui.login_username, ui.login_password, currentUser);
        if (choice == 'S') {
            ui.stackedWidget->setCurrentWidget(ui.Student_Panel);
        }
        else if (choice == 'A') {
            ui.stackedWidget->setCurrentWidget(ui.Admin_Panel);
        }
        else if (choice == 'F') {
            QMessageBox::critical(this, " Wrong Credentials ", "\n Username or Password are incorrect! \n ");
        }
        });
    courseSystem::showCourseComboBox(ui.combo_course);



    //student panel 
    courseSystem::showCourseComboBox(ui.combo_choose);

  





    setupRadioButtonLogic(ui.radioButtonA, ui.radioButtonS,
        ui.Register_S1, ui.Register_S2,
        ui.Register_S3, ui.Register_S4,
        ui.Register_A1, ui.Register_A2,
        ui.Register_A3, ui.Register_A4, ui.Register_A5);

    // FIX: Remove this separate m_system instance which conflicts with the global Sys
    // Instead, initialize the global Sys
    if (!Sys.loadData()) {
        QMessageBox::warning(this, "Data Loading Error",
            "Could not load existing data. Starting with empty data.");
    }
    else {
        std::cout << "Data loaded successfully" << std::endl;
    }
}

Coursify::~Coursify()
{
    // FIX: Make sure to save data on application exit using the global Sys
    if (!Sys.saveData()) {
        std::cerr << "Error: Failed to save data during application shutdown" << std::endl;
    }
    else {
        std::cout << "Data saved successfully on shutdown" << std::endl;
    }

    // No need to delete m_system as we removed it
}

// Add this to Coursify.h to remove the m_system member
/*
private:
    Ui::CoursifyClass ui;
    // Remove the line: courseSystem* m_system;
*/