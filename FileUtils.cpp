#include "stdafx.h"
#include "FileUtils.h"
#include <filesystem>
#include <iostream>

using namespace std;
namespace fs = std::filesystem;

// Define static file paths
const string FileUtils::STUDENTS_FILE = "students.json";
const string FileUtils::COURSES_FILE = "courses.json";
const string FileUtils::ADMINS_FILE = "admins.json";

// Check if file exists
bool FileUtils::fileExists(const string& filename) {
    return fs::exists(filename);
}

// Create empty file if it doesn't exist
void FileUtils::createFileIfNotExists(const string& filename) {
    if (!fileExists(filename)) {
        ofstream file(filename);
        file << "{}"; // Empty JSON object
        file.close();
    }
}

// Load all data from files
bool FileUtils::loadAllData(courseSystem& system) {
    try {
        map<long, course> courses;
        map<long, student> students;
        vector<admin> admins;

        bool success = true;

        if (!loadCourses(courses)) success = false;
        if (!loadStudents(students)) success = false;
        if (!loadAdmins(admins)) success = false;

        // Add all loaded data to the system
        for (auto& c : courses) {
            system.addCourse(c.second);
        }

        for (auto& s : students) {
            system.addStudent(s.second);
        }

        for (auto& a : admins) {
            system.addAdmin(a);
        }

        return success;
    }
    catch (const exception& e) {
        cerr << "Error loading all data: " << e.what() << endl;
        return false;
    }
}

// Save all data to files
bool FileUtils::saveAllData(const courseSystem& system) {
    try {
        bool coursesSaved = saveCourses(system.getAllCourses());
        bool studentsSaved = saveStudents(system.getAllStudents());
        bool adminsSaved = saveAdmins(system.getAllAdmins());

        return coursesSaved && studentsSaved && adminsSaved;
    }
    catch (const exception& e) {
        cerr << "Error saving data: " << e.what() << endl;
        return false;
    }
}

// Load courses from file
bool FileUtils::loadCourses(map<long, course>& courses) {
    try {
        createFileIfNotExists(COURSES_FILE);

        ifstream file(COURSES_FILE);
        if (!file.is_open()) {
            return false;
        }

        json j;
        file >> j;
        file.close();

        courses.clear();

        for (auto& element : j.items()) {
            long id = stol(element.key());
            auto& courseObj = element.value();

            course c;
            c.CourseID = id;
            c.title = courseObj["title"].get<string>();
            c.description = courseObj["description"].get<string>();
            c.instructor = courseObj["instructor"].get<string>();
            c.semester = courseObj["semester"].get<string>();
            c.credithours = courseObj["credithours"].get<int>();
            c.syllabus = courseObj["syllabus"].get<string>();
            c.enrolled = courseObj["enrolled"].get<bool>();

            // Load prerequisites
            if (courseObj.contains("prerequisites") && !courseObj["prerequisites"].is_null()) {
                for (auto& prereqId : courseObj["prerequisites"]) {
                    long pId = prereqId.get<long>();
                    // We'll handle prerequisites later after all courses are loaded
                }
            }

            courses[id] = c;
        }

        // Update course counter to be higher than any existing ID
        long maxId = 0;
        for (const auto& pair : courses) {
            maxId = max(maxId, pair.first);
        }
        course::counter = max(course::counter, maxId);

        return true;
    }
    catch (const exception& e) {
        cerr << "Error loading courses: " << e.what() << endl;
        return false;
    }
}

// Load students from file
bool FileUtils::loadStudents(map<long, student>& students) {
    try {
        createFileIfNotExists(STUDENTS_FILE);

        ifstream file(STUDENTS_FILE);
        if (!file.is_open()) {
            return false;
        }

        json j;
        file >> j;
        file.close();

        students.clear();

        for (auto& element : j.items()) {
            long id = stol(element.key());
            auto& studentObj = element.value();

            string username = studentObj["username"].get<string>();
            string name = studentObj["name"].get<string>();
            string email = studentObj["email"].get<string>();

            // Create student (we'll need to modify our student class to support this)
            student s;
            // s.setUsername(username);
            // s.setName(name);
            // s.setEmail(email);
            // s.setStudentID(id);

            // Load courses and grades
            if (studentObj.contains("courses") && !studentObj["courses"].is_null()) {
                for (auto& courseId : studentObj["courses"]) {
                    long cId = courseId.get<long>();
                    // We would add each course to the student here
                }
            }

            if (studentObj.contains("grades") && !studentObj["grades"].is_null()) {
                for (auto& gradeObj : studentObj["grades"].items()) {
                    long courseId = stol(gradeObj.key());
                    auto& g = gradeObj.value();

                    grade newGrade(
                        g["semester"].get<int>(),
                        g["grade"].get<string>()[0],
                        g["year"].get<int>(),
                        g["gpa"].get<float>()
                    );

                    // We would add each grade to the student here
                }
            }

            students[id] = s;
        }

        // Update student counter to be higher than any existing ID
        long maxId = 0;
        for (const auto& pair : students) {
            maxId = max(maxId, pair.first);
        }
        student::counter = max(student::counter, maxId);

        return true;
    }
    catch (const exception& e) {
        cerr << "Error loading students: " << e.what() << endl;
        return false;
    }
}

// Load admins from file
bool FileUtils::loadAdmins(vector<admin>& admins) {
    try {
        createFileIfNotExists(ADMINS_FILE);

        ifstream file(ADMINS_FILE);
        if (!file.is_open()) {
            return false;
        }

        json j;
        file >> j;
        file.close();

        admins.clear();

        for (auto& adminObj : j) {
            long id = adminObj["adminID"].get<long>();
            string username = adminObj["username"].get<string>();
            string name = adminObj["name"].get<string>();
            string email = adminObj["email"].get<string>();

            // Create admin (simplified - actual implementation would involve passwords)
            admin a(id);
            // a.setUsername(username);
            // a.setName(name);
            // a.setEmail(email);

            admins.push_back(a);
        }

        // Update admin counter to be higher than any existing ID
        long maxId = 0;
        for (const auto& a : admins) {
            maxId = max(maxId, a.getAdminID());
        }
        admin::counter = max(admin::counter, maxId);

        return true;
    }
    catch (const exception& e) {
        cerr << "Error loading admins: " << e.what() << endl;
        return false;
    }
}

// Save courses to file
bool FileUtils::saveCourses(const map<long, course>& courses) {
    try {
        json j;

        for (const auto& pair : courses) {
            const course& c = pair.second;
            string id = to_string(c.getCourseID());

            j[id] = {
                {"title", c.getTitle()},
                {"description", c.getDescription()},
                {"instructor", c.getInstructor()},
                {"semester", c.getSemester()},
                {"credithours", c.getCreditHours()},
                {"syllabus", c.getSyllabus()},
                {"enrolled", c.isEnrolled()}
            };

            // Save prerequisites as array of IDs
            json prereqsArray = json::array();
            for (const auto& prereq : c.getPrerequisites()) {
                prereqsArray.push_back(prereq.getCourseID());
            }
            j[id]["prerequisites"] = prereqsArray;
        }

        ofstream file(COURSES_FILE);
        if (!file.is_open()) {
            return false;
        }

        file << j.dump(4); // Pretty print with 4-space indentation
        file.close();

        return true;
    }
    catch (const exception& e) {
        cerr << "Error saving courses: " << e.what() << endl;
        return false;
    }
}

// Save students to file
bool FileUtils::saveStudents(const map<long, student>& students) {
    try {
        json j;

        for (const auto& pair : students) {
            const student& s = pair.second;
            string id = to_string(s.getStudentID());

            j[id] = {
                {"username", s.getUsername()},
                {"name", s.getName()},
                {"email", s.getEmail()},
                {"gpa", s.getGPA()}
            };

            // Save courses as array of IDs
            json coursesArray = json::array();
            for (const auto& coursePair : s.getCourses()) {
                coursesArray.push_back(coursePair.first);
            }
            j[id]["courses"] = coursesArray;

            // Save grades
            json gradesObj = json::object();
            try {
                auto studentGrades = s.getGrades();
                for (const auto& gradePair : studentGrades) {
                    string courseId = to_string(gradePair.first.getCourseID());
                    const grade& g = gradePair.second;

                    gradesObj[courseId] = {
                        {"semester", g.getSemester()},
                        {"grade", string(1, g.getGrade())},
                        {"year", g.getYear()},
                        {"gpa", g.getGPA()}
                    };
                }
            }
            catch (const exception&) {
                // Skip if there's an error getting grades
            }
            j[id]["grades"] = gradesObj;
        }

        ofstream file(STUDENTS_FILE);
        if (!file.is_open()) {
            return false;
        }

        file << j.dump(4); // Pretty print with 4-space indentation
        file.close();

        return true;
    }
    catch (const exception& e) {
        cerr << "Error saving students: " << e.what() << endl;
        return false;
    }
}

// Save admins to file
bool FileUtils::saveAdmins(const vector<admin>& admins) {
    try {
        json j = json::array();

        for (const auto& a : admins) {
            j.push_back({
                {"adminID", a.getAdminID()},
                {"username", a.getUsername()},
                {"name", a.getName()},
                {"email", a.getEmail()}
                });
        }

        ofstream file(ADMINS_FILE);
        if (!file.is_open()) {
            return false;
        }

        file << j.dump(4); // Pretty print with 4-space indentation
        file.close();

        return true;
    }
    catch (const exception& e) {
        cerr << "Error saving admins: " << e.what() << endl;
        return false;
    }
}
