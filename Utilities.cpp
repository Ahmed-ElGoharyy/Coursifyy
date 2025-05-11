#include "stdafx.h"
#include "Utilities.h"
#include <QMessageBox>
#include <QString>
#include <string>
#include <QWidget>
#include <Coursify.h>
#include <QtWidgets/QMainWindow>
#include "ui_Coursify.h"
#include "courseSystem.h"

using namespace std;

void display_error(string title, string message, QWidget* parent) {

    QMessageBox::critical(
        parent,
        QString::fromStdString(title),
        QString::fromStdString(message),
        QMessageBox::Ok
    );
    

   // showErrorPopup("Error", "Login failed", this);
}//For Error use :  QMessageBox::critical(this, "Title", "Error message");


//For Success use : QMessageBox::information(this, "Title", "success message");






void setupRadioButtonLogic(QRadioButton* choice1Radio, QRadioButton* choice2Radio,
    QLineEdit* lineEditA, QLineEdit* lineEditB,
    QLineEdit* lineEditC, QLineEdit* lineEditD,
    QLineEdit* lineEditE, QLineEdit* lineEditF,
    QLineEdit* lineEditG, QLineEdit* lineEditH)               // Register display functions
{
    auto updateVisibility = [=]() {


        bool Student_Selected = choice2Radio->isChecked();
        lineEditA->setVisible(Student_Selected);
        lineEditB->setVisible(Student_Selected);
        lineEditC->setVisible(Student_Selected);
        lineEditD->setVisible(Student_Selected);

        bool admin_Selected = choice1Radio->isChecked();
        lineEditE->setVisible(admin_Selected);
        lineEditF->setVisible(admin_Selected);
        lineEditG->setVisible(admin_Selected);
        lineEditH->setVisible(admin_Selected);

        };

    QObject::connect(choice1Radio, &QRadioButton::toggled, updateVisibility);
    QObject::connect(choice2Radio, &QRadioButton::toggled, updateVisibility);

    updateVisibility();
}


bool validateCredentials(QLineEdit* usernameEdit, QLineEdit* passwordEdit, QString& username, QString& password, user*& loggedUser)
{
    username = usernameEdit->text().trimmed();
    password = passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(nullptr, "Login Failed", "Username and password must not be empty.");
        return false;
    }

    // Optionally: add more validation here (length, special chars, etc.)

    return true;
}


