#include "joystick.h"
#include <QDebug>  // For debugging
#include <SDL.h>  // SDL2 header


// Used to convert joystick axis to char to send to arduino
// The first sub is from 0 to 3 (defined axes in the joystick, usually x,y,z,Rx in order)
// The second sub is negative then positive
static char axis_to_dir[4][2]= {{'L','R'},{'B','F'},{'D','U'},{'<','>'}};


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
                if(event.jaxis.axis<4)  // If it is one of the first 4 axes
                {
                    QByteArray str;  // To store the string data to be sent

                    // A flag of the direction set
                    str += axis_to_dir[event.jaxis.axis][event.jaxis.value>0];

                    // A number in range 0 - 100 (percentage speed from max)
                    str += QString::number((int) (abs(event.jaxis.value) / 327.67l));

                    // Send command signal
                    emit(sendCommand(str));
                }

                break;

            default:  // Ignore any other event
                break;
        }
    }
}
