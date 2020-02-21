TEMPLATE = app
TARGET = server

OBJECTS_DIR=.obj
MOC_DIR=.moc

# DEFINES += CODER

QT += core
QT += gui
QT += widgets
QT += network

DEPENDPATH += . src

INCLUDEPATH += .
INCLUDEPATH += ../shared
INCLUDEPATH += $$(QTDIR)/include/QtCore
INCLUDEPATH += $$(QTDIR)/include/QtNetwork

win32 {
   DEFINES += _USE_MATH_DEFINES=1
}

# Input
HEADERS += \
   ../shared/blockmapitem.h \
   ../shared/bombkickanimation.h \
   ../shared/bombmapitem.h \
   ../shared/bombpacket.h \
   ../shared/constants.h \
   ../shared/countdownpacket.h \
   ../shared/creategamedata.h \
   ../shared/creategamerequestpacket.h \
   ../shared/creategameresponsepacket.h \
   ../shared/detonationpacket.h \
   ../shared/errorpacket.h \
   ../shared/extramapitem.h \
   ../shared/extramapitemcreatedpacket.h \
   ../shared/extrashakepacket.h \
   ../shared/gameeventpacket.h \
   ../shared/gameinformation.h \
   ../shared/gameround.h \
   ../shared/gamestatspacket.h \
   ../shared/joingamerequestpacket.h \
   ../shared/joingameresponsepacket.h \
   ../shared/keypacket.h \
   ../shared/leavegamerequestpacket.h \
   ../shared/leavegameresponsepacket.h \
   ../shared/listgamesrequestpacket.h \
   ../shared/listgamesresponsepacket.h \
   ../shared/loginrequestpacket.h \
   ../shared/loginresponsepacket.h \
   ../shared/map.h \
   ../shared/mapcreaterequestpacket.h \
   ../shared/mapitem.h \
   ../shared/mapitemcreatedpacket.h \
   ../shared/mapitemdestroyedpacket.h \
   ../shared/mapitemmovepacket.h \
   ../shared/mapitempacket.h \
   ../shared/mapitemremovedpacket.h \
   ../shared/messagepacket.h \
   ../shared/packet.h \
   ../shared/player.h \
   ../shared/playerdisease.h \
   ../shared/playerinfectedpacket.h \
   ../shared/playerkilledpacket.h \
   ../shared/playermodifiedpacket.h \
   ../shared/playerrotation.h \
   ../shared/playerstats.h \
   ../shared/playersynchronizepacket.h \
   ../shared/positionpacket.h \
   ../shared/serverconfiguration.h \
   ../shared/startgamerequestpacket.h \
   ../shared/startgameresponsepacket.h \
   ../shared/stonedroppacket.h \
   ../shared/stonemapitem.h \
   ../shared/stopgamerequestpacket.h \
   ../shared/stopgameresponsepacket.h \
   ../shared/systemtools.h \
   ../shared/timepacket.h \
   src/collisiondetection.h \
   src/dummyplayer.h \
   src/extrashakepackethandler.h \
   src/extraspawn.h \
   src/game.h \
   src/server.h \
   src/servergui.h \
   src/serverversion.h

SOURCES += \
   ../shared/blockmapitem.cpp \
   ../shared/bombkickanimation.cpp \
   ../shared/bombmapitem.cpp \
   ../shared/bombpacket.cpp \
   ../shared/constants.cpp \
   ../shared/countdownpacket.cpp \
   ../shared/creategamedata.cpp \
   ../shared/creategamerequestpacket.cpp \
   ../shared/creategameresponsepacket.cpp \
   ../shared/detonationpacket.cpp \
   ../shared/errorpacket.cpp \
   ../shared/extramapitem.cpp \
   ../shared/extramapitemcreatedpacket.cpp \
   ../shared/extrashakepacket.cpp \
   ../shared/gameeventpacket.cpp \
   ../shared/gameinformation.cpp \
   ../shared/gameround.cpp \
   ../shared/gamestatspacket.cpp \
   ../shared/joingamerequestpacket.cpp \
   ../shared/joingameresponsepacket.cpp \
   ../shared/keypacket.cpp \
   ../shared/leavegamerequestpacket.cpp \
   ../shared/leavegameresponsepacket.cpp \
   ../shared/listgamesrequestpacket.cpp \
   ../shared/listgamesresponsepacket.cpp \
   ../shared/loginrequestpacket.cpp \
   ../shared/loginresponsepacket.cpp \
   ../shared/map.cpp \
   ../shared/mapcreaterequestpacket.cpp \
   ../shared/mapitem.cpp \
   ../shared/mapitemcreatedpacket.cpp \
   ../shared/mapitemdestroyedpacket.cpp \
   ../shared/mapitemmovepacket.cpp \
   ../shared/mapitempacket.cpp \
   ../shared/mapitemremovedpacket.cpp \
   ../shared/messagepacket.cpp \
   ../shared/packet.cpp \
   ../shared/player.cpp \
   ../shared/playerdisease.cpp \
   ../shared/playerinfectedpacket.cpp \
   ../shared/playerkilledpacket.cpp \
   ../shared/playermodifiedpacket.cpp \
   ../shared/playerrotation.cpp \
   ../shared/playerstats.cpp \
   ../shared/playersynchronizepacket.cpp \
   ../shared/positionpacket.cpp \
   ../shared/serverconfiguration.cpp \
   ../shared/startgamerequestpacket.cpp \
   ../shared/startgameresponsepacket.cpp \
   ../shared/stonedroppacket.cpp \
   ../shared/stonemapitem.cpp \
   ../shared/stopgamerequestpacket.cpp \
   ../shared/stopgameresponsepacket.cpp \
   ../shared/systemtools.cpp \
   ../shared/timepacket.cpp \
   src/collisiondetection.cpp \
   src/dummyplayer.cpp \
   src/extrashakepackethandler.cpp \
   src/extraspawn.cpp \
   src/game.cpp \
   src/servermain.cpp \
   src/server.cpp \
   src/servergui.cpp
