#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QThread>
#include <QDateTime>
#include <QCommandLineParser>
#include <QThread>
#include <QLockFile>
#include <QDateTime>
#include <QFile>

#include "manager.h"


bool m_debug;
QVariantMap* parameters;
manager* super;
QThread* super_manager;

void deinitialize();
void initialize(QVariantMap* parameters, bool debug);
void loadParameters(QVariantMap* parameters);
void createParametersDefaults(QVariantMap* _parameters);
void saveParameters(QVariantMap* _parameters);
void loadParametersFromFile(const QString& fileName, QVariantMap* parameters);
void setNewParameters(QVariantMap* _parameters);
void writeToLog(const QString& text);

QString logfile_path = "/var/log/asdt/s-link-rf.log";

//-----------------------------------------------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    //If detected debug compilation, we set also debug mode output
    #ifdef QT_DEBUG
        m_debug = true;
    #endif

    // Configuration management
    parameters = new QVariantMap;

//    //Load parameters from ini file
//    loadParameters(parameters);

    //Initialize
    initialize(parameters, m_debug);

    //Start RF Explorer
    super->start_rf();

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void deinitialize()
{
    super_manager->quit();
    super->disconnect();
    if (m_debug) writeToLog("[GENERAL] Deinitialized all connections correctly");
}
void initialize(QVariantMap* parameters, bool debug)
{
    super = new manager();
    super_manager = new QThread();
    super->moveToThread(super_manager);
    QObject::connect(super, &manager::log, writeToLog);
    super_manager->start();
    super->initialize(parameters, debug);
}
//-----------------------------------------------------------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------------------------------------------------------
void writeToLog(const QString& text)
{
    QString message = QString("%1 :: %2").arg(QDateTime::currentDateTime().toString("hh:mm:ss.zzz"), text);
    QFile outputFile(logfile_path);
    if (outputFile.open(QFile::Text | QFile::WriteOnly | QIODevice::Append | QIODevice::Unbuffered))
    {
        QTextStream logStream(&outputFile);
        if (logStream.device())
        {
            logStream << message;
            Qt::endl(logStream);
        }
        outputFile.close();
    }

    QTextStream stdStream(stdout);
    stdStream << message;
    Qt::endl(stdStream);
}
//-----------------------------------------------------------------------------------------------------------------------------------------
