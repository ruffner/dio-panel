#ifndef DIOPINDISPLAY_H
#define DIOPINDISPLAY_H

#include <QObject>
#include <QWidget>
#include <QList>
#include <QDebug>
#include <QLabel>
#include <QSlider>
#include <QWidget>
#include <QString>
#include <QDialog>
#include <QSpinBox>
#include <QSettings>
#include <QGroupBox>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSlider>

class DIOPinDisplay : public QWidget
{
    Q_OBJECT
public:
    explicit DIOPinDisplay(QWidget *parent = nullptr, int displayId = 0, int pinNum = 13);
    void setPinDirection(int dir);
    void setPinValue(int val);

private:
    int id;
    int pinNumber;
    int pinDirection, pinValue;
    bool enabled;
    QString pinName;

    QGroupBox *mainBox;
    QLabel *pinValueLabel;
    QPushButton *pinDirectionButton, *pinLevelButton, *pinEnabledButton;

signals:
    void emitPinDirection(int pin, int dir);
    void emitPinValue(int pin, int val);

private slots:
    void onLevelChange(bool chk);
    void onDirectionChange(bool chk);
    void onEnableButtonClicked(bool chk);
};

#endif // DIOPINDISPLAY_H
