#include "stdafx.h"
#include <QMessageBox>
#include <QString>
#include <string>
#include <QWidget>

using namespace std;

void display_error(string title, string message, QWidget* parent) {

    QMessageBox::critical(
        parent,
        QString::fromStdString(title),
        QString::fromStdString(message),
        QMessageBox::Ok
    );
    

   // showErrorPopup("Error", "Login failed", this);
}


//For Error use :  QMessageBox::critical(this, "Title", "Error message");


//For Success use : QMessageBox::information(this, "Title", "success message");


