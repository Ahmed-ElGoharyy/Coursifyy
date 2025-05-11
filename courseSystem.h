#include "user.h"
#include "course.h"
#include "admin.h"
#include "student.h"
#include "stdafx.h"
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

    bool registerAdmin(const std::string& username, const std::string& password,
        const std::string& name, const std::string& email);
    bool login(const std::string& username, const std::string& password, user*& loggedUser);
    bool addCourse(const course& newCourse);
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
};