#ifndef PROJECT_H
#define PROJECT_H

#include <QObject>

enum class FrequencyIntervalType
{
    //Стандартный интервал частот, на которых происходит калибровка.
    Standart,

    //Точки по интервалу распределены линейно.
    // f = fmin+(fmax-fmin)*t
    Linear,

    //На низкой частоте точек больше, на высокой меньше.
    // f = exp(log(fmin)+(log(fmax)-log(fmin))*t)
    Exponential,
};

class Project : public QObject
{
    Q_OBJECT
public:
    explicit Project(QObject *parent = 0);

    void loadSettings();
    void saveSettingsFreq();

    double scanFrequencyMin() { return scanFrequencyMin_;}
    double scanFrequencyMax() { return scanFrequencyMax_;}
    int scanFrequencyCount() { return scanFrequencyCount_; }
    FrequencyIntervalType scanFrequencyIntervalType() { return scanFrequencyIntervalType_; }

    void setScanFrequencyMin(double f);
    void setScanFrequencyMax(double f);
    void setScanFrequencyCount(int count);
    void setScanFrequencyIntervalType(FrequencyIntervalType type);
signals:

public slots:
protected:
    double scanFrequencyMin_;
    double scanFrequencyMax_;
    int scanFrequencyCount_;
    FrequencyIntervalType scanFrequencyIntervalType_;

};

extern Project* g_project;

#endif // PROJECT_H
