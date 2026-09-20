cmake_minimum_required(VERSION 3.20)
project(dynablaster_sdl LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Material still derives from QObject (hot-reload via QFileSystemWatcher) and a couple of
# QMap/QMutex uses survive in the ported engine core. That Qt dependency is intentionally kept
# for now - see the phase split in README.md - and gets stripped in a later pass once the
# renderer itself is fully off fixed-function GL. It will have to go before this can ever target
# a Qt-less platform (e.g. Switch), which is the actual reason GLES3 was picked in the first place.
#
# The menu system additionally needs Qt6::Gui (QImage, QMouseEvent, QKeyEvent, QColor, ...) -
# none of that is windowing-toolkit-specific, it's just Qt's basic value types for images/events,
# so it carries the same "kept for now, stripped later" status as Core above.
set(CMAKE_AUTOMOC ON)
find_package(Qt6 REQUIRED COMPONENTS Core Gui)

include(FetchContent)

set(SDL_SHARED ON CACHE BOOL "" FORCE)
set(SDL_STATIC OFF CACHE BOOL "" FORCE)
set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.2.16
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(SDL3)

file(GLOB_RECURSE DYNABLASTER_ENGINE_SOURCES
    CONFIGURE_DEPENDS
    src/tools/*.cpp src/tools/*.h
    src/math/*.cpp src/math/*.h
    src/engine/render/*.cpp src/engine/render/*.h
    src/engine/nodes/*.cpp src/engine/nodes/*.h
    src/engine/animation/*.cpp src/engine/animation/*.h
    src/engine/materials/*.cpp src/engine/materials/*.h
    src/framework/gldevice.cpp src/framework/gldevice.h
    src/framework/renderdevice.cpp src/framework/renderdevice.h
    src/framework/globaltime.cpp src/framework/globaltime.h
    src/framework/framebuffer.cpp src/framework/framebuffer.h
    src/framework/frametimer.cpp src/framework/frametimer.h
    src/framework/timerhandler.cpp src/framework/timerhandler.h
    src/framework/drawable.cpp src/framework/drawable.h
    src/image/*.cpp src/image/*.h
)

# engine/helper/chunk.{cpp,h} is a second, unrelated Chunk class that collides by name with
# tools/chunk.{cpp,h} - a genuine pre-existing duplicate in the legacy tree. Only tools/chunk.cpp
# is in the real client.pro SOURCES list and nothing includes the engine/helper one, so it's
# excluded here rather than "namespaced apart".
list(FILTER DYNABLASTER_ENGINE_SOURCES EXCLUDE REGEX "engine/helper/chunk\\.(cpp|h)$")

# FilestreamRunnable (a QRunnable-based background file loader) hits an MSVC/Qt-AUTOMOC header
# ordering issue (<cstring>'s using-declarations conflicting) unrelated to the renderer work, and
# nothing on the current critical path uses it - only plain FileStream does. Excluded until
# someone actually needs it.
list(FILTER DYNABLASTER_ENGINE_SOURCES EXCLUDE REGEX "filestreamrunnable\\.(cpp|h)$")

# cartoonshading, glowring, gouraudshading, logoreflection and shadowvolume were never in the
# legacy client.pro SOURCES list either - dead, unmaintained material classes (gouraudshading's
# render() doesn't even compile against the current Material::Buffer, for instance). Excluded
# here too rather than "fixed", matching the original project's own exclusion.
list(FILTER DYNABLASTER_ENGINE_SOURCES EXCLUDE REGEX "(cartoonshading|glowring|gouraudshading|logoreflection|shadowvolume)\\.(cpp|h)$")

add_executable(dynablaster_sdl
    src/main.cpp
    src/gles3.cpp
    src/gles3.h
    src/glescontext.cpp
    src/glescontext.h
    src/shader.cpp
    src/shader.h
    src/screenshot.cpp
    src/screenshot.h
    src/inputinjector.cpp
    src/inputinjector.h
    src/demofactory.cpp
    src/demofactory.h
    src/sdlglobaltime.cpp
    src/sdlglobaltime.h

    # Menu system port (Phase 1 of the menu-system plan): listed explicitly rather than
    # glob-included like the engine sources above, because client-sdl/src/menus/ only has the
    # subset of the real menus/ directory needed for the main menu page so far - globbing would
    # pull in not-yet-ported files.
    src/menus/vertex.cpp
    src/menus/vertex.h
    src/menus/clipper.cpp
    src/menus/clipper.h
    src/menus/defaultshader.cpp
    src/menus/defaultshader.h
    src/menus/psdlayer.cpp
    src/menus/psdlayer.h
    src/menus/bitmapfont.cpp
    src/menus/bitmapfont.h
    src/menus/fontmap.cpp
    src/menus/fontmap.h
    src/menus/fontpool.h
    src/menus/menupageanimation.cpp
    src/menus/menupageanimation.h
    src/menus/menupagefadeanimation.cpp
    src/menus/menupagefadeanimation.h
    src/menus/menupageitem.cpp
    src/menus/menupageitem.h
    src/menus/menumousecursor.cpp
    src/menus/menumousecursor.h
    src/menus/menu.cpp
    src/menus/menu.h
    src/menus/menupage.cpp
    src/menus/menupage.h
    src/menus/menudrawable.cpp
    src/menus/menudrawable.h
    src/menus/menupagebuttonitem.cpp
    src/menus/menupagebuttonitem.h
    src/menus/menupagebackgrounditem.cpp
    src/menus/menupagebackgrounditem.h
    src/menus/menupagelabelitem.cpp
    src/menus/menupagelabelitem.h
    src/menus/menupagetextedit.cpp
    src/menus/menupagetextedit.h
    src/menus/menupagelistitemelement.cpp
    src/menus/menupagelistitemelement.h
    src/menus/menupagelistitem.cpp
    src/menus/menupagelistitem.h
    src/menus/menupagecomboboxitem.cpp
    src/menus/menupagecomboboxitem.h
    src/menus/menupageeditablecomboboxitem.cpp
    src/menus/menupageeditablecomboboxitem.h
    src/menus/menupagecheckboxitem.cpp
    src/menus/menupagecheckboxitem.h
    src/menus/menupagepixmapitem.cpp
    src/menus/menupagepixmapitem.h
    src/menus/menupagescrollbar.cpp
    src/menus/menupagescrollbar.h
    src/menus/menupageslideritem.cpp
    src/menus/menupageslideritem.h
    src/menus/menupagescrollimageitem.cpp
    src/menus/menupagescrollimageitem.h

    # Sphere-fragments effect (the 3D "exploding earth/bomb" logo subsystem GameLogoDrawable is
    # built on) - listed explicitly like the menu system above, since only this subset of the real
    # client/src/effects/ tree has been ported so far.
    src/postproduction/filter.cpp
    src/postproduction/filter.h
    src/postproduction/fullscreenquad.cpp
    src/postproduction/fullscreenquad.h
    src/postproduction/blurfilter.cpp
    src/postproduction/blurfilter.h
    src/effects/spherefragments/vertex3d.h
    src/effects/spherefragments/geometryvbo.cpp
    src/effects/spherefragments/geometryvbo.h
    src/effects/spherefragments/spheregeometryvbo.cpp
    src/effects/spherefragments/spheregeometryvbo.h
    src/effects/spherefragments/bombfuzegeometryvbo.cpp
    src/effects/spherefragments/bombfuzegeometryvbo.h
    src/effects/spherefragments/bombsocketgeometryvbo.cpp
    src/effects/spherefragments/bombsocketgeometryvbo.h
    src/effects/spherefragments/blendquad.cpp
    src/effects/spherefragments/blendquad.h
    src/effects/spherefragments/duplicatealpha.cpp
    src/effects/spherefragments/duplicatealpha.h
    src/effects/spherefragments/spherefragment.cpp
    src/effects/spherefragments/spherefragment.h
    src/effects/spherefragments/spherefragmentcontainer.cpp
    src/effects/spherefragments/spherefragmentcontainer.h
    src/effects/spherefragments/spherefragmentsdrawable.cpp
    src/effects/spherefragments/spherefragmentsdrawable.h

    # GameLogoDrawable - the animated main-menu logo built on top of the sphere-fragments effect
    # above (rotating-cube pass and earth/highlight ortho pass excluded - see gamelogodrawable.h,
    # both are dead code in the real shipped game).
    src/game/gamelogopointsprite.cpp
    src/game/gamelogopointsprite.h
    src/game/gamelogodrawable.cpp
    src/game/gamelogodrawable.h

    ${DYNABLASTER_ENGINE_SOURCES}
)

# NOTE: do not add src/tools (or similar) directly as an include dir - tools/string.h vs the
# real <string.h> is a real collision, and it silently breaks Qt/STL headers that #include
# <string.h> internally (they'd resolve to our String class instead). Bare includes like
# "stream.h" from inside engine/materials were fixed to proper relative paths instead.
target_include_directories(dynablaster_sdl PRIVATE
    third_party
    src
    src/framework
    src/engine
)
target_link_libraries(dynablaster_sdl PRIVATE SDL3::SDL3 Qt6::Core Qt6::Gui)

if(WIN32)
    add_custom_command(TARGET dynablaster_sdl POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3>
            $<TARGET_FILE:Qt6::Core>
            $<TARGET_FILE:Qt6::Gui>
            $<TARGET_FILE_DIR:dynablaster_sdl>
    )
endif()

# Shaders/textures load through FileStream at runtime (not compiled in), so they need to sit
# next to the exe - mirrors the SDL3.dll copy step above.
add_custom_command(TARGET dynablaster_sdl POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/data
        $<TARGET_FILE_DIR:dynablaster_sdl>/data
)

enable_testing()
add_test(NAME selftest_smoke
    COMMAND dynablaster_sdl --selftest --screenshot=${CMAKE_BINARY_DIR}/selftest.png
)
