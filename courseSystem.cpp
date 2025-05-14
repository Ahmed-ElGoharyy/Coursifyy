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

    // Check if this course ID already exists
    if (courses.find(courseID) != courses.end()) {
        return false;
    }

    // Add the course with the same ID it already has
    courses[courseID] = newCourse;

    // Debug output
    std::cout << "Added course with ID: " << courseID << ", counter value: " << course::counter << std::endl;

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
    if (searchTerm.empty()) {
        return results;
    }

    std::string lowerSearchTerm = searchTerm;
    std::transform(lowerSearchTerm.begin(), lowerSearchTerm.end(), lowerSearchTerm.begin(),
        [](unsigned char c) { return std::tolower(c); });

    for (const auto& pair : courses) {
        const course& c = pair.second;
        std::string lowerTitle = c.getTitle();
        std::transform(lowerTitle.begin(), lowerTitle.end(), lowerTitle.begin(),
            [](unsigned char c) { return std::tolower(c); });

        if (lowerTitle.find(lowerSearchTerm) != std::string::npos) {
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

    // Make sure data directory exists
    if (!std::filesystem::exists(dataDir)) {
        std::cout << "Creating data directory: " << dataDir << std::endl;
        std::filesystem::create_directory(dataDir);
    }

    // Print course count before saving
    std::cout << "Number of courses to save: " << courses.size() << std::endl;
    for (const auto& pair : courses) {
        std::cout << "Course ID: " << pair.first
            << ", Title: " << pair.second.getTitle() << std::endl;
    }

    // Explicitly check if courses.json exists and try to remove it if it does
    std::string coursesFilePath = (absDataPath / "courses.json").string();
    if (std::filesystem::exists(coursesFilePath)) {
        std::cout << "Found existing courses file. Size: "
            << std::filesystem::file_size(coursesFilePath) << " bytes. Removing..." << std::endl;
        try {
            std::filesystem::remove(coursesFilePath);
            std::cout << "Successfully removed old courses file." << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Error removing old courses file: " << e.what() << std::endl;
        }
    }

    // Call the original save function
    bool result = FileUtils::saveAllData(*this);

    // Verify file existence after save
    if (std::filesystem::exists(coursesFilePath)) {
        std::cout << "Courses file exists after save at: " << coursesFilePath << std::endl;
        std::cout << "File size: " << std::filesystem::file_size(coursesFilePath) << " bytes" << std::endl;

        // Try to read file contents to verify it's a valid JSON
        try {
            std::ifstream file(coursesFilePath);
            if (file.is_open()) {
                std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                file.close();
                if (content.empty()) {
                    std::cout << "WARNING: Courses file is empty!" << std::endl;
                }
                else {
                    std::cout << "Courses file contains data (first 100 chars): "
                        << content.substr(0, std::min(content.size(), static_cast<size_t>(100)))
                        << (content.size() > 100 ? "..." : "") << std::endl;
                }
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Error reading back courses file: " << e.what() << std::endl;
        }
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

    // First, find the maximum existing course ID
    long maxExistingId = 0;
    for (const auto& pair : courses) {
        maxExistingId = std::max(maxExistingId, pair.first);
    }

    // Reset counter to max ID + 1
    course::resetCounter(maxExistingId + 1);
    qDebug() << "Course counter reset to:" << course::counter;

    // Structure to store course ID and its prerequisite IDs
    struct CoursePrereqData {
        long courseId;
        std::vector<long> prerequisiteIds;
    };
    std::vector<CoursePrereqData> coursePrereqs;

    // Make sure course counter starts from max ID + 1
    qDebug() << "Before import - Course counter:" << course::counter << ", Max existing ID:" << maxExistingId;
    if (maxExistingId >= course::counter) {
        course::counter = maxExistingId + 1;
        qDebug() << "Updated course counter to:" << course::counter;
    }

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
            // Create the course with empty prerequisites initially
            course newCourse(
                tokens[0].toStdString(), // title
                tokens[1].toStdString(), // description
                tokens[2].toStdString(), // instructor
                tokens[3].toStdString(), // semester
                tokens[4].toInt(),       // credit hours
                {},                      // prerequisites (empty for now)
                tokens.size() > 5 ? tokens[5].toStdString() : "" // syllabus
            );

            // Log the course ID assignment
            long newCourseId = newCourse.getCourseID();
            qDebug() << "Created new course with ID:" << newCourseId << "from CSV";

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
            if (courses.find(newCourseId) == courses.end()) {
                courses.insert({ newCourseId,newCourse });
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

    // Log final counter state
    qDebug() << "After import - Course counter:" << course::counter;

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
    saveData();
}
//admin panel func
void courseSystem::showCourseComboBox(QComboBox* comboBox) {
    comboBox->clear();
    for (const auto& pair : this->courses) {  // Use this instance's courses
        const course& c = pair.second;
        comboBox->addItem(QString::fromStdString(c.getTitle()),
            QVariant::fromValue(c.getCourseID()));
    }
}

vector<string> courseSystem::getCoursePrereqTitles(long courseId) {
    vector<std::string> result;
    const course* c = getCourse(courseId);
    if (!c) return result;  // Return empty if course not found

    // Iterate through each prerequisite course
    for (const course& prereq : c->getPrerequisites()) {
        result.push_back(prereq.getTitle());  // Get the title of the prerequisite course
    }

    return result;
}


void courseSystem::loadCoursePrereqsToListWidget(QComboBox* courseComboBox, QListWidget* prereqListWidget) {
    prereqListWidget->clear();

    QVariant selected = courseComboBox->currentData();
    if (!selected.isValid()) return;

    long courseId = selected.toLongLong();
    std::vector<std::string> prereqs = getCoursePrereqTitles(courseId);

    if (prereqs.empty()) {
        prereqListWidget->addItem("No prerequisites for this course");
        // You might want to make the item disabled so it can't be selected
        QListWidgetItem* item = prereqListWidget->item(0);
        item->setFlags(item->flags() & ~Qt::ItemIsSelectable & ~Qt::ItemIsEnabled);
        
    }
    else {
        for (const std::string& title : prereqs) {
            prereqListWidget->addItem(QString::fromStdString(title));
        }
    }
}
void courseSystem::addPrerequisiteToList(QComboBox* mainCourseComboBox, QComboBox* prereqCourseComboBox, QListWidget* prereqListWidget, QWidget* parent) {
    QString selectedPrereqTitle = prereqCourseComboBox->currentText();
    if (selectedPrereqTitle.isEmpty()) return;

    // Prevent adding the same course as its own prerequisite
    if (mainCourseComboBox->currentText() == selectedPrereqTitle) {
        QMessageBox::warning(parent, "Invalid Operation", "A course cannot be a prerequisite of itself.");
        return;
    }

    // Check for duplicates in the list
    for (int i = 0; i < prereqListWidget->count(); ++i) {
        if (prereqListWidget->item(i)->text() == selectedPrereqTitle) {
            QMessageBox::warning(parent, "Duplicate Prerequisite", "This prerequisite is already added.");
            return;
        }
    }

    // Add to list
    prereqListWidget->addItem(selectedPrereqTitle);

    // Show success message
    QMessageBox::information(parent, "Added", "Prerequisite added successfully.");
}

void courseSystem::removeSelectedPrerequisite(QComboBox* mainCourseComboBox, QListWidget* prereqListWidget, QWidget* parent) {
    QListWidgetItem* selectedItem = prereqListWidget->currentItem();
    if (!selectedItem) {
        QMessageBox::warning(parent, "No Selection", "Please select a prerequisite to remove.");
        return;
    }

    // Store title for optional feedback
    QString removedTitle = selectedItem->text();

    // Remove from list
    delete selectedItem;

    

    // Show success message
    QMessageBox::information(parent, "Removed", QString("Prerequisite '%1' removed successfully.").arg(removedTitle));
}



////////////////////////////////


bool courseSystem::addCourseToStudent(student* student, const course& courseToAdd) {
    if (!student || !getCourse(courseToAdd.getCourseID())) {
        return false; // Invalid student or course
    }

    try {
        // Check if student already has this course
        if (student->hasCourse(courseToAdd.getCourseID())) {
            return false;
        }

        // Add to student's course list with a default grade
        grade defaultGrade; // Creates a default grade (0.0 or ungraded)
        student->courses.push_back(std::make_pair(courseToAdd, defaultGrade));

        // Update the system data
        saveData();
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error adding course to student: " << e.what() << std::endl;
        return false;
    }
}
bool courseSystem::importGradesFromCSV(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(nullptr, "File Error",
            QString("Failed to open file:\n%1").arg(file.errorString()));
        return false;
    }

    QTextStream in(&file);
    int lineNumber = 0;
    int successCount = 0;
    int errorCount = 0;
    QStringList errorMessages;

    // Process header
    if (!in.atEnd()) {
        QString header = in.readLine().trimmed();
        if (header != "student_id,course_id,grade_letter") {
            QMessageBox::critical(nullptr, "Format Error",
                "Invalid CSV format.\n"
                "Expected header: student_id,course_id,grade_letter\n"
                "Found header: " + header);
            return false;
        }
    }

    while (!in.atEnd()) {
        lineNumber++;
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        QStringList tokens = line.split(',');
        if (tokens.size() != 3) {
            errorMessages << QString("Line %1: Invalid format. Expected 3 columns, found %2")
                .arg(lineNumber).arg(tokens.size());
            errorCount++;
            continue;
        }

        bool ok;
        long studentID = tokens[0].toLong(&ok);
        if (!ok) {
            errorMessages << QString("Line %1: Invalid student ID: %2")
                .arg(lineNumber).arg(tokens[0]);
            errorCount++;
            continue;
        }

        long courseID = tokens[1].toLong(&ok);
        if (!ok) {
            errorMessages << QString("Line %1: Invalid course ID: %2")
                .arg(lineNumber).arg(tokens[1]);
            errorCount++;
            continue;
        }

        QString gradeStr = tokens[2].trimmed().toUpper();
        if (gradeStr.length() != 1 ||
            (gradeStr[0] < 'A' || gradeStr[0] > 'F' ||
                (gradeStr[0] > 'D' && gradeStr[0] < 'F'))) {
            errorMessages << QString("Line %1: Invalid grade letter (must be A-F): %2")
                .arg(lineNumber).arg(tokens[2]);
            errorCount++;
            continue;
        }
        char gradeLetter = gradeStr[0].toLatin1();

        // Find student
        student* s = getStudent(studentID);
        if (!s) {
            errorMessages << QString("Line %1: Student not found with ID: %2")
                .arg(lineNumber).arg(studentID);
            errorCount++;
            continue;
        }

        // Find course
        course* c = getCourse(courseID);
        if (!c) {
            errorMessages << QString("Line %1: Course not found with ID: %2")
                .arg(lineNumber).arg(courseID);
            errorCount++;
            continue;
        }

        // Check enrollment
        if (!s->hasCourse(courseID)) {
            errorMessages << QString("Line %1: Student %2 not enrolled in course %3")
                .arg(lineNumber).arg(studentID).arg(courseID);
            errorCount++;
            continue;
        }

        // Update grade
        if (s->updateGrade(courseID, grade(1, gradeLetter, 2023))) {
            successCount++;
        }
        else {
            errorMessages << QString("Line %1: Failed to update grade for student %2 in course %3")
                .arg(lineNumber).arg(studentID).arg(courseID);
            errorCount++;
        }
    }

    file.close();

    // Show summary of errors if any occurred
    if (!errorMessages.isEmpty()) {
        QString errorReport = QString("%1 errors occurred:\n\n").arg(errorCount);
        errorReport += errorMessages.join("\n\n");

        // Show first 10 errors to avoid overwhelming the user
        if (errorMessages.size() > 10) {
            errorReport = QString("%1 errors occurred (showing first 10):\n\n").arg(errorCount);
            for (int i = 0; i < 10 && i < errorMessages.size(); i++) {
                errorReport += errorMessages[i] + "\n\n";
            }
            errorReport += "... and " + QString::number(errorMessages.size() - 10) + " more errors";
        }

        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("Import Warnings");
        msgBox.setText(QString("Completed with %1 errors").arg(errorCount));
        msgBox.setDetailedText(errorReport);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
    }

    if (successCount > 0) {
        saveData();
        QMessageBox::information(nullptr, "Import Complete",
            QString("Successfully updated %1 grades").arg(successCount));
        return true;
    }

    QMessageBox::critical(nullptr, "Import Failed",
        "No grades were updated. Please check the CSV file format.");
    return false;
}

void courseSystem::importGradesFromFile(QWidget* parent) {
    QString fileName = QFileDialog::getOpenFileName(
        parent,
        QObject::tr("Select Grades CSV File"),
        QDir::homePath(),
        QObject::tr("CSV Files (*.csv);;All Files (*)")
    );

    if (fileName.isEmpty()) {
        return; // User canceled
    }

    qDebug() << "Selected grades file:" << fileName;

    if (importGradesFromCSV(fileName)) {
        QMessageBox::information(parent, QObject::tr("Success"),
            QObject::tr("Grades imported successfully!"));
    }
    else {
        QMessageBox::warning(parent, QObject::tr("Error"),
            QObject::tr("Failed to import grades from file."));
    }
}


