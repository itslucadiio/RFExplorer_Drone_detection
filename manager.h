#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include <QThread>

#include "serialmanager.h"
#include "utils.h"

class manager : public QObject
{
    Q_OBJECT
public:
    explicit manager(QObject *parent = nullptr);
    void start_rf();

public slots:
    void initialize(QVariantMap* parameters, bool debug);
    void sendConfig(double start_freq, double end_freq);
    void editThrehold(int threshold);

signals:
    //Write a message to log
    void log(const QString& text);
    void sendMessage(QString msg);
    void send_config(double start_freq, double end_freq);
    void edit_threhold(int threshold);

    void new_config(double start_freq, double end_freq);
    void new_serial(QString serial);
    void powers_freqs(QVector<float> powerVector, QVector<double> freqsVector);
    void active_detections(QVector<Detection> detections);

private:
   //Debug flag
   bool m_debug;

   //Session parameters
   QVariantMap* m_parameters;

};

#endif // MANAGER_H
