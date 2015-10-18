#include "mainwindow.h"
#include <QAction>
#include <QToolBar>
#include <QLabel>
#include <QStatusBar>
#include <QComboBox>

#include "vnadevice.h"

#include "pugixml/pugixml.hpp"


MainWindow* mainWindow = nullptr;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , pix_red_orb(":/icons/red_orb.png")
    , pix_green_orb(":/icons/green_orb.png")
    , freqIndex(0)
    , _bStopSample(true)
{
    setWindowTitle("Balmer VNA");
    mainWindow = this;

    setGeometry(400, 250, 542, 390);

    device = new VnaDevice();
    connect(device, SIGNAL(signalClose()), this, SLOT(onCloseSerial()));

    commands = new VnaCommands(device);
    connect(commands, SIGNAL(signalEndSampling()), this, SLOT(onEndSampling()));
    connect(commands, SIGNAL(signalReceiveHard()), this, SLOT(onReceiveHard()));


    createCustomPlot();
    createActions();
    createToolbar();

    statusBisy = new QLabel();
    statusBar()->addPermanentWidget(statusBisy);

    statusConnect = new QLabel();
    statusBar()->addPermanentWidget(statusConnect);


    setCentralWidget(customPlot);

    setStatusConnected(false);
    setBisy(false);
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

    refreshAct = new QAction(QIcon(":/icons/refresh.png"), tr("Refresh graph"), this );
    connect(refreshAct, SIGNAL(triggered()), this, SLOT(onRefresh()));

    startSamplingAct = new QAction(QIcon(":/icons/blue_play.png"), tr("Start sampling"), this );
    connect(startSamplingAct, SIGNAL(triggered()), this, SLOT(onStartSampling()));

    stopSamplingAct = new QAction(QIcon(":/icons/blue_stop.png"), tr("Stop sampling"), this );
    connect(stopSamplingAct, SIGNAL(triggered()), this, SLOT(onStopSampling()));

    saveAct = new QAction(QIcon(":/icons/save.png"), tr("Save hard data"), this );
    connect(saveAct, SIGNAL(triggered()), this, SLOT(onSave()));
}

void MainWindow::createToolbar()
{
    comboRxTx = new QComboBox();
    comboRxTx->addItem("RX", QVariant(false));
    comboRxTx->addItem("TX", QVariant(true));
    connect(comboRxTx, SIGNAL(currentIndexChanged(int)), this, SLOT(onRxTxIndexChanged(int)));

    mainToolBar = addToolBar("main");
    mainToolBar->addAction(connectAct);
    mainToolBar->addAction(settingsAct);
    mainToolBar->addAction(writeTestAct);
    mainToolBar->addAction(refreshAct);
    mainToolBar->addWidget(comboRxTx);
    mainToolBar->addAction(startSamplingAct);
    mainToolBar->addAction(stopSamplingAct);
    mainToolBar->addAction(saveAct);
}

void MainWindow::createCustomPlot()
{
    customPlot = new QCustomPlot(this);
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    QVector<double> x(101), y(101); // initialize with entries 0..100
    QVector<double> y2(101);
    for (int i=0; i<101; ++i)
    {
      x[i] = i/50.0 - 1; // x goes from -1 to 1
      y[i] = x[i]*x[i];  // let's plot a quadratic function
      y2[i] = sin(x[i]);  // let's plot a quadratic function
    }
    // create graph and assign data to it:
    customPlot->addGraph();
    customPlot->graph(0)->setPen(QPen(Qt::red));
    customPlot->graph(0)->setData(x, y);

    customPlot->addGraph();
    customPlot->graph(1)->setPen(QPen(Qt::blue));
    customPlot->graph(1)->setData(x, y2);
    // give the axes some labels:
    customPlot->xAxis->setLabel("x");
    customPlot->yAxis->setLabel("y");
    // set axes ranges, so we see all data:
    customPlot->xAxis->setRange(-1, 1);
    customPlot->yAxis->setRange(0, 1);
    //customPlot->yAxis->setScaleType(QCPAxis::stLogarithmic);


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

    commands->commandInitial();
    //commands->appendCommand(new VnaCommandPing());
}

void MainWindow::writeTestData()
{
    //commands->appendCommand(new VnaCommandPing());
    setBisy(true);
    commands->commandSampling(123456);
}

void MainWindow::setStatusConnected(bool connected)
{
    statusConnect->setPixmap(QPixmap(connected?":/icons/connect_status.png":":/icons/disconnect_status.png"));
}

void MainWindow::setBisy(bool bisy)
{
    if(bisy)
        statusBisy->setPixmap(pix_red_orb);
    else
        statusBisy->setPixmap(pix_green_orb);
}

void MainWindow::onCloseSerial()
{
    setStatusConnected(false);
}

void MainWindow::onRefresh()
{
    int count = commands->samplingBufferSize();
    QVector<double> x(count), yI(count), yQ(count);
    for(int i=0; i<count; i++)
    {
        x[i] = i;
        yI[i] = commands->arrayI()[i];
        yQ[i] = commands->arrayQ()[i];
        //yI[i] = yQ[i] = commands->arrayI()[i];
    }

    customPlot->graph(0)->setData(x, yI);
    customPlot->graph(1)->setData(x, yQ);

    customPlot->rescaleAxes();
    customPlot->replot();
}

void MainWindow::onEndSampling()
{
    onRefresh();
    setBisy(false);
}

bool MainWindow::isTx()
{
    return comboRxTx->currentData().toBool();
}

void MainWindow::onRxTxIndexChanged(int index)
{
    (void)index;
    commands->appendCommand(new VnaCommandSetTx(isTx()));
}

void MainWindow::onStartSampling()
{
    arrFreq.resize(300);
    arrFreqM.resize(arrFreq.size());
    arrData.resize(arrFreq.size());
    arrAmplithudeI.resize(arrFreq.size());
    arrAmplithudeQ.resize(arrFreq.size());
    arrPhase.resize(arrFreq.size());

    for(int i=0; i<arrFreq.size(); i++)
    {
        arrFreq[i] = 1e6+i*30e6/arrFreq.size();
        arrFreqM[i] = arrFreq[i]*1e-6;
        arrAmplithudeI[i] = 0;
        arrAmplithudeQ[i] = 0;
        arrPhase[i] = 0;
    }

    _bStopSample = false;

    freqIndex = 0;

    startNextSample();
}

void MainWindow::onStopSampling()
{
    _bStopSample = true;
}

void MainWindow::startNextSample()
{
    if(_bStopSample)
        return;
    //qDebug() << "start " << freqIndex << "f=" << arrFreq[freqIndex];
    commands->appendCommand(new VnaCommandSetFreq(arrFreq[freqIndex]));
    commands->appendCommand(new VnaCommandStartSamplingAndCalculate());
    commands->appendCommand(new VnaCommandGetCalculated(10));
}

void calcFi(double csin, double ccos, double& amplitude, double& fi)
{
    /*
        input
        csin*sin(f)+ccos*cos(f) == amplitude*sin(f+fi)
        return (amplitude, fi)
    */
    amplitude = sqrt(csin*csin+ccos*ccos);
    csin /= amplitude;
    ccos /= amplitude;

    fi = atan2(ccos, csin);
}

void MainWindow::onReceiveHard()
{
    if(freqIndex>arrFreq.size())
    {
        _bStopSample = true;
        return;
    }

    const HardSamplingData& data = commands->hardData();
    arrData[freqIndex] = data;

    double Iamplitude, Ifi;
    double Qamplitude, Qfi;
    calcFi(data.i_csin, data.i_ccos, Iamplitude, Ifi);
    calcFi(data.q_csin, data.q_ccos, Qamplitude, Qfi);

    arrAmplithudeI[freqIndex] = Iamplitude/Qamplitude*8;
    //arrAmplithudeQ[freqIndex] = Qamplitude;

    double fi = Ifi-Qfi;

    if(fi>M_PI)
        fi -= M_PI*2;
    if(fi<-M_PI)
        fi += M_PI*2;

    if(freqIndex>0)
    {
        double prev = arrPhase[freqIndex-1];
        if(fi>prev+4)
            fi -= M_PI*2;
        if(fi<prev-4)
            fi += M_PI*2;
    }

    if(fi>M_PI)
        fi -= M_PI*2;
    if(fi<-M_PI)
        fi += M_PI*2;

    arrPhase[freqIndex] = fi;


    freqIndex = (freqIndex+1)%arrFreq.size();
    startNextSample();

    customPlot->graph(0)->setData(arrFreqM, arrAmplithudeI);
    //customPlot->graph(1)->setData(arrFreq, arrAmplithudeQ);
    customPlot->graph(1)->setData(arrFreqM, arrPhase);


    customPlot->rescaleAxes();
    customPlot->replot();
}

void MainWindow::onSave()
{
    saveData("/home/balmer/radio/stm32/projects/BalmerVNA/script/hard.xml");
}

void MainWindow::saveData(const QString& fileName)
{
    pugi::xml_document doc;
    pugi::xml_node root_node = doc.append_child("root");

    root_node.append_attribute("tx").set_value(isTx());

    pugi::xml_node data_node = root_node.append_child("data");
    for(int i=0; i<arrFreq.size(); i++)
    {
        pugi::xml_node h_node = data_node.append_child("h");
        const HardSamplingData& h = arrData[i];
        h_node.append_attribute("F").set_value(arrFreq[i]);
        h_node.append_attribute("freq").set_value(h.freq);
        h_node.append_attribute("q_cconst").set_value(h.q_cconst);
        h_node.append_attribute("q_csin").set_value(h.q_csin);
        h_node.append_attribute("q_ccos").set_value(h.q_ccos);
        h_node.append_attribute("q_sqr").set_value(h.q_sqr);
        h_node.append_attribute("i_cconst").set_value(h.i_cconst);
        h_node.append_attribute("i_csin").set_value(h.i_csin);
        h_node.append_attribute("i_ccos").set_value(h.i_ccos);
        h_node.append_attribute("i_sqr").set_value(h.i_sqr);
    }



    doc.save_file(fileName.toUtf8().constData());
}
