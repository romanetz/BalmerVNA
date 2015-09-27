#include "mainwindow.h"
#include <QAction>
#include <QToolBar>

#include "vnadevice.h"
#include "vnacommands.h"


MainWindow* mainWindow = nullptr;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    mainWindow = this;

    device = new VnaDevice();
    commands = new VnaCommands(device);

    createActions();
    createToolbar();
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

void MainWindow::openSerialPort()
{
    if(!device->open())
        return;

    commands->sendNone();
}

void MainWindow::writeTestData()
{
    commands->sendBigData(0, 256);
}
