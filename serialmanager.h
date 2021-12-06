#ifndef SERIALMANAGER_H
#define SERIALMANAGER_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QByteArray>
#include <math.h>
#include <tgmath.h>

class serialManager : public QObject
{
    Q_OBJECT

    public:
        serialManager(QObject* parent=nullptr);

    public slots:
        void startConnection();
        void sendCommand(QString msg);
        void send_data(QByteArray data);
        void read_data();
        void initialize(bool debug);

    signals:
        //Write a message to log
        void log(const QString& text);

    private:
        QSerialPort *rf;

        static const quint16 rf_EXPLORER = 60000;
        QString rf_port;
        bool rf_available;

        //Debug flag
        bool m_debug;

};

#endif // SERIALMANAGER_H
