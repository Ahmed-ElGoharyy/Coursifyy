#pragma once
#include <string>
#include <fstream>
#include <filesystem>
#include "courseSystem.h"
#include "json.hpp"

using namespace std;


class FileUtils {
public:
    static string studentsFilePath;
    static string coursesFilePath;
    static string adminsFilePath;
    static nlohmann::json studentToJson(const student& student);
    static nlohmann::json courseToJson(const course& course);
    static nlohmann::json adminToJson(const admin& admin);
    static nlohmann::json gradeToJson(const grade& grade);
    static nlohmann::json courseGradePairToJson(const pair<course, grade>& pair);
    static student jsonToStudent(const nlohmann::json& j);
    static course jsonToCourse(const nlohmann::json& j);
    static admin jsonToAdmin(const nlohmann::json& j);
    static grade jsonToGrade(const nlohmann::json& j);
    static pair<course, grade> jsonToCourseGradePair(const nlohmann::json& j);
    static bool saveJsonToFile(const nlohmann::json& j, const string& filePath);
    static nlohmann::json loadJsonFromFile(const string& filePath);
public:
    static void initializePaths();
    static bool saveStudents(const unordered_map<string, student>& students);
    static bool saveCourses(const map<long, course>& courses);
    static bool saveAdmins(const unordered_map<string, admin>& admins);
    static bool saveAllData(const courseSystem& system);
    static bool loadStudents(unordered_map<string, student>& students);
    static bool loadCourses(map<long, course>& courses);
    static bool loadAdmins(unordered_map<string, admin>& admins);
    static bool loadAllData(courseSystem& system);
};