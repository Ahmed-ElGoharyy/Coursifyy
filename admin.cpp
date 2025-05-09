// Implementation
#include "stdafx.h"
#include "admin.h"
#include <iostream>
#include <string>
#include <functional> // For std::hash

using namespace std;

// Initialize the static counter
long admin::counter = 0;

// Default constructor
admin::admin() : adminID(++counter) {
    setRole('A');
}

// Constructor with ID
admin::admin(long id) : adminID(id) {
    setRole('A');

    // Update counter if needed
    if (id >= counter) {
        counter = id + 1;
    }

    cout << "Administrator created with ID: " << adminID << endl;
}

// Full constructor
admin::admin(string username, string password, string name, string email)
    : user(username, password, name, email, 'A') {

    // Generate a consistent ID based on username hash
    adminID = static_cast<long>(std::hash<string>{}(username));

    // Update counter to be greater than this ID
    if (adminID >= counter) {
        counter = adminID + 1;
    }

    cout << "Administrator created with ID: " << adminID << endl;
}

admin::~admin() {
    // Destructor
}

bool admin::uploadCourseDescription(course& course, string courseID) {
    try {
        if (courseID.empty()) {
            cout << "Error: Invalid course ID" << endl;
            return false;
        }

        string newDescription;
        cout << "Enter new course description: ";
        cin >> newDescription;

        // Update course description
        course.setDescription(newDescription);

        cout << "Course description updated successfully for course ID: " << courseID << endl;
        return true;
    }
    catch (exception& e) {
        cout << "Error updating course description: " << e.what() << endl;
        return false;
    }
}

bool admin::setPrerequisites(course& course, list<class course*> prerequisites) {
    try {
        if (prerequisites.empty()) {
            cout << "No prerequisites to set" << endl;
            return true;
        }

        // Clear existing prerequisites
        course.clearPrerequisites();

        // Add each prerequisite to the course
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
        // Check if course ID is valid
        if (courseID.empty()) {
            cout << "Error: Invalid course ID" << endl;
            return false;
        }

        // Convert courseID string to long
        long courseIDLong;
        try {
            courseIDLong = stol(courseID);
        }
        catch (...) {
            cout << "Error: Invalid course ID format" << endl;
            return false;
        }

        // Get student ID
        long studentID = student.getStudentID();
        cout << "Managing grades for student ID: " << studentID << endl;

        // Update the student's grade for this course
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

bool admin::uploadGradesCSV(map<string, student> students, map<string, grade*> grades) {
    // Implementation omitted for now
    return true;
}