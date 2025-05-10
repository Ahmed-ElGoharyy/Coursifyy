#pragma once
#include <string>
#include <stdexcept>
#include <cctype>

class grade {
private:
    int Semester;
    char Grade;
    int year;
    float gpa;

public:
    grade();
    grade(int Semster, char grade, int year);

    int getSemester() const { return Semester; }
    char getGrade() const { return Grade; }
    int getYear() const { return year; }
    float getGPA() const { return gpa; }

    void setSemester(int sem) { Semester = sem; }
    void setYear(int yr) { year = yr; }
    void setGrade(char newGrade);
    void setGPA(float newGPA);

    static float calculateGPAFromGrade(char letterGrade);
};