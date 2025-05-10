#pragma once
#include <string>
#include <fstream>
#include <filesystem>
#include "courseSystem.h"
#include "json.hpp"

class FileUtils {
private:
    static std::string studentsFilePath;
    static std::string coursesFilePath;
    static std::string adminsFilePath;
    static nlohmann::json studentToJson(const student& student);
    static nlohmann::json courseToJson(const course& course);
    static nlohmann::json adminToJson(const admin& admin);
    static nlohmann::json gradeToJson(const grade& grade);
    static nlohmann::json courseGradePairToJson(const std::pair<course, grade>& pair);
    static student jsonToStudent(const nlohmann::json& j);
    static course jsonToCourse(const nlohmann::json& j);
    static admin jsonToAdmin(const nlohmann::json& j);
    static grade jsonToGrade(const nlohmann::json& j);
    static std::pair<course, grade> jsonToCourseGradePair(const nlohmann::json& j);
    static bool saveJsonToFile(const nlohmann::json& j, const std::string& filePath);
    static nlohmann::json loadJsonFromFile(const std::string& filePath);
public:
    static void initializePaths();
    static bool saveStudents(const std::unordered_map<std::string, student>& students);
    static bool saveCourses(const std::map<long, course>& courses);
    static bool saveAdmins(const std::unordered_map<std::string, admin>& admins);
    static bool saveAllData(const courseSystem& system);
    static bool loadStudents(std::unordered_map<std::string, student>& students);
    static bool loadCourses(std::map<long, course>& courses);
    static bool loadAdmins(std::unordered_map<std::string, admin>& admins);
    static bool loadAllData(courseSystem& system);
};