#include "stdafx.h"
#include "grade.h"

grade::grade() : Semester(0), Grade('N'), year(0), gpa(0.0f) {
}

grade::grade(int Semster, char grade, int year) :
    Semester(Semster), year(year) {
    setGrade(grade);
}

void grade::setGrade(char newGrade) {
    newGrade = toupper(newGrade);
    if (newGrade != 'A' && newGrade != 'B' && newGrade != 'C' &&
        newGrade != 'D' && newGrade != 'F' && newGrade != 'N') {
        throw std::invalid_argument("Invalid grade. Must be A, B, C, D, F, or N (no grade)");
    }
    Grade = newGrade;
    gpa = calculateGPAFromGrade(newGrade);
}

void grade::setGPA(float newGPA) {
    if (newGPA < 0.0f || newGPA > 4.0f) {
        throw std::invalid_argument("GPA must be between 0.0 and 4.0");
    }
    gpa = newGPA;
}

float grade::calculateGPAFromGrade(char letterGrade) {
    switch (toupper(letterGrade)) {
    case 'A': return 4.0f;
    case 'B': return 3.0f;
    case 'C': return 2.0f;
    case 'D': return 1.0f;
    case 'F': return 0.0f;
    case 'N': return 0.0f;
    default: throw std::invalid_argument("Invalid grade for GPA calculation");
    }
}