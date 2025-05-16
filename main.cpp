#include "stdafx.h"
#include "Coursify.h"
#include <QtWidgets/QApplication>
#include "courseSystem.h"

using namespace std;

int main(int argc, char *argv[])
{
    courseSystem system;

    // Load data from files
    if (system.loadData()) {
        cout << "Data loaded successfully" << endl;
    }
    else {
        cout << "Error loading data" << endl;
    }

   
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon("Icon.png"));
    Coursify w;
    w.show();
    return a.exec();
}
