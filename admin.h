#pragma once
#include <iostream>
#include <list>
#include <map>
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
    bool setPrerequisites(class course& course, list<class course*> prerequisites);
    bool manageGrades(student& student, grade& grade, string courseID);
    bool uploadGradesCSV(map<string, student> students, map<string, grade*> grades);
};