#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class Controller; }
QT_END_NAMESPACE

#include <joystick.h>
#include <serial.h>

class Controller : public QMainWindow
{
    Q_OBJECT

public:
    Controller(QWidget *parent = nullptr);
    ~Controller();

public slots:
    void FixedUpdate();

    void executeCommand(QByteArray);

public: signals:
    void sendCommand(QByteArray);

private slots:
    void on_portList_activated(const QString &arg1);

    void on_controllerList_activated(int index);

    void on_submitButton_clicked();

private:
    Ui::Controller *ui;
    SerialControl *serial;
    Joystick* joy;

};




#endif // CONTROLLER_H
