#include "stdafx.h"
#include "admin.h"
#include <iostream>
#include <string>

using namespace std;

long admin::counter = 0;

admin::admin() : adminID(++counter) {
    setRole('A');
}

admin::admin(long id) : adminID(id) {
    setRole('A');
    if (id >= counter) {
        counter = id + 1;
    }
    cout << "Administrator created with ID: " << adminID << endl;
}

admin::admin(string username, string password, string name, string email)
    : user(username, password, name, email, 'A') {
    adminID = counter++;
}

admin::~admin() {
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

bool admin::uploadGradesCSV(map<string, student> students, map<string, grade*> grades) {
    return true;
}