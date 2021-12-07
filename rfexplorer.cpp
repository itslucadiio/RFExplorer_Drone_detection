#include "rfexplorer.h"

RFExplorer::RFExplorer(QObject *parent) : QObject(parent)
{

}


//-------------------------------------------------------------------------------------------------------------------------------------------------------
void RFExplorer::initialize(QSerialPort* port, bool debug)
{
    serial_port = port;
    m_debug = debug;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------


//GETTERS AND SETTERS ------------------------------------------------------------------------------------------------------------------------------------
QString RFExplorer::getStart_Freq() const
{
    return Start_Freq;
}

void RFExplorer::setStart_Freq(const QString &value)
{
    Start_Freq = value;
}

QString RFExplorer::getFreq_step() const
{
    return Freq_step;
}

void RFExplorer::setFreq_step(const QString &value)
{
    Freq_step = value;
}

QString RFExplorer::getAmp_Top() const
{
    return Amp_Top;
}

void RFExplorer::setAmp_Top(const QString &value)
{
    Amp_Top = value;
}

QString RFExplorer::getAmp_Bottom() const
{
    return Amp_Bottom;
}

void RFExplorer::setAmp_Bottom(const QString &value)
{
    Amp_Bottom = value;
}

QString RFExplorer::getSweep_Steps() const
{
    return Sweep_Steps;
}

void RFExplorer::setSweep_Steps(const QString &value)
{
    Sweep_Steps = value;
}

QString RFExplorer::getExcp_Module_Active() const
{
    return Excp_Module_Active;
}

void RFExplorer::setExcp_Module_Active(const QString &value)
{
    Excp_Module_Active = value;
}

QString RFExplorer::getCurrent_Mode() const
{
    return Current_Mode;
}

void RFExplorer::setCurrent_Mode(const QString &value)
{
    Current_Mode = value;
}

QString RFExplorer::getMin_Freq() const
{
    return Min_Freq;
}

void RFExplorer::setMin_Freq(const QString &value)
{
    Min_Freq = value;
}

QString RFExplorer::getMax_Freq() const
{
    return Max_Freq;
}

void RFExplorer::setMax_Freq(const QString &value)
{
    Max_Freq = value;
}

QString RFExplorer::getMax_Span() const
{
    return Max_Span;
}

void RFExplorer::setMax_Span(const QString &value)
{
    Max_Span = value;
}

QString RFExplorer::getRbw() const
{
    return Rbw;
}

void RFExplorer::setRbw(const QString &value)
{
    Rbw = value;
}

QString RFExplorer::getDB_Offset() const
{
    return dB_Offset;
}

void RFExplorer::setDB_Offset(const QString &value)
{
    dB_Offset = value;
}

QString RFExplorer::getUndoccumented() const
{
    return Undoccumented;
}

void RFExplorer::setUndoccumented(const QString &value)
{
    Undoccumented = value;
}

QString RFExplorer::getPort_name() const
{
    return port_name;
}

void RFExplorer::setPort_name(const QString &value)
{
    port_name = value;
}

QString RFExplorer::getSerial_number() const
{
    return serial_number;
}

void RFExplorer::setSerial_number(const QString &value)
{
    serial_number = value;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------------------------------------------------------------
void RFExplorer::send_data(QByteArray data)
{
    if (serial_port->isOpen())
    {
        serial_port->write(data);
        serial_port->waitForBytesWritten(500);
        if (m_debug) emit log(QString("[serialManager.send_data] Data sent!"));
    }
}

void RFExplorer::read_data()
{
    while(serial_port->canReadLine()){
        const QByteArray data = serial_port->readLine();
        qDebug() << "data: " <<  data;

        QString stringData = QString(data);
        if (stringData.contains("(C) Ariel Rocholl"))
        {
            if (m_debug) emit log(QString("[serialManager] RF authenticated! Hi Ariel Rocholl."));
        }
    }
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------
