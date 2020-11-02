#include "joystick.h"
#include <QDebug>  // For debugging
#include <SDL.h>  // SDL2 header
#include <map>

// Used to convert joystick axis to char to send to arduino
// by mapping each axis to a string with negative then positive direction
// so each axis character is extracted as such
// axis_to_dir[axis_index][is_positive_direction]
static std::map<int,std::string> axis_to_dir =
    {
        { 0, "LR" },
        { 1, "BF" },
        { 2, "DU" },
        { 3, "<>" }
    };


Joystick::Joystick(QComboBox * controllerList)
{
    this->controllerList = controllerList;  // Stores the list object

    // Starts SDL subsystem
    SDL_InitSubSystem(SDL_INIT_JOYSTICK);
    SDL_JoystickEventState(SDL_ENABLE);  // Enables joystick events

    updateControllers();  // Init controllers list

}

void Joystick::updateControllers()
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

Joystick::~Joystick()
{
    // Closing SDL subsystem
    SDL_Quit();
}

void Joystick::deattach()
{
    // Deattaching old controller
    if(joy && SDL_JoystickGetAttached(joy))
    {
        qDebug()<<"Deattaching old controller";
        SDL_JoystickClose(joy);
        joy=NULL;
    }

}

void Joystick::attach(int index)
{
    joy = SDL_JoystickOpen(index);
    if(joy)  // If opened successfully
    {
        qDebug()<<"Attached controller: "<<SDL_JoystickNameForIndex(index);
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


void Joystick::execute_events()
{
    SDL_Event event;

    // If any events are in queue (non blocking)
    while(SDL_PollEvent(&event))
    {
        // Check the event type
        switch(event.type)
        {
            case SDL_JOYAXISMOTION:  //A change in joystick axis
                if(axis_to_dir.count(event.jaxis.axis))  // If the axis in our directions map
                {
                    QByteArray str;  // To store the string data to be sent

                    // A flag of the direction set
                    str += axis_to_dir[event.jaxis.axis][event.jaxis.value>0];

                    if(isAnalog)
                    {
                        // A number in range 0 - 100 (percentage speed from max)
                        str += QString::number((int) (abs(event.jaxis.value) / 327.67l));
                    }
                    else
                    {
                        // Add 100 if bigger than 3276.7 else 0
                        str += QString::number((int)(abs(event.jaxis.value) > 3276.7l)*100);
                    }

                    // Send command signal
                    emit(sendCommand(str));
                }

                break;

            default:  // Ignore any other event
                break;
        }
    }
}
