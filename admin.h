#pragma once
#include "user.h"
#include "course.h"
#include "student.h"
#include <unordered_map>
#include <list>
#include <string>
#include <vector>

using namespace std;


class admin : public user
{
private:
    long adminID;

public:
    static long counter;

    admin();
    admin(long id);
    admin(string username, string password, string name, string email);
    admin(string username, string password, string name);  //made by gohary
    ~admin();

    // Getters and Setters
    void setAdminID(long id);
    long getAdminID() const;

    // Admin Functions
    bool uploadCourseDescription(course& course, string courseID);
    bool setPrerequisites(course& course, vector<class course*> prerequisites);
    bool manageGrades(student& student, grade& newGrade, string courseID);
    bool uploadGradesCSV(unordered_map <string, student> students, list <pair<course, grade>> courses);
};