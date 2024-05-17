#include "mainwindow.h"
#include <pch.h>

int main(int argc, char *argv[])
{
    plog::init(plog::info, "LogFile.log");
    PLOGI << "\nProgram started";

    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    return a.exec();
}
