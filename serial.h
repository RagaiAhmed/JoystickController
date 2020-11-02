#ifndef SERIALCONTROL_H
#define SERIALCONTROL_H

#include <QtSerialPort/QSerialPort> // For Serial UART
#include <QtSerialPort/QSerialPortInfo> // For getting port info
#include <ui_controller.h>

#include <QComboBox>
#include<QObject>
class SerialControl : public QObject
{
    Q_OBJECT
public:
    SerialControl(QComboBox *portList);
    ~SerialControl();

    void updatePorts();

    void close();

    void connect_port(QString port);

    void execute_events();

public slots:
    void sendUART(QByteArray);

signals:
    void receiveUART(QByteArray);


private:
    QSerialPort *serial = NULL;  // The in use serial port
    QComboBox *portList = NULL;  // The list ui object

};

#endif // SERIALCONTROL_H
