#include "stdafx.h"
#include "FileUtils.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include "courseSystem.h"
#include "course.h"
#include "student.h"
#include "admin.h"
#include <json.hpp> // Using nlohmann/json

// For handling file system errors
#include <cerrno>
#include <cstring>

// Use nlohmann json namespace for convenience
using json = nlohmann::json;

// Define file paths - will be initialized with absolute paths
std::string FileUtils::DATA_DIRECTORY = "";
std::string FileUtils::STUDENTS_FILE = "";
std::string FileUtils::COURSES_FILE = "";
std::string FileUtils::ADMINS_FILE = "";

// Initialize paths - call this at program startup
void FileUtils::initializePaths() {
    // Get the current working directory as the base
    DATA_DIRECTORY = std::filesystem::current_path().string() + "/data/";

    // Create the data directory if it doesn't exist
    if (!std::filesystem::exists(DATA_DIRECTORY)) {
        try {
            std::filesystem::create_directories(DATA_DIRECTORY);
            std::cout << "Created data directory: " << DATA_DIRECTORY << std::endl;
        }
        catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Error creating data directory: " << e.what() << std::endl;
        }
    }

    // Set the full paths to files
    STUDENTS_FILE = DATA_DIRECTORY + "students.json";
    COURSES_FILE = DATA_DIRECTORY + "courses.json";
    ADMINS_FILE = DATA_DIRECTORY + "admins.json";

    std::cout << "Files will be stored at: " << std::endl;
    std::cout << "- Students: " << STUDENTS_FILE << std::endl;
    std::cout << "- Courses: " << COURSES_FILE << std::endl;
    std::cout << "- Admins: " << ADMINS_FILE << std::endl;
}

// Check if a file exists
bool FileUtils::fileExists(const std::string& filename) {
    return std::filesystem::exists(filename);
}

// Create an empty file with empty JSON structure
bool FileUtils::createEmptyFile(const std::string& filename) {
    // Make sure the directory exists
    std::filesystem::path filePath(filename);
    std::filesystem::path directory = filePath.parent_path();

    if (!directory.empty() && !std::filesystem::exists(directory)) {
        try {
            std::filesystem::create_directories(directory);
            std::cout << "Created directory: " << directory.string() << std::endl;
        }
        catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Error creating directory: " << e.what() << std::endl;
            return false;
        }
    }

    // Now try to create the file
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not create " << filename << std::endl;
        char errorMsg[256];
        std::cerr << "Error details: " << strerror_s(errorMsg, sizeof(errorMsg), errno) << std::endl;
        return false;
    }

    // Create empty JSON structure (empty array for most files)
    file << "[]" << std::endl;
    file.close();

    // Verify the file was created
    if (std::filesystem::exists(filename)) {
        std::cout << "Created empty file: " << filename << std::endl;
        std::cout << "File size: " << std::filesystem::file_size(filename) << " bytes" << std::endl;
        return true;
    }
    else {
        std::cerr << "File creation verification failed for: " << filename << std::endl;
        return false;
    }
}

// Load all data from files into the course system
bool FileUtils::loadAllData(courseSystem& system) {
    // Initialize paths if not done yet
    if (DATA_DIRECTORY.empty()) {
        initializePaths();
    }

    // Check if files exist, create them if they don't
    if (!fileExists(STUDENTS_FILE)) {
        if (!createEmptyFile(STUDENTS_FILE)) {
            std::cerr << "Failed to create students file" << std::endl;
            return false;
        }
    }

    if (!fileExists(COURSES_FILE)) {
        if (!createEmptyFile(COURSES_FILE)) {
            std::cerr << "Failed to create courses file" << std::endl;
            return false;
        }
    }

    if (!fileExists(ADMINS_FILE)) {
        if (!createEmptyFile(ADMINS_FILE)) {
            std::cerr << "Failed to create admins file" << std::endl;
            return false;
        }
    }

    // Load data from files
    bool studentsOk = loadStudents(system);
    bool coursesOk = loadCourses(system);
    bool adminsOk = loadAdmins(system);

    return studentsOk && coursesOk && adminsOk;
}

// Load students from JSON file
bool FileUtils::loadStudents(courseSystem& system) {
    // Try to open the file
    std::ifstream file(STUDENTS_FILE);
    if (!file.is_open()) {
        std::cerr << "Could not open students file for reading: " << STUDENTS_FILE << std::endl;
        char errorMsg[256];
        std::cerr << "Error details: " << strerror_s(errorMsg, sizeof(errorMsg), errno) << std::endl;
        return false;
    }

    try {
        // Parse JSON file - nlohmann/json can parse directly from streams
        json root = json::parse(file);

        // Make sure it's an array
        if (!root.is_array()) {
            std::cerr << "Students file is not a valid JSON array" << std::endl;
            file.close();
            return false;
        }

        // Process each student in the array
        for (const auto& studentJson : root) {
            student s;

            // Read basic user properties
            s.setUsername(studentJson["username"].get<std::string>());
            s.setName(studentJson["name"].get<std::string>());
            s.setEmail(studentJson["email"].get<std::string>());
            s.setRole(studentJson["role"].get<std::string>()[0]);  // Get first char of role string

            // We need direct access to set passwordHash
            s.setPassword(studentJson["passwordHash"].get<std::string>());

            // Read student-specific properties
            long studentID = studentJson["studentID"].get<int64_t>();
            s.StudentID = studentID;  // Using friend access via StudentID
            s.gpa = studentJson["gpa"].get<float>();

            // Update static counter if needed
            if (studentID >= student::counter) {
                student::counter = studentID + 1;
            }

            // Read courses and grades
            const auto& coursesJson = studentJson["courses"];
            const auto& gradesJson = studentJson["grades"];

            // Process each course for this student
            for (auto it = coursesJson.begin(); it != coursesJson.end(); ++it) {
                std::string courseIdStr = it.key();
                long courseID = std::stol(courseIdStr);

                // Create a placeholder course (will be updated later)
                course c;
                c.CourseID = courseID;
                s.courses[courseID] = c;

                // Read the grade for this course if it exists
                if (gradesJson.contains(courseIdStr)) {
                    const auto& gradeJson = gradesJson[courseIdStr];
                    grade g;

                    g.setSemester(gradeJson["semester"].get<int>());
                    g.setGrade(gradeJson["letterGrade"].get<std::string>()[0]);
                    g.setYear(gradeJson["year"].get<int>());

                    s.grades[courseID] = g;
                }
            }

            // Add student to system
            system.addStudent(s);
        }

        file.close();
        std::cout << "Successfully loaded students from file" << std::endl;
        return true;
    }
    catch (const json::parse_error& e) {
        std::cerr << "JSON parse error in loadStudents: " << e.what() << std::endl;
        file.close();
        return false;
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading students: " << e.what() << std::endl;
        file.close();
        return false;
    }
}

// Load courses from JSON file
bool FileUtils::loadCourses(courseSystem& system) {
    // Try to open the file
    std::ifstream file(COURSES_FILE);
    if (!file.is_open()) {
        std::cerr << "Could not open courses file for reading: " << COURSES_FILE << std::endl;
        char errorMsg[256];
        std::cerr << "Error details: " << strerror_s(errorMsg, sizeof(errorMsg), errno) << std::endl;
        return false;
    }

    try {
        // Parse JSON file
        json root = json::parse(file);

        // Make sure it's an array
        if (!root.is_array()) {
            std::cerr << "Courses file is not a valid JSON array" << std::endl;
            file.close();
            return false;
        }

        // First pass: create all courses
        for (const auto& courseJson : root) {
            course c;

            // Read course properties
            c.CourseID = courseJson["courseID"].get<int64_t>();
            c.setTitle(courseJson["title"].get<std::string>());
            c.setDescription(courseJson["description"].get<std::string>());
            c.setInstructor(courseJson["instructor"].get<std::string>());
            c.setSemester(courseJson["semester"].get<std::string>());
            c.setCreditHours(courseJson["creditHours"].get<int>());
            c.setSyllabus(courseJson["syllabus"].get<std::string>());
            c.setEnrolled(courseJson["enrolled"].get<bool>());

            // Update static counter if needed
            if (c.CourseID >= course::counter) {
                course::counter = c.CourseID + 1;
            }

            // Add course to system
            system.addCourse(c);
        }

        // Second pass: add prerequisites
        for (const auto& courseJson : root) {
            long courseID = courseJson["courseID"].get<int64_t>();

            course* thisCourse = system.getCourse(courseID);
            if (thisCourse) {
                const auto& prereqsJson = courseJson["prerequisites"];

                for (const auto& prereqID : prereqsJson) {
                    long prereqIDValue = prereqID.get<int64_t>();

                    course* prereqCourse = system.getCourse(prereqIDValue);
                    if (prereqCourse) {
                        thisCourse->addPrerequisite(*prereqCourse);
                    }
                }
            }
        }

        file.close();

        // Update the courses in the students' maps with the complete course information
        for (auto& studentPair : system.getAllStudents()) {
            student* s = system.getStudent(studentPair.first);
            if (s) {
                for (auto& coursePair : s->courses) {
                    course* c = system.getCourse(coursePair.first);
                    if (c) {
                        s->courses[coursePair.first] = *c;
                    }
                }
            }
        }

        std::cout << "Successfully loaded courses from file" << std::endl;
        return true;
    }
    catch (const json::parse_error& e) {
        std::cerr << "JSON parse error in loadCourses: " << e.what() << std::endl;
        file.close();
        return false;
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading courses: " << e.what() << std::endl;
        file.close();
        return false;
    }
}

// Load admins from JSON file
bool FileUtils::loadAdmins(courseSystem& system) {
    // Try to open the file
    std::ifstream file(ADMINS_FILE);
    if (!file.is_open()) {
        std::cerr << "Could not open admins file for reading: " << ADMINS_FILE << std::endl;
        char errorMsg[256];
        std::cerr << "Error details: " << strerror_s(errorMsg, sizeof(errorMsg), errno) << std::endl;
        return false;
    }

    try {
        // Parse JSON file
        json root = json::parse(file);

        // Make sure it's an array
        if (!root.is_array()) {
            std::cerr << "Admins file is not a valid JSON array" << std::endl;
            file.close();
            return false;
        }

        // Process each admin in the array
        for (const auto& adminJson : root) {
            admin a;

            // Read basic user properties
            a.setUsername(adminJson["username"].get<std::string>());
            a.setName(adminJson["name"].get<std::string>());
            a.setEmail(adminJson["email"].get<std::string>());
            a.setRole(adminJson["role"].get<std::string>()[0]);  // Get first char of role string

            // We need direct access to set passwordHash
            a.setPassword(adminJson["passwordHash"].get<std::string>());

            // Read admin-specific properties
            a.adminID = adminJson["adminID"].get<int64_t>();

            // Update static counter if needed
            if (a.adminID >= admin::counter) {
                admin::counter = a.adminID + 1;
            }

            // Add admin to system
            system.addAdmin(a);
        }

        file.close();
        std::cout << "Successfully loaded admins from file" << std::endl;
        return true;
    }
    catch (const json::parse_error& e) {
        std::cerr << "JSON parse error in loadAdmins: " << e.what() << std::endl;
        file.close();
        return false;
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading admins: " << e.what() << std::endl;
        file.close();
        return false;
    }
}

// Save all data to files
bool FileUtils::saveAllData(const courseSystem& system) {
    // Initialize paths if not done yet
    if (DATA_DIRECTORY.empty()) {
        initializePaths();
    }

    // Ensure the data directory exists
    if (!std::filesystem::exists(DATA_DIRECTORY)) {
        try {
            std::filesystem::create_directories(DATA_DIRECTORY);
            std::cout << "Created data directory: " << DATA_DIRECTORY << std::endl;
        }
        catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Error creating data directory: " << e.what() << std::endl;
            return false;
        }
    }

    bool studentsOk = saveStudents(system.getAllStudents());
    bool coursesOk = saveCourses(system.getAllCourses());
    bool adminsOk = saveAdmins(system.getAllAdmins());

    if (studentsOk && coursesOk && adminsOk) {
        std::cout << "All data has been successfully saved" << std::endl;
        return true;
    }
    else {
        std::cerr << "There were errors saving some data" << std::endl;
        return false;
    }
}

// Save students to JSON file
bool FileUtils::saveStudents(const std::map<long, student>& students) {
    std::ofstream file(STUDENTS_FILE);
    if (!file.is_open()) {
        std::cerr << "Could not open students file for writing: " << STUDENTS_FILE << std::endl;
        char errorMsg[256];
        std::cerr << "Error details: " << strerror_s(errorMsg, sizeof(errorMsg), errno) << std::endl;
        return false;
    }

    try {
        // Create JSON array
        json root = json::array();

        // Write each student
        for (const auto& pair : students) {
            const student& s = pair.second;
            json studentJson;

            // Write basic user properties
            studentJson["username"] = s.getUsername();
            studentJson["name"] = s.getName();
            studentJson["email"] = s.getEmail();
            studentJson["passwordHash"] = s.getPasswordHash();
            studentJson["role"] = std::string(1, s.getRole());  // Convert char to string

            // Write student-specific properties
            studentJson["studentID"] = s.getStudentID();
            studentJson["gpa"] = s.getGPA();

            // Write courses and grades
            json coursesJson = json::object();
            json gradesJson = json::object();

            const auto& courses = s.getCourses();
            const auto& grades = s.getGradeMap();

            for (const auto& coursePair : courses) {
                long courseID = coursePair.first;
                std::string courseIdStr = std::to_string(courseID);

                // Store course ID
                coursesJson[courseIdStr] = json::object();

                // Find corresponding grade
                auto gradeIter = grades.find(courseID);
                if (gradeIter != grades.end()) {
                    const grade& g = gradeIter->second;
                    json gradeJson;

                    gradeJson["semester"] = g.getSemester();
                    gradeJson["letterGrade"] = std::string(1, g.getGrade());
                    gradeJson["year"] = g.getYear();
                    gradeJson["gpa"] = g.getGPA();

                    gradesJson[courseIdStr] = gradeJson;
                }
            }

            studentJson["courses"] = coursesJson;
            studentJson["grades"] = gradesJson;

            // Add student to root array
            root.push_back(studentJson);
        }

        // Write JSON to file with indentation of 4 spaces
        file << root.dump(4);

        file.close();
        std::cout << "Successfully saved students to file" << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error saving students: " << e.what() << std::endl;
        file.close();
        return false;
    }
}

// Save courses to JSON file
bool FileUtils::saveCourses(const std::map<long, course>& courses) {
    std::ofstream file(COURSES_FILE);
    if (!file.is_open()) {
        std::cerr << "Could not open courses file for writing: " << COURSES_FILE << std::endl;
        char errorMsg[256];
        std::cerr << "Error details: " << strerror_s(errorMsg, sizeof(errorMsg), errno) << std::endl;
        return false;
    }

    try {
        // Create JSON array
        json root = json::array();

        // Write each course
        for (const auto& pair : courses) {
            const course& c = pair.second;
            json courseJson;

            // Write course properties
            courseJson["courseID"] = c.CourseID;
            courseJson["title"] = c.getTitle();
            courseJson["description"] = c.getDescription();
            courseJson["instructor"] = c.getInstructor();
            courseJson["semester"] = c.getSemester();
            courseJson["creditHours"] = c.getCreditHours();
            courseJson["syllabus"] = c.getSyllabus();
            courseJson["enrolled"] = c.isEnrolled();

            // Write prerequisites
            json prereqsJson = json::array();
            const auto& prereqs = c.getPrerequisites();

            for (const auto& prereq : prereqs) {
                prereqsJson.push_back(prereq.getCourseID());
            }

            courseJson["prerequisites"] = prereqsJson;

            // Add course to root array
            root.push_back(courseJson);
        }

        // Write JSON to file with indentation of 4 spaces
        file << root.dump(4);

        file.close();
        std::cout << "Successfully saved courses to file" << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error saving courses: " << e.what() << std::endl;
        file.close();
        return false;
    }
}

// Save admins to JSON file
bool FileUtils::saveAdmins(const std::vector<admin>& admins) {
    std::ofstream file(ADMINS_FILE);
    if (!file.is_open()) {
        std::cerr << "Could not open admins file for writing: " << ADMINS_FILE << std::endl;
        char errorMsg[256];
        std::cerr << "Error details: " << strerror_s(errorMsg, sizeof(errorMsg), errno) << std::endl;
        return false;
    }

    try {
        // Create JSON array
        json root = json::array();

        // Write each admin
        for (const auto& a : admins) {
            json adminJson;

            // Write basic user properties
            adminJson["username"] = a.getUsername();
            adminJson["name"] = a.getName();
            adminJson["email"] = a.getEmail();
            adminJson["passwordHash"] = a.getPasswordHash();
            adminJson["role"] = std::string(1, a.getRole());  // Convert char to string

            // Write admin-specific properties
            adminJson["adminID"] = a.getAdminID();

            // Add admin to root array
            root.push_back(adminJson);
        }

        // Write JSON to file with indentation of 4 spaces
        file << root.dump(4);

        file.close();
        std::cout << "Successfully saved admins to file" << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error saving admins: " << e.what() << std::endl;
        file.close();
        return false;
    }
}