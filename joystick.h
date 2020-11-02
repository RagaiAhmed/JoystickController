#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <SDL_joystick.h>  // For Joystick input
#include <QComboBox>



class Joystick: public QObject
{
    Q_OBJECT
public:

    // If set to true, sends varying speeds from [0 to 100], else always sends 0 or 100
    static const bool isAnalog = true;

    Joystick(QComboBox * );
    ~Joystick();

    // Updates the available joystick list
    void updateJoysticks();

    // When called, polls for events from the connected joystick
    void execute_events();

    // Deattaches the current joystick
    void deattach();

    // Attaches the specified joystick from index
    void attach(int);

signals:
    // Sends command to other classes (Sender)
    void sendCommand(QByteArray);


private:
    SDL_Joystick *joy = NULL;  // The in use joystick
    QComboBox *joystick = NULL;  // The list ui object
};

#endif // JOYSTICK_H
