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

                connect(rf_device, SIGNAL(log(QString)), this, SIGNAL(log(QString)), Qt::DirectConnection);

                if(m_devices.length()>0)
                {
                    if (m_devices.indexOf(rf_device)==0) { emit newRF1Explorer(rf_device);}
                    if (m_devices.indexOf(rf_device)==1) { emit newRF2Explorer(rf_device);}
                }
                if (m_debug) emit log(QString("[serialManager.startConnection] New RF connected at port %1!").arg(QString(serialName)));
            }
        }
    }

}
//-------------------------------------------------------------------------------------------------------------------------------------------------------
