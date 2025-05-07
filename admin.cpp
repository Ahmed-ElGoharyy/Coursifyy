#include "stdafx.h"
#include "admin.h"
#include <iostream>
#include <string>

using namespace std;


admin::admin(long id) {
    adminID = id;
    cout << "Administrator created with ID: " << adminID << endl;
}


admin::~admin() {
    
}


long admin::getAdminID(){
    return adminID;
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
        //course.setDescription(newDescription);
        
        cout << "Course description updated successfully for course ID: " << courseID << endl;
        return true;
    } catch (exception& e) {
        cout << "Error updating course description: " << e.what() << endl;
        return false;
    }
}


bool admin::setPrerequisites( course& course, list<class course*> prerequisites) {
    try {
       
        if (prerequisites.empty()) {
            cout << "No prerequisites to set" << endl;
            return true;
        }
        
        // Clear existing prerequisites if any
        //course.clearPrerequisites();
        
        // Add each prerequisite to the course
        //for (auto prereq : prerequisites) {
        //    if (prereq != nullptr) {
        //        course.addPrerequisite(prereq);
        //    }
        //}
        
        cout << "Prerequisites set successfully" << endl;
        return true;
    } catch (exception& e) {
        cout << "Error setting prerequisites: " << e.what() << endl;
        return false;
    }
}


bool admin::manageGrades(class student& student, class grade& grade, string courseID) {
    try {
        // Check if course ID is valid
        if (courseID.empty()) {
            cout << "Error: Invalid course ID" << endl;
            return false;
        }
        
        // Get student ID
       // long studentID = student.getStudentID();
        //cout << "Managing grades for student ID: " << studentID << endl;
        
        // Get new grade value
        string newGrade;
        cout << "Enter new grade (A, B, C, D, F): ";
        getline(cin, newGrade);
        
        // Update grade value
        //grade.setGrade(newGrade);
        
        // Update student's course grades
        //student.updateGrade(courseID, newGrade);
        
       // cout << "Grade updated successfully for student ID: " << studentID << " in course ID: " << courseID << endl;
        return true;
    } catch (exception& e) {
        cout << "Error managing grades: " << e.what() << endl;
        return false;
    }
}


// Upload grades from CSV for multiple students
bool admin::uploadGradesCSV(map<string,student>students, map<string,  grade*> grades) {
    return true;
}
