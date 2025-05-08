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
#include "course.h"

#include "grade.h"
#include "user.h"
using namespace std;

class student : public user {
public:
    static long counter; // Static counter for auto-incrementing IDs
    long StudentID;
    map<long, course> courses;  // courseID -> course
    map<long, grade> grades;    // courseID -> grade
    float gpa;

public:
    // Constructors
    student();
    student(string username, string password, string name, string email) noexcept(false);

    // Course and grade management
    course searchCourse(string courseName) noexcept(false);
    bool registerCourse(course newCourse) noexcept(false);
    map<course, grade> getGrades() const noexcept(false);  // Made const
    float calculateGPA() noexcept(false);
    bool generateReport() noexcept(false);
    bool updateGrade(long courseID, const grade& newGrade);
    bool hasCourse(long courseID) const;

    // Getters
    long getStudentID() const { return StudentID; }
    float getGPA() const { return gpa; }
    const map<long, course>& getCourses() const { return courses; }

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

