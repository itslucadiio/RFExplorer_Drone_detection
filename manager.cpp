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
    serialManager *rf_explorer = new serialManager();
    QThread *rf_explorer_thread = new QThread();
    rf_explorer->moveToThread(rf_explorer_thread);
    QObject::connect(rf_explorer_thread, &QThread::started, rf_explorer, &serialManager::startConnection);
    QObject::connect(rf_explorer, SIGNAL(log(QString)), this, SIGNAL(log(QString)), Qt::DirectConnection);
    QObject::connect(this, SIGNAL(sendMessage(QString)), rf_explorer, SLOT(sendCommand(QString)));
    rf_explorer->initialize(debug);
    rf_explorer_thread->start();
    //---------------------------------

}
//-------------------------------------------------------------------------------------------------------------------------------------------------------


void manager::start_rf()
{
    emit sendMessage("r");          //Reset
    QThread::sleep(20);
    emit sendMessage("C0");         //Request config datas
    QThread::sleep(20);
    emit sendMessage("CH");         //Stop spectrum analizer data dump
}
