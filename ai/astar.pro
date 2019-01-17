#-------------------------------------------------
#
# Project created by QtCreator 2012-03-18T14:17:02
#
#-------------------------------------------------

QT       += core
QT       += gui

TARGET = astar
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += .
INCLUDEPATH += ../shared

SOURCES += astarmain.cpp \
    pathfinding.cpp \
    path.cpp \
    botmap.cpp \
    astarpathfinding.cpp \
    ../shared/map.cpp \
    ../shared/timepacket.cpp \
    ../shared/stopgameresponsepacket.cpp \
    ../shared/stopgamerequestpacket.cpp \
    ../shared/stonemapitem.cpp \
    ../shared/startgameresponsepacket.cpp \
    ../shared/startgamerequestpacket.cpp \
    ../shared/positionpacket.cpp \
    ../shared/playerstats.cpp \
    ../shared/playerrotation.cpp \
    ../shared/playermodifiedpacket.cpp \
    ../shared/playerkilledpacket.cpp \
    ../shared/playerinfo.cpp \
    ../shared/player.cpp \
    ../shared/packet.cpp \
    ../shared/objectmovedpacket.cpp \
    ../shared/messagepacket.cpp \
    ../shared/mapitemremovedpacket.cpp \
    ../shared/mapitempacket.cpp \
    ../shared/mapitemdestroyedpacket.cpp \
    ../shared/mapitemcreatedpacket.cpp \
    ../shared/mapitem.cpp \
    ../shared/mapcreaterequestpacket.cpp \
    ../shared/loginresponsepacket.cpp \
    ../shared/loginrequestpacket.cpp \
    ../shared/listgamesresponsepacket.cpp \
    ../shared/listgamesrequestpacket.cpp \
    ../shared/leavegamerequestpacket.cpp \
    ../shared/keypacket.cpp \
    ../shared/joingameresponsepacket.cpp \
    ../shared/joingamerequestpacket.cpp \
    ../shared/gamestatspacket.cpp \
    ../shared/gameinformation.cpp \
    ../shared/gameeventpacket.cpp \
    ../shared/extramapitemcreatedpacket.cpp \
    ../shared/extramapitem.cpp \
    ../shared/detonationpacket.cpp \
    ../shared/creategameresponsepacket.cpp \
    ../shared/creategamerequestpacket.cpp \
    ../shared/countdownpacket.cpp \
    ../shared/constants.cpp \
    ../shared/bombpacket.cpp \
    ../shared/bombmapitem.cpp \
    ../shared/bombkickanimation.cpp \
    ../shared/blockmapitem.cpp \
    astarmap.cpp \
    astarnode.cpp

HEADERS += \
    pathfinding.h \
    path.h \
    botmap.h \
    astarpathfinding.h \
    ../shared/map.h \
    ../shared/timepacket.h \
    ../shared/stopgameresponsepacket.h \
    ../shared/stopgamerequestpacket.h \
    ../shared/stonemapitem.h \
    ../shared/startgameresponsepacket.h \
    ../shared/startgamerequestpacket.h \
    ../shared/positionpacket.h \
    ../shared/playerstats.h \
    ../shared/playerrotation.h \
    ../shared/playermodifiedpacket.h \
    ../shared/playerkilledpacket.h \
    ../shared/playerinfo.h \
    ../shared/player.h \
    ../shared/packet.h \
    ../shared/objectmovedpacket.h \
    ../shared/messagepacket.h \
    ../shared/mapitemremovedpacket.h \
    ../shared/mapitempacket.h \
    ../shared/mapitemdestroyedpacket.h \
    ../shared/mapitemcreatedpacket.h \
    ../shared/mapitem.h \
    ../shared/mapcreaterequestpacket.h \
    ../shared/loginresponsepacket.h \
    ../shared/loginrequestpacket.h \
    ../shared/listgamesresponsepacket.h \
    ../shared/listgamesrequestpacket.h \
    ../shared/leavegamerequestpacket.h \
    ../shared/keypacket.h \
    ../shared/joingameresponsepacket.h \
    ../shared/joingamerequestpacket.h \
    ../shared/gamestatspacket.h \
    ../shared/gameinformation.h \
    ../shared/gameeventpacket.h \
    ../shared/extramapitemcreatedpacket.h \
    ../shared/extramapitem.h \
    ../shared/detonationpacket.h \
    ../shared/creategameresponsepacket.h \
    ../shared/creategamerequestpacket.h \
    ../shared/countdownpacket.h \
    ../shared/constants.h \
    ../shared/bombpacket.h \
    ../shared/bombmapitem.h \
    ../shared/bombkickanimation.h \
    ../shared/blockmapitem.h \
    astarmap.h \
    astarnode.h
