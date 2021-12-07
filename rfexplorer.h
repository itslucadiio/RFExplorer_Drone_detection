#ifndef RFEXPLORER_H
#define RFEXPLORER_H

#include <QObject>
#include <QSerialPort>
#include <QDebug>

class RFExplorer : public QObject
{
    Q_OBJECT
public:
    explicit RFExplorer(QObject *parent = nullptr);

    void initialize(QSerialPort* port, bool debug);

    QString getStart_Freq() const;
    void setStart_Freq(const QString &value);

    QString getFreq_step() const;
    void setFreq_step(const QString &value);

    QString getAmp_Top() const;
    void setAmp_Top(const QString &value);

    QString getAmp_Bottom() const;
    void setAmp_Bottom(const QString &value);

    QString getSweep_Steps() const;
    void setSweep_Steps(const QString &value);

    QString getExcp_Module_Active() const;
    void setExcp_Module_Active(const QString &value);

    QString getCurrent_Mode() const;
    void setCurrent_Mode(const QString &value);

    QString getMin_Freq() const;
    void setMin_Freq(const QString &value);

    QString getMax_Freq() const;
    void setMax_Freq(const QString &value);

    QString getMax_Span() const;
    void setMax_Span(const QString &value);

    QString getRbw() const;
    void setRbw(const QString &value);

    QString getDB_Offset() const;
    void setDB_Offset(const QString &value);

    QString getUndoccumented() const;
    void setUndoccumented(const QString &value);

    QString getPort_name() const;
    void setPort_name(const QString &value);

    QString getSerial_number() const;
    void setSerial_number(const QString &value);

signals:
    void log(const QString& text); // Write message to log

public slots:
    void send_data(QByteArray data);
    void read_data();

private:

    QSerialPort* serial_port;

    QString port_name = "";
    QString serial_number = "";

    QString Start_Freq = "";
    QString Freq_step = "";
    QString Amp_Top = "";
    QString Amp_Bottom = "";
    QString Sweep_Steps = "";
    QString Excp_Module_Active = "";
    QString Current_Mode = "";
    QString Min_Freq = "";
    QString Max_Freq = "";
    QString Max_Span = "";
    QString Rbw = "";
    QString dB_Offset = "";
    QString Undoccumented = "";

    bool m_debug;

};

#endif // RFEXPLORER_H
