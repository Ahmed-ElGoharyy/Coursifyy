#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <list>
#include "json.hpp" // We'll use nlohmann's JSON library for C++
#include "courseSystem.h"
#include "course.h"
#include "student.h"
#include "admin.h"


using json = nlohmann::json;

class FileUtils {
public:

    static const std::string STUDENTS_FILE;
    static const std::string COURSES_FILE;
    static const std::string ADMINS_FILE;

    // Helper function to check if file exists
    static bool fileExists(const std::string& filename);

    // Create empty files if they don't exist
    static void createFileIfNotExists(const std::string& filename);

public:
    // Load all data from files
    static bool loadAllData(courseSystem& system);

    // Save all data to files
    static bool saveAllData(const courseSystem& system);

    // Individual load functions
    static bool loadCourses(std::map<long, course>& courses);
    static bool loadStudents(std::map<long, student>& students);
    static bool loadAdmins(std::vector<admin>& admins);

    // Individual save functions
    static bool saveCourses(const std::map<long, course>& courses);
    static bool saveStudents(const std::map<long, student>& students);
    static bool saveAdmins(const std::vector<admin>& admins);
};
