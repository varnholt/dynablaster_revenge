
// Qt
#include <QtGui/QApplication>

// joystick
#include "joystickhandler.h"
#include "joystickinterfacesdl.h"

#ifdef SDL_WIN
#undef main
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    JoystickInterfaceSDL* joystickInterface = new JoystickInterfaceSDL();
    JoystickHandler w;
    w.setInterface(joystickInterface);
    w.initialize();

    return a.exec();
}
