#pragma once
#include <iostream>
#include <list>
#include <unordered_map>
#include <vector>
#include "course.h"
#include "student.h"
#include "grade.h"
#include "user.h"

using namespace std;

class admin : public user {
private:
    long adminID;

public:
    static long counter;

    admin();
    admin(long adminID);
    admin(string username, string password, string name, string email);
    ~admin();

    long getAdminID() const { return adminID; }
    void setAdminID(long id) { adminID = id; }

    bool uploadCourseDescription(course& course, string courseID);
    bool setPrerequisites(class course& course, vector <class course*> prerequisites);
    bool manageGrades(student& student, grade& grade, string courseID);
    bool uploadGradesCSV(unordered_map <string, student> students, list <pair<course, grade>> courses);
};
