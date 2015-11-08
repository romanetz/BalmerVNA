#include "mainwindow.h"
#include "project.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("Balmer");
    QCoreApplication::setApplicationName("VNA");

    Project project;
    project.loadSettings();

    MainWindow w;
    w.show();

    setlocale(LC_NUMERIC,"en_US.UTF-8");

    return a.exec();
}
