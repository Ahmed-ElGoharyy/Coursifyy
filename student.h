#pragma once
#ifndef STUDENT_H
#define STUDENT_H
#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include "course.h"
#include "grade.h"
#include "user.h"

class FileUtils; // Added for file access

using namespace std;

class student : public user {
private:
    // Student-specific attributes that aren't exposed directly
    long StudentID;   // courseID -> grade
    float gpa;

public:
    static long counter; // Static counter for auto-incrementing IDs
    map<long, course> courses;  // courseID -> course
    map<long, grade> grades;
    friend class FileUtils; // Give FileUtils direct access

    // Constructors
    student();
    student(string username, string password, string name, string email) noexcept(false);

    // Course and grade management
    course searchCourse(string courseName) const noexcept(false);
    bool registerCourse(course newCourse) noexcept(false);
    map<course, grade> getGrades() const noexcept(false);
    float calculateGPA() noexcept(false);
    bool generateReport() const noexcept(false);
    bool updateGrade(long courseID, const grade& newGrade);
    bool hasCourse(long courseID) const;

    // Getters
    long getStudentID() const { return StudentID; }
    float getGPA() const { return gpa; }
    const map<long, course>& getCourses() const { return courses; }
    const map<long, grade>& getGradeMap() const { return grades; }

    // Exception class
    class student_exception : public runtime_error {
    public:
        student_exception(const string& msg) : runtime_error(msg) {}
    };
};

// Need to specify how to compare course objects for map usage
namespace std {
    template<>
    struct less<course> {
        bool operator()(const course& lhs, const course& rhs) const {
            return lhs.getCourseID() < rhs.getCourseID();
        }
    };
}

#endif // STUDENT_H