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
    void onRefresh();
    void onEndSampling();
private:
    void createActions();
    void createToolbar();
    void createCustomPlot();

    void setStatusConnected(bool connected);
    void setBisy(bool bisy);
private:

    VnaDevice* device;
    VnaCommands* commands;

    QToolBar* mainToolBar;

    QAction* connectAct;
    QAction* settingsAct;
    QAction* writeTestAct;
    QAction* refreshAct;

    QLabel* statusConnect;
    QLabel* statusBisy;

    QCustomPlot* customPlot;

    QPixmap pix_red_orb;
    QPixmap pix_green_orb;
};

extern MainWindow* mainWindow;

#endif // MAINWINDOW_H
