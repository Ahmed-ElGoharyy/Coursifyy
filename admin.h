#pragma once
#include "user.h"
#include "course.h"
#include "student.h"
#include <unordered_map>
#include <list>
#include <string>
#include <vector>

class admin : public user
{
private:
    long adminID;

public:
    static long counter;

    admin();
    admin(long id);
    admin(std::string username, std::string password, std::string name, std::string email);
    admin(std::string username, std::string password, std::string name);  //made by gohary
    ~admin();

    // Getters and Setters
    void setAdminID(long id);
    long getAdminID() const;

    // Admin Functions
    bool uploadCourseDescription(course& course, std::string courseID);
    bool setPrerequisites(course& course, std::vector<class course*> prerequisites);
    bool manageGrades(student& student, grade& newGrade, std::string courseID);
    bool uploadGradesCSV(std::unordered_map <std::string, student> students, std::list <std::pair<course, grade>> courses);
};