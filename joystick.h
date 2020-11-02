#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <SDL_joystick.h>  // For joystick
#include <ui_controller.h>

#include <QComboBox>

class Joystick: public QObject
{
    Q_OBJECT
public:
    Joystick(QComboBox * controllerList);
    ~Joystick();

    void updateControllers();


    void execute_events();


    void deattach();

    void attach(int);

signals:
    void sendCommand(QByteArray);



private:
    SDL_Joystick *joy = NULL;  // The in use joystick
    QComboBox *controllerList = NULL;  // The list ui object
};

#endif // JOYSTICK_H
