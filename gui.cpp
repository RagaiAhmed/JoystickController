#include "gui.h"

#include <ui_gui.h>

#include <QDebug>  // For debugging
#include <QIntValidator>  // For validating text input
#include <QTimer> // For make an event polling function executed at equal intervals

QTimer *timer;  // The timer used for the pollJoystick

QIntValidator limiter1000(0,1000);  // Limits text to only integers in range [0,1000]
QIntValidator limiter255(0,255);  // Limits text to only integers in range [0,1000]

MainController::MainController(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainController)
{
    ui->setupUi(this);

    // Validator for text inputs
    ui->HnormLineEdit->setValidator(&limiter1000);
    ui->VnormLineEdit->setValidator(&limiter1000);

    ui->CutLineEdit->setValidator(&limiter1000);
    ui->PwmStartLineEdit->setValidator(&limiter255);

    // Init Qtimer
    timer = new QTimer;

    // Connect Timer timeout -> GUI fixedUpdate
    connect(timer,SIGNAL(timeout()),this,SLOT(FixedUpdate()));

    timer->start(timerPeriod); // Executes timer every 100ms

    joy = new Joystick(ui->joystickList);  // Initializes joystick handling object
    serial = new SerialControl(ui->portList);  // Initializes serial UART handling object

    // Connect GUI sendCommand -> Serial sendUART
    connect(this,SIGNAL(sendCommand(QByteArray)),serial,SLOT(sendUART(QByteArray)));

    // Connect Joystick sendCommand -> Serial sendUART
    connect(joy,SIGNAL(sendCommand(QByteArray)),serial,SLOT(sendUART(QByteArray)));

    // Connect serial receiveUART -> GUI executeCommand
    connect(serial,SIGNAL(receiveUART(QByteArray)),this,SLOT(executeCommand(QByteArray)));


}
MainController::~MainController()
{
    delete joy;  // Deletes the joystick handler
    delete serial;  // Deletes the serial handler
    delete ui;
}

void MainController::FixedUpdate()
{
    joy->execute_events();  // Get joystick events and executes them

    serial->execute_events();  // Reads any incoming data and executes them
}

void MainController::on_portList_activated(const QString &arg1)
{
    // Close old serial
    serial->close();

    // If item "Port" is chosen then update the available ports and return
    if(arg1=="Port")
    {
        // Updates ports list
        serial->updatePorts();
        return;
    }

    // Start new serial port
    serial->connect_port(arg1);

}

void MainController::on_joystickList_activated(int index)
{
    // Deattaches old controller
    joy->deattach();


    // If item "Controller" is chosen then update the available ports and return
    if(index==0)
    {
        // Updates ports list
        joy->updateJoysticks();
        return;
    }

    // Attaches new Controller
    joy->attach(index-1);

}

// Sends the new max speeds to the arduino
void MainController::on_submitButton_clicked()
{

    // Constructs speeds strings
    QByteArray h_cmd;
    h_cmd += "H" + QString::number(ui->HnormLineEdit->text().toInt());

    QByteArray v_cmd;
    v_cmd += "V" + QString::number(ui->VnormLineEdit->text().toInt());

    QByteArray cutoff_cmd;
    cutoff_cmd += "C" + QString::number(ui->CutLineEdit->text().toInt());

    QByteArray pwm_cmd;
    pwm_cmd += "S" + QString::number(ui->PwmStartLineEdit->text().toInt());



    // Emits the speeds commands
    emit(sendCommand(h_cmd)); // The horizontal max speed
    emit(sendCommand(v_cmd)); // The vertical max speed
    emit(sendCommand(cutoff_cmd)); // The cutoff speed
    emit(sendCommand(pwm_cmd)); // The starting pwm

}


// Executes a received command
void MainController::executeCommand(QByteArray str)
{
    char ind = str[0]; // Gets first char as indicator

    str.remove(0,1);
    QLineEdit *textBox;

    // Finds the command meaning
    switch(ind)
    {

        case 'P':  // pressure textbox
            textBox = ui->PLineEdit;
            break;
        case 'X':  // Gyro X value
            textBox = ui->GXLineEdit;
            break;
        case 'Y':  // Gyro Y value
            textBox = ui->GYLineEdit;
            break;
        case 'Z':  // Gyro Z value
            textBox = ui->GZLineEdit;
            break;
        case 'x':  // Acc X value
            textBox = ui->AXLineEdit;
            break;
        case 'y':  // Acc Y value
            textBox = ui->AYLineEdit;
            break;
        case 'z':  // Acc Z value
            textBox = ui->AZLineEdit;
               break;
        default:
            qDebug()<<"Recieved invalid data !!";
            return;
    }

    textBox->setText(str);
}
