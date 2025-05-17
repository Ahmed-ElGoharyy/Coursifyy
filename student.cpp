#include "stdafx.h"
#include "student.h"
#include "course.h"
#include "user.h" 
#include <QPdfWriter>
#include <QPainter>
#include <QTextDocument>
#include <QMessageBox>
#include <QPageLayout>
#include <QPageSize>
using namespace std;


// Initialize static counter
long student::counter = 0;
student::student() : user(), StudentID(counter++), gpa(0.0), max_credit_hours(21), currentSemester("Fall 2023") {
    setRole('S');
    cout << "Created new student with ID: " << StudentID << endl;
}

student::student(string username, string password, string name, string email, string semester)
    : user(username, password, name, email, 'S'), gpa(0.0), max_credit_hours(21), currentSemester(semester) {
    StudentID = counter++;
    cout << "Created new student with ID: " << StudentID << endl;
}


student::student(string username, string password, string name, string email)
    : user(username, password, name, email, 'S'), gpa(0.0), max_credit_hours(21) {
    StudentID = counter++;
    cout << "Created new student with ID: " << StudentID << endl;
}

student::student(string username, string password, string name)
    : user(username, password, name, "null", 'S'), gpa(0.0), max_credit_hours(21) {
    StudentID = counter++;
    cout << "Created new student with ID: " << StudentID << endl;
}

pair<course, grade> student::getonegrade(string courseName) const noexcept(false) {
    for (const auto& pair : courses) {
        if (pair.first.getTitle() == courseName) {
            return pair;
        }
    }
    throw student_exception("Course not found: " + courseName);
}

bool student::registerCourse(course newCourse) noexcept(false) {
    // Check if course already exists
    for (const auto& pair : courses) {
        if (pair.first.getCourseID() == newCourse.getCourseID()) {
            throw student_exception("Course already registered: " + newCourse.getTitle());
        }
    }

    int courseHours = newCourse.getCreditHours();
    if (courseHours <= 0) {
        throw student_exception("Invalid credit hours: Course has " +
            to_string(courseHours) + " credit hours");
    }

    // Check remaining credit hours
    if (courseHours > max_credit_hours) {
        throw student_exception("Cannot register for course: Exceeds maximum credit hours (" +
            to_string(max_credit_hours) + " remaining)");
    }

    // Add course with default grade
    grade newGrade;
    courses.push_back(make_pair(newCourse, newGrade));

    // Update remaining credit hours
    max_credit_hours -= courseHours;

    return true;
}

list<pair<course, grade>> student::getGrades() const noexcept(false) {
    return courses;
}

float student::calculateGPA() noexcept(false) {
    if (courses.empty()) return 0.0f;

    float totalWeightedPoints = 0.0f;
    int totalCreditHours = 0;

    for (const auto& coursePair : courses) {
        if (coursePair.second.getGrade() != 'N') {  // Or whatever represents "no grade"
            totalWeightedPoints += coursePair.second.getGPA() * coursePair.first.getCreditHours();
            totalCreditHours += coursePair.first.getCreditHours();
        }
    }

    return totalCreditHours > 0 ? totalWeightedPoints / totalCreditHours : 0.0f;
}


bool student::hasCourse(long courseID) const {
    for (const auto& coursePair : courses) {
        if (coursePair.first.getCourseID() == courseID) {
            return true;
        }
    }
    return false;
}

bool student::hasCompletedCourse(long courseID) const {
    for (const auto& pair : courses) {
        if (pair.first.getCourseID() == courseID && pair.second.getGrade() != 'N' && pair.second.getGrade() != 'F') {
            return true;
        }
    }
    return false;
}

int student::modifycredithours(course c)
{
    max_credit_hours -= c.getCreditHours();
    return this->max_credit_hours;
}



bool student::generateReport() {
    try {
        // Create PDF filename
        QString filename = QString("student_%1_report.pdf").arg(StudentID);

        // Set up PDF writer with A4 size
        QPdfWriter pdfWriter(filename);

        // Set A4 portrait size (210mm x 297mm)
        pdfWriter.setPageSize(QPageSize(QPageSize::A4));

        // Use smaller margins to maximize content area (8mm)
        const qreal margin = 8;
        pdfWriter.setPageMargins(QMarginsF(margin, margin, margin, margin), QPageLayout::Millimeter);

        // Set high resolution
        pdfWriter.setResolution(300);

        // Initialize painter
        QPainter painter(&pdfWriter);

        // Get the actual paint rectangle in device coordinates
        QRectF pageRect = pdfWriter.pageLayout().paintRectPixels(pdfWriter.resolution());

        // Create document and set its size to match the paint area exactly
        QTextDocument doc;
        doc.setPageSize(pageRect.size());

        // Calculate available height to ensure we fill the page
        qreal availableHeight = pageRect.height();

        // Prepare HTML content
        QString htmlContent;
        htmlContent += "<html><body style='font-family: Arial; margin: 0; padding: 0;'>";

        // Header section - calculate about 8% of available height
        qreal headerHeight = availableHeight * 0.08;
        htmlContent += QString("<div style='width: 100%%; height: %1px; margin-bottom: 12px;'>").arg(headerHeight);
        htmlContent += "<h1 style='font-size: 22px; text-align: center; margin-bottom: 5px;'>STUDENT ACADEMIC REPORT</h1>";
        htmlContent += "<hr style='border: 1px solid #333; margin-bottom: 5px;'>";
        htmlContent += "</div>";

        // Student info section - about 12% of available height
        qreal infoHeight = availableHeight * 0.12;
        htmlContent += QString("<div style='margin-bottom: 10px; min-height: %1px;'>").arg(infoHeight);
        htmlContent += "<h2 style='font-size: 16px; color: #333; border-bottom: 1px solid #ddd; padding-bottom: 3px; margin-bottom: 6px;'>STUDENT INFORMATION</h2>";
        htmlContent += QString("<p style='font-size: 12px; margin: 3px 0;'><b>ID:</b> %1</p>").arg(StudentID);
        htmlContent += QString("<p style='font-size: 12px; margin: 3px 0;'><b>Name:</b> %1</p>").arg(QString::fromStdString(getName()));
        htmlContent += QString("<p style='font-size: 12px; margin: 3px 0;'><b>Email:</b> %1</p>").arg(QString::fromStdString(getEmail()));
        htmlContent += "</div>";

        // Academic summary section - about 12% of available height
        qreal summaryHeight = availableHeight * 0.12;
        htmlContent += QString("<div style='margin-bottom: 10px; min-height: %1px;'>").arg(summaryHeight);
        this->calculateGPA();
        htmlContent += "<h2 style='font-size: 16px; color: #333; border-bottom: 1px solid #ddd; padding-bottom: 3px; margin-bottom: 6px;'>ACADEMIC SUMMARY</h2>";
        htmlContent += QString("<p style='font-size: 12px; margin: 3px 0;'><b>GPA:</b> %1</p>").arg(QString::number(this->getGPA(), 'f', 2));
        htmlContent += QString("<p style='font-size: 12px; margin: 3px 0;'><b>Remaining Credit Hours:</b> %1</p>").arg(this->max_credit_hours);
        htmlContent += "</div>";

        // Calculate table height to fill remaining space (about 63% of available height)
        qreal courseTableHeight = availableHeight * 0.63;

        // Courses table - full width with height set to fill remaining space
        htmlContent += QString("<div style='min-height: %1px;'>").arg(courseTableHeight);
        htmlContent += "<h2 style='font-size: 16px; color: #333; margin: 5px 0;'>COURSE RECORDS</h2>";
        htmlContent += "<table border='1' cellpadding='3' cellspacing='0' width='100%' style='"
            "font-size: 11px; "
            "border-collapse: collapse; "
            "margin-bottom: 8px;'>";

        // Table header
        htmlContent += "<thead>";
        htmlContent += "<tr style='background-color: #f2f2f2; font-weight: bold;'>";
        htmlContent += "<th align='left' style='padding: 5px;'>Course Title</th>";
        htmlContent += "<th align='center' style='padding: 5px; width: 50px;'>ID</th>";
        htmlContent += "<th align='center' style='padding: 5px; width: 50px;'>Credits</th>";
        htmlContent += "<th align='center' style='padding: 5px; width: 50px;'>Grade</th>";
        htmlContent += "<th align='center' style='padding: 5px; width: 50px;'>Points</th>";
        htmlContent += "</tr>";
        htmlContent += "</thead>";

        // Table body
        htmlContent += "<tbody>";

        // Get number of courses to calculate row height
        int numCourses = courses.size();
        // Make sure we show at least 10 rows even if there are fewer courses
        int minRows = 10;
        int totalRows = std::max(numCourses, minRows);

        // Add actual course rows
        for (const auto& pair : courses) {
            const course& c = pair.first;
            const grade& g = pair.second;

            htmlContent += "<tr>";
            htmlContent += QString("<td style='padding: 5px;'>%1</td>").arg(QString::fromStdString(c.getTitle()));
            htmlContent += QString("<td style='padding: 5px; text-align: center;'>%1</td>").arg(c.getCourseID());
            htmlContent += QString("<td style='padding: 5px; text-align: center;'>%1</td>").arg(c.getCreditHours());
            htmlContent += QString("<td style='padding: 5px; text-align: center;'>%1</td>").arg(g.getGrade());
            htmlContent += QString("<td style='padding: 5px; text-align: center;'>%1</td>").arg(g.getGPA());
            htmlContent += "</tr>";
        }

        // Add empty rows if needed to fill the page
        if (numCourses < minRows) {
            for (int i = numCourses; i < minRows; i++) {
                htmlContent += "<tr style='height: 25px;'>";
                htmlContent += "<td style='padding: 5px;'>&nbsp;</td>";
                htmlContent += "<td style='padding: 5px;'>&nbsp;</td>";
                htmlContent += "<td style='padding: 5px;'>&nbsp;</td>";
                htmlContent += "<td style='padding: 5px;'>&nbsp;</td>";
                htmlContent += "<td style='padding: 5px;'>&nbsp;</td>";
                htmlContent += "</tr>";
            }
        }

        htmlContent += "</tbody></table>";
        htmlContent += "</div>";

        // Footer with generation info - about 5% of available height
        qreal footerHeight = availableHeight * 0.05;
        htmlContent += QString("<div style='text-align: center; font-size: 10px; color: #666; height: %1px;'>").arg(footerHeight);
        htmlContent += "<hr style='border: 1px solid #ddd; margin: 8px 0;'>";
        htmlContent += QString("<p>Generated on %1 by Coursify Academic System</p>")
            .arg(QDateTime::currentDateTime().toString("MMMM d, yyyy h:mm AP"));
        htmlContent += "</div>";

        htmlContent += "</body></html>";

        // Set HTML content
        doc.setHtml(htmlContent);

        // Draw the content
        doc.drawContents(&painter);

        // Show success message with file path
        QMessageBox::information(nullptr, "Report Generated",
            QString("Full A4 PDF report successfully created:\n\n%1")
            .arg(QDir::toNativeSeparators(QFileInfo(filename).absoluteFilePath())));

        return true;
    }
    catch (const std::exception& e) {
        QMessageBox::critical(nullptr, "Error",
            QString("Failed to generate PDF report:\n%1").arg(e.what()));
        return false;
    }
}
bool student::addCourseToPlan(const course& courseToAdd) {
    try {
        // Check if course already exists in student's list
        for (const auto& [existingCourse, grade] : courses) {
            if (existingCourse.getCourseID() == courseToAdd.getCourseID()) {
                return false; // Course already exists
            }
        }

        if (max_credit_hours < courseToAdd.getCreditHours()) {
            return false;
        }

        // Create a default grade
        grade defaultGrade;

        // Add the course to the student's list
        courses.push_back(make_pair(courseToAdd, defaultGrade));
        // Update remaining credit hours
        this->modifycredithours(courseToAdd);

        return true;
    }
    catch (const exception& e) {
        cerr << "Error adding course to student: " << e.what() << endl;
        return false;
    }
}
bool student::updateGrade(long courseID, const grade& newGrade) {
    for (auto& coursePair : courses) {
        if (coursePair.first.getCourseID() == courseID) {
            coursePair.second = newGrade;
            coursePair.first.setEnrolled(true);
            this->gpa = calculateGPA(); // Recalculate GPA
            return true;
        }
    }
    return false;
}

