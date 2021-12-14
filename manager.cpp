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
