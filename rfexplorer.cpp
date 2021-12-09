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
int RFExplorer::getUndoccumented() const
{
    return Undoccumented;
}

void RFExplorer::setUndoccumented(int value)
{
    Undoccumented = value;
}

int RFExplorer::getDB_Offset() const
{
    return dB_Offset;
}

void RFExplorer::setDB_Offset(int value)
{
    dB_Offset = value;
}

int RFExplorer::getRbw() const
{
    return Rbw;
}

void RFExplorer::setRbw(int value)
{
    Rbw = value;
}

int RFExplorer::getMax_Span() const
{
    return Max_Span;
}

void RFExplorer::setMax_Span(int value)
{
    Max_Span = value;
}

int RFExplorer::getMax_Freq() const
{
    return Max_Freq;
}

void RFExplorer::setMax_Freq(int value)
{
    Max_Freq = value;
}

int RFExplorer::getMin_Freq() const
{
    return Min_Freq;
}

void RFExplorer::setMin_Freq(int value)
{
    Min_Freq = value;
}

QString RFExplorer::getCurrent_Mode() const
{
    return Current_Mode;
}

void RFExplorer::setCurrent_Mode(const QString &value)
{
    Current_Mode = value;
}

int RFExplorer::getExcp_Module_Active() const
{
    return Excp_Module_Active;
}

void RFExplorer::setExcp_Module_Active(int value)
{
    Excp_Module_Active = value;
}

int RFExplorer::getSweep_Steps() const
{
    return Sweep_Steps;
}

void RFExplorer::setSweep_Steps(int value)
{
    Sweep_Steps = value;
}

int RFExplorer::getAmp_Bottom() const
{
    return Amp_Bottom;
}

void RFExplorer::setAmp_Bottom(int value)
{
    Amp_Bottom = value;
}

int RFExplorer::getAmp_Top() const
{
    return Amp_Top;
}

void RFExplorer::setAmp_Top(int value)
{
    Amp_Top = value;
}

int RFExplorer::getFreq_step() const
{
    return Freq_step;
}

void RFExplorer::setFreq_step(int value)
{
    Freq_step = value;
}

int RFExplorer::getStart_Freq() const
{
    return Start_Freq;
}

void RFExplorer::setStart_Freq(int value)
{
    Start_Freq = value;
}

int RFExplorer::getThreshold() const
{
    return threshold;
}

void RFExplorer::setThreshold(int value)
{
    threshold = value;
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
        //qDebug() << "data: " <<  data;

        QString stringData = QString(data);
        if (stringData.contains("(C) Ariel Rocholl"))
        {
            if (m_debug) emit log(QString("[serialManager] RF authenticated! Hi Ariel Rocholl."));
        }

        //Configuration params
        if (stringData.contains("#C2-F:"))
        {
            QVector<QString> setupFields = {"Start_Freq", "Freq_step", "Amp_Top", "Amp_Bottom", "Sweep_Steps", "Excp_Module_Active", "Current_Mode", "Min_Freq", "Max_Freq", "Max_Span", "Rbw", "dB_Offset", "Undoccumented"};
            QVector<QString> modeFields = {"Spectrum Analizer", "RF Generator", "Wifi Analizer", "Unknown"};
            QString dataString = stringData.split(":")[1];
            QStringList configList = dataString.split(",");
            qDebug() << "CONFIG: " << dataString;
            if (configList.length() != setupFields.length())
                if (m_debug) emit log(QString("[serialManager.read_data] Received unexpectd number of config values."));

            try
            {
                setStart_Freq(configList[0].toInt());
                setFreq_step(configList[1].toInt());
                setAmp_Top(configList[2].toInt());
                setAmp_Bottom(configList[3].toInt());
                setSweep_Steps(configList[4].toInt());
                setExcp_Module_Active(configList[5].toInt());
                setCurrent_Mode(modeFields[configList[6].toInt()]);
                setMin_Freq(configList[7].toInt());
                setMax_Freq(configList[8].toInt());
                setMax_Span(configList[9].toInt());
                setRbw(configList[10].toInt());
                setDB_Offset(configList[11].toInt());
                setUndoccumented(configList[12].trimmed().toInt());
                //emit signal
            }
            catch (...) {
                if (m_debug) emit log(QString("[serialManager.read_data] Some error occurred."));
            }
        }

        //Serial number
        if (stringData.contains("#Sn"))
        {
            setSerial_number(stringData.split("Sn")[1].trimmed());
            //emit signal
        }

        //Data
        if (stringData.contains("$S"))
        {
            //qDebug() << data;
            QByteArray hexData = data.toHex();
            QByteArray checkedData;
            for (int i = 0; i < hexData.length(); ++i)
            {
                if( (i > 5) && (i < (hexData.length() - 4)))
                    checkedData = checkedData + hexData[i];
            }

            double sweep_start = this->getStart_Freq()/1e3;
            double sweep_step = this->getFreq_step()/1e6;
            double sweep_end = sweep_start + (sweep_step * this->getSweep_Steps() * 0.999999);

            double slope = (this->getAmp_Bottom() - this->getAmp_Top())/255.0;

           QVector<double> powerVector;
           QVector<double> freqsVector;
            for (int i = 0; i < checkedData.length(); i = i + 2)
            {
                QByteArray pairData;
                bool valid;
                pairData.append(checkedData[i]);
                pairData.append(checkedData[i+1]);
                int decValue = pairData.toUInt(&valid,16);
                double powerValue = -20.0 + (slope * decValue);
                powerVector.append(powerValue);
            }

            double frequency = sweep_start - sweep_step;
            for (int i = 0; i < this->getSweep_Steps(); ++i)
            {
                frequency = frequency + sweep_step;
                freqsVector.append(frequency);
            }

            qDebug() << "POWER ARRAY: " << powerVector;
            qDebug() << "FREQ ARRAY: " << freqsVector;
            qDebug() << "ARRAY LENGTHS: " << powerVector.length() << "  " << freqsVector.length();
        }

    }
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------
