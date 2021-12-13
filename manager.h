#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include <QThread>

#include "mainwindow.h"
#include "serialmanager.h"
#include "utils.h"

class manager : public QObject
{
    Q_OBJECT
public:
    explicit manager(QObject *parent = nullptr);
    //void start_rf();

public slots:
    void initialize(QVariantMap* parameters, bool debug);
    //void sendConfig(double start_freq, double end_freq);
    //void editThrehold(int threshold);

signals:
    //Write a message to log
    void log(const QString& text);
    void newRFExplorer(RFExplorer* device);

    //void sendMessage(QString msg);

    //void new_config(double start_freq, double end_freq);
    //void new_serial(QString serial);
    //void powers_freqs(QVector<float> powerVector, QVector<double> freqsVector);
    //void active_detections(QVector<Detection> detections);

private:


   //Debug flag
   bool m_debug;

   //User interface
   MainWindow* m_ui;

   //Session parameters
   QVariantMap* m_parameters;

};

#endif // MANAGER_H
