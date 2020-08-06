#include "diohardwareinterface.h"

DIOHardwareInterface::DIOHardwareInterface(QWidget *parent) : QWidget(parent), isConnected(false)
{
    this->setLayout(new QVBoxLayout());
    this->layout()->setContentsMargins(0,0,0,0);
    this->layout()->setSizeConstraint(QLayout::SetFixedSize);

    // DEVICE CONNECTION GROUPING
    QGroupBox *connectionGroupBox = new QGroupBox(QString("Connection"));
    connectionGroupBox->setLayout(new QHBoxLayout());
    connectionGroupBox->layout()->setContentsMargins(6,6,6,6);
    serialComboBox = new QComboBox(this);
    // ADD ALL SERIAL PORTS
    Q_FOREACH(QSerialPortInfo port, QSerialPortInfo::availablePorts()) {
       serialComboBox->addItem(port.portName());
    }
    connectButton = new QPushButton("Connect", this);


    connectionLabel = new QLabel(QString("Not connected"));
    connectionGroupBox->layout()->addWidget(connectionLabel);
    connectionGroupBox->layout()->addWidget(serialComboBox);
    connectionGroupBox->layout()->addWidget(connectButton);
    this->layout()->addWidget(connectionGroupBox);

    serial = new QSerialPort(this);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onRequestUpdate()));

    connect(connectButton, SIGNAL(clicked()), this, SLOT(onConnectAttempt()));
    connect(serial, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
}

void DIOHardwareInterface::onConnectAttempt()
{
    if( !isConnected ){
       // SET BAUD, MAYBE OTHER SHIT
        serial->setPortName(serialComboBox->currentText());
       serial->setBaudRate(QSerialPort::Baud115200);

       qDebug() << "opening serial port";

       if( serial->open(QSerialPort::ReadWrite) ){
           qDebug() << "opened serial port!";

           lastCommand.push_front(CMD_GETPINLIST);
            sendCommand(lastCommand.front());
            lastCommand.push_front(CMD_GETDIRMASK);
            sendCommand(lastCommand.front());
            lastCommand.push_front(CMD_GETVALMASK);
            sendCommand(lastCommand.front());

            emit connected();
       } else {
           qDebug() << "failed to open serial port";
       }
   } else {
       emit disconnected();
   }
}

void DIOHardwareInterface::sendCommand(QString s)
{
    //qDebug() << "got " << s << " in send command";

    QByteArray b(s.size(), ' ');

    for( int i=0; i<s.size(); i++ ) b[i] = s.at(i).cell();

    b.push_back('\n');
    serial->write(b);

    //qDebug() << "sent '" << b << "' over serial";
}

void DIOHardwareInterface::onReadyRead()
{
    buffer += serial->readAll();

    int pos = buffer.indexOf('\n');

    // if there is at least one whole reponse in the buffer
    if( pos != -1 ){
        QByteArray data = buffer.mid(0,pos+1);
        buffer.remove(0,pos+1);
        parseResponse(data);
    }
}

void DIOHardwareInterface::onRequestUpdate()
{
    lastCommand.push_front(CMD_GETVALMASK);
    sendCommand(lastCommand.front());
}

// parse received packet and clear packet buffer
void DIOHardwareInterface::parseResponse(QByteArray data)
{
    data.chop(3);
    data.remove(0,1);

    //qDebug() << "got response data: " << data;

    if( lastCommand.size() == 0 ){
        qDebug() << "empty command list, returning";
        return;
    }

    if( lastCommand.back() == CMD_GETPINLIST ){
        QStringList chunks = QString(data).split(",");
        for( int i=0; i < chunks.size(); i++ ){
            pins.push_back(QPair<int,int>(i,chunks[i].toInt()));
            pinDirs.push_back(0);
            pinVals.push_back(0);
            qDebug() << "adding pin " << chunks[i].toInt();
        }
        emit emitPinList(pins);

        qDebug() << "starting polling timer";
        timer->start(50);

    }
    else if( lastCommand.back() == CMD_GETDIRMASK ){
        QStringList chunks = QString(data).split(",");
        for( int i=0; i < chunks.size(); i++ ){
            pinDirs[i] = chunks[i].toInt();
        }
        emit emitDirMask(pinDirs);
    }
    else if( lastCommand.back() == CMD_GETVALMASK ){
        QStringList chunks = QString(data).split(",");
        for( int i=0; i < chunks.size(); i++ ){
            pinVals[i] = chunks[i].toInt();
        }
        emit emitValMask(pinVals);
    }
    lastCommand.pop_back();
}

void DIOHardwareInterface::onSetPinDirection(int pin, int dir)
{
    pinDirs[pin] = dir;
    QString str = QString(CMD_SETDIRMASK) + '\n' + "{";

    for( int i=0; i<pinDirs.size(); i++ ){
        str += QString::number(pinDirs[i]);

        if( i < pinDirs.size() - 1) {
            str += ",";
        }
    }
    str += "}";

    qDebug() << "onSetPinDirection string: " << str;

    sendCommand(str);
}

void DIOHardwareInterface::onSetPinValue(int pin, int val)
{
    pinVals[pin] = val;
    QString str = QString(CMD_SETVALMASK) + '\n' + "{";

    for( int i=0; i<pinVals.size(); i++ ){
        str += QString::number(pinVals[i]);

        if( i < pinVals.size() - 1) {
            str += ",";
        }
    }
    str += "}";

    qDebug() << "onSetPinValue string: " << str;

    sendCommand(str);
}
