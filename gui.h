#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainController; }
QT_END_NAMESPACE

#include <joystick.h>  // For Joystick logic class
#include <serial.h>  // For Serial logic class

class MainController : public QMainWindow
{
    Q_OBJECT

public:
    MainController(QWidget *parent = nullptr);
    ~MainController();

    // The time in ms between each timer tick
    static const int timerPeriod = 100;

public slots:

    // Is executed every timer tick aka "timeout"
    void FixedUpdate();

    // Executes command to modify GUI (Receiver)
    void executeCommand(QByteArray);

public: signals:

    // Sends command to other classes (Sender)
    void sendCommand(QByteArray);

private slots:

    // When a new port is chosen
    void on_portList_activated(const QString &arg1);

    // When a new joystick is chosen
    void on_joystickList_activated(int index);

    // When the submit button is clicked
    void on_submitButton_clicked();

private:
    Ui::MainController *ui;  // UI pointer (to edit GUI)
    SerialControl *serial;  // Serial handler class pointer (to init and deconstruct when needed)
    Joystick* joy;  // Joystick handler class pointer ( to init and deconstruct when needed)

};




#endif // CONTROLLER_H
