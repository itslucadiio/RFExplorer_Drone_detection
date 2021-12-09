#include "serialmanager.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>

serialManager::serialManager(QObject* parent) : QObject(parent)
{
}

void serialManager::initialize(bool debug)
{
    m_debug = debug;
    if (m_debug) emit log(QString("[serialManager.initialize] SerialManager object created."));
}

void serialManager::startConnection(){


    //Search rf
    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {
        if (serialPortInfo.hasVendorIdentifier())
        {
            if (serialPortInfo.productIdentifier() == rf_EXPLORER)
            {
                QString serialName = "";
                if (m_debug) emit log(QString("[serialManager.startConnection] New RF found."));

                serialName = serialPortInfo.portName();
                QSerialPort* newSerial = new QSerialPort();
                newSerial->setPortName(serialName);
                newSerial->setBaudRate(500000);

                RFExplorer* rf_device = new RFExplorer();
                rf_device->initialize(newSerial, m_debug);
                m_devices.append(rf_device);
                rf.append(newSerial);

                qDebug() << (newSerial->open(QIODevice::ReadWrite) ? "True" : "False");
                connect(newSerial, SIGNAL(readyRead()), rf_device, SLOT(read_data()), Qt::DirectConnection);
                connect(this, SIGNAL(send_data(QByteArray)), rf_device, SLOT(send_data(QByteArray)), Qt::DirectConnection);
                connect(rf_device, SIGNAL(log(QString)), this, SIGNAL(log(QString)), Qt::DirectConnection);
                connect(this, SIGNAL(send_config(double, double)), rf_device, SLOT(send_config(double,double)), Qt::DirectConnection);

                if (m_debug) emit log(QString("[serialManager.startConnection] New RF connected at port %1!").arg(QString(serialName)));
            }
        }
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
    emit send_data(data);
}
//SLOT TO SEND CONFIG FROM FRONTEND TO RF EXPLORER
void serialManager::sendConfig(double start_freq, double end_freq)
{
    emit send_config(start_freq, end_freq);
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------
