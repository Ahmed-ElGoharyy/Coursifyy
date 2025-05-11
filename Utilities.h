#include "stdafx.h"
#include <QMessageBox>
#include <QString>
#include <string>
#include <QWidget>
#include <Coursify.h>
#include <QtWidgets/QMainWindow>
#include "ui_Coursify.h"
#include "courseSystem.h"
#include "course.h"
#include "user.h"
#include "admin.h"
#include "student.h"
#include "FileUtils.h"


using namespace std;

void setupRadioButtonLogic(QRadioButton* choice1Radio, QRadioButton* choice2Radio,
    QLineEdit* lineEditA, QLineEdit* lineEditB,
    QLineEdit* lineEditC, QLineEdit* lineEditD,
    QLineEdit* lineEditE, QLineEdit* lineEditF,
    QLineEdit* lineEditG, QLineEdit* lineEditH, QLineEdit* lineEditI);

bool validateCredentials(QLineEdit* usernameEdit, QLineEdit* passwordEdit, QString& username, QString& password, user*& loggedUser);
