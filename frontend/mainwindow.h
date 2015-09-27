#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "vnadevice.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    VnaDevice* device;
};

extern MainWindow* mainWindow;

#endif // MAINWINDOW_H
