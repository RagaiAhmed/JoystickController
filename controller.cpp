#include "controller.h"
#include "ui_controller.h"
#include <QIntValidator>

Controller::Controller(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Controller)
{
    ui->setupUi(this);

    // Validator for text input
    QIntValidator *limiter = new QIntValidator(0,1000);
    ui->HLineEdit->setValidator(limiter);
    ui->VLineEdit->setValidator(limiter);

    // Configuring port
    serial = new QSerialPort;
    serial->setPortName("COM3");
    serial->open(QSerialPort::WriteOnly);

    // Arduino specific configuration
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
}

Controller::~Controller()
{
    // Closing the port
    if(serial->isOpen()) serial->close();

    delete ui;
}

