#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QMainWindow>


QT_BEGIN_NAMESPACE
namespace Ui { class Controller; }
QT_END_NAMESPACE

class Controller : public QMainWindow
{
    Q_OBJECT

public:
    Controller(QWidget *parent = nullptr);
    ~Controller();

public slots:
    void pollJoystick();

private slots:
    void on_portList_activated(const QString &arg1);

    void on_controllerList_activated(int index);

    void on_submitButton_clicked();

private:
    Ui::Controller *ui;

};
#endif // CONTROLLER_H
