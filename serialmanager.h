#ifndef SERIALMANAGER_H
#define SERIALMANAGER_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QByteArray>
#include <math.h>
#include <tgmath.h>
#include <QDebug>

#include "rfexplorer.h"
#include "utils.h"


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
        void new_config(double start_freq, double end_freq);
        void new_serial(QString serial);
        void powers_freqs(QVector<float> powerVector, QVector<double> freqsVector);
        void active_detections(QVector<Detection> detections);

    private:
        QVector<QSerialPort*> rf;

        static const quint16 rf_EXPLORER = 60000;

        //Debug flag
        bool m_debug;

        void decode_sweep(QByteArray data);

        QVector<RFExplorer*> m_devices;

};

#endif // SERIALMANAGER_H
