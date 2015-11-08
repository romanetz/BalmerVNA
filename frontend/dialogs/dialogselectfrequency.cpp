#include "dialogselectfrequency.h"

#include <QFormLayout>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>

DialogSelectFrequency::DialogSelectFrequency(QWidget *parent)
    : QDialog(parent)
{
    comboFreqType = new QComboBox();
    comboFreqType->addItem("Standart series");
    comboFreqType->addItem("Linear");
    comboFreqType->addItem("Log");

    spinFrequencyMin = new QDoubleSpinBox();
    spinFrequencyMin->setRange(10, 100e3);
    spinFrequencyMin->setValue(10);
    spinFrequencyMax = new QDoubleSpinBox();
    spinFrequencyMax->setRange(10, 100e3);
    spinFrequencyMin->setValue(100e3);

    spinSamplesCount = new QSpinBox();
    spinSamplesCount->setRange(2, 1000);

    QFormLayout* topLayout;
    topLayout = new QFormLayout();
    topLayout->addRow("Frequency type", comboFreqType);
    topLayout->addRow("Frequency min (KHz)", spinFrequencyMin);
    topLayout->addRow("Frequency max (KHz)", spinFrequencyMax);
    topLayout->addRow("Samples count", spinSamplesCount);

    this->setLayout(topLayout);
}

