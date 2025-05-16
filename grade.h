#pragma once
#include <string>
#include <stdexcept>
#include <cctype>

using namespace std;


class grade {
private:
    int Semester;
    char Grade;
    int year;
    float gpa;

public:
    grade();
    grade(int Semester, char grade, int year);

    int getSemester() const { return Semester; }
    char getGrade() const { return Grade; }
    int getYear() const { return year; }
    float getGPA() const { return gpa; }

    void setSemester(int sem) { Semester = sem; }
    void setYear(int yr) { year = yr; }
    void setGrade(char newGrade);
    void setGPA(float gpatemp);

    void GetGradeFromGPA(float newGPA);

    static float calculateGPAFromGrade(char letterGrade);
};