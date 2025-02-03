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

    QByteArray getRF1JSONData();
    QByteArray getRF2JSONData();
    QByteArray getRFJSONStatus();
    bool changeRF1Threshold(QJsonObject json);
    bool changeRF2Threshold(QJsonObject json);
    bool restoreRF1MaxPowers();
    bool restoreRF2MaxPowers();
    bool changeRF1RightFreq(QJsonObject json);
    bool changeRF1LeftFreq(QJsonObject json);
    bool changeRF1Freq24();
    bool changeRF1Freq58();
    bool changeRF2RightFreq(QJsonObject json);
    bool changeRF2LeftFreq(QJsonObject json);
    bool changeRF2Freq24();
    bool changeRF2Freq58();

public slots:
    void initialize(QVariantMap* parameters, bool debug);

signals:
    //Write a message to log
    void log(const QString& text);
    void newRFExplorer(RFExplorer* device);

private:


   //Debug flag
   bool m_debug;

   //User interface
   MainWindow* m_ui;

   //Session parameters
   QVariantMap* m_parameters;

};

#endif // MANAGER_H
