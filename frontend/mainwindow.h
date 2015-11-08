#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>

#include "qcustomplot/qcustomplot.h"
#include "vnacommands.h"

class QAction;
class QToolBar;
class QLabel;
class QComboBox;

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

    void onRxTxIndexChanged(int index);

    void onStartSampling();
    void onStopSampling();

    void onReceiveHard();

    void onSave();

    void onSettings();
private:
    void createActions();
    void createToolbar();
    void createCustomPlot();

    void setStatusConnected(bool connected);
    void setBisy(bool bisy);

    void startNextSample();

    void saveData(const QString& fileName);

    //Сейчас девайс сэмплирует tx данные (иначе rx)
    bool isTx();
private:

    VnaDevice* device;
    VnaCommands* commands;

    QToolBar* mainToolBar;
    QComboBox* comboRxTx;


    QAction* connectAct;
    QAction* settingsAct;
    QAction* writeTestAct;
    QAction* refreshAct;
    QAction* startSamplingAct;
    QAction* stopSamplingAct;
    QAction* saveAct;

    QLabel* statusConnect;
    QLabel* statusBisy;

    QCustomPlot* customPlot;

    QPixmap pix_red_orb;
    QPixmap pix_green_orb;

    QVector<double> arrFreq;
    QVector<HardSamplingData> arrData;
    QVector<double> arrFreqM;
    QVector<double> arrAmplithudeI;
    QVector<double> arrAmplithudeQ;
    QVector<double> arrPhase;
    int freqIndex;

    bool _bStopSample;
};

extern MainWindow* mainWindow;

#endif // MAINWINDOW_H
