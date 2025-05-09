#include "stdafx.h"
#include "Coursify.h"
#include <QtWidgets/QApplication>
#include "courseSystem.h"
int main(int argc, char *argv[])
{
    courseSystem system;

    // Load data from files
    if (system.loadData()) {
        std::cout << "Data loaded successfully" << std::endl;
    }
    else {
        std::cout << "Error loading data" << std::endl;
    }
    QApplication a(argc, argv);
    Coursify w;
    w.show();
    return a.exec();
}
