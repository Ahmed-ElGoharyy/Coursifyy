#include "stdafx.h"
#include "admin.h"
#include <iostream>
#include <string>
using namespace std;

long admin::counter = 1; // Start at 1 instead of 0

admin::admin() {
    adminID = counter;
    counter += 1; // Explicitly increment by 1
    setRole('A');
    cout << "Default Administrator created with ID: " << adminID << " (counter now: " << counter << ")" << endl;
}

admin::admin(long id) : adminID(id) {
    setRole('A');
    if (id >= counter) {
        counter = id + 1; // Ensure counter is always higher than any existing ID
    }
    cout << "Administrator created with ID: " << adminID << " (counter now: " << counter << ")" << endl;
}

admin::admin(string username, string password, string name, string email)
    : user(username, password, name, email, 'A') {
    adminID = counter;
    counter += 1; // Explicitly increment by 1
    cout << "Administrator created with ID: " << adminID << " (counter now: " << counter << ")" << endl;
}

admin::admin(string username, string password, string name)
    : user(username, password, name, " null", 'A') {
    adminID = counter;
    counter += 1; // Explicitly increment by 1
    cout << "Administrator created with ID: " << adminID << " (counter now: " << counter << ")" << endl;
}

admin::~admin() {
}

void admin::setAdminID(long id) {
    adminID = id;
    // Update counter if needed to avoid ID collisions in future
    if (id >= counter) {
        counter = id + 1;
        cout << "Admin counter updated to: " << counter << " from ID: " << id << endl;
    }
}

long admin::getAdminID() const {
    return adminID;
}

// Rest of the methods remain unchanged
bool admin::uploadCourseDescription(course& course, string courseID) {
    try {
        if (courseID.empty()) {
            cout << "Error: Invalid course ID" << endl;
            return false;
        }
        string newDescription;
        cout << "Enter new course description: ";
        cin >> newDescription;
        course.setDescription(newDescription);
        cout << "Course description updated successfully for course ID: " << courseID << endl;
        return true;
    }
    catch (exception& e) {
        cout << "Error updating course description: " << e.what() << endl;
        return false;
    }
}

bool admin::setPrerequisites(course& course, vector<class course*> prerequisites) {
    try {
        if (prerequisites.empty()) {
            cout << "No prerequisites to set" << endl;
            return true;
        }
        course.clearPrerequisites();
        for (auto prereq : prerequisites) {
            if (prereq != nullptr) {
                course.addPrerequisite(*prereq);
            }
        }
        cout << "Prerequisites set successfully" << endl;
        return true;
    }
    catch (exception& e) {
        cout << "Error setting prerequisites: " << e.what() << endl;
        return false;
    }
}

bool admin::manageGrades(student& student, grade& newGrade, string courseID) {
    try {
        if (courseID.empty()) {
            cout << "Error: Invalid course ID" << endl;
            return false;
        }
        long courseIDLong;
        try {
            courseIDLong = stol(courseID);
        }
        catch (...) {
            cout << "Error: Invalid course ID format" << endl;
            return false;
        }
        long studentID = student.getStudentID();
        cout << "Managing grades for student ID: " << studentID << endl;
        if (student.updateGrade(courseIDLong, newGrade)) {
            cout << "Grade updated successfully for student ID: " << studentID
                << " in course ID: " << courseID << endl;
            return true;
        }
        else {
            cout << "Student is not enrolled in this course" << endl;
            return false;
        }
    }
    catch (exception& e) {
        cout << "Error managing grades: " << e.what() << endl;
        return false;
    }
}

bool admin::uploadGradesCSV(unordered_map <string, student> students, list <pair<course, grade>> courses) {
    return true;
}