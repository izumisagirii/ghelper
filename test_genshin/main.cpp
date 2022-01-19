#include "mainwindow.h"
#include "ghthread.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow gh;
    gh.show();
    return a.exec();
}
