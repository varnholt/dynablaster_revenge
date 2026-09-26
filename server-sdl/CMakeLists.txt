cmake_minimum_required(VERSION 3.20)
project(dynablaster_server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core)

include(FetchContent)

set(SDL_SHARED ON CACHE BOOL "" FORCE)
set(SDL_STATIC OFF CACHE BOOL "" FORCE)
set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.4.16
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(SDL3)

set(SDLNET_SHARED ON CACHE BOOL "" FORCE)
set(SDLNET_STATIC OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    SDL3_net
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_net.git
    GIT_TAG release-3.2.0
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(SDL3_net)

# shared/ (sibling directory, the real client<->server wire protocol) - the exact set server.pro
# lists, plus playerrotation.{h,cpp}: not in server.pro's own SOURCES/HEADERS lists, but
# player.h #includes it directly (PlayerRotation is a real Player member), so it's compiled in
# regardless. playerinfo.{h,cpp} is client-only (nothing in server/src references it) and stays
# excluded.
set(DYNABLASTER_SHARED_SOURCES
    ../shared/blockmapitem.cpp
    ../shared/blockmapitem.h
    ../shared/binaryreader.cpp
    ../shared/binaryreader.h
    ../shared/binarywriter.cpp
    ../shared/binarywriter.h
    ../shared/bombkickanimation.cpp
    ../shared/bombkickanimation.h
    ../shared/bombmapitem.cpp
    ../shared/bombmapitem.h
    ../shared/bombpacket.cpp
    ../shared/bombpacket.h
    ../shared/constants.cpp
    ../shared/constants.h
    ../shared/countdownpacket.cpp
    ../shared/countdownpacket.h
    ../shared/creategamedata.cpp
    ../shared/creategamedata.h
    ../shared/creategamerequestpacket.cpp
    ../shared/creategamerequestpacket.h
    ../shared/creategameresponsepacket.cpp
    ../shared/creategameresponsepacket.h
    ../shared/detonationpacket.cpp
    ../shared/detonationpacket.h
    ../shared/errorpacket.cpp
    ../shared/errorpacket.h
    ../shared/extramapitem.cpp
    ../shared/extramapitem.h
    ../shared/extramapitemcreatedpacket.cpp
    ../shared/extramapitemcreatedpacket.h
    ../shared/extrashakepacket.cpp
    ../shared/extrashakepacket.h
    ../shared/gameeventpacket.cpp
    ../shared/gameeventpacket.h
    ../shared/gameinformation.cpp
    ../shared/gameinformation.h
    ../shared/gameround.cpp
    ../shared/gameround.h
    ../shared/gamestatspacket.cpp
    ../shared/gamestatspacket.h
    ../shared/joingamerequestpacket.cpp
    ../shared/joingamerequestpacket.h
    ../shared/joingameresponsepacket.cpp
    ../shared/joingameresponsepacket.h
    ../shared/keypacket.cpp
    ../shared/keypacket.h
    ../shared/leavegamerequestpacket.cpp
    ../shared/leavegamerequestpacket.h
    ../shared/leavegameresponsepacket.cpp
    ../shared/leavegameresponsepacket.h
    ../shared/listgamesrequestpacket.cpp
    ../shared/listgamesrequestpacket.h
    ../shared/listgamesresponsepacket.cpp
    ../shared/listgamesresponsepacket.h
    ../shared/loginrequestpacket.cpp
    ../shared/loginrequestpacket.h
    ../shared/loginresponsepacket.cpp
    ../shared/loginresponsepacket.h
    ../shared/map.cpp
    ../shared/map.h
    ../shared/mapcreaterequestpacket.cpp
    ../shared/mapcreaterequestpacket.h
    ../shared/mapitem.cpp
    ../shared/mapitem.h
    ../shared/mapitemcreatedpacket.cpp
    ../shared/mapitemcreatedpacket.h
    ../shared/mapitemdestroyedpacket.cpp
    ../shared/mapitemdestroyedpacket.h
    ../shared/mapitemmovepacket.cpp
    ../shared/mapitemmovepacket.h
    ../shared/mapitempacket.cpp
    ../shared/mapitempacket.h
    ../shared/mapitemremovedpacket.cpp
    ../shared/mapitemremovedpacket.h
    ../shared/messagepacket.cpp
    ../shared/messagepacket.h
    ../shared/packet.cpp
    ../shared/packet.h
    ../shared/packetstreambuffer.cpp
    ../shared/packetstreambuffer.h
    ../shared/player.cpp
    ../shared/player.h
    ../shared/playerdisease.cpp
    ../shared/playerdisease.h
    ../shared/playerrotation.cpp
    ../shared/playerrotation.h
    ../shared/playerinfectedpacket.cpp
    ../shared/playerinfectedpacket.h
    ../shared/playerkilledpacket.cpp
    ../shared/playerkilledpacket.h
    ../shared/playermodifiedpacket.cpp
    ../shared/playermodifiedpacket.h
    ../shared/playerstats.cpp
    ../shared/playerstats.h
    ../shared/playersynchronizepacket.cpp
    ../shared/playersynchronizepacket.h
    ../shared/point.cpp
    ../shared/point.h
    ../shared/positionpacket.cpp
    ../shared/positionpacket.h
    ../shared/serverconfiguration.cpp
    ../shared/serverconfiguration.h
    ../shared/startgamerequestpacket.cpp
    ../shared/startgamerequestpacket.h
    ../shared/startgameresponsepacket.cpp
    ../shared/startgameresponsepacket.h
    ../shared/stonedroppacket.cpp
    ../shared/stonedroppacket.h
    ../shared/stonemapitem.cpp
    ../shared/stonemapitem.h
    ../shared/stopgamerequestpacket.cpp
    ../shared/stopgamerequestpacket.h
    ../shared/stopgameresponsepacket.cpp
    ../shared/stopgameresponsepacket.h
    ../shared/systemtools.cpp
    ../shared/systemtools.h
    ../shared/timepacket.cpp
    ../shared/timepacket.h
    ../shared/vec2.cpp
    ../shared/vec2.h
)

# ../server/src (sibling directory) - same files client-sdl embeds. dummyplayer.{h,cpp} is left
# out: its only caller is servergui.cpp (dropped, unused Qt Widgets console), nothing in
# server.cpp/game.cpp references DummyPlayer at all - it's dead code without the GUI.
set(DYNABLASTER_SERVER_SOURCES
    ../server/src/collisiondetection.cpp
    ../server/src/collisiondetection.h
    ../server/src/extrashakepackethandler.cpp
    ../server/src/extrashakepackethandler.h
    ../server/src/extraspawn.cpp
    ../server/src/extraspawn.h
    ../server/src/game.cpp
    ../server/src/game.h
    ../server/src/server.cpp
    ../server/src/server.h
    ../server/src/serverversion.h
)

add_executable(dynablaster_server
    src/main.cpp
    ${DYNABLASTER_SHARED_SOURCES}
    ${DYNABLASTER_SERVER_SOURCES}
)

set_target_properties(dynablaster_server PROPERTIES AUTOMOC ON)

target_include_directories(dynablaster_server PRIVATE
    ../shared
    ../server/src
)

target_link_libraries(dynablaster_server PRIVATE Qt6::Core SDL3::SDL3 SDL3_net::SDL3_net)

if(WIN32)
    add_custom_command(TARGET dynablaster_server POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:Qt6::Core>
            $<TARGET_FILE:SDL3::SDL3>
            $<TARGET_FILE:SDL3_net::SDL3_net>
            $<TARGET_FILE_DIR:dynablaster_server>
    )
endif()
