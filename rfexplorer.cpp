#include "rfexplorer.h"

RFExplorer::RFExplorer(QObject *parent)
{

}


//-------------------------------------------------------------------------------------------------------------------------------------------------------
void RFExplorer::initialize(QSerialPort* port, bool debug)
{
    serial_port = port;
    m_debug = debug;
    startConnection();

}

void RFExplorer::startConnection()
{
    m_connected = serial_port->open(QIODevice::ReadWrite);
    if(m_connected)
    {
        connect(serial_port, SIGNAL(readyRead()), this, SLOT(read_data()), Qt::DirectConnection);
        //connect(serial_port, SIGNAL(aboutToClose()), this, SLOT(read_data()), Qt::DirectConnection);
        sendCommand("r");          //Reset
        QThread::sleep(10);
        sendCommand("C0");         //Request config datas

    }
}

void RFExplorer::stopConnection()
{
    serial_port->disconnect();

    if (m_connected)
    {
            serial_port->close();
    }

}

void RFExplorer::startSweeper()
{
        sendCommand("r");          //Reset
        QThread::sleep(5);
        sendCommand("C0");         //Request config datas
        QThread::sleep(100);
        sendCommand("CH");         //Stop spectrum analizer data dump

}


void RFExplorer::read_data()
{
    while(serial_port->canReadLine()){
        const QByteArray data = serial_port->readLine();

        QString stringData = QString(data);
        qDebug() << "DATA: " << stringData;

        if (stringData.contains("(C) Ariel Rocholl"))
        {
            if (m_debug) emit log(QString("[serialManager] RF authenticated! Hi Ariel Rocholl."));
        }
        //Module info
        if (stringData.contains("#C2-M:"))
        {
            QString dataString = stringData.split(":")[1];
            QStringList modulelist = dataString.split(",");
            m_model = modulelist[0];
            m_exp_module = modulelist[1];
            m_fw_version = modulelist[2];

            emit new_module_info();
            //qDebug() << "MODULE: " << dataString;
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
                setThreshold(abs((getAmp_Top()-getAmp_Bottom())/2)+getAmp_Bottom());
                setSweep_Steps(configList[4].toInt());
                setExcp_Module_Active(configList[5].toInt());
                setCurrent_Mode(modeFields[configList[6].toInt()]);
                setMin_Freq(configList[7].toInt());
                setMax_Freq(configList[8].toInt());
                setMax_Span(configList[9].toInt());
                setRbw(configList[10].toInt());
                setDB_Offset(configList[11].toInt());
                setUndoccumented(configList[12].trimmed().toInt());
                setEnd_Freq(getStart_Freq()+((getSweep_Steps()* getFreq_step())/1000));

                //emit signal with config
                emit new_config(getStart_Freq(), getSweep_Steps(), getFreq_step(), getThreshold(), getSerial_number());
                if (m_debug) emit log(QString("[serialManager.read_data] New Config received."));

            }
            catch (...) {
                if (m_debug) emit log(QString("[serialManager.read_data] Some error occurred."));
            }
        }

        //Serial number
        if (stringData.contains("#Sn"))
        {
            setSerial_number(stringData.split("Sn")[1].trimmed());
            //emit signal with config
            emit new_serial(getSerial_number());
            if (m_debug) emit log(QString("[serialManager.read_data] New Serial Number received."));
        }

        //Data
        if (stringData.contains("$S"))
        {
            QByteArray hexData = data.toHex();
            QByteArray checkedData;
            for (int i = 0; i < hexData.length(); ++i)
            {
                if( (i > 5) && (i < (hexData.length() - 4)))
                    checkedData = checkedData + hexData[i];
            }
            //qDebug() << checkedData;

            double sweep_start = this->getStart_Freq()/1e3;
            double sweep_step = this->getFreq_step()/1e6;

            //POWERS DATA BLOCK
            //----------------------------------------------------------------------------
            QVector<int> thresholdCounter;
            QVector<float> highPower;
            QVector<float> _powerVector;
            for (int i = 0; i < checkedData.length(); i = i + 2)
            {
                QByteArray pairData;
                bool valid;
                pairData.append(checkedData[i]);
                pairData.append(checkedData[i+1]);
                int decValue = pairData.toInt(&valid,16);
                //double powerValue = this->getAmp_Bottom() + (slope * decValue);
                float powerValue = -(decValue/2.0);
                _powerVector.append(powerValue);

                //Detection of high signals (above threshold)
                if (powerValue >= this->getThreshold()){  //Look for db above threshold
                    thresholdCounter.append(i);
                    highPower.append(powerValue);
                }
            }
            m_powerVector = _powerVector;


            //FREQUENCIES DATA BLOCK
            //----------------------------------------------------------------------------
            double frequency = sweep_start - sweep_step;
            QVector<double> highFreq; //Vector de frequencies que han saltat
            QVector<double> _freqsVector; //Vector de frequencies del sweep
            for (int i = 0; i < this->getSweep_Steps(); ++i)
            {
                frequency = frequency + sweep_step;
                _freqsVector.append(frequency);

                //Detection of high signals (above threshold)
                if (thresholdCounter.contains(i))    //Look for corresponding frequency position
                    highFreq.append(frequency);
            }
            m_freqsVector = _freqsVector; //Assign new values at end (hopping no mutex needed)


            //DETECTIONS DATA BLOCK
            //----------------------------------------------------------------------------
            QVector<Detection> _detections;
            _detections = m_detections; //Cloning to manage data

            for (int j = 0; j < highFreq.length(); ++j) {
                bool found = false;
                if(_detections.length() == 0)
                {
                    _detections.append({highFreq[j], highPower[j], 0});
                }
                else
                {
                    for (int i = 0; i < _detections.length(); ++i) {
                        if(_detections[i].freq == highFreq[j])
                            found = true;
                    }
                    if(!found)
                        _detections.append({highFreq[j],highPower[j],0});
                }
            }

            for (int i = 0; i < _detections.length(); ++i) {
                qDebug() << "ANALYSING THE VALUE: " << _detections[i].freq << "WITH : "<< _detections[i].power << " dBm WITH m_detections: " << _detections[i].counter;
                if(highFreq.contains(_detections[i].freq))
                {
                    _detections[i].counter++;
                }
                else
                {
                    if(_detections[i].counter > 2)
                    {
                        _detections[i].counter = _detections[i].counter / 2;
                    }
                    else
                    {
                        _detections.remove(i);
                    }
                }
            }

            m_detections = _detections; //Set new values

        }
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------
void RFExplorer::sendCommand(QString msg)
{
    QString array;
    array.append("#");
    array.append(char(msg.length() + 2));
    array.append(msg);
    QByteArray data = array.toUtf8();

    send_data(data);
}

void RFExplorer::send_data(QByteArray data)
{
    if (serial_port->isOpen())
    {
        serial_port->write(data);
        serial_port->waitForBytesWritten(500);
        if (m_debug) emit log(QString("[serialManager.send_data] Data sent!"));
    }
}

void RFExplorer::send_config(double start_freq, double end_freq)
{
    //Change config request of RF Explorer device
    QString _start_freq = QString::number(start_freq,'f',0);
    QString _end_freq = QString::number(end_freq,'f',0);

    if((start_freq>4800000)&(start_freq<6000000)
       &(m_model=="006")&(Excp_Module_Active==1))
    {
        QString switchCmd;
        switchCmd.append("CM");
        switchCmd.append(char(0x00));
        sendCommand(switchCmd);
        QThread::sleep(3);
    }
    if((start_freq>15000)&(start_freq<2700000)
       &(Excp_Module_Active==0))
    {
        QString switchCmd;
        switchCmd.append("CM");
        switchCmd.append(char(0x01));
        sendCommand(switchCmd);
        QThread::sleep(3);
    }


    QString data;
    data.append("C2-F:");

    if (_start_freq.length()<7)
    {
        std::string sta = _start_freq.toStdString();
        int diff_0 = 7-_start_freq.length();
        sta.insert(0,diff_0,'0');
        _start_freq= QString::fromStdString(sta);
    }

    if (_end_freq.length()<7)
    {
        std::string str = _end_freq.toStdString();
        int diff_0 = 7-_end_freq.length();
        str.insert(0,diff_0,'0');
        _end_freq= QString::fromStdString(str);
    }

    data.append(_start_freq);
    data.append(",");
    data.append(_end_freq);
    data.append(",");
    data.append(QString(getAmp_Top()));
    data.append(",");
    data.append(QString(getAmp_Bottom()));
    sendCommand(data);

    QThread::sleep(4);
    sendCommand("C0");


}

void RFExplorer::edit_threshold(int threshold)
{
    setThreshold(threshold);
}




//------------------------------------------------------------------------------------------------------------------------------------
//GETTERS AND SETTERS
//------------------------------------------------------------------------------------------------------------------------------------

QVector<float> RFExplorer::getPowerVector()
{
    return m_powerVector;
}

QVector<double> RFExplorer::getFreqsVector()
{
    return m_freqsVector;
}

QVector<Detection> RFExplorer::getDetections()
{
    return m_detections;
}

QString RFExplorer::getModel()
{
    return m_model;
}


QString RFExplorer::getExpModule()
{
    return m_exp_module;
}

QString RFExplorer::getFwVer()
{
    return m_fw_version;
}

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

int RFExplorer::getEnd_Freq() const
{
    return End_Freq;
}

void RFExplorer::setEnd_Freq(int value)
{
    End_Freq = value;
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
