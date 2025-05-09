// Implementation file
#include "stdafx.h"
#include "courseSystem.h"
#include "FileUtils.h"
#include <algorithm>
#include <iostream>

// Constructor
courseSystem::courseSystem() {
    // Initialize with empty collections
}

// Destructor
courseSystem::~courseSystem() {
    // Save data upon destruction
    saveData();
}

// Register a new student
bool courseSystem::registerStudent(const std::string& username, const std::string& password,
    const std::string& name, const std::string& email) {
    try {
        // Check if username is already in use
        for (const auto& pair : students) {
            if (pair.second.getUsername() == username) {
                return false; // Username already exists
            }
        }

        // Create new student
        student newStudent(username, password, name, email);

        // Add to the collection
        students[newStudent.getStudentID()] = newStudent;

        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error registering student: " << e.what() << std::endl;
        return false;
    }
}

// Register a new admin
bool courseSystem::registerAdmin(const std::string& username, const std::string& password,
    const std::string& name, const std::string& email) {
    try {
        // Check if username is already in use
        for (const auto& a : admins) {
            if (a.getUsername() == username) {
                return false; // Username already exists
            }
        }

        // Create new admin
        admin newAdmin(username, password, name, email);

        // Add to the collection
        admins.push_back(newAdmin);

        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error registering admin: " << e.what() << std::endl;
        return false;
    }
}

// Authenticate a user by username and password
bool courseSystem::authenticateUser(const std::string& username, const std::string& password, user*& loggedUser) {
    // First check students
    for (auto& pair : students) {
        if (pair.second.getUsername() == username) {
            if (pair.second.authenticate(password)) {
                loggedUser = &pair.second;
                return true;
            }
            return false; // Password incorrect
        }
    }

    // Then check admins
    for (auto& a : admins) {
        if (a.getUsername() == username) {
            if (a.authenticate(password)) {
                loggedUser = &a;
                return true;
            }
            return false; // Password incorrect
        }
    }

    return false; // User not found
}

// Login with username and password
bool courseSystem::login(const std::string& username, const std::string& password, user*& loggedUser) {
    return authenticateUser(username, password, loggedUser);
}

// Add a new course
bool courseSystem::addCourse(const course& newCourse) {
    long courseID = newCourse.getCourseID();

    // Check if course already exists
    if (courses.find(courseID) != courses.end()) {
        return false; // Course ID already exists
    }

    // Add the course
    courses[courseID] = newCourse;
    return true;
}

// Update an existing course
bool courseSystem::updateCourse(long courseID, const course& updatedCourse) {
    // Check if course exists
    if (courses.find(courseID) == courses.end()) {
        return false; // Course not found
    }

    // Update the course (preserving the original ID)
    courses[courseID] = updatedCourse;

    return true;
}

// Remove a course
bool courseSystem::removeCourse(long courseID) {
    // Check if course exists
    auto it = courses.find(courseID);
    if (it == courses.end()) {
        return false; // Course not found
    }

    // Remove the course
    courses.erase(it);
    return true;
}

// Get a course by ID
course* courseSystem::getCourse(long courseID) {
    auto it = courses.find(courseID);
    if (it != courses.end()) {
        return &it->second;
    }
    return nullptr; // Course not found
}

// Add a new student
bool courseSystem::addStudent(const student& newStudent) {
    long studentID = newStudent.getStudentID();

    // Check if student already exists
    if (students.find(studentID) != students.end()) {
        return false; // Student ID already exists
    }

    // Add the student
    students[studentID] = newStudent;
    return true;
}

// Update an existing student
bool courseSystem::updateStudent(long studentID, const student& updatedStudent) {
    // Check if student exists
    if (students.find(studentID) == students.end()) {
        return false; // Student not found
    }

    // Update the student
    students[studentID] = updatedStudent;
    return true;
}

// Get a student by ID
student* courseSystem::getStudent(long studentID) {
    auto it = students.find(studentID);
    if (it != students.end()) {
        return &it->second;
    }
    return nullptr; // Student not found
}

// Get a student by username
student* courseSystem::getStudentByUsername(const std::string& username) {
    for (auto& pair : students) {
        if (pair.second.getUsername() == username) {
            return &pair.second;
        }
    }
    return nullptr; // Student not found
}

// Add a new admin
bool courseSystem::addAdmin(const admin& newAdmin) {
    // Check if username is already used
    for (const auto& a : admins) {
        if (a.getUsername() == newAdmin.getUsername()) {
            return false; // Username already exists
        }
    }

    // Add the admin
    admins.push_back(newAdmin);
    return true;
}

// Get an admin by username
admin* courseSystem::getAdminByUsername(const std::string& username) {
    for (auto& a : admins) {
        if (a.getUsername() == username) {
            return &a;
        }
    }
    return nullptr; // Admin not found
}

// Load data from files
bool courseSystem::loadData() {
    FileUtils::initializePaths();
    return FileUtils::loadAllData(*this);
}

// Save data to files
bool courseSystem::saveData() {
    return FileUtils::saveAllData(*this);
}