cmake_minimum_required(VERSION 3.16)
project(tomb3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIR})
include_directories(/usr/local/include) # Forza include se serve

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_definitions(-D_XOPEN_SOURCE=700 -D_POSIX_C_SOURCE=200809L)
# Include directory globale
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/tomb3
    ${CMAKE_CURRENT_SOURCE_DIR}/tomb3/game
    ${CMAKE_CURRENT_SOURCE_DIR}/tomb3/global
    ${CMAKE_CURRENT_SOURCE_DIR}/tomb3/newstuff
    ${CMAKE_CURRENT_SOURCE_DIR}/tomb3/script
    ${CMAKE_CURRENT_SOURCE_DIR}/tomb3/specific
    ${CMAKE_CURRENT_SOURCE_DIR}/tomb3/3dsystem
)

# Lista di tutti i file sorgente (puoi aggiornarla con nuovi file)
set(SOURCES
    tomb3/tomb3/tomb3.cpp
    tomb3/tomb3/pch.cpp
    tomb3/3dsystem/3d_gen.cpp
    tomb3/3dsystem/hwinsert.cpp
    tomb3/3dsystem/phd_math.cpp
    tomb3/3dsystem/scalespr.cpp
    tomb3/game/51baton.cpp
    tomb3/game/51civvy.cpp
    tomb3/game/51laser.cpp
    tomb3/game/51rocket.cpp
    tomb3/game/armysmg.cpp
    tomb3/game/autogun.cpp
    tomb3/game/biggun.cpp
    tomb3/game/bird.cpp
    tomb3/game/blowpipe.cpp
    tomb3/game/boat.cpp
    tomb3/game/boomute.cpp
    tomb3/game/box.cpp
    tomb3/game/camera.cpp
    tomb3/game/cinema.cpp
    tomb3/game/clawmute.cpp
    tomb3/game/cleaner.cpp
    tomb3/game/cobra.cpp
    tomb3/game/collide.cpp
    tomb3/game/compy.cpp
    tomb3/game/control.cpp
    tomb3/game/croc.cpp
    tomb3/game/demo.cpp
    tomb3/game/diver.cpp
    tomb3/game/dog.cpp
    tomb3/game/dragfire.cpp
    tomb3/game/draw.cpp
    tomb3/game/effect2.cpp
    tomb3/game/effects.cpp
    tomb3/game/firehead.cpp
    tomb3/game/fish.cpp
    tomb3/game/flamer.cpp
    tomb3/game/flymaker.cpp
    tomb3/game/footprnt.cpp
    tomb3/game/fusebox.cpp
    tomb3/game/gameflow.cpp
    tomb3/game/hair.cpp
    tomb3/game/health.cpp
    tomb3/game/hybrid.cpp
    tomb3/game/inventry.cpp
    tomb3/game/invfunc.cpp
    tomb3/game/items.cpp
    tomb3/game/kayak.cpp
    tomb3/game/lara.cpp
    tomb3/game/lara1gun.cpp
    tomb3/game/lara2gun.cpp
    tomb3/game/laraclmb.cpp
    tomb3/game/laraelec.cpp
    tomb3/game/larafire.cpp
    tomb3/game/laraflar.cpp
    tomb3/game/laramisc.cpp
    tomb3/game/larasurf.cpp
    tomb3/game/laraswim.cpp
    tomb3/game/lasers.cpp
    tomb3/game/lizman.cpp
    tomb3/game/londboss.cpp
    tomb3/game/londsec.cpp
    tomb3/game/lot.cpp
    tomb3/game/minecart.cpp
    tomb3/game/missile.cpp
    tomb3/game/monkey.cpp
    tomb3/game/moveblok.cpp
    tomb3/game/mpgun.cpp
    tomb3/game/objects.cpp
    tomb3/game/objlight.cpp
    tomb3/game/oilred.cpp
    tomb3/game/oilsmg.cpp
    tomb3/game/orca.cpp
    tomb3/game/people.cpp
    tomb3/game/pickup.cpp
    tomb3/game/prisoner.cpp
    tomb3/game/punk.cpp
    tomb3/game/quadbike.cpp
    tomb3/game/rapmaker.cpp
    tomb3/game/raptor.cpp
    tomb3/game/rat.cpp
    tomb3/game/savegame.cpp
    tomb3/game/sealmute.cpp
    tomb3/game/setup.cpp
    tomb3/game/shiva.cpp
    tomb3/game/sound.cpp
    tomb3/game/sphere.cpp
    tomb3/game/sub.cpp
    tomb3/game/swat.cpp
    tomb3/game/target.cpp
    tomb3/game/text.cpp
    tomb3/game/tiger.cpp
    tomb3/game/tonyboss.cpp
    tomb3/game/train.cpp
    tomb3/game/traps.cpp
    tomb3/game/trex.cpp
    tomb3/game/tribeaxe.cpp
    tomb3/game/triboss.cpp
    tomb3/game/willboss.cpp
    tomb3/game/wingmute.cpp
    tomb3/game/winston.cpp
    tomb3/newstuff/discord.cpp
    tomb3/newstuff/LaraDraw.cpp
    tomb3/newstuff/map.cpp
    tomb3/newstuff/pausemenu.cpp
    tomb3/newstuff/Picture2.cpp
    tomb3/newstuff/psxsaves.cpp
    tomb3/newstuff/registry.cpp
    tomb3/newstuff/setupdlg.cpp
    tomb3/script/interpreter.cpp
    tomb3/script/scripter.cpp
    tomb3/specific/audio.cpp
    tomb3/specific/dd.cpp
    tomb3/specific/di.cpp
    tomb3/specific/display.cpp
    tomb3/specific/drawbars.cpp
    tomb3/specific/draweffects.cpp
    tomb3/specific/drawprimitive.cpp
    tomb3/specific/ds.cpp
    tomb3/specific/dxshell.cpp
    tomb3/specific/file.cpp
    tomb3/specific/fmv.cpp
    tomb3/specific/frontend.cpp
    tomb3/specific/game.cpp
    tomb3/specific/hwrender.cpp
    tomb3/specific/init.cpp
    tomb3/specific/input.cpp
    tomb3/specific/litesrc.cpp
    tomb3/specific/option.cpp
    tomb3/specific/output.cpp
    tomb3/specific/picture.cpp
    tomb3/specific/smain.cpp
    tomb3/specific/specific.cpp
    tomb3/specific/texture.cpp
    tomb3/specific/time.cpp
    tomb3/specific/transform.cpp
    tomb3/specific/utils.cpp
    tomb3/specific/winmain.cpp
)

add_executable(tomb3 ${SOURCES})

target_link_libraries(tomb3 ${SDL2_LIBRARIES})
target_link_libraries(tomb3 ${OpenGL_LIBRARIES})