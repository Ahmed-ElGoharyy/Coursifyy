#include "stdafx.h"
#include "FileUtils.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <filesystem>
using namespace std;


string FileUtils::studentsFilePath = "students.json";
string FileUtils::coursesFilePath = "courses.json";
string FileUtils::adminsFilePath = "admins.json";

void FileUtils::initializePaths() {
    // Get the executable's directory as base path
    filesystem::path execPath = filesystem::current_path();
    string dataDir = (execPath / "data").string() + "/";

    cout << "Setting data directory to: " << dataDir << endl;

    if (!filesystem::exists(dataDir)) {
        cout << "Creating data directory: " << dataDir << endl;
        filesystem::create_directory(dataDir);
    }

    studentsFilePath = dataDir + "students.json";
    coursesFilePath = dataDir + "courses.json";
    adminsFilePath = dataDir + "admins.json";

    cout << "Files will be saved to:" << endl;
    cout << "- Students: " << studentsFilePath << endl;
    cout << "- Courses: " << coursesFilePath << endl;
    cout << "- Admins: " << adminsFilePath << endl;
}

nlohmann::json FileUtils::studentToJson(const student& s) {
    nlohmann::json j;
    j["username"] = s.getUsername();
    j["password"] = s.getPassword();
    j["name"] = s.getName();
    j["email"] = s.getEmail();
    j["role"] = string(1, s.getRole());
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
    j["role"] = string(1, a.getRole());
    j["admin_id"] = a.getAdminID();
    return j;
}

nlohmann::json FileUtils::gradeToJson(const grade& g) {
    nlohmann::json j;
    j["semester"] = g.getSemester();
    j["grade"] = string(1, g.getGrade());
    j["year"] = g.getYear();
    j["gpa"] = g.getGPA();
    return j;
}


nlohmann::json FileUtils::courseGradePairToJson(const pair<course, grade>& pair) {
    nlohmann::json j;
    j["course"] = courseToJson(pair.first);
    j["grade"] = gradeToJson(pair.second);
    return j;
}

student FileUtils::jsonToStudent(const nlohmann::json& j) {
    student s(
        j["username"].get<string>(),
        j["password"].get<string>(),
        j["name"].get<string>(),
        j["email"].get<string>(),
        j["current_semester"].get<string>()  // Add semester
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
    c.setTitle(j["title"].get<string>());
    c.setDescription(j["description"].get<string>());
    c.setInstructor(j["instructor"].get<string>());
    c.setSemester(j["semester"].get<string>());
    c.setCreditHours(j["credit_hours"].get<int>());
    c.setSyllabus(j["syllabus"].get<string>());
    c.setEnrolled(j["enrolled"].get<bool>());

    return c;
}

admin FileUtils::jsonToAdmin(const nlohmann::json& j) {
    // Create admin with specific ID to avoid counter incrementing
    long adminId = j["admin_id"].get<long>();
    cout << "Creating admin from JSON with fixed ID: " << adminId << endl;

    admin a(adminId);

    // Set properties manually
    a.setUsername(j["username"].get<string>());
    a.setPassword(j["password"].get<string>());
    a.setName(j["name"].get<string>());
    a.setEmail(j["email"].get<string>());
    a.setRole('A');

    return a;
}

grade FileUtils::jsonToGrade(const nlohmann::json& j) {
    grade g;
    g.setSemester(j["semester"].get<int>());

    // Get the grade character
    char gradeChar = j["grade"].get<string>()[0];
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
pair<course, grade> FileUtils::jsonToCourseGradePair(const nlohmann::json& j) {
    course c = jsonToCourse(j["course"]);
    grade g = jsonToGrade(j["grade"]);
    return make_pair(c, g);
}


nlohmann::json FileUtils::loadJsonFromFile(const string& filePath) {
    try {
        if (!filesystem::exists(filePath)) {
            cout << "File does not exist: " << filePath << endl;
            return nlohmann::json::array();
        }

        ifstream file(filePath);
        if (!file.is_open()) {
            cout << "Could not open file for reading: " << filePath << endl;
            return nlohmann::json::array();
        }

        file.seekg(0, ios::end);
        if (file.tellg() == 0) {
            cout << "File is empty: " << filePath << endl;
            file.close();
            return nlohmann::json::array();
        }
        file.seekg(0, ios::beg);

        nlohmann::json j;
        try {
            file >> j;
        }
        catch (const nlohmann::json::parse_error& e) {
            cerr << "JSON parse error in file " << filePath << ": " << e.what() << endl;
            file.close();
            return nlohmann::json::array();
        }

        file.close();
        return j;
    }
    catch (const exception& e) {
        cerr << "Error loading JSON from file: " << e.what() << endl;
        return nlohmann::json::array();
    }
}

bool FileUtils::saveStudents(const unordered_map<string, student>& students) {
    try {
        nlohmann::json studentsArray = nlohmann::json::array();
        for (const auto& pair : students) {
            studentsArray.push_back(studentToJson(pair.second));
        }
        return saveJsonToFile(studentsArray, studentsFilePath);
    }
    catch (const exception& e) {
        cerr << "Error saving students: " << e.what() << endl;
        return false;
    }
}

bool FileUtils::saveCourses(const map<long, course>& courses) {
    try {
        // Explicitly clear the file first
        ofstream clearFile(coursesFilePath, ios::trunc);
        if (!clearFile.is_open()) {
            cerr << "Error: Could not open courses file for clearing: " << coursesFilePath << endl;
            return false;
        }
        clearFile.close();
        cout << "Courses file cleared successfully: " << coursesFilePath << endl;

        // Now create the JSON array and save it
        nlohmann::json coursesArray = nlohmann::json::array();
        for (const auto& pair : courses) {
            coursesArray.push_back(courseToJson(pair.second));
        }

        cout << "Saving " << courses.size() << " courses to JSON array" << endl;

        return saveJsonToFile(coursesArray, coursesFilePath);
    }
    catch (const exception& e) {
        cerr << "Error saving courses: " << e.what() << endl;
        return false;
    }
}

bool FileUtils::saveAdmins(const unordered_map<string, admin>& admins) {
    try {
        nlohmann::json adminsArray = nlohmann::json::array();
        for (const auto& pair : admins) {
            adminsArray.push_back(adminToJson(pair.second));
        }
        return saveJsonToFile(adminsArray, adminsFilePath);
    }
    catch (const exception& e) {
        cerr << "Error saving admins: " << e.what() << endl;
        return false;
    }
}

bool FileUtils::saveAllData(const courseSystem& system) {
    // Add debugging information
    cout << "Saving all data to disk..." << endl;
    cout << "Students: " << system.getAllStudents().size() << endl;
    cout << "Courses: " << system.getAllCourses().size() << endl;
    cout << "Admins: " << system.getAllAdmins().size() << endl;

    // Print paths being used
    cout << "Students file path: " << studentsFilePath << endl;
    cout << "Courses file path: " << coursesFilePath << endl;
    cout << "Admins file path: " << adminsFilePath << endl;

    // Create data directory if it doesn't exist
    string dataDir = "./data/";
    if (!filesystem::exists(dataDir)) {
        cout << "Creating data directory: " << dataDir << endl;
        filesystem::create_directory(dataDir);
    }

    // Save each data set
    bool studentsSuccess = saveStudents(system.getAllStudents());
    bool coursesSuccess = saveCourses(system.getAllCourses());
    bool adminsSuccess = saveAdmins(system.getAllAdmins());

    // Report results
    if (!studentsSuccess) {
        cerr << "Failed to save students data." << endl;
    }
    else {
        cout << "Students data saved successfully." << endl;
    }

    if (!coursesSuccess) {
        cerr << "Failed to save courses data." << endl;
    }
    else {
        cout << "Courses data saved successfully." << endl;
    }

    if (!adminsSuccess) {
        cerr << "Failed to save admins data." << endl;
    }
    else {
        cout << "Admins data saved successfully." << endl;
    }

    return studentsSuccess && coursesSuccess && adminsSuccess;
}

bool FileUtils::saveJsonToFile(const nlohmann::json& j, const string& filePath) {
    try {
        filesystem::path path(filePath);
        filesystem::path dir = path.parent_path();

        cout << "Saving JSON data to: " << filePath << endl;
        cout << "Parent directory: " << dir.string() << endl;

        if (!dir.empty() && !filesystem::exists(dir)) {
            cout << "Creating directory: " << dir.string() << endl;
            filesystem::create_directories(dir);
        }

        // Get absolute path to verify where we're writing
        filesystem::path absPath = filesystem::absolute(path);
        cout << "Absolute file path: " << absPath.string() << endl;

        // Open file with truncation flag to explicitly clear contents before writing
        ofstream file(filePath, ios::trunc);
        if (!file.is_open()) {
            cerr << "Could not open file for writing: " << filePath << endl;

            // Try to diagnose the issue
            if (!filesystem::exists(dir)) {
                cerr << "Directory does not exist: " << dir.string() << endl;
            }

            // Check permissions 
            error_code ec;
            auto perms = filesystem::status(dir, ec).permissions();
            if (ec) {
                cerr << "Error checking permissions: " << ec.message() << endl;
            }
            else {
                cout << "Directory permissions allow write: "
                    << ((perms & filesystem::perms::owner_write) != filesystem::perms::none)
                    << endl;
            }

            return false;
        }

        file << setw(4) << j << endl;
        file.close();

        // Verify file was written
        if (filesystem::exists(filePath)) {
            cout << "File successfully written: " << filePath
                << " (size: " << filesystem::file_size(filePath) << " bytes)" << endl;
            return true;
        }
        else {
            cerr << "File write operation completed but file doesn't exist: " << filePath << endl;
            return false;
        }
    }
    catch (const exception& e) {
        cerr << "Error saving JSON to file: " << e.what() << endl;
        return false;
    }
}
bool FileUtils::loadStudents(unordered_map<string, student>& students) {
    try {
        nlohmann::json studentsArray = loadJsonFromFile(studentsFilePath);
        if (studentsArray.empty()) {
            cout << "No students data to load." << endl;
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
            maxId = max(maxId, pair.second.getStudentID());
        }
        student::counter = maxId + 1;

        cout << "Student counter initialized to: " << student::counter << endl;
        return true;
    }
    catch (const exception& e) {
        cerr << "Error loading students: " << e.what() << endl;
        return false;
    }
}

bool FileUtils::loadCourses(map<long, course>& courses) {
    try {
        nlohmann::json coursesArray = loadJsonFromFile(coursesFilePath);
        if (coursesArray.empty()) {
            cout << "No courses data to load." << endl;
            return true;
        }

        courses.clear();

        // First pass: Find the maximum course ID
        long maxId = 0;
        for (const auto& courseJson : coursesArray) {
            long courseId = courseJson["course_id"].get<long>();
            maxId = max(maxId, courseId);
        }

        // Reset counter to max ID + 1
        course::resetCounter(maxId + 1);
        cout << "Course counter reset to: " << course::counter << endl;

        // Second pass: Load all courses
        for (const auto& courseJson : coursesArray) {
            course c = jsonToCourse(courseJson);
            courses[c.getCourseID()] = c;
        }

        return true;
    }
    catch (const exception& e) {
        cerr << "Error loading courses: " << e.what() << endl;
        return false;
    }
}

bool FileUtils::loadAdmins(unordered_map<string, admin>& admins) {
    try {
        nlohmann::json adminsArray = loadJsonFromFile(adminsFilePath);
        if (adminsArray.empty()) {
            cout << "No admins data to load." << endl;
            return true;
        }

        // Reset counter before loading anything
        admin::counter = 1;
        cout << "Admin counter reset to 1 before loading" << endl;

        admins.clear();

        // Find max ID first before creating any admins
        long maxId = 0;
        for (const auto& adminJson : adminsArray) {
            long adminId = adminJson["admin_id"].get<long>();
            maxId = max(maxId, adminId);
        }

        // Make sure the static counter starts from the highest existing ID + 1
        admin::counter = maxId + 1;
        cout << "Admin counter initialized to: " << admin::counter << " (max ID found: " << maxId << ")" << endl;

        // Temporarily store a copy of the counter
        long counterBeforeCreation = admin::counter;

        // Now create the admin objects using direct construction with ID
        for (const auto& adminJson : adminsArray) {
            long adminId = adminJson["admin_id"].get<long>();
            string username = adminJson["username"].get<string>();
            string password = adminJson["password"].get<string>();
            string name = adminJson["name"].get<string>();
            string email = adminJson["email"].get<string>();

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
            cout << "WARNING: Admin counter changed during creation from "
                << counterBeforeCreation << " to " << admin::counter
                << " - fixing it back!" << endl;
            admin::counter = counterBeforeCreation;
        }

        cout << "Admin loading complete. Final counter value: " << admin::counter << endl;

        return true;
    }
    catch (const exception& e) {
        cerr << "Error loading admins: " << e.what() << endl;
        return false;
    }
}

bool FileUtils::loadAllData(courseSystem& system) {
    map<long, course> courses;
    unordered_map<string, student> students;
    unordered_map<string, admin> admins;

    bool coursesSuccess = loadCourses(courses);
    if (!coursesSuccess) {
        cerr << "Failed to load courses data." << endl;
        return false;
    }

    bool studentsSuccess = loadStudents(students);
    if (!studentsSuccess) {
        cerr << "Failed to load students data." << endl;
        return false;
    }

    bool adminsSuccess = loadAdmins(admins);
    if (!adminsSuccess) {
        cerr << "Failed to load admins data." << endl;
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
                string username = studentJson["username"].get<string>();
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