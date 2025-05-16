#include "course.h"
#include "stdafx.h"
#include "student.h"
#include <algorithm>
#include <iostream>

using namespace std;


// Initialize static counter
long course::counter = 100;

course::course() :
    CourseID(counter++),  // Directly use and increment counter to avoid double incrementation
    title(""),
    description(""),
    instructor(""),
    semester(""),
    credithours(0),
    syllabus(""),
    enrolled(false)
{
    cout << "Created default course with ID: " << CourseID
        << ", counter now: " << counter << endl;
}

course::course(string title, string description, string instructor,
    string semester, int credithours, vector<course> prerequisites,
    string syllabus) :
    CourseID(counter++),  // Directly use and increment counter to avoid double incrementation
    title(title),
    description(description),
    instructor(instructor),
    semester(semester),
    credithours(credithours),
    prerequisites(prerequisites),
    syllabus(syllabus),
    enrolled(false)
{
    cout << "Created course with ID: " << CourseID
        << ", title: " << title
        << ", counter now: " << counter << endl;
}

void course::addPrerequisite(const course& prereq) {
    // Check if this prerequisite already exists
    if (find(prerequisites.begin(), prerequisites.end(), prereq) == prerequisites.end()) {
        prerequisites.push_back(prereq);
        cout << "Added prerequisite " << prereq.getCourseID()
            << " to course " << CourseID << endl;
    }
}

void course::removePrerequisite(long courseId) {
    auto it = remove_if(prerequisites.begin(), prerequisites.end(),
        [courseId](const course& c) { return c.getCourseID() == courseId; });
    if (it != prerequisites.end()) {
        prerequisites.erase(it, prerequisites.end());
        cout << "Removed prerequisite " << courseId
            << " from course " << CourseID << endl;
    }
}

void course::clearPrerequisites() {
    prerequisites.clear();
    cout << "Cleared all prerequisites for course " << CourseID << endl;
}

bool course::checkPrerequisites(const student& s) const {
    // If no prerequisites, automatically passes
    if (prerequisites.empty()) {
        return true;
    }

    // Check each prerequisite course
    for (const auto& prereq : prerequisites) {
        // First check if prerequisite is in student's completed courses
        if (!s.hasCompletedCourse(prereq.getCourseID())) {
            // If not completed, check if it's in their current courses
            if (!s.hasCourse(prereq.getCourseID())) {
                cout << "Missing prerequisite: " << prereq.getTitle()
                    << " (ID: " << prereq.getCourseID() << ")"
                    << " for course " << title << endl;
                return false;
            }
            else {
                cout << "You Failed At : " << prereq.getTitle()
                    << " (ID: " << prereq.getCourseID() << ")"
                    << " for course " << title << endl;
                return false;
            }
        }
    }
    return true;
}