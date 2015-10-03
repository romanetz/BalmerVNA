#include "mainwindow.h"
#include <QAction>
#include <QToolBar>
#include <QLabel>
#include <QStatusBar>

#include "vnadevice.h"
#include "vnacommands.h"


MainWindow* mainWindow = nullptr;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    mainWindow = this;

    setGeometry(400, 250, 542, 390);

    device = new VnaDevice();
    connect(device, SIGNAL(signalClose()), this, SLOT(onCloseSerial()));

    commands = new VnaCommands(device);
    connect(commands, SIGNAL(signalNoneComplete()), this, SLOT(onNoneComplete()));


    createCustomPlot();
    createActions();
    createToolbar();

    statusConnect = new QLabel();
    statusBar()->addPermanentWidget(statusConnect);

    this->setCentralWidget(customPlot);

    setStatusConnected(false);
}

MainWindow::~MainWindow()
{
    delete commands;
    delete device;
    mainWindow = nullptr;
}

void MainWindow::createActions()
{
    connectAct = new QAction(QIcon(":/icons/connect.png"), tr("Connect"), this );
    connect(connectAct, SIGNAL(triggered()), this, SLOT(openSerialPort()));

    settingsAct = new QAction(QIcon(":/icons/settings.png"), tr("Settings"), this );

    writeTestAct = new QAction(QIcon(":/icons/file_send.png"), tr("Write test data"), this );
    connect(writeTestAct, SIGNAL(triggered()), this, SLOT(writeTestData()));
}

void MainWindow::createToolbar()
{
    mainToolBar = addToolBar("main");
    mainToolBar->addAction(connectAct);
    mainToolBar->addAction(settingsAct);
    mainToolBar->addAction(writeTestAct);
}

void MainWindow::createCustomPlot()
{
    customPlot = new QCustomPlot(this);
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    QVector<double> x(101), y(101); // initialize with entries 0..100
    for (int i=0; i<101; ++i)
    {
      x[i] = i/50.0 - 1; // x goes from -1 to 1
      y[i] = x[i]*x[i];  // let's plot a quadratic function
    }
    // create graph and assign data to it:
    customPlot->addGraph();
    customPlot->graph(0)->setData(x, y);
    // give the axes some labels:
    customPlot->xAxis->setLabel("x");
    customPlot->yAxis->setLabel("y");
    // set axes ranges, so we see all data:
    customPlot->xAxis->setRange(-1, 1);
    customPlot->yAxis->setRange(0, 1);

    customPlot->replot();
}

void MainWindow::openSerialPort()
{
    if(!device->open())
    {
        setStatusConnected(false);
        return;
    }

    setStatusConnected(true);

    commands->addCommand(new VnaCommandNone());
}

void MainWindow::writeTestData()
{
    commands->addCommand(new VnaCommandBigData(0, 256));
}

void MainWindow::setStatusConnected(bool connected)
{
    statusConnect->setPixmap(QPixmap(connected?":/icons/connect_status.png":":/icons/disconnect_status.png"));
}

void MainWindow::onCloseSerial()
{
    setStatusConnected(false);
}

void MainWindow::onNoneComplete()
{
    //commands->sendSetFreq(123456);
}
