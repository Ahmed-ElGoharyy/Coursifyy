#include "courseSystem.h"
#include "stdafx.h"
#include "course.h"
#include "user.h"
#include "admin.h"
#include "student.h"
#include "FileUtils.h"

courseSystem::courseSystem() {
    FileUtils::initializePaths();
    if (!loadData()) {
        if (admins.empty()) {
            admin defaultAdmin("admin", "admin123", "Administrator", "admin@school.edu");
            admins[defaultAdmin.getAdminID()] = defaultAdmin;
            saveData();
        }
    }
}

courseSystem::~courseSystem() {
    if (!students.empty() || !courses.empty() || !admins.empty()) {
        saveData();
    }
}

bool courseSystem::registerStudent(const std::string& username, const std::string& password,
    const std::string& name, const std::string& email) {
    try {
        for (const auto& pair : students) {
            if (pair.second.getUsername() == username) {
                return false;
            }
        }

        student newStudent(username, password, name, email);
        if (newStudent.isValidUsername(username) &&
            newStudent.isValidPassword(password) &&
            newStudent.isValidEmail(email)) {

            students[newStudent.getStudentID()] = newStudent;
            saveData();
            return true;
        }
        else
            return false;
    }
    catch (const std::exception& e) {
        std::cerr << "Error registering student: " << e.what() << std::endl;
        return false;
    }
}

bool courseSystem::registerAdmin(const std::string& username, const std::string& password,
    const std::string& name, const std::string& email) {
    try {
        for (const auto& pair : admins) {
            if (pair.second.getUsername() == username) {
                return false;
            }
        }

        admin newAdmin(username, password, name, email);
        if (newAdmin.isValidUsername(username) &&
            newAdmin.isValidPassword(password) &&
            newAdmin.isValidEmail(email)) {

            admins[newAdmin.getAdminID()] = newAdmin;
            saveData();
            return true;
        }
        else
            return false;
    }
    catch (const std::exception& e) {
        std::cerr << "Error registering admin: " << e.what() << std::endl;
        return false;
    }
}

bool courseSystem::authenticateUser(const std::string& username, const std::string& password, user*& loggedUser) {
    for (auto& pair : students) {
        if (pair.second.getUsername() == username) {
            if (pair.second.authenticate(password)) {
                loggedUser = &pair.second;
                return true;
            }
            return false;
        }
    }


    for (auto& pair : admins) {
        if (pair.second.getUsername() == username) {
            if (pair.second.authenticate(password)) {
                loggedUser = &pair.second;
                return true;
            }
            return false;
        }
    }

    return false;
}



bool courseSystem::login(const std::string& username, const std::string& password, user*& loggedUser) {
    return authenticateUser(username, password, loggedUser);
}

bool courseSystem::addCourse(const course& newCourse) {
    long courseID = newCourse.getCourseID();
    if (courses.find(courseID) != courses.end()) {
        return false;
    }

    courses[courseID] = newCourse;
    saveData();
    return true;
}

bool courseSystem::updateCourse(long courseID, const course& updatedCourse) {
    if (courses.find(courseID) == courses.end()) {
        return false;
    }

    courses[courseID] = updatedCourse;
    saveData();
    return true;
}

bool courseSystem::removeCourse(long courseID) {
    auto it = courses.find(courseID);
    if (it == courses.end()) {
        return false;
    }

    courses.erase(it);
    saveData();
    return true;
}

course* courseSystem::getCourse(long courseID) {
    auto it = courses.find(courseID);
    if (it != courses.end()) {
        return &it->second;
    }
    return nullptr;
}

std::vector<course> courseSystem::searchCourses(const std::string& searchTerm) const {
    std::vector<course> results;
    std::string lowerSearchTerm = searchTerm;
    std::transform(lowerSearchTerm.begin(), lowerSearchTerm.end(), lowerSearchTerm.begin(),
        [](unsigned char c) { return std::tolower(c); });

    for (const auto& pair : courses) {
        const course& c = pair.second;
        std::string lowerTitle = c.getTitle();
        std::string lowerDesc = c.getDescription();
        std::string lowerInstructor = c.getInstructor();

        std::transform(lowerTitle.begin(), lowerTitle.end(), lowerTitle.begin(),
            [](unsigned char c) { return std::tolower(c); });
        std::transform(lowerDesc.begin(), lowerDesc.end(), lowerDesc.begin(),
            [](unsigned char c) { return std::tolower(c); });
        std::transform(lowerInstructor.begin(), lowerInstructor.end(), lowerInstructor.begin(),
            [](unsigned char c) { return std::tolower(c); });

        if (lowerTitle.find(lowerSearchTerm) != std::string::npos ||
            lowerDesc.find(lowerSearchTerm) != std::string::npos ||
            lowerInstructor.find(lowerSearchTerm) != std::string::npos) {
            results.push_back(c);
        }
    }

    return results;
}

bool courseSystem::addStudent(const student& newStudent) {
    long studentID = newStudent.getStudentID();
    if (students.find(studentID) != students.end()) {
        return false;
    }

    students[studentID] = newStudent;
    saveData();
    return true;
}

bool courseSystem::updateStudent(long studentID, const student& updatedStudent) {
    if (students.find(studentID) == students.end()) {
        return false;
    }

    students[studentID] = updatedStudent;
    saveData();
    return true;
}

student* courseSystem::getStudent(long studentID) {
    auto it = students.find(studentID);
    if (it != students.end()) {
        return &it->second;
    }
    return nullptr;
}

student* courseSystem::getStudentByUsername(const std::string& username) {
    for (auto& pair : students) {
        if (pair.second.getUsername() == username) {
            return &pair.second;
        }
    }
    return nullptr;
}

bool courseSystem::addAdmin(const admin& newAdmin) {
    long adminID = newAdmin.getAdminID();
    if (admins.find(adminID) != admins.end()) {
        return false;
    }

    for (const auto& pair : admins) {
        if (pair.second.getUsername() == newAdmin.getUsername()) {
            return false;
        }
    }

    admins[adminID] = newAdmin;
    saveData();
    return true;
}

admin* courseSystem::getAdminByUsername(const std::string& username) {
    for (auto& pair : admins) {
        if (pair.second.getUsername() == username) {
            return &pair.second;
        }
    }
    return nullptr;
}

admin* courseSystem::getAdmin(long adminID) {
    auto it = admins.find(adminID);
    if (it != admins.end()) {
        return &it->second;
    }
    return nullptr;
}

bool courseSystem::updateStudentGrade(long studentID, long courseID, const grade& newGrade) {
    student* s = getStudent(studentID);
    if (!s) {
        return false;
    }

    course* c = getCourse(courseID);
    if (!c) {
        return false;
    }

    bool result = s->updateGrade(courseID, newGrade);
    if (result) {
        saveData();
    }
    return result;
}

bool courseSystem::enrollStudentInCourse(long studentID, long courseID) {
    student* s = getStudent(studentID);
    course* c = getCourse(courseID);

    if (!s || !c) return false;
    if (!c->checkPrerequisites(*s)) {
        return false;
    }

    try {
        if (s->registerCourse(*c)) {
            c->setEnrolled(true);
            saveData();
            return true;
        }
    }
    catch (...) {
        return false;
    }
    return false;
}


bool courseSystem::loadData() {
    return FileUtils::loadAllData(*this);
}



bool courseSystem::saveData() {
    return FileUtils::saveAllData(*this);
}