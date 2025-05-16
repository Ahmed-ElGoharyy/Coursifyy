#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <list>

using namespace std;


class student;
class FileUtils;

class course {
private:
    string title;
    string description;
    string instructor;
    string semester;
    int credithours;
    vector<course> prerequisites;
    string syllabus;
    bool enrolled;

public:
    static long counter;
    long CourseID;

    friend class FileUtils;

    // Counter management - simplified to avoid double incrementation
    static void resetCounter(long value = 100) { counter = value; }

    // Constructors
    course();
    course(string title, string description, string instructor,
        string semester, int credithours, vector<course> prerequisites,
        string syllabus);

    // Prerequisite management
    void addPrerequisite(const course& prereq);
    void removePrerequisite(long courseId);
    void clearPrerequisites();
    bool checkPrerequisites(const student& s) const;

    // Getters
    long getCourseID() const { return CourseID; }
    string getTitle() const { return title; }
    string getDescription() const { return description; }
    string getInstructor() const { return instructor; }
    string getSemester() const { return semester; }
    int getCreditHours() const { return credithours; }
    string getSyllabus() const { return syllabus; }
    bool isEnrolled() const { return enrolled; }
    vector<course> getPrerequisites() const { return prerequisites; }

    // Setters
    void setTitle(const string& t) { title = t; }
    void setDescription(const string& desc) { description = desc; }
    void setInstructor(const string& inst) { instructor = inst; }
    void setSemester(const string& sem) { semester = sem; }
    void setCreditHours(int credits) { credithours = credits; }
    void setSyllabus(const string& syll) { syllabus = syll; }
    void setEnrolled(bool status) { enrolled = status; }

    // Operators
    bool operator==(const course& other) const {
        return CourseID == other.CourseID;
    }
};