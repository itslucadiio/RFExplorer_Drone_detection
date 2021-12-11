#ifndef RFEXPLORER_H
#define RFEXPLORER_H

#include <QObject>
#include <QSerialPort>
#include <QDebug>
#include "utils.h"

class RFExplorer : public QObject
{
    Q_OBJECT
public:
    explicit RFExplorer(QObject *parent = nullptr);

    void initialize(QSerialPort* port, bool debug);

    int getStart_Freq() const;
    void setStart_Freq(int value);

    int getFreq_step() const;
    void setFreq_step(int value);

    int getAmp_Top() const;
    void setAmp_Top(int value);

    int getAmp_Bottom() const;
    void setAmp_Bottom(int value);

    int getSweep_Steps() const;
    void setSweep_Steps(int value);

    int getExcp_Module_Active() const;
    void setExcp_Module_Active(int value);

    QString getCurrent_Mode() const;
    void setCurrent_Mode(const QString &value);

    int getMin_Freq() const;
    void setMin_Freq(int value);

    int getMax_Freq() const;
    void setMax_Freq(int value);

    int getMax_Span() const;
    void setMax_Span(int value);

    int getRbw() const;
    void setRbw(int value);

    int getDB_Offset() const;
    void setDB_Offset(int value);

    int getUndoccumented() const;
    void setUndoccumented(int value);

    QString getSerial_number() const;
    void setSerial_number(const QString &value);

    int getThreshold() const;
    void setThreshold(int value);

signals:
    void log(const QString& text); // Write message to log
    void new_config(double start_freq, double end_freq);
    void new_serial(QString serial);
    void powers_freqs(QVector<float> powerVector, QVector<double> freqsVector);
    void active_detections(QVector<Detection> detections);

public slots:
    void send_data(QByteArray data);
    void read_data();
    void send_config(double start_freq, double end_freq);
    void edit_threshold(int threshold);

private:

    QSerialPort* serial_port;

    QString port_name = "";
    QString serial_number = "";

    QVector<Detection> detections;

    int threshold = 0;

    int Start_Freq = 0;
    int Freq_step = 0;
    int Amp_Top = 0;
    int Amp_Bottom = 0;
    int Sweep_Steps = 0;
    int Excp_Module_Active = 0;
    QString Current_Mode = "";
    int Min_Freq = 0;
    int Max_Freq = 0;
    int Max_Span = 0;
    int Rbw = 0;
    int dB_Offset = 0;
    int Undoccumented = 0;

    bool m_debug;

};

#endif // RFEXPLORER_H
