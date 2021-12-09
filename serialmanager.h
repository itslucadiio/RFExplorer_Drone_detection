#ifndef SERIALMANAGER_H
#define SERIALMANAGER_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QByteArray>
#include <math.h>
#include <tgmath.h>
#include "rfexplorer.h"
#include <QDebug>


class serialManager : public QObject
{
    Q_OBJECT

    public:
        serialManager(QObject* parent=nullptr);

    public slots:
        void startConnection();
        void sendCommand(QString msg);
        void initialize(bool debug);
        void sendConfig(double start_freq, double end_freq);
        void editThrehold(int threshold);

    signals:
        //Write a message to log
        void log(const QString& text);
        void send_data(QByteArray data);
        void send_config(double start_freq, double end_freq);
        void edit_threhold(int threshold);

    private:
        QVector<QSerialPort*> rf;

        static const quint16 rf_EXPLORER = 60000;

        //Debug flag
        bool m_debug;

        void decode_sweep(QByteArray data);

        QVector<RFExplorer*> m_devices;

};

#endif // SERIALMANAGER_H
