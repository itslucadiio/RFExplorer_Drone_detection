#include "serialmanager.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>

serialManager::serialManager(QObject* parent) : QObject(parent)
{
    rf = new QSerialPort(this);
}

void serialManager::initialize(bool debug)
{
    m_debug = debug;
    if (m_debug) emit log(QString("[serialManager.initialize] SerialManager object created."));
}

void serialManager::startConnection(){
    rf_available = false;
    rf_port = "";
    QString serialName = "";

    //Search rf
    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()) {
        if (serialPortInfo.hasVendorIdentifier()){
            if (serialPortInfo.productIdentifier() == rf_EXPLORER){
                if (m_debug) emit log(QString("[serialManager.startConnection] RF found."));
                rf_available = true;
                serialName = serialPortInfo.portName();
            }
        }
    }

    //Connect
    if (rf_available){
        rf_port = serialName;
        rf->setPortName(rf_port);
        qDebug() << (rf->open(QIODevice::ReadWrite) ? "True" : "False");
        connect(rf, SIGNAL(readyRead()), this, SLOT(read_data()), Qt::DirectConnection);
        //rf->setDataBits(QSerialPort::Data8);
        rf->setBaudRate(500000);
        //rf->setParity(QSerialPort::NoParity);
        //rf->setStopBits(QSerialPort::OneStop);
        //rf->setFlowControl(QSerialPort::NoFlowControl);
        if (m_debug) emit log(QString("[serialManager.startConnection] RF connected!"));
    }

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------
void serialManager::send_data(QByteArray data)
{
    if (rf->isOpen())
    {
        rf->write(data);
        rf->waitForBytesWritten(500);
        if (m_debug) emit log(QString("[serialManager.send_data] Data sent!"));
        //read_data();
    }
}
void serialManager::read_data()
{
    while (rf->canReadLine())
    {
        const QByteArray data = rf->readLine();
        QString stringData = QString(data);
        qDebug() << "data: " <<  stringData;
    }
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------------------------------------------------------------
void serialManager::sendCommand(QString msg)
{
    QString array;
    array.append("#");
    array.append(char(msg.length() + 2));
    array.append(msg);

    QByteArray data = array.toUtf8();
    send_data(data);
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------
