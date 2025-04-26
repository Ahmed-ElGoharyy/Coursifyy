#include "stdafx.h"
#include "Coursify.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Coursify w;
    w.show();
    return a.exec();
}
