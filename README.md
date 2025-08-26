# tomb3 in gl
## Tomb Raider 3 from DirectX to OpenGL porting project

## DISCLAIMER
This is just a fork of Troye's tomb3, with all games files ported with AI with minimal OpenGL/SDL2 definitions

## Goal
Porting the entire game from Windows and DirectX to all platforms in OpenGL and SDL
Pull Requests are always welcome!

## CONTRIBUTING
Use the cmake file

## Progress
I currently finished to porting the files, for now with minimal definitions, so not complete
The executable will exit but when I try to run it gives error when it tries to find the symbols.

### Credits:
Checkm8Croft: For porting the project

Troye: Main decompilation effort, main developer.

ChocolateFan: Additional decompilation and development.

Arsunt: for the amazing TR2Main and allowing us to use his code for some features.

Lahm86: for the amazing config tool.

LostArtefacts: The new icon is inspired to the TR2X Icon, since I used its psd file

### License:
This project is licensed under the GNU General Public License - see the [LICENSE](https://github.com/Trxyebeep/tomb3/blob/master/LICENSE.md) file for details

## Q&A

Hypotethic:
Q: Which platforms supports?
A: macOS (tested on Sequoia) Windows (tested on 11) Linux (tested on Ubuntu)

Q: Will it exit a mobile version for iOS/Android?
A: I hope it! Since is a C++ and GL/SDL project, it could be possbile. Well I could try with Xcode for iOS, I don't have an Andrioid for test the Android Version

Q: You'll port also tomb4 and tomb5?
A: Well I think that when I'll finish tomb3, I give me a pause and probably I'll start with tomb4. In past I also ported TOMB5 PSX Emulator from 32 bits to 64 bits, [see here](https://github.com/Checkm8Croft/TOMB5) 