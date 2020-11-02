#include "serial.h"
#include <QDebug>  // For debugging


SerialControl::SerialControl(QComboBox *portList)
{
    // Stores parent ui pointer
    this->portList = portList;

    // Updates ports list
    updatePorts();
}

void SerialControl::updatePorts()
{
    // Clears old items
    portList->clear();

    // Puts empty port
    portList->addItem("Port");

    // Puts ports in the list
    Q_FOREACH(QSerialPortInfo port, QSerialPortInfo::availablePorts())
        portList->addItem(port.portName());

    qDebug()<<"Updated available ports!";

}

SerialControl::~SerialControl()
{
    // Closing the port
    if(serial && serial->isOpen())
    {
        qDebug()<<"Closed port: "<<serial->portName();
        serial->close();
        delete serial;
    }
}


void SerialControl::close()
{
    // Closing old port
    if(serial && serial->isOpen())
    {
        qDebug()<<"Closed port: "<<serial->portName();
        serial->close();
        delete serial;
        serial = NULL;
    }
}

void SerialControl::connect_port(QString port)
{
    // Configuring new port
    serial = new QSerialPort;
    serial->setPortName(port);
    if(serial->open(QSerialPort::ReadWrite))  // If opened successfully
    {
        // Arduino specific configuration
        serial->setBaudRate(QSerialPort::Baud9600);
        serial->setDataBits(QSerialPort::Data8);
        serial->setParity(QSerialPort::NoParity);
        serial->setStopBits(QSerialPort::OneStop);
        serial->setFlowControl(QSerialPort::NoFlowControl);

        qDebug()<<"Opened port: "<<serial->portName();
    }
    else  // If an error occurred
    {
        delete serial;
        serial = NULL;
        qDebug()<<"Couldn't open port: "<<port;
    }
}

void SerialControl::sendUART(QByteArray str)
{
    if (serial) serial->write(str+"\n");

    qDebug(str);
}


// TODO make it receive QStrings space terminated
void SerialControl::execute_events()
{
    QByteArray cmd;
    while(serial && serial->bytesAvailable())  // If there is data in the stream
    {
        cmd = serial->readLine();  // Read a line
        emit(receiveUART(cmd));  // Send the line as command
    }

}

