# tomb3 in gl
## Tomb Raider 3 from DirectX to OpenGL porting project

## Goal
Porting the entire game from Windows and DirectX to all platforms in OpenGL and SDL
Pull Requests are always welcome!

## CONTRIBUTING
Use the cmake file

## Progress
I currently modified:
- Types.h (one of the main headers, so I could say that I have 1/3 of the work ready)
- Pch.h
- Registry.h
- Tomb3.cpp
- Init.h
- Winmain.cpp and h
- 3d_gen.cpp
- Hwinsert.h and cpp
- File.h
- Hwrender.h
- Game.h
- Init.h
- Texture.h
- Health.cpp
- Invfunc.cpp
- Audio.cpp and h
- dd.cpp and h
- di.cpp and h
- display.cpp

### Credits:
Checkm8Croft: For porting the project

Troye: Main decompilation effort, main developer.

ChocolateFan: Additional decompilation and development.

Arsunt: for the amazing TR2Main and allowing us to use his code for some features.

Lahm86: for the amazing config tool.

LostArtefacts: The new icon is inspired to the TR2X Icon, since I used its psd file

### License:
This project is licensed under the GNU General Public License - see the [LICENSE](https://github.com/Trxyebeep/tomb3/blob/master/LICENSE.md) file for details

## (Hypothetic) Q&A

Q: Which platforms supports?
A: macOS (tested on Sequoia) Windows (tested on 11) Linux (tested on Ubuntu)

Q: Will it exit a mobile version for iOS/Android?
A: I hope it! Since is a C++ and GL/SDL project, it could be possbile. Well I could try with Xcode for iOS, I don't have an Andrioid for test the Android Version