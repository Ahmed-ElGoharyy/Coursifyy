#pragma once
#include <string>
#include <map>
#include <vector>
#include "course.h"
#include "student.h"
#include "admin.h"

class FileUtils; // Forward declaration

class courseSystem {
    friend class FileUtils; // Allow FileUtils to access private members for loading/saving

private:
    std::map<long, student> students;    // studentID -> student
    std::map<long, course> courses;      // courseID -> course
    std::vector<admin> admins;           // List of administrators

    // Internal helpers
    bool authenticateUser(const std::string& username, const std::string& password, user*& loggedUser);

public:
    courseSystem();
    ~courseSystem();

    // User management
    bool registerStudent(const std::string& username, const std::string& password,
        const std::string& name, const std::string& email);
    bool registerAdmin(const std::string& username, const std::string& password,
        const std::string& name, const std::string& email);
    bool login(const std::string& username, const std::string& password, user*& loggedUser);

    // Course management
    bool addCourse(const course& newCourse);
    bool updateCourse(long courseID, const course& updatedCourse);
    bool removeCourse(long courseID);
    course* getCourse(long courseID);

    // Student management
    bool addStudent(const student& newStudent);
    bool updateStudent(long studentID, const student& updatedStudent);
    student* getStudent(long studentID);
    student* getStudentByUsername(const std::string& username);

    // Admin management
    bool addAdmin(const admin& newAdmin);
    admin* getAdminByUsername(const std::string& username);

    // Data access for file operations
    const std::map<long, student>& getAllStudents() const { return students; }
    const std::map<long, course>& getAllCourses() const { return courses; }
    const std::vector<admin>& getAllAdmins() const { return admins; }

    // File operations
    bool loadData();
    bool saveData();
};