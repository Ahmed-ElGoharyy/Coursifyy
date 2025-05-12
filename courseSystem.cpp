#include "courseSystem.h"
#include "stdafx.h"
#include "course.h"
#include "user.h"
#include "admin.h"
#include "student.h"
#include "FileUtils.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTranslator>


courseSystem::courseSystem() {
    FileUtils::initializePaths();
    if (!loadData()) {
        if (admins.empty()) {
            admin defaultAdmin("admin", "admin123", "Administrator", "admin@school.edu");
            admins[defaultAdmin.getUsername()] = defaultAdmin;
            saveData();
        }
    }
}

courseSystem::~courseSystem() {
    if (!students.empty() || !courses.empty() || !admins.empty()) {
        saveData();
    }
}



bool courseSystem::registerStudent(QLineEdit* namee, QLineEdit* usernamee, QLineEdit* passwordd, QLineEdit* confirmpasswordd) {
    try {
        QString uname = usernamee->text().trimmed();
        string username = uname.toStdString();
        QString pword = passwordd->text();
        string password = pword.toStdString();
        QString nam = namee->text().trimmed();
        string name = nam.toStdString();
        QString cpassword = confirmpasswordd->text();
        string confirmpassword = cpassword.toStdString();

        if (uname.isEmpty() || pword.isEmpty() || nam.isEmpty() || cpassword.isEmpty()) {
            QMessageBox::warning(nullptr, " Error  ", " \n Please fill the required info \n");
            return false;
        }

        if (password != confirmpassword) {
            QMessageBox::warning(nullptr, "Password don't match ", " \n Confirm your password again. \n");
            return false;
        }

        if (students.find(username) != students.end()) {
            QMessageBox::warning(nullptr, "Username already exists ", " \n Username Already taken. \n Choose another username. \n");
            return false;
        }

        if (user::isValidUsername(username) && user::isValidPassword(password)) {

            string hashedpass = user::hashPassword(password);

            student newStudent(username, hashedpass, name);
            students[username] = newStudent;
            saveData();

            return true;
        }
        else {
            QMessageBox::warning(nullptr, "Bad Username and Password format",
                " \n Username must be from 3 to 20 characters. \n Password should contain Uppercase, Lowercase & digit \n and More than 6 chars \n");
            return false;
        }
    }
    catch (exception& e) {
        cout << "Error registering student: " << e.what() << endl;
        return false;
    }
}





bool courseSystem::registerAdmin(QLineEdit* namee, QLineEdit* usernamee,
    QLineEdit* passwordd, QLineEdit* confirmpasswordd, QLineEdit* keyedit) {
    try {
        QString uname = usernamee->text().trimmed();
        string username = uname.toStdString();
        QString pword = passwordd->text();
        string password = pword.toStdString();
        QString nam = namee->text().trimmed();
        string name = nam.toStdString();
        QString cpassword = confirmpasswordd->text();
        string confirmpassword = cpassword.toStdString();
        QString keyy = keyedit->text().trimmed();
        string key = keyy.toStdString();

        if (uname.isEmpty() || pword.isEmpty() || nam.isEmpty() || cpassword.isEmpty()) {
            QMessageBox::warning(nullptr, " Error  ", " \n Please fill the required info \n");
            return false;
        }

        if (password != confirmpassword) {
            QMessageBox::warning(nullptr, "Passwords don't match ", " \n Confirm your password again. \n");
            return false;
        }

        if (admins.find(username) != admins.end()) {
            QMessageBox::warning(nullptr, "Username already exists ", " \n Username Already taken. \n Choose another username. \n");
            return false;
        }

        if (key != "1234") {
            QMessageBox::warning(nullptr, "Wrong Administrator Key.", " \n You can't register as admin without knowing the secret key \n \n PS : All Admins know that the key is '1234' :) \n");
            return false;
        }

        if (user::isValidUsername(username) && user::isValidPassword(password)) {
            string hashedpass = user::hashPassword(password);

            // Debug output before creating admin
            cout << "About to create new admin. Current admin counter: " << admin::counter << endl;

            admin newadmin(username, hashedpass, name);

            // Debug output after creating admin
            cout << "New admin created with ID: " << newadmin.getAdminID() << endl;
            cout << "Admin counter after creation: " << admin::counter << endl;

            admins[username] = newadmin;
            saveData();

            return true;
        }
        else {
            QMessageBox::warning(nullptr, "Bad Username and Password format",
                " \n Username must be from 3 to 20 characters. \n Password should contain Uppercase, Lowercase & digit \n and More than 6 chars \n");
            return false;
        }
    }
    catch (exception& e) {
        cout << "Error registering admin: " << e.what() << endl;
        return false;
    }
}




char courseSystem::authenticateUser(QLineEdit* usernameEdit, QLineEdit* passwordEdit, user*& loggedUser)   // USED
{
    QString username = usernameEdit->text().trimmed();
    string uname = username.toStdString();
    QString password = passwordEdit->text();
    string pword = password.toStdString();


    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(nullptr, " Error  ", " \n Username and password must not be empty. \n");
        usernameEdit->clear();
        passwordEdit->clear();

        return 'E';  //error

    }

    auto studentIt = students.find(uname);
    if (studentIt != students.end()) {
        if (studentIt->second.authenticate(pword)) {
            loggedUser = &studentIt->second;
            usernameEdit->clear();
            passwordEdit->clear();
            return 'S';
        }

    }


    auto adminIt = admins.find(uname);
    if (adminIt != admins.end()) {
        if (adminIt->second.authenticate(pword)) {
            loggedUser = &adminIt->second;
            usernameEdit->clear();
            passwordEdit->clear();
            return 'A';
        }

    }


    usernameEdit->clear();
    passwordEdit->clear();
    return 'F';


}





bool courseSystem::addCourse(const course& newCourse) {
    long courseID = newCourse.getCourseID();
    if (courses.find(courseID) != courses.end()) {
        return false;
    }
    courses[courseID] = newCourse;
    return true;
}

bool courseSystem::updateCourse(long courseID, const course& updatedCourse) {
    if (courses.find(courseID) == courses.end()) {
        return false;
    }

    courses[courseID] = updatedCourse;
    saveData();
    return true;
}

bool courseSystem::removeCourse(long courseID) {
    auto it = courses.find(courseID);
    if (it == courses.end()) {
        return false;
    }

    courses.erase(it);
    saveData();
    return true;
}

course* courseSystem::getCourse(long courseID) {
    auto it = courses.find(courseID);
    if (it != courses.end()) {
        return &it->second;
    }
    return nullptr;
}

std::vector<course> courseSystem::searchCourses(const std::string& searchTerm) const {
    std::vector<course> results;
    std::string lowerSearchTerm = searchTerm;
    std::transform(lowerSearchTerm.begin(), lowerSearchTerm.end(), lowerSearchTerm.begin(),
        [](unsigned char c) { return std::tolower(c); });

    for (const auto& pair : courses) {
        const course& c = pair.second;
        std::string lowerTitle = c.getTitle();
        std::string lowerDesc = c.getDescription();
        std::string lowerInstructor = c.getInstructor();

        std::transform(lowerTitle.begin(), lowerTitle.end(), lowerTitle.begin(),
            [](unsigned char c) { return std::tolower(c); });
        std::transform(lowerDesc.begin(), lowerDesc.end(), lowerDesc.begin(),
            [](unsigned char c) { return std::tolower(c); });
        std::transform(lowerInstructor.begin(), lowerInstructor.end(), lowerInstructor.begin(),
            [](unsigned char c) { return std::tolower(c); });

        if (lowerTitle.find(lowerSearchTerm) != std::string::npos ||
            lowerDesc.find(lowerSearchTerm) != std::string::npos ||
            lowerInstructor.find(lowerSearchTerm) != std::string::npos) {
            results.push_back(c);
        }
    }

    return results;
}

bool courseSystem::addStudent(const student& newStudent) {
    std::string username = newStudent.getUsername();
    if (students.find(username) != students.end()) {
        return false;
    }

    students[username] = newStudent;
    saveData();
    return true;
}

bool courseSystem::updateStudent(const std::string& username, const student& updatedStudent) {
    if (students.find(username) == students.end()) {
        return false;
    }

    students[username] = updatedStudent;
    saveData();
    return true;
}

student* courseSystem::getStudent(long studentID) {
    for (auto& pair : students) {
        if (pair.second.getStudentID() == studentID) {
            return &pair.second;
        }
    }
    return nullptr;
}

student* courseSystem::getStudentByUsername(const std::string& username) {
    auto it = students.find(username);
    if (it != students.end()) {
        return &it->second;
    }
    return nullptr;
}

bool courseSystem::addAdmin(const admin& newAdmin) {
    std::string username = newAdmin.getUsername();
    if (admins.find(username) != admins.end()) {
        return false;
    }

    admins[username] = newAdmin;
    saveData();
    return true;
}

admin* courseSystem::getAdminByUsername(const std::string& username) {
    auto it = admins.find(username);
    if (it != admins.end()) {
        return &it->second;
    }
    return nullptr;
}

admin* courseSystem::getAdmin(long adminID) {
    for (auto& pair : admins) {
        if (pair.second.getAdminID() == adminID) {
            return &pair.second;
        }
    }
    return nullptr;
}

bool courseSystem::updateStudentGrade(const std::string& username, long courseID, const grade& newGrade) {
    student* s = getStudentByUsername(username);
    if (!s) {
        return false;
    }

    course* c = getCourse(courseID);
    if (!c) {
        return false;
    }

    bool result = s->updateGrade(courseID, newGrade);
    if (result) {
        saveData();
    }
    return result;
}

bool courseSystem::enrollStudentInCourse(const std::string& username, long courseID) {
    student* s = getStudentByUsername(username);
    course* c = getCourse(courseID);

    if (!s || !c) return false;
    if (!c->checkPrerequisites(*s)) {
        return false;
    }

    try {
        if (s->registerCourse(*c)) {
            c->setEnrolled(true);
            saveData();
            return true;
        }
    }
    catch (...) {
        return false;
    }
    return false;
}


bool courseSystem::loadData() {
    bool result = FileUtils::loadAllData(*this);

    // Add debugging output to see what was loaded
    std::cout << "=== DATA LOADING RESULTS ===" << std::endl;
    std::cout << "Loading result: " << (result ? "SUCCESS" : "FAILURE") << std::endl;
    std::cout << "Students loaded: " << students.size() << std::endl;
    std::cout << "Courses loaded: " << courses.size() << std::endl;
    std::cout << "Admins loaded: " << admins.size() << std::endl;

    // Print course details if any were loaded
    if (!courses.empty()) {
        std::cout << "LOADED COURSES:" << std::endl;
        for (const auto& pair : courses) {
            std::cout << "  Course ID: " << pair.first
                << ", Title: " << pair.second.getTitle()
                << ", Instructor: " << pair.second.getInstructor() << std::endl;
        }
    }
    else {
        std::cout << "No courses were loaded from file." << std::endl;
        // Check if the courses file exists
        if (std::filesystem::exists(FileUtils::coursesFilePath)) {
            std::cout << "Courses file exists at: " << FileUtils::coursesFilePath << std::endl;
            std::cout << "File size: " << std::filesystem::file_size(FileUtils::coursesFilePath) << " bytes" << std::endl;
        }
        else {
            std::cout << "Courses file does not exist at: " << FileUtils::coursesFilePath << std::endl;
        }
    }

    return result;
}



bool courseSystem::saveData() {
    // Print current working directory
    std::cout << "Current working directory: "
        << std::filesystem::current_path().string() << std::endl;

    // Print the expected data directory path
    std::string dataDir = "./data/";
    std::filesystem::path absDataPath = std::filesystem::absolute(dataDir);
    std::cout << "Absolute data directory path: " << absDataPath.string() << std::endl;

    // Print expected file paths
    std::cout << "Expected courses file: " << absDataPath.string() + "/courses.json" << std::endl;

    // Print course count before saving
    std::cout << "Number of courses to save: " << courses.size() << std::endl;
    for (const auto& pair : courses) {
        std::cout << "Course ID: " << pair.first
            << ", Title: " << pair.second.getTitle() << std::endl;
    }

    // Call the original save function
    bool result = FileUtils::saveAllData(*this);

    // Verify file existence after save
    std::string coursesFilePath = (absDataPath / "courses.json").string();
    if (std::filesystem::exists(coursesFilePath)) {
        std::cout << "Courses file exists after save at: " << coursesFilePath << std::endl;
        std::cout << "File size: " << std::filesystem::file_size(coursesFilePath) << " bytes" << std::endl;
    }
    else {
        std::cout << "WARNING: Courses file does not exist after save!" << std::endl;
    }

    return result;
}
bool courseSystem::importCoursesFromCSV(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file:" << file.errorString();
        return false;
    }

    QTextStream in(&file);
    int lineNumber = 0;
    int successCount = 0;

    // Structure to store course ID and its prerequisite IDs
    struct CoursePrereqData {
        long courseId;
        std::vector<long> prerequisiteIds;
    };
    std::vector<CoursePrereqData> coursePrereqs;

    // Skip header if present
    if (!in.atEnd()) in.readLine();

    // First pass: Create all courses without prerequisites
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        lineNumber++;
        if (line.isEmpty()) continue;

        QStringList tokens;
        bool inQuotes = false;
        QString currentField;

        // Parse CSV line correctly handling quoted fields
        for (QChar c : line) {
            if (c == '"') {
                inQuotes = !inQuotes;
            }
            else if (c == ',' && !inQuotes) {
                tokens.append(currentField.trimmed());
                currentField.clear();
            }
            else {
                currentField += c;
            }
        }
        tokens.append(currentField.trimmed());

        if (tokens.size() < 6) {
            qDebug() << "Skipping line" << lineNumber << ": insufficient fields";
            continue;
        }

        try {
            // Parse course data
            course newCourse(
                tokens[0].toStdString(), // title
                tokens[1].toStdString(), // description
                tokens[2].toStdString(), // instructor
                tokens[3].toStdString(), // semester
                tokens[4].toInt(),       // credit hours
                {},                      // prerequisites (empty for now)
                tokens.size() > 5 ? tokens[5].toStdString() : "" // syllabus
            );

            // Store the new course ID
            long newCourseId = newCourse.getCourseID();

            // Parse prerequisite IDs
            std::vector<long> prereqIds;
            if (tokens.size() > 6 && !tokens[6].isEmpty()) {
                for (const QString& idStr : tokens[6].split(';', Qt::SkipEmptyParts)) {
                    bool ok;
                    long id = idStr.toLong(&ok);
                    if (ok) prereqIds.push_back(id);
                }
            }

            // Add course to the system
            if (addCourse(newCourse)) {
                coursePrereqs.push_back({ newCourseId, prereqIds });
                successCount++;
                qDebug() << "Added course:" << newCourse.getTitle().c_str() << "with ID:" << newCourseId;
            }
            else {
                qDebug() << "Failed to add course:" << newCourse.getTitle().c_str() << "with ID:" << newCourseId;
            }
        }
        catch (const std::exception& e) {
            qDebug() << "Error processing line" << lineNumber << ":" << e.what();
        }
        catch (...) {
            qDebug() << "Unknown error processing line" << lineNumber;
        }
    }

    file.close();

    // Second pass: Set prerequisites
    for (const auto& data : coursePrereqs) {
        course* currentCourse = getCourse(data.courseId);
        if (!currentCourse) {
            qDebug() << "Course not found in system:" << data.courseId;
            continue;
        }

        // Clear any existing prerequisites
        currentCourse->clearPrerequisites();

        // Add each prerequisite by fetching the actual course object
        for (long prereqId : data.prerequisiteIds) {
            course* prereqCourse = getCourse(prereqId);
            if (prereqCourse) {
                currentCourse->addPrerequisite(*prereqCourse);
                qDebug() << "Added prerequisite:" << prereqCourse->getTitle().c_str()
                    << "to course:" << currentCourse->getTitle().c_str();
            }
            else {
                qDebug() << "Prerequisite course not found:" << prereqId
                    << " for course:" << currentCourse->getTitle().c_str();
            }
        }
    }

    // Save the data to persist the changes
    qDebug() << "Attempting to save data with" << successCount << "new courses";
    bool saveResult = saveData();
    if (saveResult) {
        qDebug() << "Data saved successfully";
    }
    else {
        qDebug() << "Failed to save data after import";
    }

    return successCount > 0 && saveResult;
}

void courseSystem::importCoursesFromFile(QWidget* parent) {
    // Open file dialog to select CSV file
    QString fileName = QFileDialog::getOpenFileName(
        parent,
        QObject::tr("Select Course CSV File"),
        QDir::homePath(),
        QObject::tr("CSV Files (*.csv);;All Files (*)")
    );

    if (fileName.isEmpty()) {
        return; // User canceled
    }

    qDebug() << "Selected file:" << fileName;

    // Call the import function with the selected file
    if (importCoursesFromCSV(fileName)) {
        QMessageBox::information(parent, QObject::tr("Success"),
            QObject::tr("Courses imported successfully!"));
    }
    else {
        QMessageBox::warning(parent, QObject::tr("Error"),
            QObject::tr("Failed to import courses from file."));
    }
}