#pragma once
#include <iostream>
#include <string>
#include <list>

// Forward declaration
class student;
class FileUtils; // For file access

class course
{
private:
    std::string title;
    std::string description;
    std::string instructor;
    std::string semester;
    int credithours;
    std::list<course> prerequisites; // A list of prerequisite courses
    std::string syllabus;
    bool enrolled;

    // Static counter for auto-incrementing IDs
    static long counter;

public:
    long CourseID; // Made public for FileUtils access

    friend class FileUtils; // Give FileUtils direct access

    // Constructors
    course();
    course(std::string title, std::string description, std::string instructor, std::string semester,
        int credithours, std::list<std::string> prerequisites, std::string syllabus);

    // Prerequisite management
    void addPrerequisite(const course& prereq);
    void clearPrerequisites();
    bool checkPrerequisites(student& s, const std::list<std::string>& prereqIDs);

    // Getters and setters
    long getCourseID() const { return CourseID; }
    std::string getTitle() const { return title; }
    std::string getDescription() const { return description; }
    std::string getInstructor() const { return instructor; }
    std::string getSemester() const { return semester; }
    int getCreditHours() const { return credithours; }
    std::string getSyllabus() const { return syllabus; }
    bool isEnrolled() const { return enrolled; }
    std::list<course> getPrerequisites() const { return prerequisites; }

    void setTitle(const std::string& t) { title = t; }
    void setDescription(const std::string& desc) { description = desc; }
    void setInstructor(const std::string& inst) { instructor = inst; }
    void setSemester(const std::string& sem) { semester = sem; }
    void setCreditHours(int credits) { credithours = credits; }
    void setSyllabus(const std::string& syll) { syllabus = syll; }
    void setEnrolled(bool status) { enrolled = status; }

    // For comparison in maps/sets
    bool operator==(const course& other) const {
        return CourseID == other.CourseID;
    }
};