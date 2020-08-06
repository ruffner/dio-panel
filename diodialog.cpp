#include "diodialog.h"

DIODialog::DIODialog(QWidget *parent)
    : QDialog(parent)
{
    this->setLayout(new QVBoxLayout());
    this->layout()->setContentsMargins(10,10,10,10);

    hardware = new DIOHardwareInterface();
    connect(hardware, SIGNAL(connected()), this, SLOT(onHardwareConnected()));
    connect(hardware, SIGNAL(disconnected()), this, SLOT(onHardwareDisconnected()));
    connect(hardware, SIGNAL(emitPinList(QList<QPair<int,int>>)), this, SLOT(onAddPins(QList<QPair<int,int>>)));
    connect(hardware, SIGNAL(emitDirMask(QList<int>)), this, SLOT(onUpdateDirMask(QList<int>)));
    connect(hardware, SIGNAL(emitValMask(QList<int>)), this, SLOT(onUpdateValMask(QList<int>)));

    pinDisplayGroupBox = new QGroupBox("Pins");
    pinDisplayLayout = new QGridLayout();
    pinDisplayGroupBox->setLayout(pinDisplayLayout);

    this->layout()->addWidget(hardware);
    //this->layout()->addWidget(pinSelectionGroupBox);
    this->layout()->addWidget(pinDisplayGroupBox);
    ((QVBoxLayout*)(this->layout()))->addSpacing(20);
}

DIODialog::~DIODialog()
{
}

void DIODialog::onAddPins(QList<QPair<int,int>> pins)
{
    DIOPinDisplay *temp;

    for( int i=0; i<pins.size(); i++ ){
        temp = new DIOPinDisplay(this,pins[i].first, pins[i].second);
        connect(temp, SIGNAL(emitPinDirection(int, int)), hardware, SLOT(onSetPinDirection(int, int)));
        connect(temp, SIGNAL(emitPinValue(int, int)), hardware, SLOT(onSetPinValue(int, int)));
        pinDisplays.push_back(temp);
        pinDisplayLayout->addWidget(temp, (int)(i/8), i % 8);
    }
}

void DIODialog::onUpdateDirMask(QList<int> dirs)
{
    if( dirs.size() != pinDisplays.size() ){
        qDebug() << "ERROR pin count mismatch";
        return;
    }

    for( int i=0; i<dirs.size(); i++ ){
        pinDisplays[i]->setPinDirection(dirs[i]);
    }
}

void DIODialog::onUpdateValMask(QList<int> vals)
{
    if( vals.size() != pinDisplays.size() ){
        qDebug() << "ERROR pin count mismatch";
        return;
    }

    for( int i=0; i<vals.size(); i++ ){
        pinDisplays[i]->setPinValue(vals[i]);
    }
}

void DIODialog::onHardwareConnected()
{
    qDebug() << "hardware connected";



}

void DIODialog::onHardwareDisconnected()
{
    qDebug() << "hardware disconnected";
}
