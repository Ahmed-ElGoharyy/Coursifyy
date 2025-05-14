#include "course.h"
#include "stdafx.h"
#include "student.h"
#include <algorithm>
#include <iostream>

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
    std::cout << "Created default course with ID: " << CourseID
        << ", counter now: " << counter << std::endl;
}

course::course(std::string title, std::string description, std::string instructor,
    std::string semester, int credithours, std::vector<course> prerequisites,
    std::string syllabus) :
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
    std::cout << "Created course with ID: " << CourseID
        << ", title: " << title
        << ", counter now: " << counter << std::endl;
}

void course::addPrerequisite(const course& prereq) {
    // Check if this prerequisite already exists
    if (std::find(prerequisites.begin(), prerequisites.end(), prereq) == prerequisites.end()) {
        prerequisites.push_back(prereq);
        std::cout << "Added prerequisite " << prereq.getCourseID()
            << " to course " << CourseID << std::endl;
    }
}

void course::removePrerequisite(long courseId) {
    auto it = std::remove_if(prerequisites.begin(), prerequisites.end(),
        [courseId](const course& c) { return c.getCourseID() == courseId; });
    if (it != prerequisites.end()) {
        prerequisites.erase(it, prerequisites.end());
        std::cout << "Removed prerequisite " << courseId
            << " from course " << CourseID << std::endl;
    }
}

void course::clearPrerequisites() {
    prerequisites.clear();
    std::cout << "Cleared all prerequisites for course " << CourseID << std::endl;
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
                std::cout << "Missing prerequisite: " << prereq.getTitle()
                    << " (ID: " << prereq.getCourseID() << ")"
                    << " for course " << title << std::endl;
                return false;
            }
        }
    }
    return true;
}