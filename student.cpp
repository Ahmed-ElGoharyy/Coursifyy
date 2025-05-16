#include "stdafx.h"
#include "student.h"
#include "course.h"
#include "user.h"   

using namespace std;


// Initialize static counter
long student::counter = 0;
student::student() : user(), StudentID(counter++), gpa(0.0), max_credit_hours(21), currentSemester("Fall 2023") {
    setRole('S');
    cout << "Created new student with ID: " << StudentID << endl;
}

student::student(string username, string password, string name, string email, string semester)
    : user(username, password, name, email, 'S'), gpa(0.0), max_credit_hours(21), currentSemester(semester) {
    StudentID = counter++;
    cout << "Created new student with ID: " << StudentID << endl;
}


student::student(string username, string password, string name, string email)
    : user(username, password, name, email, 'S'), gpa(0.0), max_credit_hours(21) {
    StudentID = counter++;
    cout << "Created new student with ID: " << StudentID << endl;
}

student::student(string username, string password, string name)
    : user(username, password, name, "null", 'S'), gpa(0.0), max_credit_hours(21) {
    StudentID = counter++;
    cout << "Created new student with ID: " << StudentID << endl;
}

pair<course, grade> student::getonegrade(string courseName) const noexcept(false) {
    for (const auto& pair : courses) {
        if (pair.first.getTitle() == courseName) {
            return pair;
        }
    }
    throw student_exception("Course not found: " + courseName);
}

bool student::registerCourse(course newCourse) noexcept(false) {
    // Check if course already exists
    for (const auto& pair : courses) {
        if (pair.first.getCourseID() == newCourse.getCourseID()) {
            throw student_exception("Course already registered: " + newCourse.getTitle());
        }
    }

    int courseHours = newCourse.getCreditHours();
    if (courseHours <= 0) {
        throw student_exception("Invalid credit hours: Course has " +
            to_string(courseHours) + " credit hours");
    }

    // Check remaining credit hours
    if (courseHours > max_credit_hours) {
        throw student_exception("Cannot register for course: Exceeds maximum credit hours (" +
            to_string(max_credit_hours) + " remaining)");
    }

    // Add course with default grade
    grade newGrade;
    courses.push_back(make_pair(newCourse, newGrade));

    // Update remaining credit hours
    max_credit_hours -= courseHours;

    return true;
}

list<pair<course, grade>> student::getGrades() const noexcept(false) {
    return courses;
}

float student::calculateGPA() noexcept(false) {
    if (courses.empty()) return 0.0f;

    float totalWeightedPoints = 0.0f;
    int totalCreditHours = 0;

    for (const auto& coursePair : courses) {
        if (coursePair.second.getGrade() != 'N') {  // Or whatever represents "no grade"
            totalWeightedPoints += coursePair.second.getGPA() * coursePair.first.getCreditHours();
            totalCreditHours += coursePair.first.getCreditHours();
        }
    }

    return totalCreditHours > 0 ? totalWeightedPoints / totalCreditHours : 0.0f;
}


bool student::hasCourse(long courseID) const {
    for (const auto& coursePair : courses) {
        if (coursePair.first.getCourseID() == courseID) {
            return true;
        }
    }
    return false;
}

bool student::hasCompletedCourse(long courseID) const {
    for (const auto& pair : courses) {
        if (pair.first.getCourseID() == courseID && pair.second.getGrade() != 'N' && pair.second.getGrade() != 'F') {
            return true;
        }
    }
    return false;
}

int student::modifycredithours(course c)
{
    max_credit_hours -= c.getCreditHours();
    return this->max_credit_hours;
}

bool student::generateReport() {
    try {
        string filename = "student_" + to_string(StudentID) + "_report.txt";
        ofstream report(filename);

        if (!report.is_open()) {
            throw student_exception("Failed to create report file: " + filename);
        }

        report << "Student Report" << endl;
        report << "==============" << endl;
        report << "ID: " << StudentID << endl;
        report << "Name: " << getName() << endl;
        report << "Email: " << getEmail() << endl;
        this->calculateGPA();
        report << "GPA: " << fixed << setprecision(2) << this->getGPA() << endl;
        report << "Remaining Credit Hours: " << this->max_credit_hours << endl;
        report << endl;
        report << "Courses:" << endl;
        report << "--------" << endl;
        report << left << setw(40) << "Course Title"
            << setw(10) << "ID"
            << setw(10) << "Credits"
            << setw(10) << "Grade"
            << setw(10) << "Points" << endl;

        for (const auto& pair : courses) {
            const course& c = pair.first;
            const grade& g = pair.second;

            report << left << setw(40) << c.getTitle()
                << setw(10) << c.getCourseID()
                << setw(10) << c.getCreditHours()
                << setw(10) << g.getGrade()
                << setw(10) << g.getGPA() << endl;
        }

        report.close();
        return true;
    }
    catch (const exception& e) {
        cerr << "Error generating report: " << e.what() << endl;
        return false;
    }
}
bool student::addCourseToPlan(const course& courseToAdd) {
    try {
        // Check if course already exists in student's list
        for (const auto& [existingCourse, grade] : courses) {
            if (existingCourse.getCourseID() == courseToAdd.getCourseID()) {
                return false; // Course already exists
            }
        }

        if (max_credit_hours < courseToAdd.getCreditHours()) {
            return false;
        }

        // Create a default grade
        grade defaultGrade;

        // Add the course to the student's list
        courses.push_back(make_pair(courseToAdd, defaultGrade));
		// Update remaining credit hours
        this->modifycredithours(courseToAdd);

        return true;
    }
    catch (const exception& e) {
        cerr << "Error adding course to student: " << e.what() << endl;
        return false;
    }
}
bool student::updateGrade(long courseID, const grade& newGrade) {
    for (auto& coursePair : courses) {
        if (coursePair.first.getCourseID() == courseID) {
            coursePair.second = newGrade;
            coursePair.first.setEnrolled(true);
            this->gpa = calculateGPA(); // Recalculate GPA
            return true;
        }
    }
    return false;
}

