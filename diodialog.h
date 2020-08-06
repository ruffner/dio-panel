#ifndef DIODIALOG_H
#define DIODIALOG_H

#include <QDialog>
#include <QComboBox>

#include "diopindisplay.h"
#include "diohardwareinterface.h"

#define NUM_PROMICRO_PINS 19


class DIODialog : public QDialog
{
    Q_OBJECT

public:
    DIODialog(QWidget *parent = nullptr);
    ~DIODialog();

public slots:
    void onAddPins(QList<QPair<int,int>> pins);
    void onUpdateDirMask(QList<int> dirs);
    void onUpdateValMask(QList<int> vals);
    void onHardwareConnected();
    void onHardwareDisconnected();

private:
    QComboBox *availablePinsComboBox;
    QPushButton *addPinButton;
    QGroupBox *pinDisplayGroupBox, *pinSelectionGroupBox;
    QGridLayout *pinDisplayLayout;
    QList<DIOPinDisplay*> pinDisplays;
    DIOHardwareInterface *hardware;
};

#endif // DIODIALOG_H
