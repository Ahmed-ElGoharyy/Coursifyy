#include "course.h"
#include "stdafx.h"
#include "student.h"
#include <algorithm>

long course::counter = 1000;

course::course() :
    CourseID(counter++),
    title(""),
    description(""),
    instructor(""),
    semester(""),
    credithours(0),
    syllabus(""),
    enrolled(false) {
}

course::course(std::string title, std::string description, std::string instructor,
    std::string semester, int credithours, std::vector<course> prerequisites,
    std::string syllabus) :
    CourseID(counter++),
    title(title),
    description(description),
    instructor(instructor),
    semester(semester),
    credithours(credithours),
    prerequisites(prerequisites),
    syllabus(syllabus),
    enrolled(false) {
}

void course::addPrerequisite(const course& prereq) {
    prerequisites.push_back(prereq);
}

void course::removePrerequisite(long courseId) {
    prerequisites.erase(
        std::remove_if(prerequisites.begin(), prerequisites.end(),
            [courseId](const course& c) { return c.getCourseID() == courseId; }),
        prerequisites.end()
    );
}

void course::clearPrerequisites() {
    prerequisites.clear();
}

bool course::checkPrerequisites(const student& s) const {
    for (const auto& prereq : prerequisites) {
        if (!s.hasCompletedCourse(prereq.getCourseID())) {
            return false;
        }
    }
    return true;
}