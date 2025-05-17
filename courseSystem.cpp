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
#include <QStringList>
#include <QString>
using namespace std;






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

        // Check for empty fields
        if (uname.isEmpty() || pword.isEmpty() || nam.isEmpty() || cpassword.isEmpty()) {
            QMessageBox::warning(nullptr, " Error  ", " \n Please fill the required info \n");
            return false;
        }

        // Check if name contains numbers or invalid characters
        QRegularExpression nameRegex("^[\\p{L} .'-]+$", QRegularExpression::UseUnicodePropertiesOption);
        if (!nameRegex.match(nam).hasMatch()) {
            QMessageBox::warning(nullptr, "Invalid Name", " \n Name should only contain letters, spaces, hyphens, apostrophes, or dots.\n");
            return false;
        }

        // Enhanced username validation
        QRegularExpression usernameRegex("^(?=.*[a-zA-Z])[a-zA-Z0-9]+$");
        if (!usernameRegex.match(uname).hasMatch()) {
            QMessageBox::warning(nullptr, "Invalid Username",
                " \n Username must contain at least one letter \n"
                " and can only contain letters and numbers.\n"
                " It cannot be numbers only.\n");
            return false;
        }

        // Check password match
        if (password != confirmpassword) {
            QMessageBox::warning(nullptr, "Password don't match ", " \n Confirm your password again. \n");
            return false;
        }

        // Check if username exists in either students or admins
        if (students.find(username) != students.end() || admins.find(username) != admins.end()) {
            QMessageBox::warning(nullptr, "Username already exists",
                "\n Username already taken by another user.\n Please choose another username.\n");
            return false;
        }

        // Validate password format
        if (user::isValidPassword(password)) {
            string hashedpass = user::hashPassword(password);

            // Default to current semester (you might want to make this configurable)
            student newStudent(username, hashedpass, name, "Fall 2023");
            students[username] = newStudent;
            saveData();

            return true;
        }
        else {
            QMessageBox::warning(nullptr, "Bad Password format",
                " \n Password should contain: \n"
                " - At least one uppercase letter \n"
                " - At least one lowercase letter \n"
                " - At least one digit \n"
                " - Minimum 6 characters \n");
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

        // Check for empty fields
        if (uname.isEmpty() || pword.isEmpty() || nam.isEmpty() || cpassword.isEmpty() || keyy.isEmpty()) {
            QMessageBox::warning(nullptr, " Error  ", " \n Please fill all the required fields \n");
            return false;
        }

        // Validate name format
        QRegularExpression nameRegex("^[\\p{L} .'-]+$", QRegularExpression::UseUnicodePropertiesOption);
        if (!nameRegex.match(nam).hasMatch()) {
            QMessageBox::warning(nullptr, "Invalid Name", " \n Name should only contain letters, spaces, hyphens, apostrophes, or dots.\n");
            return false;
        }

        // Enhanced username validation
        QRegularExpression usernameRegex("^(?=.*[a-zA-Z])[a-zA-Z0-9]+$");
        if (!usernameRegex.match(uname).hasMatch()) {
            QMessageBox::warning(nullptr, "Invalid Username",
                " \n Username must contain at least one letter \n"
                " and can only contain letters and numbers.\n"
                " It cannot be numbers only.\n");
            return false;
        }

        // Check password match
        if (password != confirmpassword) {
            QMessageBox::warning(nullptr, "Passwords don't match", " \n Confirm your password again. \n");
            return false;
        }

        // Check if username exists in either admins or students
        if (admins.find(username) != admins.end() || students.find(username) != students.end()) {
            QMessageBox::warning(nullptr, "Username already exists",
                "\n Username already taken by another user.\n Please choose another username.\n");
            return false;
        }

        // Validate admin key (consider moving this to a configuration file)
        if (key != "1234") {
            QMessageBox::warning(nullptr, "Wrong Administrator Key",
                " \n You can't register as admin without the correct key \n"
                " \n PS: The key is '1234' for demonstration purposes \n");
            return false;
        }

        // Validate password format
        if (user::isValidPassword(password)) {
            string hashedpass = user::hashPassword(password);

            admin newadmin(username, hashedpass, name);
            admins[username] = newadmin;
            saveData();

            return true;
        }
        else {
            QMessageBox::warning(nullptr, "Invalid Password Format",
                " \n Password should contain: \n"
                " - At least one uppercase letter \n"
                " - At least one lowercase letter \n"
                " - At least one digit \n"
                " - Minimum 6 characters \n");
            return false;
        }
    }
    catch (exception& e) {
        cout << "Error registering admin: " << e.what() << endl;
        QMessageBox::critical(nullptr, "Registration Error",
            " \n An unexpected error occurred during registration. \n"
            " Please try again or contact support. \n");
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
    cout << "Added course with ID: " << courseID << ", counter value: " << course::counter << endl;

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

vector<course> courseSystem::searchCourses(const string& searchTerm) const {
    vector<course> results;
    if (searchTerm.empty()) {
        return results;
    }

    string lowerSearchTerm = searchTerm;
    transform(lowerSearchTerm.begin(), lowerSearchTerm.end(), lowerSearchTerm.begin(),
        [](unsigned char c) { return tolower(c); });

    for (const auto& pair : courses) {
        const course& c = pair.second;
        string lowerTitle = c.getTitle();
        transform(lowerTitle.begin(), lowerTitle.end(), lowerTitle.begin(),
            [](unsigned char c) { return tolower(c); });

        if (lowerTitle.find(lowerSearchTerm) != string::npos) {
            results.push_back(c);
        }
    }

    return results;
}
bool courseSystem::addStudent(const student& newStudent) {
    string username = newStudent.getUsername();
    if (students.find(username) != students.end()) {
        return false;
    }

    students[username] = newStudent;
    saveData();
    return true;
}

bool courseSystem::updateStudent(const string& username, const student& updatedStudent) {
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

student* courseSystem::getStudentByUsername(const string& username) {
    auto it = students.find(username);
    if (it != students.end()) {
        return &it->second;
    }
    return nullptr;
}

bool courseSystem::addAdmin(const admin& newAdmin) {
    string username = newAdmin.getUsername();
    if (admins.find(username) != admins.end()) {
        return false;
    }

    admins[username] = newAdmin;
    saveData();
    return true;
}

admin* courseSystem::getAdminByUsername(const string& username) {
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

bool courseSystem::updateStudentGrade(const string& username, long courseID, const grade& newGrade) {
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

bool courseSystem::enrollStudentInCourse(const string& username, long courseID) {
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
    cout << "=== DATA LOADING RESULTS ===" << endl;
    cout << "Loading result: " << (result ? "SUCCESS" : "FAILURE") << endl;
    cout << "Students loaded: " << students.size() << endl;
    cout << "Courses loaded: " << courses.size() << endl;
    cout << "Admins loaded: " << admins.size() << endl;

    // Print course details if any were loaded
    if (!courses.empty()) {
        cout << "LOADED COURSES:" << endl;
        for (const auto& pair : courses) {
            cout << "  Course ID: " << pair.first
                << ", Title: " << pair.second.getTitle()
                << ", Instructor: " << pair.second.getInstructor() << endl;
        }
    }
    else {
        cout << "No courses were loaded from file." << endl;
        // Check if the courses file exists
        if (filesystem::exists(FileUtils::coursesFilePath)) {
            cout << "Courses file exists at: " << FileUtils::coursesFilePath << endl;
            cout << "File size: " << filesystem::file_size(FileUtils::coursesFilePath) << " bytes" << endl;
        }
        else {
            cout << "Courses file does not exist at: " << FileUtils::coursesFilePath << endl;
        }
    }

    return result;
}



bool courseSystem::saveData() {
    // Print current working directory
    cout << "Current working directory: "
        << filesystem::current_path().string() << endl;

    // Print the expected data directory path
    string dataDir = "./data/";
    filesystem::path absDataPath = filesystem::absolute(dataDir);
    cout << "Absolute data directory path: " << absDataPath.string() << endl;

    // Print expected file paths
    cout << "Expected courses file: " << absDataPath.string() + "/courses.json" << endl;

    // Make sure data directory exists
    if (!filesystem::exists(dataDir)) {
        cout << "Creating data directory: " << dataDir << endl;
        filesystem::create_directory(dataDir);
    }

    // Print course count before saving
    cout << "Number of courses to save: " << courses.size() << endl;
    for (const auto& pair : courses) {
        cout << "Course ID: " << pair.first
            << ", Title: " << pair.second.getTitle() << endl;
    }

    // Explicitly check if courses.json exists and try to remove it if it does
    string coursesFilePath = (absDataPath / "courses.json").string();
    if (filesystem::exists(coursesFilePath)) {
        cout << "Found existing courses file. Size: "
            << filesystem::file_size(coursesFilePath) << " bytes. Removing..." << endl;
        try {
            filesystem::remove(coursesFilePath);
            cout << "Successfully removed old courses file." << endl;
        }
        catch (const exception& e) {
            cerr << "Error removing old courses file: " << e.what() << endl;
        }
    }

    // Call the original save function
    bool result = FileUtils::saveAllData(*this);

    // Verify file existence after save
    if (filesystem::exists(coursesFilePath)) {
        cout << "Courses file exists after save at: " << coursesFilePath << endl;
        cout << "File size: " << filesystem::file_size(coursesFilePath) << " bytes" << endl;

        // Try to read file contents to verify it's a valid JSON
        try {
            ifstream file(coursesFilePath);
            if (file.is_open()) {
                string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
                file.close();
                if (content.empty()) {
                    cout << "WARNING: Courses file is empty!" << endl;
                }
                else {
                    cout << "Courses file contains data (first 100 chars): "
                        << content.substr(0, min(content.size(), static_cast<size_t>(100)))
                        << (content.size() > 100 ? "..." : "") << endl;
                }
            }
        }
        catch (const exception& e) {
            cerr << "Error reading back courses file: " << e.what() << endl;
        }
    }
    else {
        cout << "WARNING: Courses file does not exist after save!" << endl;
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
        maxExistingId = max(maxExistingId, pair.first);
    }

    // Reset counter to max ID + 1
    course::resetCounter(maxExistingId + 1);
    qDebug() << "Course counter reset to:" << course::counter;

    // Structure to store course ID and its prerequisite IDs
    struct CoursePrereqData {
        long courseId;
        vector<long> prerequisiteIds;
    };
    vector<CoursePrereqData> coursePrereqs;

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
            vector<long> prereqIds;
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
        catch (const exception& e) {
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
    vector<string> result;
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
    vector<string> prereqs = getCoursePrereqTitles(courseId);

    if (prereqs.empty()) {
        prereqListWidget->addItem("No prerequisites for this course");
        // You might want to make the item disabled so it can't be selected
        QListWidgetItem* item = prereqListWidget->item(0);
        item->setFlags(item->flags() & ~Qt::ItemIsSelectable & ~Qt::ItemIsEnabled);
        
    }
    else {
        for (const string& title : prereqs) {
            prereqListWidget->addItem(QString::fromStdString(title));
        }
    }
}
void courseSystem::addPrerequisiteToList(QComboBox* mainCourseComboBox, QComboBox* prereqCourseComboBox, QListWidget* prereqListWidget, QWidget* parent) {
    QString selectedPrereqTitle = prereqCourseComboBox->currentText();
    if (selectedPrereqTitle.isEmpty()) return;
   //bgeb atnan id mn comboBox
    long mainCourseId = mainCourseComboBox->currentData().toLongLong();
    long prereqCourseId = prereqCourseComboBox->currentData().toLongLong();

    if (mainCourseId == prereqCourseId) {
        QMessageBox::warning(parent, "Invalid Operation", "A course cannot be a prerequisite of itself.");
        return;
    }

    course* mainCourse = getCourse(mainCourseId);
    course* prereqCourse = getCourse(prereqCourseId);

    if (!mainCourse || !prereqCourse) return;
  //foreach btlf 3la kol prereq bta3t main 
    for (const course& c : mainCourse->getPrerequisites()) {
        if (c.getCourseID() == prereqCourseId) {
            QMessageBox::warning(parent, "Duplicate", "This prerequisite is already assigned.");
            return;
        }
    }
    // 3l4ane my7sl4 loop btlf 3la prereq bta3t elsecound zy ma moo 3ayz 
    for (const course& sub : prereqCourse->getPrerequisites()) {
        if (sub.getCourseID() == mainCourse->getCourseID()) {
            QMessageBox::warning(parent, "Redundant Prerequisite",
                "This course already depends on the same prerequisite through another path.");
            return;
        }
    }
   
    if (prereqListWidget->count() == 1 &&
        prereqListWidget->item(0)->text() == "No prerequisites for this course") {
        delete prereqListWidget->takeItem(0);
    }

    // b7ot prereq fy vector 
    mainCourse->addPrerequisite(*prereqCourse);
    prereqListWidget->addItem(selectedPrereqTitle);

    QMessageBox::information(parent, "Added", "Prerequisite added and saved to the course.");
}

void courseSystem::removeSelectedPrerequisite(QComboBox* mainCourseComboBox, QListWidget* prereqListWidget, QWidget* parent) {
    QListWidgetItem* selectedItem = prereqListWidget->currentItem();
    if (!selectedItem) {
        QMessageBox::warning(parent, "No Selection", "Please select a prerequisite to remove.");
        return;
    }

    QString removedTitle = selectedItem->text();
    long mainCourseId = mainCourseComboBox->currentData().toLongLong();
    course* mainCourse = getCourse(mainCourseId);

    if (!mainCourse) return;

   
    long prereqIdToRemove = -1;
    //foreach 3l4ane ageb elid bta3 elprereq ely a5trto
    for (const course& c : mainCourse->getPrerequisites()) {
        if (c.getTitle() == removedTitle.toStdString()) {
            prereqIdToRemove = c.getCourseID();
            break;
        }
    }

    if (prereqIdToRemove != -1) {
        mainCourse->removePrerequisite(prereqIdToRemove);
    }

    delete selectedItem;

    QMessageBox::information(parent, "Removed", QString("Prerequisite '%1' removed from course.").arg(removedTitle));
}


void courseSystem::assignGradeToStudent(QLineEdit* usernameEdit, QLineEdit* courseIdEdit, QComboBox* gradeComboBox, QWidget* parent) {
    string username = usernameEdit->text().toStdString();

    string courseString = courseIdEdit->text().toStdString();
    long courseId = courseIdEdit->text().toLongLong();
    QString selectedGrade = gradeComboBox->currentText();

    if ( username.empty() || courseString.empty()) {
        QMessageBox::warning(parent, "Error", "Enter all fields.");
        return;
    }

    // b4of elstudent mwgod fy hashmap wla l2
    auto it = students.find(username);
    if (it == students.end()) {
        QMessageBox::warning(parent, "Error", "Student not found.");
        return;
    }

    student& stu = it->second;

    
    course* foundCourse = nullptr;
    //blf 3la list course ely student msglha 3l4ane 
    for (auto& pair : stu.courses) {
        if (pair.first.getCourseID() == courseId) {
            foundCourse = &pair.first;
            break;
        }
    }

    if (!foundCourse) {
        QMessageBox::warning(parent, "Error", "Course not registered for this student.\n \n or not a valid course ID");
        return;
    }

    
    grade g;
    if (selectedGrade == "A") g.setGrade('A');
    else if (selectedGrade == "B") g.setGrade('B');
    else if (selectedGrade == "C") g.setGrade('C');
    else if (selectedGrade == "D") g.setGrade('D');
    else if (selectedGrade == "F") g.setGrade('F');
    else {
        QMessageBox::warning(parent, "Error", "Invalid grade selected.");
        return;
    }

    
    bool updated = stu.updateGrade(courseId, g);
    if (updated) {
        QMessageBox::information(parent, "Success", "Grade updated successfully.");
    }
    else {
        QMessageBox::warning(parent, "Failure", "Failed to update grade.");
    }
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

        // Check credit hours
        int courseHours = courseToAdd.getCreditHours();
        if (student->max_credit_hours < courseHours) {
            return false; // Not enough remaining credit hours
        }
		// Check if course has prerequisites
		if (!courseToAdd.checkPrerequisites(*student)) {
			return false; // Prerequisites not met
		}

        // Add to student's course list with a default grade
        grade defaultGrade; // Creates a default grade (0.0 or ungraded)
        student->courses.push_back(make_pair(courseToAdd, defaultGrade));

        // Update remaining credit hours
        student->max_credit_hours -= courseHours;

        // Update the system data
        saveData();
        return true;
    }
    catch (const exception& e) {
        cerr << "Error adding course to student: " << e.what() << endl;
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
        if (!ok || tokens[1].isEmpty()) {
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

        // Set enrolled to true for this course before updating grade
        

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

void courseSystem::showStudentCourseGrade(QComboBox* courseComboBox, QListWidget* gradesListWidget, QWidget* parent) {
    // Ensure currentUser is a student
    student* stu = dynamic_cast<student*>(currentUser);
    if (!stu) {
        QMessageBox::warning(parent, "Error", "No student is currently logged in.");
        return;
    }

    // Get selected course title
    QString selectedCourseTitle = courseComboBox->currentText();
    if (selectedCourseTitle.isEmpty()) {
        QMessageBox::warning(parent, "No Course Selected", "Please select a course.");
        return;
    }

    // Find course ID by title
    long courseId = -1;
    for (const auto& [id, c] : courses) {
        if (QString::fromStdString(c.getTitle()) == selectedCourseTitle) {
            courseId = id;
            break;
        }
    }

    if (courseId == -1) {
        QMessageBox::warning(parent, "Course Not Found", "Selected course not found.");
        return;
    }

    // Get grade
    const auto& gradesMap = stu->getGrades();  // returns map<long, grade>
   
}





// report 

QStringList  courseSystem:: getStudentCourseReport(student* currentStudent) {
    QStringList report;

    if (!currentStudent) return report;

    const auto& courses = currentStudent->getGrades();

    report << QString("%1 | %2 | %3 | %4 | %5")
        .arg("Course Title", -25)
        .arg("Course ID", -12)
        .arg("Credit Hours", -12)
        .arg("Grade", -8)
        .arg("Points", -8);
    report << QString(70, '-');

    for (const auto& pair : courses) {
        const course& c = pair.first;
        const grade& g = pair.second;

        QString line = QString("%1   | %2 | %3 | %4 | %5")
            .arg(QString::fromStdString(c.getTitle()), -40)
            .arg(QString::number(c.getCourseID()), -6)
            .arg(QString::number(c.getCreditHours()), -7)
            .arg(QString(g.getGrade()), -5)
            .arg(QString::number(g.getGPA()), -6);
        report << line;
        report << QString(70,' ');
    }

    return report;
}

//view grades 
QStringList  courseSystem::getStudentGrades(student* currentStudent) {
    QStringList report;

    if (!currentStudent) return report;

    const auto& courses = currentStudent->getGrades();

    report << QString("%1 | %2 | %3 ")
        .arg("Course Title", -25)
        .arg("Course ID", -12)
        .arg("Grade", -8);
    report << QString(70, '-');

    for (const auto& pair : courses) {
        const course& c = pair.first;
        const grade& g = pair.second;

        QString line = QString("%1   | %2 | %3 ")
            .arg(QString::fromStdString(c.getTitle()), -40)
            .arg(QString::number(c.getCourseID()), -6)
            .arg(QString(g.getGrade()), -5);
        report << line;
        report << QString(70, ' ');
    }

    return report;
}
