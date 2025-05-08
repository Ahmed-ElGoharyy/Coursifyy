#include "student.h"
#include "stdafx.h"
// Initialize the static counter
long student::counter = 0;

// Default constructor
student::student() : user(), StudentID(++counter), gpa(0.0f) {
}

// Parameterized constructor
student::student(string username, string password, string name, string email) noexcept(false)
    : user() {
    // Validate input parameters
    if (username.empty() || password.empty() || name.empty() || email.empty()) {
        throw student_exception("All student fields must be provided");
    }

    // Set user properties
    setUsername(username);
    setPassword(password);
    setName(name);
    setEmail(email);

    // Initialize student-specific properties
    StudentID = ++counter;
    gpa = 0.0f;
}

// Search for a course by name
course student::searchCourse(string courseName) noexcept(false) {
    for (const auto& pair : courses) {
        if (pair.second.getTitle() == courseName) {
            return pair.second;
        }
    }
    throw student_exception("Course not found: " + courseName);
}

// Register for a new course
bool student::registerCourse(course newCourse) noexcept(false) {
    long courseID = newCourse.getCourseID();

    // Check if already registered
    if (courses.find(courseID) != courses.end()) {
        throw student_exception("Already registered for this course");
    }

    // Add the course to the student's course list
    courses[courseID] = newCourse;

    // Initialize grade for this course (can be updated later)
    grades[courseID] = grade(); // Default grade

    return true;
}

// Get all grades - returns a map of course to grade pairs
map<course, grade> student::getGrades() const noexcept(false) {
    map<course, grade> result;

    for (const auto& pair : grades) {
        long courseID = pair.first;
        // Find the corresponding course object
        auto courseIter = courses.find(courseID);
        if (courseIter != courses.end()) {
            result[courseIter->second] = pair.second;
        }
    }

    return result;
}

// Calculate the student's GPA based on all grades
float student::calculateGPA() noexcept(false) {
    if (grades.empty()) {
        return 0.0f;
    }

    float totalPoints = 0.0f;
    int totalCredits = 0;

    for (const auto& pair : grades) {
        long courseID = pair.first;
        const grade& g = pair.second;

        // Skip courses with no grade assigned yet
        if (g.getGrade() == 'N') {
            continue;
        }

        // Find the course to get its credit hours
        auto courseIter = courses.find(courseID);
        if (courseIter != courses.end()) {
            int credits = courseIter->second.getCreditHours();
            totalPoints += g.getGPA() * credits;
            totalCredits += credits;
        }
    }

    if (totalCredits == 0) {
        return 0.0f;
    }

    // Update and return the calculated GPA
    this->gpa = totalPoints / totalCredits;
    return this->gpa;
}

// Generate a report of all courses and grades
bool student::generateReport() noexcept(false) {
    try {
        string filename = "report_" + to_string(StudentID) + ".txt";
        ofstream report(filename);

        if (!report.is_open()) {
            throw student_exception("Unable to create report file");
        }

        // Header
        report << "Student Report\n";
        report << "==============\n";
        report << "Name: " << getName() << "\n";
        report << "ID: " << StudentID << "\n";
        report << "Email: " << getEmail() << "\n";
        report << "GPA: " << fixed << setprecision(2) << calculateGPA() << "\n\n";

        // Course details
        report << "Courses:\n";
        report << setw(30) << left << "Course Title"
            << setw(10) << left << "Credits"
            << setw(10) << left << "Grade"
            << setw(10) << left << "Semester"
            << setw(10) << left << "Year" << "\n";
        report << string(70, '-') << "\n";

        for (const auto& pair : courses) {
            const course& c = pair.second;
            long courseID = c.getCourseID();

            // Get grade information if available
            char gradeChar = 'N';  // Default to 'N' for no grade
            int semester = 0;
            int year = 0;

            auto gradeIter = grades.find(courseID);
            if (gradeIter != grades.end()) {
                gradeChar = gradeIter->second.getGrade();
                semester = gradeIter->second.getSemester();
                year = gradeIter->second.getYear();
            }

            report << setw(30) << left << c.getTitle()
                << setw(10) << left << c.getCreditHours()
                << setw(10) << left << gradeChar
                << setw(10) << left << semester
                << setw(10) << left << year << "\n";
        }

        report.close();
        return true;
    }
    catch (const exception& e) {
        cout << "Error generating report: " << e.what() << endl;
        return false;
    }
}

// Update a grade for a specific course
bool student::updateGrade(long courseID, const grade& newGrade) {
    // Check if the student is registered for this course
    if (courses.find(courseID) == courses.end()) {
        return false;
    }

    // Update the grade
    grades[courseID] = newGrade;

    // Recalculate GPA
    calculateGPA();

    return true;
}

// Check if student has a specific course
bool student::hasCourse(long courseID) const {
    return courses.find(courseID) != courses.end();
}