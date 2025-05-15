#include "user.h"
#include "course.h"
#include "admin.h"
#include "student.h"
#include "stdafx.h"
#include <QStringList>
#include <QString>

#pragma once
class courseSystem {
    friend class FileUtils;
private:
    std::unordered_map<std::string, student> students;
    std::map<long, course> courses;
    std::unordered_map<std::string, admin> admins;
    user* currentUser;

public:
    courseSystem();
    ~courseSystem();

    char authenticateUser(QLineEdit* usernameEdit, QLineEdit* passwordEdit, user*& loggedUser);

    bool registerStudent(QLineEdit* namee, QLineEdit* usernamee, QLineEdit* passwordd, QLineEdit* confirmpasswordd);

    bool registerAdmin(QLineEdit* namee, QLineEdit* usernamee, QLineEdit* passwordd, QLineEdit* confirmpasswordd, QLineEdit* keyedit);


    bool login(const std::string& username, const std::string& password, user*& loggedUser);
    bool addCourse(const course& newCourse);  // Remove the courses parameter
    bool updateCourse(long courseID, const course& updatedCourse);
    bool removeCourse(long courseID);
    course* getCourse(long courseID);
    std::vector<course> searchCourses(const std::string& searchTerm) const;
    bool addStudent(const student& newStudent);
    bool updateStudent(const std::string& username, const student& updatedStudent);
    student* getStudent(long studentID);
    student* getStudentByUsername(const std::string& username);
    bool addAdmin(const admin& newAdmin);
    admin* getAdminByUsername(const std::string& username);
    admin* getAdmin(long adminID);
    bool updateStudentGrade(const std::string& username, long courseID, const grade& newGrade);
    bool enrollStudentInCourse(const std::string& username, long courseID);
    const std::unordered_map<std::string, student>& getAllStudents() const { return students; }
    const std::map<long, course>& getAllCourses() const { return courses; }
    const std::unordered_map<std::string, admin>& getAllAdmins() const { return admins; }
    bool loadData();
    bool saveData();
    void importCoursesFromFile(QWidget* parent);
    bool importCoursesFromCSV(const QString& filePath);
     void showCourseComboBox(QComboBox* comboBox);
    vector <string> getCoursePrereqTitles(long courseId);
    void addPrerequisiteToList(QComboBox* mainCourseComboBox, QComboBox* prereqCourseComboBox, QListWidget* prereqListWidget, QWidget* parent);
    void removeSelectedPrerequisite(QComboBox* mainCourseComboBox, QListWidget* prereqListWidget, QWidget* parent);
    void loadCoursePrereqsToListWidget(QComboBox* courseComboBox, QListWidget* prereqListWidget);
   void assignGradeToStudent(QLineEdit* usernameEdit, QLineEdit* courseIdEdit, QComboBox* gradeComboBox, QWidget* parent);

public:
    bool addCourseToStudent(student* student, const course& courseToAdd);
    bool importGradesFromCSV(const QString& filePath);
    void importGradesFromFile(QWidget* parent);

    void showStudentCourseGrade(QComboBox* courseComboBox, QListWidget* gradesListWidget, QWidget* parent = nullptr);
    //void populateListFromReport(student* currentStudent);
    void populateListFromReport(student* currentStudent, QListWidget* listWidget, QWidget* parent);

   
    QStringList getStudentCourseReport(student* currentStudent);

    void populateListWidgetUI(student* currentStudent, QListWidget* listWidget);

};