#include "manager.h"

manager::manager(QObject *parent) : QObject(parent)
{

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------
void manager::initialize(QVariantMap* parameters, bool debug)
{
    m_debug = debug;
    m_parameters = parameters;

    //SERIAL---------------------------
    serialManager *serialer = new serialManager();
    QThread *serialer_thread = new QThread();
    serialer->moveToThread(serialer_thread);
    QObject::connect(serialer_thread, &QThread::started, serialer, &serialManager::startConnection);
    QObject::connect(serialer, SIGNAL(log(QString)), this, SIGNAL(log(QString)), Qt::DirectConnection);
    QObject::connect(this, SIGNAL(sendMessage(QString)), serialer, SLOT(sendCommand(QString)));
    QObject::connect(this, SIGNAL(send_config(double,double)), serialer, SLOT(send_config(double,double)), Qt::DirectConnection);
    QObject::connect(this, SIGNAL(edit_threhold(int)), serialer, SLOT(edit_threshold(int)), Qt::DirectConnection);
    //SIGNALS TO RECEIVE DATA FROM RF EXPLORER
    QObject::connect(serialer, SIGNAL(new_config(double,double)), this, SIGNAL(new_config(double,double)), Qt::DirectConnection);
    QObject::connect(serialer, SIGNAL(new_serial(QString)), this, SIGNAL(new_serial(QString)), Qt::DirectConnection);
    QObject::connect(serialer, SIGNAL(powers_freqs(QVector<float>,QVector<double>)), this, SIGNAL(powers_freqs(QVector<float>,QVector<double>)), Qt::DirectConnection);
    QObject::connect(serialer, SIGNAL(active_detections(QVector<Detection>)), this, SIGNAL(active_detections(QVector<Detection>)), Qt::DirectConnection);
    serialer->initialize(debug);
    serialer_thread->start();
    //---------------------------------

}
//-------------------------------------------------------------------------------------------------------------------------------------------------------


void manager::start_rf()
{
    emit sendMessage("r");          //Reset
    QThread::sleep(10);
    emit sendMessage("C0");         //Request config datas
    QThread::sleep(100);
    emit sendMessage("CH");         //Stop spectrum analizer data dump
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------
//SLOT TO SEND CONFIG FROM FRONTEND TO RF EXPLORER
void manager::sendConfig(double start_freq, double end_freq)
{
    emit send_config(start_freq, end_freq);
}
//SLOT TO SEND THRESHOLD FROM FRONTEND TO RF EXPLORER
void manager::editThrehold(int threshold)
{
    emit edit_threhold(threshold);
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------
