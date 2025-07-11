#include "stdafx.h"
#include "Coursify.h"
#include "Utilities.h"
#include "courseSystem.h"

using namespace std;


// Global variables
courseSystem Sys;             // Global course system instance
user* currentUser = nullptr;  // Current logged in user

Coursify::Coursify(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    ui.stackedWidget->setCurrentWidget(ui.Login);   // Start at login page

    //For Search Courses
    ui.searchResultsList->hide();
    // Set up navigation connections
    connect(ui.signup_button, &QPushButton::clicked, this, [=]() {
        ui.stackedWidget->setCurrentWidget(ui.Register);
        });
    connect(ui.back_button, &QPushButton::clicked, this, [=]() {
        clearAllListsAndFields();
        ui.stackedWidget->setCurrentWidget(ui.Login);
        });

    connect(ui.back_button_2, &QPushButton::clicked, this, [=]() {
        clearAllListsAndFields();
        ui.stackedWidget->setCurrentWidget(ui.Login);
        });

    connect(ui.back_button_3, &QPushButton::clicked, this, [=]() {
        clearAllListsAndFields();
        ui.stackedWidget->setCurrentWidget(ui.Login);
        });


    // In Coursify constructor, after other connections:
    connect(ui.gradesUploadButton, &QPushButton::clicked, this, [this]() {
        // Only allow admins to upload grades
        if (dynamic_cast<admin*>(currentUser)) {
            Sys.importGradesFromFile(this);
            refreshAdminUI();
        }
        else {
            QMessageBox::warning(this, "Permission Denied",
                "Only administrators can upload grades.");
        }
        });

    // Use the global Sys instance for importing CSV files
    connect(ui.pushButton_uploadDescription, &QPushButton::clicked, this, [this]() {
        Sys.importCoursesFromFile(this);
        refreshAdminUI();
        });




    //admin panell
    Sys.showCourseComboBox(ui.combo_course);
    Sys.showCourseComboBox(ui.combo_course);
    Sys.showCourseComboBox(ui.combo_pre);
    connect(ui.combo_course, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=]() {
        Sys.loadCoursePrereqsToListWidget(ui.combo_course, ui.list_prereq);
        });
    connect(ui.pushButton_add, &QPushButton::clicked, this, [=]() {
        Sys.addPrerequisiteToList(ui.combo_course, ui.combo_pre, ui.list_prereq, this);
        });

    // Remove a selected prerequisite when "Remove" is clicked
    connect(ui.pushButton_remove, &QPushButton::clicked, this, [=]() {
        Sys.removeSelectedPrerequisite(ui.combo_course, ui.list_prereq, this);
        });
        connect(ui.pushButton_submitGrade, &QPushButton::clicked, this, [=]() {
     Sys.assignGradeToStudent(ui.StudentUser, ui.lineEdit_courseID_2 , ui.comboBox, this);
     });


    ////////////////////////////////

    //student panel 


    // Search 
        connect(ui.searchBar, &QLineEdit::textChanged, this, &Coursify::onSearchTextChanged);
        connect(ui.searchResultsList, &QListWidget::itemClicked, this, &Coursify::onSearchResultSelected);

        ui.searchBar->installEventFilter(this);
        ui.searchResultsList->installEventFilter(this);



		// pre requisites 
    Sys.showCourseComboBox(ui.combo_choose);
    connect(ui.combo_choose, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=]() {
        Sys.loadCoursePrereqsToListWidget(ui.combo_choose, ui.list_showpreq);
        });

   

	//view grades
    connect(ui.pushButton_g, &QPushButton::clicked, this, [=]() {
        student* currentStudent = dynamic_cast<student*>(currentUser);
        if (!currentStudent) {
            QMessageBox::critical(this, "Error", "User type mismatch. Cannot load student data.");
            return;
        }

        QStringList report = Sys.getStudentGrades(currentStudent);
        ui.listWidget_g->clear();
        ui.listWidget_g->addItems(report);
     });


    //report grades 
    connect(ui.tabWidget_2, &QTabWidget::currentChanged, this, [=]() {
        student* currentStudent = dynamic_cast<student*>(currentUser);
        if (!currentStudent) {
            QMessageBox::critical(this, "Error", "User type mismatch. Cannot load student data.");
            return;
        }

        ui.textEdit_n->setText(QString::fromStdString(currentStudent->getName()));  // Name
        ui.textEdit_i->setText(QString::number(currentStudent->getStudentID()));           // ID
        ui.textEdit_g->setText(QString::number(currentStudent->calculateGPA(), 'f', 2)); // GPA

        QStringList report = Sys.getStudentCourseReport(currentStudent);
        ui.listWidget_r->clear();
        ui.listWidget_r->addItems(report);
     });
   
  


    //report  export
    connect(ui.export_button, &QPushButton::clicked, this, [=]() {

        ui.export_button->setVisible(false);

        student* currentStudent = dynamic_cast<student*>(currentUser);
        currentStudent->generateReport();
        
        ui.export_button->setVisible(true);

        QMessageBox::information(this, "Export Complete", "Report successfully exported as Txt.");
        });
     
    ///////////////////////////////////////

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

    connect(ui.pushButton_uploadDescription, &QPushButton::clicked, this, [this]() {
        Sys.importCoursesFromFile(this);  // Use global Sys instance instead of m_system
        });




    // Login functionality
    connect(ui.login_button, &QPushButton::clicked, this, [=]() {
        char choice = Sys.authenticateUser(ui.login_username, ui.login_password, currentUser);
        if (choice == 'S') {
            ui.stackedWidget->setCurrentWidget(ui.Student_Panel);
            ui.tabWidget_2->setCurrentIndex(0);
            student* currentStudent = dynamic_cast<student*>(currentUser);
            ui.label_15->setText(QString::fromStdString("Welcome,  " + currentStudent->getName()));

            // Reset and refresh UI
            resetUIForLogin();
        }
        else if (choice == 'A') {
            ui.stackedWidget->setCurrentWidget(ui.Admin_Panel);
            admin* currentAdmin = dynamic_cast<admin*>(currentUser);
            ui.label_14->setText(QString::fromStdString("Welcome,  " + currentAdmin->getName()));

            // Reset and refresh UI
            resetUIForLogin();
        }
        else if (choice == 'F') {
            QMessageBox::critical(this, "Wrong Credentials", "\nUsername or Password are incorrect!\n");
        }
        });





    setupRadioButtonLogic(ui.radioButtonA, ui.radioButtonS,
        ui.Register_S1, ui.Register_S2,
        ui.Register_S3, ui.Register_S4,
        ui.Register_A1, ui.Register_A2,
        ui.Register_A3, ui.Register_A4, ui.Register_A5);

 
    // Instead, initialize the global Sys
    if (!Sys.loadData()) {
        QMessageBox::warning(this, "Data Loading Error",
            "Could not load existing data. Starting with empty data.");
    }
    else {
        cout << "Data loaded successfully" << endl;
    }
}

Coursify::~Coursify()
{
    // FIX: Make sure to save data on application exit using the global Sys
    if (!Sys.saveData()) {
        cerr << "Error: Failed to save data during application shutdown" << endl;
    }
    else {
        cout << "Data saved successfully on shutdown" << endl;
    }

    // No need to delete m_system as we removed it
}

// Add this to Coursify.h to remove the m_system member
/*
private:
    Ui::CoursifyClass ui;
    // Remove the line: courseSystem* m_system;
*/


bool Coursify::eventFilter(QObject* obj, QEvent* event) {
    if (obj == ui.searchBar && event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            if (!ui.searchResultsList->isHidden() && ui.searchResultsList->count() > 0) {
                // Select the first item if nothing is selected
                if (!ui.searchResultsList->currentItem()) {
                    ui.searchResultsList->setCurrentRow(0);
                }
                onSearchResultSelected(ui.searchResultsList->currentItem());
                return true;
            }
        }
        else if (keyEvent->key() == Qt::Key_Down) {
            if (!ui.searchResultsList->isHidden() && ui.searchResultsList->count() > 0) {
                ui.searchResultsList->setFocus();
                if (!ui.searchResultsList->currentItem()) {
                    ui.searchResultsList->setCurrentRow(0);
                }
                return true;
            }
        }
    }
    else if (obj == ui.searchResultsList && event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            if (ui.searchResultsList->currentItem()) {
                onSearchResultSelected(ui.searchResultsList->currentItem());
                return true;
            }
        }
        else if (keyEvent->key() == Qt::Key_Escape) {
            ui.searchResultsList->hide();
            ui.searchBar->setFocus();
            return true;
        }
    }
    return QMainWindow::eventFilter(obj, event);
}
void Coursify::onSearchTextChanged(const QString& text) {
    ui.searchResultsList->clear();

    if (text.isEmpty()) {
        ui.searchResultsList->hide();
        return;
    }

    auto results = Sys.searchCourses(text.toStdString());

    for (const auto& course : results) {
        QString title = QString::fromStdString(course.getTitle());
        QString highlightedTitle = highlightMatchingChars(title, text);

        QListWidgetItem* item = new QListWidgetItem();
        item->setData(Qt::UserRole, QVariant::fromValue(course.getCourseID()));

        // Simplified display - only show the highlighted title and ID
        item->setText(QString("%1 (ID: %2)").arg(highlightedTitle).arg(course.getCourseID()));
        ui.searchResultsList->addItem(item);
    }

    if (!results.empty()) {
        QPoint pos = ui.searchBar->mapToParent(QPoint(0, ui.searchBar->height()));
        ui.searchResultsList->move(pos);
        ui.searchResultsList->setFixedWidth(ui.searchBar->width());
        ui.searchResultsList->show();
    }
    else {
        ui.searchResultsList->hide();
    }
}
void Coursify::onSearchResultSelected(QListWidgetItem* item) {
    if (!item || !currentUser) return;

    // Only students can add courses
    student* currentStudent = dynamic_cast<student*>(currentUser);
    if (!currentStudent) {
        QMessageBox::information(this, "Info", "Only students can add courses");
        return;
    }

    long courseId = item->data(Qt::UserRole).toLongLong();
    course* selectedCourse = Sys.getCourse(courseId);

    if (!selectedCourse) return;
    if (currentStudent->hasCourse(courseId)) {
        QMessageBox::warning(this, "Warning",
            "Course is already in your plan");
        return;

    }
    else if (selectedCourse->getCreditHours() > currentStudent->max_credit_hours && selectedCourse->checkPrerequisites(*currentStudent)) {
    
        QMessageBox::warning(this, "Warning",
            "Your Credit Hours Isn`t Enough");
        return;
    }
    else if (selectedCourse->getCreditHours() > currentStudent->max_credit_hours && !selectedCourse->checkPrerequisites(*currentStudent)) {
        QMessageBox::warning(this, "Warning",
            "-Your Credit Hours Isn`t Enough. \n-You Dont`t Meet The Right Prerequisites.");
        return;

    }
    // Check prerequisites
    else if (selectedCourse->checkPrerequisites(*currentStudent) && selectedCourse->getCreditHours() <= currentStudent->max_credit_hours) {
        // Add to student's course list (without enrolling)
        if (currentStudent->addCourseToPlan(*selectedCourse)) {
            Sys.saveData(); // Save changes
            QMessageBox::information(this, "Success",
                QString("Course %1 added to your plan").arg(selectedCourse->getTitle().c_str()));
            return;

        }
        else {
            QMessageBox::warning(this, "Error",
                "Undefined Behavior");
            return;

        }
    }
    else {
        // Show missing prerequisites
        QString missingPrereqs;
        for (const auto& prereq : selectedCourse->getPrerequisites()) {
            if (!currentStudent->hasCompletedCourse(prereq.getCourseID())) {
                missingPrereqs += QString("\n- %1").arg(prereq.getTitle().c_str());
            }
        }

        QMessageBox::warning(this, "Prerequisites Not Met",
            QString("You need to complete these courses first:%1").arg(missingPrereqs));
    }

    // Clear search
    ui.searchBar->clear();
    ui.searchResultsList->hide();
}


void Coursify::resetUIForLogin() {
    // Clear all UI elements
    clearAllListsAndFields();

    // Reset combo boxes
    ui.combo_course->clear();
    ui.combo_choose->clear();
    ui.combo_pre->clear();

    // Reload data based on current user type
    if (dynamic_cast<student*>(currentUser)) {
        refreshStudentUI();
    }
    else if (dynamic_cast<admin*>(currentUser)) {
        refreshAdminUI();
    }
}

void Coursify::refreshStudentUI() {
    student* currentStudent = dynamic_cast<student*>(currentUser);
    if (!currentStudent) return;

    // Refresh student panel components
    Sys.showCourseComboBox(ui.combo_choose);
    Sys.loadCoursePrereqsToListWidget(ui.combo_choose, ui.list_showpreq);

    // Refresh grades list
    QStringList grades = Sys.getStudentGrades(currentStudent);
    ui.listWidget_g->clear();
    ui.listWidget_g->addItems(grades);

    // Refresh report tab
    QStringList report = Sys.getStudentCourseReport(currentStudent);
    ui.listWidget_r->clear();
    ui.listWidget_r->addItems(report);
}

void Coursify::refreshAdminUI() {
    // Refresh admin panel components
    Sys.showCourseComboBox(ui.combo_course);
    Sys.showCourseComboBox(ui.combo_pre);
    Sys.loadCoursePrereqsToListWidget(ui.combo_course, ui.list_prereq);
}
QString Coursify::highlightMatchingChars(const QString& text, const QString& searchTerm) {
    return text;
}
void Coursify::clearAllListsAndFields() {
    // Clear all list widgets
    ui.list_prereq->clear();
    ui.list_showpreq->clear();
    ui.listWidget_g->clear();
    ui.listWidget_r->clear();
    ui.searchResultsList->clear();

    // Clear input fields
    ui.login_username->clear();
    ui.login_password->clear();
    ui.Register_S1->clear();
    ui.Register_S2->clear();
    ui.Register_S3->clear();
    ui.Register_S4->clear();
    ui.Register_A1->clear();
    ui.Register_A2->clear();
    ui.Register_A3->clear();
    ui.Register_A4->clear();
    ui.Register_A5->clear();

    // Hide search results
    ui.searchResultsList->hide();
}