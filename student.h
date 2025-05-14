#pragma once
#ifndef STUDENT_H
#define STUDENT_H

#include <iostream>
#include <string>
#include <list>
#include <map>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include "course.h"
#include "grade.h"
#include "user.h"


using namespace std;

class student : public user {
private:
    long StudentID;
    float gpa;
    string currentSemester;

public:
    static long counter; // Static counter for auto-incrementing IDs
    list<pair<course, grade>> courses;
    int max_credit_hours;

    // Constructors
public:
    student();
    student(string username, string password, string name, string email) noexcept(false);
    student(string username, string password, string name) noexcept(false);
    student(string username, string password, string name, string email, string semester) noexcept(false);

    // Course and grade management
    course searchCourse(string courseName) const noexcept(false);
    pair<course, grade> getonegrade(string courseName) const noexcept(false);
    string getCurrentSemester() const { return currentSemester; }
    void setCurrentSemester(const string& semester) { currentSemester = semester; }
    bool registerCourse(course newCourse) noexcept(false);
    list<pair<course, grade>> getGrades() const noexcept(false);
    float calculateGPA() noexcept(false);
    bool changeSemester(const string& newSemester) {
        currentSemester = newSemester;
        return true;
    }
    bool generateReport();
    bool updateGrade(long courseID, const grade& newGrade);
    bool hasCourse(long courseID) const;
    bool hasCompletedCourse(long courseID) const;
    int modifycredithours(course c);

    // Getters
    long getStudentID() const { return StudentID; }
    float getGPA() const { return gpa; }
    void setGPA(float x) { gpa = x; }
    void setID(long id) { StudentID = id; }
    list<pair<course, grade>> getCourses() const { return courses; }
    bool addCourseToPlan(const course& courseToAdd);

    // Exception class
    class student_exception : public runtime_error {
    public:
        student_exception(const string& msg) : runtime_error(msg) {}
    };
};

namespace std {
    template<>
    struct less<course> {
        bool operator()(const course& lhs, const course& rhs) const {
            return lhs.getCourseID() < rhs.getCourseID();
        }
    };
}


#endif // STUDENT_H