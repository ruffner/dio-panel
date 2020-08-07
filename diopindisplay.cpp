#include "diopindisplay.h"

DIOPinDisplay::DIOPinDisplay(QWidget *parent, int displayId, int pinNum) : QWidget(parent), pinName("pin"), id(displayId), pinNumber(pinNum), enabled(false)
{
    this->setLayout(new QVBoxLayout());
    this->layout()->setContentsMargins(0,0,0,0);
    this->layout()->setSizeConstraint(QLayout::SetFixedSize);

    pinName = "Pin " + QString::number(pinNum);

    // BASIC PIN STATE AND DIRECTION CONTROL + READ LABEL
    QGroupBox *controlsGroupBox = new QGroupBox(pinName);
    controlsGroupBox->setLayout(new QVBoxLayout());
    controlsGroupBox->layout()->setContentsMargins(6,6,6,6);

    pinLevelButton = new QPushButton("Pin Level", this);
    pinLevelButton->setCheckable(true);
    pinDirectionButton = new QPushButton("Pin Direction", this);
    pinDirectionButton->setCheckable(true);
    pinValueLabel = new QLabel(QString("Read Value"));
    pinEnabledButton = new QPushButton("Enabled");
    pinEnabledButton->setCheckable(true);
    pinEnabledButton->setChecked(true);

    connect(pinEnabledButton, SIGNAL(clicked(bool)), this, SLOT(onEnableButtonClicked(bool)));

    connect(pinLevelButton, SIGNAL(clicked(bool)), this, SLOT(onLevelChange(bool)));
    connect(pinDirectionButton, SIGNAL(clicked(bool)), this, SLOT(onDirectionChange(bool)));

    controlsGroupBox->layout()->addWidget(pinEnabledButton);
    controlsGroupBox->layout()->addWidget(pinDirectionButton);
    controlsGroupBox->layout()->addWidget(pinValueLabel);
    controlsGroupBox->layout()->addWidget(pinLevelButton);

    this->layout()->addWidget(controlsGroupBox);
}

void DIOPinDisplay::onLevelChange(bool checked)
{
    if( checked ){
        emit emitPinValue(id, 1);
    } else {
        emit emitPinValue(id, 0);
    }
}

void DIOPinDisplay::onDirectionChange(bool checked)
{
    if( checked ){
        emit emitPinDirection(id, 1);
        setPinDirection(1);
    } else {
        emit emitPinDirection(id, 0);
        setPinDirection(0);
    }
}

void DIOPinDisplay::setPinValue(int val)
{
    pinValue = val;

    if( pinValue ){
        pinLevelButton->setText("HIGH");
        pinValueLabel->setText("1");
        pinLevelButton->setChecked(true);
    } else {
        pinLevelButton->setText("LOW");
        pinValueLabel->setText("0");
        pinLevelButton->setChecked(false);
    }
}

void DIOPinDisplay::setPinDirection(int dir)
{
    pinDirection = dir;

    if( pinDirection ){
        pinDirectionButton->setChecked(true);
        pinDirectionButton->setText("OUTPUT");
        pinLevelButton->setDisabled(false);
    } else {
        pinDirectionButton->setChecked(false);
        pinDirectionButton->setText("INPUT");
        pinLevelButton->setDisabled(true);
    }
}

void DIOPinDisplay::onEnableButtonClicked(bool en)
{
    if( !en ){
        pinLevelButton->setVisible(false);
        pinDirectionButton->setVisible(false);
        pinValueLabel->setVisible(false);
        pinEnabledButton->setText("Enable");
    } else {
        pinLevelButton->setVisible(true);
        pinDirectionButton->setVisible(true);
        pinValueLabel->setVisible(true);
        pinEnabledButton->setText("Enabled");
    }
}
