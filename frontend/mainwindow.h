#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QAction;
class QToolBar;

class VnaDevice;
class VnaCommands;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void openSerialPort();
    void writeTestData();
private:
    void createActions();
    void createToolbar();
private:

    VnaDevice* device;
    VnaCommands* commands;

    QToolBar* mainToolBar;

    QAction* connectAct;
    QAction* settingsAct;
    QAction* writeTestAct;
};

extern MainWindow* mainWindow;

#endif // MAINWINDOW_H
