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
    std::string dataDir = "./data/";
    if (!std::filesystem::exists(dataDir)) {
        std::filesystem::create_directory(dataDir);
    }
    studentsFilePath = dataDir + "students.json";
    coursesFilePath = dataDir + "courses.json";
    adminsFilePath = dataDir + "admins.json";
}

nlohmann::json FileUtils::studentToJson(const student& s) {
    nlohmann::json j;
    j["username"] = s.getUsername();
    j["password"] = s.getPassword(); // Changed from password_hash to password
    j["name"] = s.getName();
    j["email"] = s.getEmail();
    j["role"] = std::string(1, s.getRole());
    j["student_id"] = s.getStudentID();
    j["gpa"] = s.getGPA();
    j["max_credit_hours"] = s.max_credit_hours;

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
        j["password"].get<std::string>(), // Changed from password_hash to password
        j["name"].get<std::string>(),
        j["email"].get<std::string>()
    );

    s.setID(j["student_id"].get<long>());
    s.setGPA(j["gpa"].get<float>());
    s.max_credit_hours = j["max_credit_hours"].get<int>();
    return s;
}

course FileUtils::jsonToCourse(const nlohmann::json& j) {
    course c(
        j["title"].get<std::string>(),
        j["description"].get<std::string>(),
        j["instructor"].get<std::string>(),
        j["semester"].get<std::string>(),
        j["credit_hours"].get<int>(),
        {},
        j["syllabus"].get<std::string>()
    );

    c.CourseID = j["course_id"].get<long>();
    c.setEnrolled(j["enrolled"].get<bool>());
    return c;
}

admin FileUtils::jsonToAdmin(const nlohmann::json& j) {
    admin a(
        j["username"].get<std::string>(),
        j["password"].get<std::string>(), // Changed from password_hash to password
        j["name"].get<std::string>(),
        j["email"].get<std::string>()
    );

    a.setAdminID(j["admin_id"].get<long>());
    return a;
}

grade FileUtils::jsonToGrade(const nlohmann::json& j) {
    grade g;
    g.setSemester(j["semester"].get<int>());
    g.setGrade(j["grade"].get<std::string>()[0]);
    g.setYear(j["year"].get<int>());
    g.setGPA(j["gpa"].get<float>());
    return g;
}

std::pair<course, grade> FileUtils::jsonToCourseGradePair(const nlohmann::json& j) {
    course c = jsonToCourse(j["course"]);
    grade g = jsonToGrade(j["grade"]);
    return std::make_pair(c, g);
}

bool FileUtils::saveJsonToFile(const nlohmann::json& j, const std::string& filePath) {
    try {
        std::filesystem::path path(filePath);
        std::filesystem::path dir = path.parent_path();
        if (!dir.empty() && !std::filesystem::exists(dir)) {
            std::filesystem::create_directories(dir);
        }

        std::ofstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Could not open file for writing: " << filePath << std::endl;
            return false;
        }

        file << std::setw(4) << j << std::endl;
        file.close();
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error saving JSON to file: " << e.what() << std::endl;
        return false;
    }
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
        nlohmann::json coursesArray = nlohmann::json::array();
        for (const auto& pair : courses) {
            coursesArray.push_back(courseToJson(pair.second));
        }
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
    bool studentsSuccess = saveStudents(system.getAllStudents());
    bool coursesSuccess = saveCourses(system.getAllCourses());
    bool adminsSuccess = saveAdmins(system.getAllAdmins());

    if (!studentsSuccess) {
        std::cerr << "Failed to save students data." << std::endl;
    }
    if (!coursesSuccess) {
        std::cerr << "Failed to save courses data." << std::endl;
    }
    if (!adminsSuccess) {
        std::cerr << "Failed to save admins data." << std::endl;
    }

    return studentsSuccess && coursesSuccess && adminsSuccess;
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

        // Update the student counter if needed to avoid ID collisions
        long maxId = 0;
        for (const auto& pair : students) {
            maxId = std::max(maxId, pair.second.getStudentID());
        }
        if (maxId >= student::counter) {
            student::counter = maxId + 1;
        }

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
        for (const auto& courseJson : coursesArray) {
            course c = jsonToCourse(courseJson);
            courses[c.getCourseID()] = c;
        }

        // Update the course counter if needed to avoid ID collisions
        long maxId = 0;
        for (const auto& pair : courses) {
            maxId = std::max(maxId, pair.first);
        }
        if (maxId >= course::counter) {
            course::counter = maxId + 1;
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

        admins.clear();
        for (const auto& adminJson : adminsArray) {
            admin a = jsonToAdmin(adminJson);
            admins[a.getUsername()] = a;
        }

        // Update the admin counter if needed to avoid ID collisions
        long maxId = 0;
        for (const auto& pair : admins) {
            maxId = std::max(maxId, pair.second.getAdminID());
        }
        if (maxId >= admin::counter) {
            admin::counter = maxId + 1;
        }

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