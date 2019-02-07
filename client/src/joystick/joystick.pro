QT += network
TARGET = joysticktest
TEMPLATE = app

SOURCES += main.cpp \
    joystickinfo.cpp \
    joystickinterfacesdl.cpp \
    joystickinterface.cpp \
    joystickhandler.cpp

HEADERS += \
    joystickinfo.h \
    joystickinterfacesdl.h \
    joystickinterface.h \
    joystickconstants.h \
    joystickhandler.h \
    joystickballvector.h

LIBS += -lSDL

win32 {
   DEFINES += SDL_WIN
}
