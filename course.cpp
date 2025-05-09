#include "stdafx.h"
#include "course.h"
#include "student.h"
#include <stdexcept>

// Initialize static counter
long course::counter = 0;

// Default constructor
course::course() :
    CourseID(++counter),
    title(""),
    description(""),
    instructor(""),
    semester(""),
    credithours(0),
    syllabus(""),
    enrolled(false) {
}

// Parameterized constructor
course::course(std::string title, std::string description, std::string instructor, std::string semester,
    int credithours, std::list<std::string> prereqIDs, std::string syllabus) :
    CourseID(++counter),
    title(title),
    description(description),
    instructor(instructor),
    semester(semester),
    credithours(credithours),
    syllabus(syllabus),
    enrolled(false) {
    // Prerequisites will be added separately after all courses are created
}

// Add a prerequisite course
void course::addPrerequisite(const course& prereq) {
    // Check if this prerequisite already exists
    for (const auto& existingPrereq : prerequisites) {
        if (existingPrereq.CourseID == prereq.CourseID) {
            return; // Already exists, don't add duplicate
        }
    }

    // Add the prerequisite
    prerequisites.push_back(prereq);
}

// Clear all prerequisites
void course::clearPrerequisites() {
    prerequisites.clear();
}

// Check if a student meets the prerequisites for this course
bool course::checkPrerequisites(student& s, const std::list<std::string>& prereqIDs) {
    // Check if there are no prerequisites
    if (prerequisites.empty() && prereqIDs.empty()) {
        return true; // No prerequisites needed
    }

    // Check prerequisites from the list
    for (const auto& prereq : prerequisites) {
        long prereqID = prereq.CourseID;

        // Check if student has taken this prerequisite
        auto it = s.grades.find(prereqID);
        if (it == s.grades.end()) {
            return false; // Prerequisite not taken
        }

        // Check if student passed this prerequisite (grade better than F)
        if (it->second.getGrade() == 'F') {
            return false; // Failed prerequisite
        }
    }

    // Additionally check any course IDs provided
    for (const auto& prereqIDStr : prereqIDs) {
        try {
            long prereqID = std::stol(prereqIDStr);

            // Check if student has taken this prerequisite
            auto it = s.grades.find(prereqID);
            if (it == s.grades.end()) {
                return false; // Prerequisite not taken
            }

            // Check if student passed this prerequisite
            if (it->second.getGrade() == 'F') {
                return false; // Failed prerequisite
            }
        }
        catch (const std::exception& e) {
            // Invalid ID format, skip it
            continue;
        }
    }

    return true; // All prerequisites are met
}