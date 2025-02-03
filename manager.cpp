#include "manager.h"
#include "mainwindow.h"

manager::manager(QObject *parent) : QObject(parent)
{

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------
void manager::initialize(QVariantMap* parameters, bool debug)
{
    m_debug = debug;
    m_parameters = parameters;

    m_ui = new MainWindow();
    m_ui->show();

    //SERIAL---------------------------
    serialManager *serialer = new serialManager();
    QThread *serialer_thread = new QThread();
    serialer->moveToThread(serialer_thread);

    QObject::connect(serialer_thread, &QThread::started, serialer, &serialManager::startConnection);
    QObject::connect(serialer, SIGNAL(log(QString)), this, SIGNAL(log(QString)), Qt::DirectConnection);
    QObject::connect(serialer, SIGNAL(newRF1Explorer(RFExplorer*)), m_ui, SLOT(newRF1Explorer(RFExplorer*)), Qt::DirectConnection);
    QObject::connect(serialer, SIGNAL(newRF2Explorer(RFExplorer*)), m_ui, SLOT(newRF2Explorer(RFExplorer*)), Qt::DirectConnection);

    //Note: Detection struct -> import from "utils.h"
    serialer_thread->start();
    serialer->initialize(debug);
    //---------------------------------

}


QByteArray manager::getRF1JSONData()
{
    QJsonObject data = m_ui->getRF1JSONChartdata();
    QJsonDocument doc(data);
    return doc.toJson();
}

QByteArray manager::getRF2JSONData()
{
    QJsonObject data = m_ui->getRF2JSONChartdata();
    QJsonDocument doc(data);
    return doc.toJson();
}

QByteArray manager::getRFJSONStatus()
{
    QJsonObject data = m_ui->status();
    QJsonDocument doc(data);
    return doc.toJson();
}

bool manager::changeRF1Threshold(QJsonObject json)
{
    bool response = m_ui->changeRF1Threshold(json);
    return response;
}

bool manager::changeRF2Threshold(QJsonObject json)
{
    bool response = m_ui->changeRF2Threshold(json);
    return response;
}

bool manager::restoreRF1MaxPowers()
{
    bool response = m_ui->restoreRF1MaxPowers();
    return response;
}

bool manager::restoreRF2MaxPowers()
{
    bool response = m_ui->restoreRF2MaxPowers();
    return response;
}

bool manager::changeRF1RightFreq(QJsonObject json)
{
    bool response = m_ui->changeRF1RightFrequency(json);
    return response;
}

bool manager::changeRF1LeftFreq(QJsonObject json)
{
    bool response = m_ui->changeRF1LeftFrequency(json);
    return response;
}

bool manager::changeRF1Freq24()
{
    bool response = m_ui->changeRF1Freq24();
    return response;
}

bool manager::changeRF1Freq58()
{
    bool response = m_ui->changeRF1Freq58();
    return response;
}

bool manager::changeRF2RightFreq(QJsonObject json)
{
    bool response = m_ui->changeRF2RightFrequency(json);
    return response;
}

bool manager::changeRF2LeftFreq(QJsonObject json)
{
    bool response = m_ui->changeRF2LeftFrequency(json);
    return response;
}

bool manager::changeRF2Freq24()
{
    bool response = m_ui->changeRF2Freq24();
    return response;
}

bool manager::changeRF2Freq58()
{
    bool response = m_ui->changeRF2Freq58();
    return response;
}
