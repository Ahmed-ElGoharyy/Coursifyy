#pragma once
#include <iostream>
#include <list>
#include <map>
#include "course.h"
#include "student.h"
#include "grade.h"

using namespace std;

class admin
{
    long adminID;


    public:

       admin(long adminID);

       ~admin();
       long getAdminID();

    
       bool uploadCourseDescription(course& course ,string courseID);

       bool setPrerequisites(class course& course, list<class course*> prerequisites);

    bool manageGrades(student &student,grade& grade, string courseID);
       
       bool uploadGradesCSV(map<string,student>students, map<string, grade*> grades);

};
