#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Coursify.h"
#include "courseSystem.h"
#include "Utilities.h"

using namespace std;


class Coursify : public QMainWindow
{
    Q_OBJECT

public:
    Coursify(QWidget* parent = nullptr);
    ~Coursify();


protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private slots:
    void onSearchTextChanged(const QString& text);
    void onSearchResultSelected(QListWidgetItem* item);
    QString highlightMatchingChars(const QString& text, const QString& searchTerm);

    void clearAllListsAndFields();

private:
    Ui::CoursifyClass ui;

private:
    courseSystem* m_system; // Your course system instance

private:
    void resetUIForLogin();
    void refreshStudentUI();
    void refreshAdminUI();

};
