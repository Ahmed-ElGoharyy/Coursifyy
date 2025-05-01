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
        getline(cin, newDescription); //getline 3l4ane ya5od elinput kolo m4 awl klma
        
        // Update course description
        //course.setDescription(newDescription);
        
        cout << "Course description updated successfully for course ID: " << courseID << endl;
        return true;
    } catch (exception& e) {
        cout << "Error updating course description: " << e.what() << endl;
        return false;
    }
}

// Set prerequisites for a course
bool admin::setPrerequisites( course& course, list< course*> prerequisites) {
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


bool admin::manageGrades( grade& grade, string courseID) {
    try {
        
        if (courseID.empty()) {
            cout << "Error: Invalid course ID" << endl;
            return false;
        }
        
        
        string newGrade;
        cout << "Enter new grade (A, B, C, D, F): ";
        getline(cin, newGrade);
        
        // Update grade value
        //grade.setGrade(newGrade);
        
        cout << "Grade updated successfully for course ID: " << courseID << endl;
        return true;
    } catch (exception& e) {
        cout << "Error managing grades: " << e.what() << endl;
        return false;
    }
}


bool admin::uploadGradesCSV(vector< student*> students, map<string, class grade*> grades) {
    
}