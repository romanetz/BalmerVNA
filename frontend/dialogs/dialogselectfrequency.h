#ifndef DIALOGSELECTFREQUENCY_H
#define DIALOGSELECTFREQUENCY_H

#include <QDialog>

class QFormLayout;
class QComboBox;
class QDoubleSpinBox;
class QSpinBox;


class DialogSelectFrequency : public QDialog
{
    Q_OBJECT
public:
    explicit DialogSelectFrequency(QWidget *parent = 0);

    ~DialogSelectFrequency();
signals:

public slots:

protected:
protected:
    QComboBox* comboFreqType;
    QDoubleSpinBox* spinFrequencyMin;
    QDoubleSpinBox* spinFrequencyMax;
    QSpinBox* spinSamplesCount;
};

#endif // DIALOGSELECTFREQUENCY_H
