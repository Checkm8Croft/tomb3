cmake_minimum_required(VERSION 3.16)
project(tomb3_script)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_definitions(GL_SILENCE_DEPRECATION)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../game
    ${CMAKE_CURRENT_SOURCE_DIR}/../global
    ${CMAKE_CURRENT_SOURCE_DIR}/../newstuff
    ${CMAKE_CURRENT_SOURCE_DIR}/../specific
    ${CMAKE_CURRENT_SOURCE_DIR}/../3dsystem
    /usr/local/include
    /usr/local/include/SDL2
)

link_directories(/usr/local/lib)

# Tutti i file cpp tranne tomb3.cpp
set(GAME_SOURCES
    tomb3.cpp
    ../tomb3/pch.cpp
    ../3dsystem/3d_gen.cpp
    ../3dsystem/hwinsert.cpp
    ../3dsystem/phd_math.cpp
    ../3dsystem/scalespr.cpp
    ../game/51baton.cpp
    ../game/51civvy.cpp
    ../game/51laser.cpp
    ../game/51rocket.cpp
    ../game/armysmg.cpp
    ../game/autogun.cpp
    ../game/biggun.cpp
    ../game/bird.cpp
    ../game/blowpipe.cpp
    ../game/boat.cpp
    ../game/boomute.cpp
    ../game/box.cpp
    ../game/camera.cpp
    ../game/cinema.cpp
    ../game/clawmute.cpp
    ../game/cleaner.cpp
    ../game/cobra.cpp
    ../game/collide.cpp
    ../game/compy.cpp
    ../game/control.cpp
    ../game/croc.cpp
    ../game/demo.cpp
    ../game/diver.cpp
    ../game/dog.cpp
    ../game/dragfire.cpp
    ../game/draw.cpp
    ../game/effect2.cpp
    ../game/effects.cpp
    ../game/firehead.cpp
    ../game/fish.cpp
    ../game/flamer.cpp
    ../game/flymaker.cpp
    ../game/footprnt.cpp
    ../game/fusebox.cpp
    ../game/gameflow.cpp
    ../game/hair.cpp
    ../game/health.cpp
    ../game/hybrid.cpp
    ../game/inventry.cpp
    ../game/invfunc.cpp
    ../game/items.cpp
    ../game/kayak.cpp
    ../game/lara.cpp
    ../game/lara1gun.cpp
    ../game/lara2gun.cpp
    ../game/laraclmb.cpp
    ../game/laraelec.cpp
    ../game/larafire.cpp
    ../game/laraflar.cpp
    ../game/laramisc.cpp
    ../game/larasurf.cpp
    ../game/laraswim.cpp
    ../game/lasers.cpp
    ../game/lizman.cpp
    ../game/londboss.cpp
    ../game/londsec.cpp
    ../game/lot.cpp
    ../game/minecart.cpp
    ../game/missile.cpp
    ../game/monkey.cpp
    ../game/moveblok.cpp
    ../game/mpgun.cpp
    ../game/objects.cpp
    ../game/objlight.cpp
    ../game/oilred.cpp
    ../game/oilsmg.cpp
    ../game/orca.cpp
    ../game/people.cpp
    ../game/pickup.cpp
    ../game/prisoner.cpp
    ../game/punk.cpp
    ../game/quadbike.cpp
    ../game/rapmaker.cpp
    ../game/raptor.cpp
    ../game/rat.cpp
    ../game/savegame.cpp
    ../game/sealmute.cpp
    ../game/setup.cpp
    ../game/shiva.cpp
    ../game/sound.cpp
    ../game/sphere.cpp
    ../game/sub.cpp
    ../game/swat.cpp
    ../game/target.cpp
    ../game/text.cpp
    ../game/tiger.cpp
    ../game/tonyboss.cpp
    ../game/train.cpp
    ../game/traps.cpp
    ../game/trex.cpp
    ../game/tribeaxe.cpp
    ../game/triboss.cpp
    ../game/willboss.cpp
    ../game/wingmute.cpp
    ../game/winston.cpp
    ../newstuff/LaraDraw.cpp
    ../newstuff/map.cpp
    ../newstuff/pausemenu.cpp
    ../newstuff/Picture2.cpp
    ../newstuff/psxsaves.cpp
    ../newstuff/registry.cpp
    ../newstuff/setupdlg.cpp
    ../specific/audio.cpp
    ../specific/dd.cpp
    ../specific/di.cpp
    ../specific/display.cpp
    ../specific/drawbars.cpp
    ../specific/draweffects.cpp
    ../specific/drawprimitive.cpp
    ../specific/ds.cpp
    ../specific/dxshell.cpp
    ../specific/file.cpp
    ../specific/fmv.cpp
    ../specific/frontend.cpp
    ../specific/game.cpp
    ../specific/hwrender.cpp
    ../specific/init.cpp
    ../specific/input.cpp
    ../specific/litesrc.cpp
    ../specific/option.cpp
    ../specific/output.cpp
    ../specific/picture.cpp
    ../specific/single.cpp
    ../specific/smain.cpp
    ../specific/specific.cpp
    ../specific/texture.cpp
    ../specific/time.cpp
    ../specific/transform.cpp
    ../specific/utils.cpp
    winmain.cpp
)

set(SCRIPTER_SOURCES
    scripter.cpp
    interpreter.cpp
)

# === Scripter ===
add_executable(scripter
    script_out.cpp  # main di scripter
    ${SCRIPTER_SOURCES}
    ${GAME_SOURCES}
)

# === Interpreter ===
add_executable(interpreter
    script_in.cpp  # main di interpreter
    ${SCRIPTER_SOURCES}
    ${GAME_SOURCES}
)

foreach(target scripter interpreter)
    target_link_libraries(${target}
        PRIVATE
            /usr/local/lib/libSDL2.dylib
            /usr/local/lib/libSDL2_image.dylib
            /usr/local/lib/libSDL2_mixer.dylib
            "-framework OpenGL"
            "-framework Cocoa"
            "-framework IOKit"
            "-framework CoreVideo"
            "-framework AudioToolbox"
            "-framework ForceFeedback"
    )
endforeach()
