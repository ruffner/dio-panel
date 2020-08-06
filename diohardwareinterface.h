#ifndef DIOHARDWAREINTERFACE_H
#define DIOHARDWAREINTERFACE_H

#include <QObject>
#include <QDebug>
#include <QList>
#include <QPair>
#include <QWidget>
#include <QHBoxLayout>
#include <QSerialPort>
#include <QApplication>
#include <QComboBox>
#include <QLabel>
#include <QTimer>
#include <QGroupBox>
#include <QPushButton>
#include <QSerialPortInfo>
#include <QDialogButtonBox>

// same as arduino code
#define CMD_GETPINLIST "getpinlist"
#define CMD_GETDIRMASK "getdirmask"
#define CMD_SETDIRMASK "setdirmask"
#define CMD_SETVALMASK "setvalmask"
#define CMD_GETVALMASK "getvalmask"

class DIOHardwareInterface : public QWidget
{
    Q_OBJECT
public:
    explicit DIOHardwareInterface(QWidget *parent = nullptr);

public slots:
    void onSetPinDirection(int pin, int dir);
    void onSetPinValue(int pin, int val);

private:
    void sendCommand(QString cmd);
    void parseResponse(QByteArray data);

    QTimer *timer;
    QList<QPair<int,int>> pins;
    QList<int> pinDirs, pinVals;
    bool isConnected;
    QList<QString> lastCommand;
    QSerialPort *serial;
    QByteArray buffer;
    QLabel *connectionLabel;
    QComboBox *serialComboBox;
    QPushButton *connectButton;

signals:
    void connected();
    void disconnected();
    void emitPinList(QList<QPair<int,int>> pins);
    void emitDirMask(QList<int> mask);
    void emitValMask(QList<int> mask);

private slots:
    void onRequestUpdate();
    void onConnectAttempt();
    void onReadyRead();

};

#endif // DIOHARDWAREINTERFACE_H
