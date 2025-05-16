#include "stdafx.h"

// grade.cpp
#include "grade.h"
using namespace std;


grade::grade() : Semester(1), Grade('N'), year(2023), gpa(0.0f) {}

grade::grade(int Semester, char Grade, int year)
    : Semester(Semester), Grade(toupper(Grade)), year(year) {
    this->gpa = calculateGPAFromGrade(this->Grade);
}

void grade::setGrade(char newGrade) {
    Grade = toupper(newGrade);
    gpa = calculateGPAFromGrade(Grade);
}

void grade::setGPA(float gpatemp)
{
    this->gpa == gpatemp;
}

void grade::GetGradeFromGPA(float newGPA) {
    gpa = newGPA;
    // Update letter grade based on GPA
    if (gpa >= 3.5f) Grade = 'A';
    else if (gpa >= 2.5f) Grade = 'B';
    else if (gpa >= 1.5f) Grade = 'C';
    else if (gpa >= 0.5f) Grade = 'D';
    else Grade = 'F';
}

float grade::calculateGPAFromGrade(char letterGrade) {
    letterGrade = toupper(letterGrade);
    switch (letterGrade) {
    case 'A': return 4.0f;
    case 'B': return 3.0f;
    case 'C': return 2.0f;
    case 'D': return 1.0f;
    case 'F': return 0.0f;
    default: return 0.0f;
    }
}