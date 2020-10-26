#include "controller.h"
#include "ui_controller.h"
#include <QIntValidator>
#include <QDebug>
#include <SDL.h>
#include <SDL_gamecontroller.h>
#include <SDL_joystick.h>


void updatePorts(QComboBox *portList)
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


Controller::Controller(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Controller)
{
    ui->setupUi(this);

    // Validator for text input
    QIntValidator *limiter = new QIntValidator(0,1000);
    ui->HLineEdit->setValidator(limiter);
    ui->VLineEdit->setValidator(limiter);

    // Updates ports list
    updatePorts(ui->portList);

    SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER);

    /* Open the first available controller. */
    qDebug()<<SDL_NumJoysticks();
    for (int i = 0; i < SDL_NumJoysticks(); ++i) {
                qDebug()<<SDL_JoystickNameForIndex(i);
    }
}

Controller::~Controller()
{
    // Closing the port
    if(serial && serial->isOpen())
    {
        qDebug()<<"Closed port: "<<serial->portName();
        serial->close();
        delete serial;
    }

    delete ui;
}


void Controller::on_portList_activated(const QString &arg1)
{
    // Closing old port
    if(serial && serial->isOpen())
    {
        qDebug()<<"Closed port: "<<serial->portName();
        serial->close();
        delete serial;
        serial = NULL;
    }

    // If item "Port" is chosen then update the available ports and return
    if(arg1=="Port")
    {
        // Updates ports list
        updatePorts(ui->portList);
        return;
    }

    // Configuring new port
    serial = new QSerialPort;
    serial->setPortName(arg1);
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
        qDebug()<<"Couldn't open port: "<<arg1;
    }

}
