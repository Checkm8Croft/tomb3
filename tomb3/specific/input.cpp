#include "../tomb3/pch.h"
#include "input.h"
#include "dd.h"
#include "di.h"
#include "dxshell.h"
#include "display.h"
#include "../game/invfunc.h"
#include "../game/objects.h"
#include "../game/laramisc.h"
#include "winmain.h"
#include "../game/gameflow.h"
#include "../game/camera.h"
#include "../game/control.h"
#include "../game/lara.h"
#include "picture.h"
#include "smain.h"
#include "hwrender.h"
#include "../tomb3/tomb3.h"
#include <SDL2/SDL.h>  // Aggiungi include SDL

const char* KeyboardButtons[272] =
{
	0,
	"ESC", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "+", "BKSP",
	"TAB", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "<", ">", "RET",
	"CTRL", "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'", "`",
	"SHIFT", "#", "Z", "X", "C", "V", "B", "N", "M", ",", ".", "/", "SHIFT",
	"PADx", "ALT", "SPACE", "CAPS", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	"NMLK", 0,
	"PAD7", "PAD8", "PAD9", "PAD-",
	"PAD4", "PAD5", "PAD6", "PAD+",
	"PAD1", "PAD2", "PAD3",
	"PAD0", "PAD.", 0, 0, "\\", 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	"ENTER", "CTRL", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	"SHIFT", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	"PAD/", 0, 0, "ALT", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	"HOME", "UP", "PGUP", 0, "LEFT", 0, "RIGHT", 0, "END", "DOWN", "PGDN", "INS", "DEL",
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	"JOY1", "JOY2", "JOY3", "JOY4", "JOY5", "JOY6", "JOY7", "JOY8",
	"JOY9", "JOY10", "JOY11", "JOY12", "JOY13", "JOY14", "JOY15", "JOY16"
};

// Mappa i tasti DIK a SDL_SCANCODE
short layout[2][NLAYOUTKEYS] =
{
	{SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT, SDL_SCANCODE_PERIOD, SDL_SCANCODE_SLASH, SDL_SCANCODE_RSHIFT,
	SDL_SCANCODE_RALT, SDL_SCANCODE_RCTRL, SDL_SCANCODE_SPACE, SDL_SCANCODE_COMMA, SDL_SCANCODE_KP_0, SDL_SCANCODE_END, SDL_SCANCODE_ESCAPE, SDL_SCANCODE_P,},

	{SDL_SCANCODE_KP_8, SDL_SCANCODE_KP_2, SDL_SCANCODE_KP_4, SDL_SCANCODE_KP_6, SDL_SCANCODE_KP_DECIMAL, SDL_SCANCODE_KP_1, SDL_SCANCODE_RSHIFT,
	SDL_SCANCODE_RALT, SDL_SCANCODE_RCTRL, SDL_SCANCODE_SPACE, SDL_SCANCODE_KP_5, SDL_SCANCODE_KP_0, SDL_SCANCODE_END, SDL_SCANCODE_ESCAPE, SDL_SCANCODE_P}
};

long bLaraOn = 1;
long bRoomOn = 1;
long bObjectOn = 1;
long bAObjectOn = 1;
long bEffectOn = 1;
char bInvItemsOff = 0;

long input;
long FinishLevelCheat;
long conflict[15];
uchar keymap[256];

// Funzione helper per controllare se un tasto SDL è premuto
bool sdl_key_pressed(SDL_Scancode scancode)
{
    const Uint8* state = SDL_GetKeyboardState(NULL);
    return state[scancode] != 0;
}

long Key(long number)
{
	short key;

	key = layout[1][number];

	if (sdl_key_pressed((SDL_Scancode)key))
		return 1;

	switch (key)
	{
	case SDL_SCANCODE_RCTRL:
		return sdl_key_pressed(SDL_SCANCODE_LCTRL);

	case SDL_SCANCODE_LCTRL:
		return sdl_key_pressed(SDL_SCANCODE_RCTRL);

	case SDL_SCANCODE_RSHIFT:
		return sdl_key_pressed(SDL_SCANCODE_LSHIFT);

	case SDL_SCANCODE_LSHIFT:
		return sdl_key_pressed(SDL_SCANCODE_RSHIFT);

	case SDL_SCANCODE_RALT:
		return sdl_key_pressed(SDL_SCANCODE_LALT);

	case SDL_SCANCODE_LALT:
		return sdl_key_pressed(SDL_SCANCODE_RALT);
	}

	if (conflict[number])
		return 0;

	key = layout[0][number];

	if (sdl_key_pressed((SDL_Scancode)key))
		return 1;

	switch (key)
	{
	case SDL_SCANCODE_RCTRL:
		return sdl_key_pressed(SDL_SCANCODE_LCTRL);

	case SDL_SCANCODE_LCTRL:
		return sdl_key_pressed(SDL_SCANCODE_RCTRL);

	case SDL_SCANCODE_RSHIFT:
		return sdl_key_pressed(SDL_SCANCODE_LSHIFT);

	case SDL_SCANCODE_LSHIFT:
		return sdl_key_pressed(SDL_SCANCODE_RSHIFT);

	case SDL_SCANCODE_RALT:
		return sdl_key_pressed(SDL_SCANCODE_LALT);

	case SDL_SCANCODE_LALT:
		return sdl_key_pressed(SDL_SCANCODE_RALT);
	}

	return 0;
}

long S_UpdateInput()
{
	long linput;
	static long med_debounce = 0;
	static bool pause_debounce = 0;
	static bool F7_debounce = 0;

	// Processa gli eventi SDL
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			return 1; // Window closed
		}
	}

	// Leggi lo stato della tastiera SDL
	const Uint8* sdlKeyState = SDL_GetKeyboardState(NULL);
	
	// Aggiorna keymap per compatibilità (se necessario)
	for (int i = 0; i < 256; i++) {
		keymap[i] = sdlKeyState[i] ? 0x80 : 0x00;
	}

	linput = 0;

	if (Key(0))
		linput |= IN_FORWARD;

	if (Key(1))
		linput |= IN_BACK;

	if (Key(2))
		linput |= IN_LEFT;

	if (Key(3))
		linput |= IN_RIGHT;

	if (Key(4))
		linput |= IN_DUCK;

	if (Key(5))
		linput |= IN_SPRINT;

	if (Key(6))
		linput |= IN_WALK;

	if (Key(7))
		linput |= IN_JUMP;

	if (Key(8))
		linput |= IN_ACTION;

	if (Key(9))
		linput |= IN_DRAW;

	if (Key(10))
		linput |= IN_FLARE;

	if (Key(11))
		linput |= IN_LOOK;

	if (Key(12))
		linput |= IN_ROLL;

	if (Key(14) && !pictureFading)
	{
		if (!pause_debounce)
		{
			pause_debounce = 1;
			linput |= IN_PAUSE;
		}
	}
	else
		pause_debounce = 0;

	if (linput & IN_WALK && !(linput & (IN_FORWARD | IN_BACK)))
	{
		if (linput & IN_LEFT)
			linput = (linput & ~IN_LEFT) | IN_LSTEP;
		else if (linput & IN_RIGHT)
			linput = (linput & ~IN_RIGHT) | IN_RSTEP;
	}

	if (sdl_key_pressed(SDL_SCANCODE_KP_MULTIPLY))
	{
		farz -= 50;

		if (farz < 0x2000)
			farz = 0x2000;

		distanceFogValue = farz - 0x2000;
	}

	if (sdl_key_pressed(SDL_SCANCODE_KP_DIVIDE))
	{
		farz += 50;

		if (farz > 0x5000)
			farz = 0x5000;

		distanceFogValue = farz - 0x2000;
	}

	if (Key(13) && camera.type != CINEMATIC_CAMERA && !pictureFading)
		linput |= IN_OPTION;

	if (linput & IN_FORWARD && linput & IN_BACK)
		linput |= IN_ROLL;

	if (sdl_key_pressed(SDL_SCANCODE_RETURN) || linput & IN_ACTION)
		linput |= IN_SELECT;

	if (sdl_key_pressed(SDL_SCANCODE_ESCAPE))
		linput |= IN_DESELECT;

	if ((linput & (IN_RIGHT | IN_LEFT)) == (IN_RIGHT | IN_LEFT))
		linput -= IN_RIGHT | IN_LEFT;

	if (GnGameMode == GAMEMODE_IN_GAME && !nLoadedPictures)
	{
		if (sdl_key_pressed(SDL_SCANCODE_KP_PLUS))
			IncreaseScreenSize();

		if (sdl_key_pressed(SDL_SCANCODE_KP_MINUS))
			DecreaseScreenSize();
	}
	
	if (sdl_key_pressed(SDL_SCANCODE_F7))
	{
		if (!F7_debounce)
		{
			F7_debounce = 1;
			tomb3.psx_contrast = !tomb3.psx_contrast;
			HWR_InitState();
			S_SaveSettings();
		}
	}
	else
		F7_debounce = 0;

	if (sdl_key_pressed(SDL_SCANCODE_1) && Inv_RequestItem(GUN_OPTION))
		lara.request_gun_type = LG_PISTOLS;
	else if (sdl_key_pressed(SDL_SCANCODE_2) && Inv_RequestItem(SHOTGUN_OPTION))
		lara.request_gun_type = LG_SHOTGUN;
	else if (sdl_key_pressed(SDL_SCANCODE_3) && Inv_RequestItem(MAGNUM_OPTION))
		lara.request_gun_type = LG_MAGNUMS;
	else if (sdl_key_pressed(SDL_SCANCODE_4) && Inv_RequestItem(UZI_OPTION))
		lara.request_gun_type = LG_UZIS;
	else if (sdl_key_pressed(SDL_SCANCODE_5) && Inv_RequestItem(HARPOON_OPTION))
		lara.request_gun_type = LG_HARPOON;
	else if (sdl_key_pressed(SDL_SCANCODE_6) && Inv_RequestItem(M16_OPTION))
		lara.request_gun_type = LG_M16;
	else if (sdl_key_pressed(SDL_SCANCODE_7) && Inv_RequestItem(ROCKET_OPTION))
		lara.request_gun_type = LG_ROCKET;
	else if (sdl_key_pressed(SDL_SCANCODE_8) && Inv_RequestItem(GRENADE_OPTION))
		lara.request_gun_type = LG_GRENADE;

	if (sdl_key_pressed(SDL_SCANCODE_0) && Inv_RequestItem(MEDI_OPTION))
	{
		if (!med_debounce)
		{
			UseItem(MEDI_OPTION);
			med_debounce = 15;
		}
	}
	else if (sdl_key_pressed(SDL_SCANCODE_9) && Inv_RequestItem(BIGMEDI_OPTION))
	{
		if (!med_debounce)
		{
			UseItem(BIGMEDI_OPTION);
			med_debounce = 15;
		}
	}
	else if (med_debounce)
		med_debounce--;

	// Rimuovi codice DirectX specifico per salvare schermate
	// if (key_pressed(DIK_APOSTROPHE))
	//     DXSaveScreen(App.BackBuffer);

	if (FinishLevelCheat)
	{
		level_complete = 1;
		FinishLevelCheat = 0;
	}

	if (!gameflow.loadsave_disabled && !pictureFading)
	{
		if (sdl_key_pressed(SDL_SCANCODE_F5))
		{
			if (!tomb3.psx_saving)
				linput |= IN_SAVE;
		}
		else if (sdl_key_pressed(SDL_SCANCODE_F6))
			linput |= IN_LOAD;
	}

	input = linput;
	
	// Controlla se la finestra è chiusa
	SDL_PumpEvents();
	return 0; // Sempre ritorna 0, la chiusura della finestra è gestita dagli eventi SDL
}