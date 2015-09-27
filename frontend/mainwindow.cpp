#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow* mainWindow = nullptr;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    mainWindow = this;
    ui->setupUi(this);

    device = new VnaDevice();
    if(device->open())
    {
        device->sendNone();
    }
}

MainWindow::~MainWindow()
{
    delete ui;

    delete device;
    mainWindow = nullptr;
}
