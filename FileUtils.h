#pragma once

#include <string>
#include <map>
#include <vector>

class courseSystem;
class student;
class course;
class admin;

/**
 * FileUtils class for handling file operations in the course system
 */
class FileUtils {
public:
    // Static file paths - will be initialized with absolute paths
    static std::string DATA_DIRECTORY;
    static std::string STUDENTS_FILE;
    static std::string COURSES_FILE;
    static std::string ADMINS_FILE;

    /**
     * Initialize file paths based on current working directory
     */
    static void initializePaths();

    /**
     * Check if a file exists
     * @param filename The path to the file
     * @return True if file exists, false otherwise
     */
    static bool fileExists(const std::string& filename);

    /**
     * Create an empty file with empty JSON structure
     * @param filename The path to the file
     * @return True if successful, false otherwise
     */
    static bool createEmptyFile(const std::string& filename);

    /**
     * Load all data from files into the course system
     * @param system The course system to load data into
     * @return True if successful, false otherwise
     */
    static bool loadAllData(courseSystem& system);

    /**
     * Load students from JSON file
     * @param system The course system to load students into
     * @return True if successful, false otherwise
     */
    static bool loadStudents(courseSystem& system);

    /**
     * Load courses from JSON file
     * @param system The course system to load courses into
     * @return True if successful, false otherwise
     */
    static bool loadCourses(courseSystem& system);

    /**
     * Load admins from JSON file
     * @param system The course system to load admins into
     * @return True if successful, false otherwise
     */
    static bool loadAdmins(courseSystem& system);

    /**
     * Save all data to files
     * @param system The course system to save data from
     * @return True if successful, false otherwise
     */
    static bool saveAllData(const courseSystem& system);

    /**
     * Save students to JSON file
     * @param students The map of students to save
     * @return True if successful, false otherwise
     */
    static bool saveStudents(const std::map<long, student>& students);

    /**
     * Save courses to JSON file
     * @param courses The map of courses to save
     * @return True if successful, false otherwise
     */
    static bool saveCourses(const std::map<long, course>& courses);

    /**
     * Save admins to JSON file
     * @param admins The vector of admins to save
     * @return True if successful, false otherwise
     */
    static bool saveAdmins(const std::vector<admin>& admins);
};