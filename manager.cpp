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
    QObject::connect(serialer, SIGNAL(newRFExplorer(RFExplorer*)), m_ui, SLOT(newRFExplorer(RFExplorer*)), Qt::DirectConnection);



    //QObject::connect(this, SIGNAL(sendMessage(QString)), serialer, SLOT(sendCommand(QString)));
    //QObject::connect(this, SIGNAL(send_config(double,double)), serialer, SLOT(sendConfig(double,double)), Qt::DirectConnection);
    //QObject::connect(this, SIGNAL(edit_threhold(int)), serialer, SLOT(editThrehold(int)), Qt::DirectConnection);

    //SIGNALS TO RECEIVE DATA FROM RF EXPLORER
    //QObject::connect(serialer, SIGNAL(new_config(double,double)), this, SIGNAL(new_config(double,double)), Qt::DirectConnection);
    //QObject::connect(serialer, SIGNAL(new_serial(QString)), this, SIGNAL(new_serial(QString)), Qt::DirectConnection);
    //QObject::connect(serialer, SIGNAL(powers_freqs(QVector<float>,QVector<double>)), this, SIGNAL(powers_freqs(QVector<float>,QVector<double>)), Qt::DirectConnection);
    //QObject::connect(serialer, SIGNAL(active_detections(QVector<Detection>)), this, SIGNAL(active_detections(QVector<Detection>)), Qt::DirectConnection);

    //Note: Detection struct -> import from "utils.h"
    serialer_thread->start();
    serialer->initialize(debug);
    //---------------------------------

}
