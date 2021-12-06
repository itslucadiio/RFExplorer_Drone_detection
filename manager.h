#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include <QThread>

#include "serialmanager.h"

class manager : public QObject
{
    Q_OBJECT
public:
    explicit manager(QObject *parent = nullptr);
    void start_rf();

public slots:
    void initialize(QVariantMap* parameters, bool debug);

signals:
    //Write a message to log
    void log(const QString& text);

    void sendMessage(QString msg);

private:
   //Debug flag
   bool m_debug;

   //Session parameters
   QVariantMap* m_parameters;

};

#endif // MANAGER_H
