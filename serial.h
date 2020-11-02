#ifndef SERIALCONTROL_H
#define SERIALCONTROL_H

#include <QtSerialPort/QSerialPort> // For Serial UART
#include <QtSerialPort/QSerialPortInfo> // For getting port info

#include <QComboBox>
#include<QObject>
class SerialControl : public QObject
{
    Q_OBJECT
public:
    SerialControl(QComboBox *);
    ~SerialControl();

    // Update the available ports list
    void updatePorts();

    // Closes the current serial port
    void close();

    // Connects to a serial port given it's name
    void connect_port(QString port);

    // When called, receives data from UART
    void execute_events();

public slots:
    // Sends data over UART
    void sendUART(QByteArray);

signals:
    // Emitted when received data over UART
    void receiveUART(QByteArray);


private:
    QSerialPort *serial = NULL;  // The in use serial port
    QComboBox *portList = NULL;  // The list ui object

};

#endif // SERIALCONTROL_H
