#include <QThread>
#include <QDateTime>
#include <QCommandLineParser>
#include <QThread>
#include <QLockFile>
#include <QDateTime>
#include <QFile>
#include <QApplication>
#include <QtHttpServer>

#include "manager.h"
#include "mainwindow.h"

bool m_debug;
QVariantMap* parameters;
manager* super;
QThread* super_manager;
QHttpServer httpServer;     //Web Frontend and API

void deinitialize();
void initialize(QVariantMap* parameters, bool debug);
void loadParameters(QVariantMap* parameters);
void createParametersDefaults(QVariantMap* _parameters);
void saveParameters(QVariantMap* _parameters);
void loadParametersFromFile(const QString& fileName, QVariantMap* parameters);
void setNewParameters(QVariantMap* _parameters);
void writeToLog(const QString& text);
int createWebFrontend();

QString logfile_path = "/var/log/asdt/s-link-rf.log";

//-----------------------------------------------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    //QGuiApplication app(argc, argv);

    //If detected debug compilation, we set also debug mode output
    #ifdef QT_DEBUG
        m_debug = true;
    #endif

    createWebFrontend();

    // Configuration management
    parameters = new QVariantMap;

//    //Load parameters from ini file
//    loadParameters(parameters); //no parameters needed at the moment

    //Initialize
    initialize(parameters, m_debug);

    return a.exec();
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
int createWebFrontend()
{
    // WEB UI FILE DEPLOY BLOCK -------------------------------------------------------------------------------------------
    httpServer.route("/",[](QHttpServerResponder &&responder)
    {
        QByteArray _data;
        QHttpServerResponder::StatusCode status;
        QFile file(":/html/index.html");
        if(!file.open(QFile::ReadOnly | QFile::Text))
        {   _data = "OOPS! What are you looking for? This is not for you eyes! Let me report.. ;).";
            status = QHttpServerResponder::StatusCode(404);

        }
        else
        {   _data = file.readAll();
            status = QHttpServerResponder::StatusCode(200);
        }
        responder.write(_data,"text/html");
        file.close();

    });
    httpServer.route("/","GET",[](QString filename, QHttpServerResponder &&responder)
    {
        QByteArray _data;
        QHttpServerResponder::StatusCode status;
        QString new_filename;

        if (filename=="")
            new_filename = ":/html/index.html";

        QFile file(new_filename);
        if(!file.open(QFile::ReadOnly | QFile::Text))
        {   _data = "OOPS! What are you looking for? This is not for you eyes! Let me report.. ;).";
            status = QHttpServerResponder::StatusCode(404);

        }
        else
        {   _data = file.readAll();
            status = QHttpServerResponder::StatusCode(200);
        }
        responder.write(_data,"text/html");
        file.close();
    });

    httpServer.route("/css/","GET",[](const QString filename,QHttpServerResponder &&responder)
    {
        QByteArray _data;
        QHttpServerResponder::StatusCode status;

        QFile file(":/css/"+filename);
        if(!file.open(QFile::ReadOnly | QFile::Text))
        {
            _data = "OOPS! What are you looking for? This is not for you eyes! Let me report.. ;).";
            status = QHttpServerResponder::StatusCode(404);}
        else
        {
            _data = file.readAll();
            status = QHttpServerResponder::StatusCode(200);
        }
            responder.write(_data,"text/css",status);
        file.close();
    });

    httpServer.route("/img/","GET",[](const QString filename)
    {
        return QHttpServerResponse::fromFile(":img/"+filename);
     });

    httpServer.route("/js/","GET",[](const QString filename)
    {
        return QHttpServerResponse::fromFile(":js/"+filename);
     });
    httpServer.route("/sfx/","GET",[](const QString filename)
    {
        return QHttpServerResponse::fromFile(":sfx/"+filename);
     });
    //---------------------------------------------------------------------------------------------------------------------


    //---------------------------------------------------------------------------------------------------------------------
    httpServer.route("/api/devices","GET", [](const  QHttpServerRequest &request)
    {
        QHostAddress remote = request.remoteAddress();
        QVariantMap headers = request.headers();

        QByteArray data = super->getRFJSONStatus();
        return QHttpServerResponse("application/json",data,QHttpServerResponder::StatusCode(200));
     });
    httpServer.route("/api/charts/rf1","GET", [](const  QHttpServerRequest &request)
    {
        QHostAddress remote = request.remoteAddress();
        QVariantMap headers = request.headers();

        QByteArray data = super->getRF1JSONData();
        return QHttpServerResponse("application/json",data,QHttpServerResponder::StatusCode(200));
     });
    httpServer.route("/api/charts/rf2","GET", [](const  QHttpServerRequest &request)
    {
        QHostAddress remote = request.remoteAddress();
        QVariantMap headers = request.headers();

        QByteArray data = super->getRF2JSONData();
        return QHttpServerResponse("application/json",data,QHttpServerResponder::StatusCode(200));
     });
    httpServer.route("/api/threshold/rf1", "POST", [](const  QHttpServerRequest &request)
    {
        QUrlQuery query= request.query();
        QByteArray body = QByteArray::fromPercentEncoding(request.body());

        //JSON parsing
        QJsonDocument jsonResponse = QJsonDocument::fromJson(body);
        QJsonObject response = jsonResponse.object();
        qDebug() << response;

        if (super->changeRF1Threshold(response) == true)
        {
            QByteArray data = "{\"success\":\"true\"}";
            return QHttpServerResponse("application/json",data,QHttpServerResponder::StatusCode(200));
        }

        QByteArray data = "{\"success\":\"false\"}";
        return QHttpServerResponse("application/json",data,QHttpServerResponder::StatusCode(200));
     });
    httpServer.route("/api/threshold/rf2", "POST", [](const  QHttpServerRequest &request)
    {
        QUrlQuery query= request.query();
        QByteArray body = QByteArray::fromPercentEncoding(request.body());

        //JSON parsing
        QJsonDocument jsonResponse = QJsonDocument::fromJson(body);
        QJsonObject response = jsonResponse.object();
        qDebug() << response;

        if (super->changeRF2Threshold(response) == true)
        {
            QByteArray data = "{\"success\":\"true\"}";
            return QHttpServerResponse("application/json",data,QHttpServerResponder::StatusCode(200));
        }

        QByteArray data = "{\"success\":\"false\"}";
        return QHttpServerResponse("application/json",data,QHttpServerResponder::StatusCode(200));
     });
    httpServer.route("/api/restore/rf1", "POST", [](const  QHttpServerRequest &request)
    {
        QUrlQuery query= request.query();
        QByteArray body = QByteArray::fromPercentEncoding(request.body());

        //JSON parsing
        QJsonDocument jsonResponse = QJsonDocument::fromJson(body);
        QJsonObject response = jsonResponse.object();
        qDebug() << response;

        if (super->restoreRF1MaxPowers() == true)
        {
            QByteArray data = "{\"success\":\"true\"}";
            return QHttpServerResponse("application/json",data,QHttpServerResponder::StatusCode(200));
        }

        QByteArray data = "{\"success\":\"false\"}";
        return QHttpServerResponse("application/json",data,QHttpServerResponder::StatusCode(200));
     });
    httpServer.route("/api/restore/rf2", "POST", [](const  QHttpServerRequest &request)
    {
        QUrlQuery query= request.query();
        QByteArray body = QByteArray::fromPercentEncoding(request.body());

        //JSON parsing
        QJsonDocument jsonResponse = QJsonDocument::fromJson(body);
        QJsonObject response = jsonResponse.object();
        qDebug() << response;

        if (super->restoreRF2MaxPowers() == true)
        {
            QByteArray data = "{\"success\":\"true\"}";
            return QHttpServerResponse("application/json",data,QHttpServerResponder::StatusCode(200));
        }

        QByteArray data = "{\"success\":\"false\"}";
        return QHttpServerResponse("application/json",data,QHttpServerResponder::StatusCode(200));
     });
    httpServer.route("/api/frequency/right/rf1", "POST", [](const  QHttpServerRequest &request)
    {
        QUrlQuery query= request.query();
        QByteArray body = QByteArray::fromPercentEncoding(request.body());

        //JSON parsing
        QJsonDocument jsonResponse = QJsonDocument::fromJson(body);
        QJsonObject response = jsonResponse.object();
        qDebug() << response;

        if (super->changeRF1RightFreq(response) == true)
        {
            QByteArray data = "{\"success\":\"true\"}";
            return QHttpServerResponse("application/json",data,QHttpServerResponder::StatusCode(200));
        }

        QByteArray data = "{\"success\":\"false\"}";
        return QHttpServerResponse("application/json",data,QHttpServerResponder::StatusCode(200));
     });
    httpServer.route("/api/frequency/left/rf1", "POST", [](const  QHttpServerRequest &request)
    {
        QUrlQuery query= request.query();
        QByteArray body = QByteArray::fromPercentEncoding(request.body());

        //JSON parsing
        QJsonDocument jsonResponse = QJsonDocument::fromJson(body);
        QJsonObject response = jsonResponse.object();
        qDebug() << response;

        if (super->changeRF1LeftFreq(response) == true)
        {
            QByteArray data = "{\"success\":\"true\"}";
            return QHttpServerResponse("application/json",data,QHttpServerResponder::StatusCode(200));
        }

        QByteArray data = "{\"success\":\"false\"}";
        return QHttpServerResponse("application/json",data,QHttpServerResponder::StatusCode(200));
     });
    httpServer.route("/api/frequency/right/rf2", "POST", [](const  QHttpServerRequest &request)
    {
        QUrlQuery query= request.query();
        QByteArray body = QByteArray::fromPercentEncoding(request.body());

        //JSON parsing
        QJsonDocument jsonResponse = QJsonDocument::fromJson(body);
        QJsonObject response = jsonResponse.object();
        qDebug() << response;

        if (super->changeRF2RightFreq(response) == true)
        {
            QByteArray data = "{\"success\":\"true\"}";
            return QHttpServerResponse("application/json",data,QHttpServerResponder::StatusCode(200));
        }

        QByteArray data = "{\"success\":\"false\"}";
        return QHttpServerResponse("application/json",data,QHttpServerResponder::StatusCode(200));
     });
    httpServer.route("/api/frequency/left/rf2", "POST", [](const  QHttpServerRequest &request)
    {
        QUrlQuery query= request.query();
        QByteArray body = QByteArray::fromPercentEncoding(request.body());

        //JSON parsing
        QJsonDocument jsonResponse = QJsonDocument::fromJson(body);
        QJsonObject response = jsonResponse.object();
        qDebug() << response;

        if (super->changeRF2LeftFreq(response) == true)
        {
            QByteArray data = "{\"success\":\"true\"}";
            return QHttpServerResponse("application/json",data,QHttpServerResponder::StatusCode(200));
        }

        QByteArray data = "{\"success\":\"false\"}";
        return QHttpServerResponse("application/json",data,QHttpServerResponder::StatusCode(200));
     });
    httpServer.route("/api/frequency/24/rf1", "POST", [](const  QHttpServerRequest &request)
    {
        QUrlQuery query= request.query();
        QByteArray body = QByteArray::fromPercentEncoding(request.body());

        //JSON parsing
        QJsonDocument jsonResponse = QJsonDocument::fromJson(body);
        QJsonObject response = jsonResponse.object();
        qDebug() << response;

        if (super->changeRF1Freq24() == true)
        {
            QByteArray data = "{\"success\":\"true\"}";
            return QHttpServerResponse("application/json",data,QHttpServerResponder::StatusCode(200));
        }

        QByteArray data = "{\"success\":\"false\"}";
        return QHttpServerResponse("application/json",data,QHttpServerResponder::StatusCode(200));
     });
    httpServer.route("/api/frequency/58/rf1", "POST", [](const  QHttpServerRequest &request)
    {
        QUrlQuery query= request.query();
        QByteArray body = QByteArray::fromPercentEncoding(request.body());

        //JSON parsing
        QJsonDocument jsonResponse = QJsonDocument::fromJson(body);
        QJsonObject response = jsonResponse.object();
        qDebug() << response;

        if (super->changeRF1Freq58() == true)
        {
            QByteArray data = "{\"success\":\"true\"}";
            return QHttpServerResponse("application/json",data,QHttpServerResponder::StatusCode(200));
        }

        QByteArray data = "{\"success\":\"false\"}";
        return QHttpServerResponse("application/json",data,QHttpServerResponder::StatusCode(200));
     });
    httpServer.route("/api/frequency/24/rf2", "POST", [](const  QHttpServerRequest &request)
    {
        QUrlQuery query= request.query();
        QByteArray body = QByteArray::fromPercentEncoding(request.body());

        //JSON parsing
        QJsonDocument jsonResponse = QJsonDocument::fromJson(body);
        QJsonObject response = jsonResponse.object();
        qDebug() << response;

        if (super->changeRF2Freq24() == true)
        {
            QByteArray data = "{\"success\":\"true\"}";
            return QHttpServerResponse("application/json",data,QHttpServerResponder::StatusCode(200));
        }

        QByteArray data = "{\"success\":\"false\"}";
        return QHttpServerResponse("application/json",data,QHttpServerResponder::StatusCode(200));
     });
    httpServer.route("/api/frequency/58/rf2", "POST", [](const  QHttpServerRequest &request)
    {
        QUrlQuery query= request.query();
        QByteArray body = QByteArray::fromPercentEncoding(request.body());

        //JSON parsing
        QJsonDocument jsonResponse = QJsonDocument::fromJson(body);
        QJsonObject response = jsonResponse.object();
        qDebug() << response;

        if (super->changeRF2Freq58() == true)
        {
            QByteArray data = "{\"success\":\"true\"}";
            return QHttpServerResponse("application/json",data,QHttpServerResponder::StatusCode(200));
        }

        QByteArray data = "{\"success\":\"false\"}";
        return QHttpServerResponse("application/json",data,QHttpServerResponder::StatusCode(200));
     });
    //---------------------------------------------------------------------------------------------------------------------


    //---------------------------------------------------------------------------------------------------------------------
    const auto port = httpServer.listen(QHostAddress::Any, 80);
    if (!port){
        writeToLog(QString("[ERROR] Failed to open WEB server at port 80"));
        return 0;
    }
    writeToLog(QString("Started WEB UI at http://127.0.0.1:%1/").arg(port));
    //---------------------------------------------------------------------------------------------------------------------

    return 1;
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
            logStream << message << "\n";
            //Qt::endl(logStream);
        }
        outputFile.close();
    }

    QTextStream stdStream(stdout);
    stdStream << message << "\n";
    //(Qt::endl(stdStream);
}
//-----------------------------------------------------------------------------------------------------------------------------------------
