#include "dialogselectfrequency.h"

#include <QFormLayout>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>

#include "project.h"

DialogSelectFrequency::DialogSelectFrequency(QWidget *parent)
    : QDialog(parent)
{
    comboFreqType = new QComboBox();
    comboFreqType->addItem("Standart series", (int)FrequencyIntervalType::Standart);
    comboFreqType->addItem("Linear", (int)FrequencyIntervalType::Linear);
    comboFreqType->addItem("Exponential", (int)FrequencyIntervalType::Exponential);

    for(int i=0; i<comboFreqType->count(); i++)
    {
        if(comboFreqType->itemData(i).toInt()==(int)g_project->scanFrequencyIntervalType())
        {
            comboFreqType->setCurrentIndex(i);
            break;
        }
    }


    spinFrequencyMin = new QDoubleSpinBox();
    spinFrequencyMin->setRange(10, 100e3);
    spinFrequencyMin->setValue(g_project->scanFrequencyMin()*1e-3);
    spinFrequencyMax = new QDoubleSpinBox();
    spinFrequencyMax->setRange(10, 100e3);
    spinFrequencyMax->setValue(g_project->scanFrequencyMax()*1e-3);

    spinSamplesCount = new QSpinBox();
    spinSamplesCount->setRange(2, 1000);
    spinSamplesCount->setValue(g_project->scanFrequencyCount());

    QFormLayout* topLayout;
    topLayout = new QFormLayout();
    topLayout->addRow("Frequency type", comboFreqType);
    topLayout->addRow("Frequency min (KHz)", spinFrequencyMin);
    topLayout->addRow("Frequency max (KHz)", spinFrequencyMax);
    topLayout->addRow("Samples count", spinSamplesCount);

    this->setLayout(topLayout);
}


DialogSelectFrequency::~DialogSelectFrequency()
{
    g_project->setScanFrequencyMin(spinFrequencyMin->value()*1e3);
    g_project->setScanFrequencyMax(spinFrequencyMax->value()*1e3);
    g_project->setScanFrequencyCount(spinSamplesCount->value());
    g_project->setScanFrequencyIntervalType((FrequencyIntervalType)comboFreqType->currentData().toInt());

    g_project->saveSettingsFreq();
}
