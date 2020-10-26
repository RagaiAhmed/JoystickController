#include "controller.h"
#include "ui_controller.h"

#include <QDebug>  // For debugging
#include <QIntValidator>  // For validating text input
#include <SDL.h>  // SDL2 header
#include <SDL_joystick.h>  // For joystick
#include <QtSerialPort/QSerialPort> // For Serial UART
#include <QtSerialPort/QSerialPortInfo> // For getting port info
#include <QTimer> // For make an event polling function executed at equal intervals

SDL_Joystick *joy = NULL;  // The in use joystick
QSerialPort *serial = NULL;  // The in use serial port
QTimer *timer;  // The timer used for the pollJoystick

// Used to convert joystick axis to char to send to arduino
// The first sub is from 0 to 3 (defined axes in the joystick, usually x,y,z,Rx in order)
// The second sub is negative then positive
char axis_to_dir[4][2]= {{'L','R'},{'B','F'},{'D','U'},{'<','>'}};

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

void Controller::pollJoystick()
{
    SDL_Event event;

    // If any events are in queue (non blocking)
    while(SDL_PollEvent(&event))
    {
        // Check the event type
        switch(event.type)
        {
            case SDL_JOYAXISMOTION:  //A change in joystick axis
                if(event.jaxis.axis<4&&abs(event.jaxis.value))  // If it is one of the first 4 axes
                {
                    QByteArray str;  // To store the string data to be sent

                    // A flag of the direction set
                    str += axis_to_dir[event.jaxis.axis][event.jaxis.value>0];

                    // A number in range 0 - 100 (percentage speed from max)
                    str += QString::number((int) (abs(event.jaxis.value) / 327.67l));


                    // Sends to arduino
                    if(serial) serial->write(str);

                    qDebug()<<QString::fromStdString(str.toStdString());  // prints for debugging
                }

                break;
            default:
                break;
        }
    }
}


Controller::Controller(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Controller)
{
    ui->setupUi(this);

    // Starts SDL subsystem
    SDL_InitSubSystem(SDL_INIT_JOYSTICK);
    SDL_JoystickEventState(SDL_ENABLE);  // Enables joystick events

    timer = new QTimer;
    connect(timer,SIGNAL(timeout()),this,SLOT(pollJoystick()));

    timer->start(10); // Executes timer every 10ms

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

// Sends the new max speeds to the arduino
void Controller::on_submitButton_clicked()
{
    QByteArray string;
    string+="H" + QString::number(ui->HLineEdit->text().toInt());  // The horizontal max speed
    string+="V" + QString::number(ui->VLineEdit->text().toInt());  // The vertical max speed

    if(serial) serial->write(string);

    qDebug(string);
}
