#pragma once
#include <string>
#include <stdexcept>
#include <cctype>

class grade
{
private:
    int Semster;   // Semester number (1 or 2 typically)
    char Grade;    // Letter grade (A, B, C, D, F, or N for no grade)
    int year;      // Academic year
    float gpa;     // GPA value for this grade

public:
    grade();
    grade(int Semster, char grade, int year);

    // Getters
    int getSemester() const { return Semster; }
    char getGrade() const { return Grade; }
    int getYear() const { return year; }
    float getGPA() const { return gpa; }

    // Setters
    void setSemester(int sem) { Semster = sem; }
    void setYear(int yr) { year = yr; }
    void setGrade(char newGrade);
    void setGPA(float newGPA);

    // Calculate GPA from letter grade
    static float calculateGPAFromGrade(char letterGrade);
};