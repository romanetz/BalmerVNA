#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>

#include "qcustomplot/qcustomplot.h"

class QAction;
class QToolBar;
class QLabel;

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

    void onCloseSerial();

    void onNoneComplete();
    void onRefresh();
private:
    void createActions();
    void createToolbar();
    void createCustomPlot();

    void setStatusConnected(bool connected);

private:

    VnaDevice* device;
    VnaCommands* commands;

    QToolBar* mainToolBar;

    QAction* connectAct;
    QAction* settingsAct;
    QAction* writeTestAct;
    QAction* refreshAct;

    QLabel* statusConnect;

    QCustomPlot* customPlot;
};

extern MainWindow* mainWindow;

#endif // MAINWINDOW_H
