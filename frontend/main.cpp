#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    setlocale(LC_NUMERIC,"en_US.UTF-8");

    return a.exec();
}
