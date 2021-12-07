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

                if (m_debug) emit log(QString("[serialManager.startConnection] New RF connected at port %1!").arg(QString(serialName)));
            }
        }
    }

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------
void serialManager::read_data()
{
    foreach (QSerialPort* port, rf) {
        while(port->canReadLine()){
            const QByteArray data = port->readLine();
            qDebug() << "data: " <<  data;

            QString stringData = QString(data);
            if (stringData.contains("(C) Ariel Rocholl"))
            {
                if (m_debug) emit log(QString("[serialManager] RF authenticated! Hi Ariel Rocholl."));
            }

            //Configuration params
//                if (stringData.contains("#C2-F:"))
//                {
//                    QVector<QString> setupFields = ["Start_Freq", "Freq_step", "Amp_Top", "Amp_Bottom", "Sweep_Steps", "Excp_Module_Active", "Current_Mode", "Min_Freq", "Max_Freq", "Max_Span", "Rbw", "dB_Offset", "Undoccumented"];
//                    QString dataString = stringData.split(":")[1];
//                    QStringList configList = dataString.split(",");
//                    if (configList.length() != setupFields.length())
//                        if (m_debug) emit log(QString("[serialManager] Ereceived unexpectd number of config values."));

//                    foreach(QString str, setupFields)
//                    {

//                    }
//                }


        }
    }

//    while (rf->canReadLine())
//    {
//        const QByteArray data = rf->readLine();
//        qDebug() << "data: " <<  data;

//        QString stringData = QString(data);
//        if (stringData.contains("(C) Ariel Rocholl"))
//        {
//            if (m_debug) emit log(QString("[serialManager] RF authenticated! Hi Ariel Rocholl."));
//        }

//        //Configuration params
//        if (stringData.contains("#C2-F:"))
//        {
//            QVector<QString> setupFields = {"Start_Freq", "Freq_step", "Amp_Top", "Amp_Bottom", "Sweep_Steps", "Excp_Module_Active", "Current_Mode", "Min_Freq", "Max_Freq", "Max_Span", "Rbw", "dB_Offset, "Undoccumented};
//            QString dataString = stringData.split(":")[1];
//            QStringList configList = dataString.split(",");
//            if (configList.length() != setupFields.length())
//                if (m_debug) emit log(QString("[serialManager] Ereceived unexpectd number of config values."));

//            foreach(QString str, setupFields)
//            {

//            }
//        }


//    }
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
//-------------------------------------------------------------------------------------------------------------------------------------------------------
