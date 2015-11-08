#include "project.h"

#include <QSettings>

Project* g_project = nullptr;

Project::Project(QObject *parent)
    : QObject(parent)
{
    g_project = this;

    scanFrequencyMin_ = 10e3;
    scanFrequencyMax_ = 100e6;
    scanFrequencyCount_ = 100;
    scanFrequencyIntervalType_ = FrequencyIntervalType::Standart;
}

void Project::loadSettings()
{
    QSettings settings;

    settings.beginGroup("ScanFrequency");
    scanFrequencyMin_ = settings.value("min", scanFrequencyMin_).toDouble();
    scanFrequencyMax_ = settings.value("max", scanFrequencyMax_).toDouble();
    scanFrequencyCount_ = settings.value("count", scanFrequencyCount_).toInt();
    scanFrequencyIntervalType_ = (FrequencyIntervalType)settings.value("type", (int)scanFrequencyIntervalType_).toInt();
    settings.endGroup();

}

void Project::saveSettingsFreq()
{
    QSettings settings;

    settings.beginGroup("ScanFrequency");
    settings.setValue("min", scanFrequencyMin_);
    settings.setValue("max", scanFrequencyMax_);
    settings.setValue("count", scanFrequencyCount_);
    settings.setValue("type", (int)scanFrequencyIntervalType_);
    settings.endGroup();
}


void Project::setScanFrequencyMin(double f)
{
    scanFrequencyMin_ = f;
}

void Project::setScanFrequencyMax(double f)
{
    scanFrequencyMax_ = f;
}

void Project::setScanFrequencyCount(int count)
{
    scanFrequencyCount_ = count;
}

void Project::setScanFrequencyIntervalType(FrequencyIntervalType type)
{
    scanFrequencyIntervalType_ = type;
}
