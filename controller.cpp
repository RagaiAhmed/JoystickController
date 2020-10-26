#include "controller.h"
#include "ui_controller.h"

#include <QDebug>  // For debugging
#include <QIntValidator>  // For validating text input
#include <SDL.h>  // SDL2 header
#include <SDL_joystick.h>  // For joystick
#include <QtSerialPort/QSerialPort> // For Serial UART
#include <QtSerialPort/QSerialPortInfo> // For getting port info

SDL_Joystick *joy = NULL;  // The in use joystick
QSerialPort *serial = NULL;  // The in use serial port


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

void updateControllers(QComboBox * controllerList)
{
    // Clears old items
    controllerList->clear();

    // Puts empty contoller
    controllerList->addItem("Controller");

    // Puts Controller in the list
    for(int i=0;i<SDL_NumJoysticks();++i)
        controllerList->addItem(SDL_JoystickNameForIndex(i));

    qDebug()<<"Updated available controllers!";
}


Controller::Controller(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Controller)
{
    ui->setupUi(this);

    // Starts SDL subsystem
    SDL_InitSubSystem(SDL_INIT_JOYSTICK);

    // Validator for text input
    QIntValidator *limiter = new QIntValidator(0,1000);
    ui->HLineEdit->setValidator(limiter);
    ui->VLineEdit->setValidator(limiter);

    // Updates ports list
    updatePorts(ui->portList);

    // Updates controller list
    updateControllers(ui->controllerList);
}

Controller::~Controller()
{
    // Closing SDL subsystem
    SDL_Quit();

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

void Controller::on_controllerList_activated(int index)
{
    // Deattaching old controller
    if(joy && SDL_JoystickGetAttached(joy))
    {
        qDebug()<<"Deattaching old controller";
        SDL_JoystickClose(joy);
        joy=NULL;
    }

    // If item "Controller" is chosen then update the available ports and return
    if(index==0)
    {
        // Updates ports list
        updateControllers(ui->controllerList);
        return;
    }

    // Attach new Controller
    joy = SDL_JoystickOpen(index-1);
    if(joy)  // If opened successfully
    {
        qDebug()<<"Attached controller: "<<SDL_JoystickNameForIndex(index-1);
        qDebug()<<"Number of Axes: "<<SDL_JoystickNumAxes(joy);
        qDebug()<<"Number of Buttons: "<<SDL_JoystickNumButtons(joy);
        qDebug()<<"Number of Balls: "<<SDL_JoystickNumBalls(joy);
        qDebug()<<"Number of Hats: "<<SDL_JoystickNumHats(joy);
    }
    else
    {
        joy=NULL;
        qDebug()<<"Couldn't attach Joystick";
    }
}
