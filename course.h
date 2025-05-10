#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <list>

class student;
class FileUtils;

class course {
private:
    std::string title;
    std::string description;
    std::string instructor;
    std::string semester;
    int credithours;
    std::vector<course> prerequisites;
    std::string syllabus;
    bool enrolled;
    static long counter;

public:
    long CourseID;
    friend class FileUtils;

    course();
    course(std::string title, std::string description, std::string instructor, std::string semester,
        int credithours, std::vector<course> prerequisites, std::string syllabus);

    void addPrerequisite(const course& prereq);
    void removePrerequisite(long courseId);
    void clearPrerequisites();
    bool checkPrerequisites(const student& s) const;

    long getCourseID() const { return CourseID; }
    std::string getTitle() const { return title; }
    std::string getDescription() const { return description; }
    std::string getInstructor() const { return instructor; }
    std::string getSemester() const { return semester; }
    int getCreditHours() const { return credithours; }
    std::string getSyllabus() const { return syllabus; }
    bool isEnrolled() const { return enrolled; }
    std::vector<course> getPrerequisites() const { return prerequisites; }

    void setTitle(const std::string& t) { title = t; }
    void setDescription(const std::string& desc) { description = desc; }
    void setInstructor(const std::string& inst) { instructor = inst; }
    void setSemester(const std::string& sem) { semester = sem; }
    void setCreditHours(int credits) { credithours = credits; }
    void setSyllabus(const std::string& syll) { syllabus = syll; }
    void setEnrolled(bool status) { enrolled = status; }

    bool operator==(const course& other) const {
        return CourseID == other.CourseID;
    }
};