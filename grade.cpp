// Implementation
#include "stdafx.h"
#include "grade.h"


// Default constructor
grade::grade() : Semster(0), Grade('N'), year(0), gpa(0.0f) {
    // 'N' represents no grade assigned yet
}

// Parameterized constructor
grade::grade(int Semster, char grade, int year) :
    Semster(Semster), year(year) {

    // Set grade and automatically calculate GPA
    setGrade(grade);
}

// Set grade and update GPA accordingly
void grade::setGrade(char newGrade) {
    // Convert to uppercase
    newGrade = toupper(newGrade);

    // Validate grade
    if (newGrade != 'A' && newGrade != 'B' && newGrade != 'C' &&
        newGrade != 'D' && newGrade != 'F' && newGrade != 'N') {
        throw std::invalid_argument("Invalid grade. Must be A, B, C, D, F, or N (no grade)");
    }

    Grade = newGrade;
    gpa = calculateGPAFromGrade(newGrade);
}

// Set GPA directly
void grade::setGPA(float newGPA) {
    if (newGPA < 0.0f || newGPA > 4.0f) {
        throw std::invalid_argument("GPA must be between 0.0 and 4.0");
    }
    gpa = newGPA;
}

// Calculate GPA from letter grade
float grade::calculateGPAFromGrade(char letterGrade) {
    switch (toupper(letterGrade)) {
    case 'A': return 4.0f;
    case 'B': return 3.0f;
    case 'C': return 2.0f;
    case 'D': return 1.0f;
    case 'F': return 0.0f;
    case 'N': return 0.0f; // No grade assigned yet
    default: throw std::invalid_argument("Invalid grade for GPA calculation");
    }
}