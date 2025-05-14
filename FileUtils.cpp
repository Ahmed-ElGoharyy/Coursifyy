#include "stdafx.h"
#include "FileUtils.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <filesystem>

std::string FileUtils::studentsFilePath = "students.json";
std::string FileUtils::coursesFilePath = "courses.json";
std::string FileUtils::adminsFilePath = "admins.json";

void FileUtils::initializePaths() {
    // Get the executable's directory as base path
    std::filesystem::path execPath = std::filesystem::current_path();
    std::string dataDir = (execPath / "data").string() + "/";

    std::cout << "Setting data directory to: " << dataDir << std::endl;

    if (!std::filesystem::exists(dataDir)) {
        std::cout << "Creating data directory: " << dataDir << std::endl;
        std::filesystem::create_directory(dataDir);
    }

    studentsFilePath = dataDir + "students.json";
    coursesFilePath = dataDir + "courses.json";
    adminsFilePath = dataDir + "admins.json";

    std::cout << "Files will be saved to:" << std::endl;
    std::cout << "- Students: " << studentsFilePath << std::endl;
    std::cout << "- Courses: " << coursesFilePath << std::endl;
    std::cout << "- Admins: " << adminsFilePath << std::endl;
}

nlohmann::json FileUtils::studentToJson(const student& s) {
    nlohmann::json j;
    j["username"] = s.getUsername();
    j["password"] = s.getPassword();
    j["name"] = s.getName();
    j["email"] = s.getEmail();
    j["role"] = std::string(1, s.getRole());
    j["student_id"] = s.getStudentID();
    j["gpa"] = s.getGPA();
    j["max_credit_hours"] = s.max_credit_hours;
    j["current_semester"] = s.getCurrentSemester();

    nlohmann::json coursesArray = nlohmann::json::array();
    for (const auto& pair : s.courses) {
        coursesArray.push_back(courseGradePairToJson(pair));
    }
    j["courses"] = coursesArray;

    return j;
}

nlohmann::json FileUtils::courseToJson(const course& c) {
    nlohmann::json j;
    j["course_id"] = c.getCourseID();
    j["title"] = c.getTitle();
    j["description"] = c.getDescription();
    j["instructor"] = c.getInstructor();
    j["semester"] = c.getSemester();
    j["credit_hours"] = c.getCreditHours();
    j["syllabus"] = c.getSyllabus();
    j["enrolled"] = c.isEnrolled();

    // Save prerequisites as array of course IDs
    nlohmann::json prereqArray = nlohmann::json::array();
    for (const auto& prereq : c.getPrerequisites()) {
        prereqArray.push_back(prereq.getCourseID());
    }
    j["prerequisites"] = prereqArray;

    return j;
}

nlohmann::json FileUtils::adminToJson(const admin& a) {
    nlohmann::json j;
    j["username"] = a.getUsername();
    j["password"] = a.getPassword(); // Changed from password_hash to password
    j["name"] = a.getName();
    j["email"] = a.getEmail();
    j["role"] = std::string(1, a.getRole());
    j["admin_id"] = a.getAdminID();
    return j;
}

nlohmann::json FileUtils::gradeToJson(const grade& g) {
    nlohmann::json j;
    j["semester"] = g.getSemester();
    j["grade"] = std::string(1, g.getGrade());
    j["year"] = g.getYear();
    j["gpa"] = g.getGPA();
    return j;
}


nlohmann::json FileUtils::courseGradePairToJson(const std::pair<course, grade>& pair) {
    nlohmann::json j;
    j["course"] = courseToJson(pair.first);
    j["grade"] = gradeToJson(pair.second);
    return j;
}

student FileUtils::jsonToStudent(const nlohmann::json& j) {
    student s(
        j["username"].get<std::string>(),
        j["password"].get<std::string>(),
        j["name"].get<std::string>(),
        j["email"].get<std::string>(),
        j["current_semester"].get<std::string>()  // Add semester
    );

    s.setID(j["student_id"].get<long>());
    s.setGPA(j["gpa"].get<float>());
    s.max_credit_hours = j["max_credit_hours"].get<int>();
    return s;
}

course FileUtils::jsonToCourse(const nlohmann::json& j) {
    course c;

    // Temporarily save the counter value
    long oldCounter = course::counter;

    // Set ID directly (this will be used instead of assigning a new one)
    c.CourseID = j["course_id"].get<long>();

    // Restore counter if needed (in case the constructor changed it)
    course::counter = oldCounter;

    // Set other properties
    c.setTitle(j["title"].get<std::string>());
    c.setDescription(j["description"].get<std::string>());
    c.setInstructor(j["instructor"].get<std::string>());
    c.setSemester(j["semester"].get<std::string>());
    c.setCreditHours(j["credit_hours"].get<int>());
    c.setSyllabus(j["syllabus"].get<std::string>());
    c.setEnrolled(j["enrolled"].get<bool>());

    return c;
}

admin FileUtils::jsonToAdmin(const nlohmann::json& j) {
    // Create admin with specific ID to avoid counter incrementing
    long adminId = j["admin_id"].get<long>();
    std::cout << "Creating admin from JSON with fixed ID: " << adminId << std::endl;

    admin a(adminId);

    // Set properties manually
    a.setUsername(j["username"].get<std::string>());
    a.setPassword(j["password"].get<std::string>());
    a.setName(j["name"].get<std::string>());
    a.setEmail(j["email"].get<std::string>());
    a.setRole('A');

    return a;
}

grade FileUtils::jsonToGrade(const nlohmann::json& j) {
    grade g;
    g.setSemester(j["semester"].get<int>());

    // Get the grade character
    char gradeChar = j["grade"].get<std::string>()[0];
    g.setGrade(gradeChar);

    g.setYear(j["year"].get<int>());

    // Only call GetGradeFromGPA if grade is not 'N'
    if (gradeChar != 'N') {
        g.GetGradeFromGPA(j["gpa"].get<float>());
    }
    else {
        // Set GPA to 0 or keep existing value for 'N' grades
        g.setGPA(0.0f);
    }

    return g;
}
std::pair<course, grade> FileUtils::jsonToCourseGradePair(const nlohmann::json& j) {
    course c = jsonToCourse(j["course"]);
    grade g = jsonToGrade(j["grade"]);
    return std::make_pair(c, g);
}


nlohmann::json FileUtils::loadJsonFromFile(const std::string& filePath) {
    try {
        if (!std::filesystem::exists(filePath)) {
            std::cout << "File does not exist: " << filePath << std::endl;
            return nlohmann::json::array();
        }

        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cout << "Could not open file for reading: " << filePath << std::endl;
            return nlohmann::json::array();
        }

        file.seekg(0, std::ios::end);
        if (file.tellg() == 0) {
            std::cout << "File is empty: " << filePath << std::endl;
            file.close();
            return nlohmann::json::array();
        }
        file.seekg(0, std::ios::beg);

        nlohmann::json j;
        try {
            file >> j;
        }
        catch (const nlohmann::json::parse_error& e) {
            std::cerr << "JSON parse error in file " << filePath << ": " << e.what() << std::endl;
            file.close();
            return nlohmann::json::array();
        }

        file.close();
        return j;
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading JSON from file: " << e.what() << std::endl;
        return nlohmann::json::array();
    }
}

bool FileUtils::saveStudents(const std::unordered_map<std::string, student>& students) {
    try {
        nlohmann::json studentsArray = nlohmann::json::array();
        for (const auto& pair : students) {
            studentsArray.push_back(studentToJson(pair.second));
        }
        return saveJsonToFile(studentsArray, studentsFilePath);
    }
    catch (const std::exception& e) {
        std::cerr << "Error saving students: " << e.what() << std::endl;
        return false;
    }
}

bool FileUtils::saveCourses(const std::map<long, course>& courses) {
    try {
        // Explicitly clear the file first
        std::ofstream clearFile(coursesFilePath, std::ios::trunc);
        if (!clearFile.is_open()) {
            std::cerr << "Error: Could not open courses file for clearing: " << coursesFilePath << std::endl;
            return false;
        }
        clearFile.close();
        std::cout << "Courses file cleared successfully: " << coursesFilePath << std::endl;

        // Now create the JSON array and save it
        nlohmann::json coursesArray = nlohmann::json::array();
        for (const auto& pair : courses) {
            coursesArray.push_back(courseToJson(pair.second));
        }

        std::cout << "Saving " << courses.size() << " courses to JSON array" << std::endl;

        return saveJsonToFile(coursesArray, coursesFilePath);
    }
    catch (const std::exception& e) {
        std::cerr << "Error saving courses: " << e.what() << std::endl;
        return false;
    }
}

bool FileUtils::saveAdmins(const std::unordered_map<std::string, admin>& admins) {
    try {
        nlohmann::json adminsArray = nlohmann::json::array();
        for (const auto& pair : admins) {
            adminsArray.push_back(adminToJson(pair.second));
        }
        return saveJsonToFile(adminsArray, adminsFilePath);
    }
    catch (const std::exception& e) {
        std::cerr << "Error saving admins: " << e.what() << std::endl;
        return false;
    }
}

bool FileUtils::saveAllData(const courseSystem& system) {
    // Add debugging information
    std::cout << "Saving all data to disk..." << std::endl;
    std::cout << "Students: " << system.getAllStudents().size() << std::endl;
    std::cout << "Courses: " << system.getAllCourses().size() << std::endl;
    std::cout << "Admins: " << system.getAllAdmins().size() << std::endl;

    // Print paths being used
    std::cout << "Students file path: " << studentsFilePath << std::endl;
    std::cout << "Courses file path: " << coursesFilePath << std::endl;
    std::cout << "Admins file path: " << adminsFilePath << std::endl;

    // Create data directory if it doesn't exist
    std::string dataDir = "./data/";
    if (!std::filesystem::exists(dataDir)) {
        std::cout << "Creating data directory: " << dataDir << std::endl;
        std::filesystem::create_directory(dataDir);
    }

    // Save each data set
    bool studentsSuccess = saveStudents(system.getAllStudents());
    bool coursesSuccess = saveCourses(system.getAllCourses());
    bool adminsSuccess = saveAdmins(system.getAllAdmins());

    // Report results
    if (!studentsSuccess) {
        std::cerr << "Failed to save students data." << std::endl;
    }
    else {
        std::cout << "Students data saved successfully." << std::endl;
    }

    if (!coursesSuccess) {
        std::cerr << "Failed to save courses data." << std::endl;
    }
    else {
        std::cout << "Courses data saved successfully." << std::endl;
    }

    if (!adminsSuccess) {
        std::cerr << "Failed to save admins data." << std::endl;
    }
    else {
        std::cout << "Admins data saved successfully." << std::endl;
    }

    return studentsSuccess && coursesSuccess && adminsSuccess;
}

bool FileUtils::saveJsonToFile(const nlohmann::json& j, const std::string& filePath) {
    try {
        std::filesystem::path path(filePath);
        std::filesystem::path dir = path.parent_path();

        std::cout << "Saving JSON data to: " << filePath << std::endl;
        std::cout << "Parent directory: " << dir.string() << std::endl;

        if (!dir.empty() && !std::filesystem::exists(dir)) {
            std::cout << "Creating directory: " << dir.string() << std::endl;
            std::filesystem::create_directories(dir);
        }

        // Get absolute path to verify where we're writing
        std::filesystem::path absPath = std::filesystem::absolute(path);
        std::cout << "Absolute file path: " << absPath.string() << std::endl;

        // Open file with truncation flag to explicitly clear contents before writing
        std::ofstream file(filePath, std::ios::trunc);
        if (!file.is_open()) {
            std::cerr << "Could not open file for writing: " << filePath << std::endl;

            // Try to diagnose the issue
            if (!std::filesystem::exists(dir)) {
                std::cerr << "Directory does not exist: " << dir.string() << std::endl;
            }

            // Check permissions 
            std::error_code ec;
            auto perms = std::filesystem::status(dir, ec).permissions();
            if (ec) {
                std::cerr << "Error checking permissions: " << ec.message() << std::endl;
            }
            else {
                std::cout << "Directory permissions allow write: "
                    << ((perms & std::filesystem::perms::owner_write) != std::filesystem::perms::none)
                    << std::endl;
            }

            return false;
        }

        file << std::setw(4) << j << std::endl;
        file.close();

        // Verify file was written
        if (std::filesystem::exists(filePath)) {
            std::cout << "File successfully written: " << filePath
                << " (size: " << std::filesystem::file_size(filePath) << " bytes)" << std::endl;
            return true;
        }
        else {
            std::cerr << "File write operation completed but file doesn't exist: " << filePath << std::endl;
            return false;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error saving JSON to file: " << e.what() << std::endl;
        return false;
    }
}
bool FileUtils::loadStudents(std::unordered_map<std::string, student>& students) {
    try {
        nlohmann::json studentsArray = loadJsonFromFile(studentsFilePath);
        if (studentsArray.empty()) {
            std::cout << "No students data to load." << std::endl;
            return true;
        }

        students.clear();
        for (const auto& studentJson : studentsArray) {
            student s = jsonToStudent(studentJson);

            students[s.getUsername()] = s;
        }

        // Update the student counter to the highest ID + 1
        long maxId = 0;
        for (const auto& pair : students) {
            maxId = std::max(maxId, pair.second.getStudentID());
        }
        student::counter = maxId + 1;

        std::cout << "Student counter initialized to: " << student::counter << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading students: " << e.what() << std::endl;
        return false;
    }
}

bool FileUtils::loadCourses(std::map<long, course>& courses) {
    try {
        nlohmann::json coursesArray = loadJsonFromFile(coursesFilePath);
        if (coursesArray.empty()) {
            std::cout << "No courses data to load." << std::endl;
            return true;
        }

        courses.clear();

        // First pass: Find the maximum course ID
        long maxId = 0;
        for (const auto& courseJson : coursesArray) {
            long courseId = courseJson["course_id"].get<long>();
            maxId = std::max(maxId, courseId);
        }

        // Reset counter to max ID + 1
        course::resetCounter(maxId + 1);
        std::cout << "Course counter reset to: " << course::counter << std::endl;

        // Second pass: Load all courses
        for (const auto& courseJson : coursesArray) {
            course c = jsonToCourse(courseJson);
            courses[c.getCourseID()] = c;
        }

        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading courses: " << e.what() << std::endl;
        return false;
    }
}

bool FileUtils::loadAdmins(std::unordered_map<std::string, admin>& admins) {
    try {
        nlohmann::json adminsArray = loadJsonFromFile(adminsFilePath);
        if (adminsArray.empty()) {
            std::cout << "No admins data to load." << std::endl;
            return true;
        }

        // Reset counter before loading anything
        admin::counter = 1;
        std::cout << "Admin counter reset to 1 before loading" << std::endl;

        admins.clear();

        // Find max ID first before creating any admins
        long maxId = 0;
        for (const auto& adminJson : adminsArray) {
            long adminId = adminJson["admin_id"].get<long>();
            maxId = std::max(maxId, adminId);
        }

        // Make sure the static counter starts from the highest existing ID + 1
        admin::counter = maxId + 1;
        std::cout << "Admin counter initialized to: " << admin::counter << " (max ID found: " << maxId << ")" << std::endl;

        // Temporarily store a copy of the counter
        long counterBeforeCreation = admin::counter;

        // Now create the admin objects using direct construction with ID
        for (const auto& adminJson : adminsArray) {
            long adminId = adminJson["admin_id"].get<long>();
            std::string username = adminJson["username"].get<std::string>();
            std::string password = adminJson["password"].get<std::string>();
            std::string name = adminJson["name"].get<std::string>();
            std::string email = adminJson["email"].get<std::string>();

            // Create admin directly with fixed ID without incrementing counter
            admin a(adminId);
            a.setUsername(username);
            a.setPassword(password);
            a.setName(name);
            a.setEmail(email);
            a.setRole('A');

            admins[username] = a;
        }

        // Verify counter didn't change unexpectedly
        if (admin::counter != counterBeforeCreation) {
            std::cout << "WARNING: Admin counter changed during creation from "
                << counterBeforeCreation << " to " << admin::counter
                << " - fixing it back!" << std::endl;
            admin::counter = counterBeforeCreation;
        }

        std::cout << "Admin loading complete. Final counter value: " << admin::counter << std::endl;

        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading admins: " << e.what() << std::endl;
        return false;
    }
}

bool FileUtils::loadAllData(courseSystem& system) {
    std::map<long, course> courses;
    std::unordered_map<std::string, student> students;
    std::unordered_map<std::string, admin> admins;

    bool coursesSuccess = loadCourses(courses);
    if (!coursesSuccess) {
        std::cerr << "Failed to load courses data." << std::endl;
        return false;
    }

    bool studentsSuccess = loadStudents(students);
    if (!studentsSuccess) {
        std::cerr << "Failed to load students data." << std::endl;
        return false;
    }

    bool adminsSuccess = loadAdmins(admins);
    if (!adminsSuccess) {
        std::cerr << "Failed to load admins data." << std::endl;
        return false;
    }

    if (coursesSuccess && studentsSuccess && adminsSuccess) {
        system.courses = courses;
        system.students = students;
        system.admins = admins;

        // Set up prerequisites
        nlohmann::json coursesArray = loadJsonFromFile(coursesFilePath);
        if (!coursesArray.empty()) {
            for (const auto& courseJson : coursesArray) {
                long courseId = courseJson["course_id"].get<long>();
                auto courseIt = system.courses.find(courseId);
                if (courseIt != system.courses.end()) {
                    auto& c = courseIt->second;
                    c.clearPrerequisites();
                    if (courseJson.contains("prerequisites") && courseJson["prerequisites"].is_array()) {
                        for (const auto& prereqId : courseJson["prerequisites"]) {
                            long id = prereqId.get<long>();
                            auto prereqIt = system.courses.find(id);
                            if (prereqIt != system.courses.end()) {
                                c.addPrerequisite(prereqIt->second);
                            }
                        }
                    }
                }
            }
        }

        // Load student courses
        nlohmann::json studentsArray = loadJsonFromFile(studentsFilePath);
        if (!studentsArray.empty()) {
            for (const auto& studentJson : studentsArray) {
                std::string username = studentJson["username"].get<std::string>();
                auto studentIt = system.students.find(username);
                if (studentIt != system.students.end()) {
                    auto& s = studentIt->second;
                    s.courses.clear();

                    if (studentJson.contains("courses") && studentJson["courses"].is_array()) {
                        for (const auto& coursePairJson : studentJson["courses"]) {
                            long courseId = coursePairJson["course"]["course_id"].get<long>();
                            auto courseIt = system.courses.find(courseId);
                            if (courseIt != system.courses.end()) {
                                grade g = jsonToGrade(coursePairJson["grade"]);
                                s.courses.emplace_back(courseIt->second, g);
                            }
                        }
                    }
                    s.calculateGPA();
                }
            }
        }

        return true;
    }

    return false;
}