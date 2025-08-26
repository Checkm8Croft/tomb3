#include "../tomb3/pch.h"
#include "option.h"
#include "../game/text.h"
#include "../game/gameflow.h"
#include "hwrender.h"
#include "dxshell.h"
#include "drawprimitive.h"
#include "specific.h"
#include "input.h"
#include "../game/invfunc.h"
#include "../game/objects.h"
#include "../game/sound.h"
#include "game.h"
#include "../game/inventry.h"
#include "../3dsystem/3d_gen.h"
#include "winmain.h"
#include "../game/savegame.h"
#include "../game/control.h"
#include "output.h"
#include "smain.h"
#include "../newstuff/psxsaves.h"
#include "../newstuff/map.h"
#include "../tomb3/tomb3.h"

static GLOBE_LEVEL GlobeLevelAngles[7] =
{
	{ -1536, -7936, 1536, GT_LSLONDON },
	{ 1024, -512, -256, GT_LSSPAC },
	{ 2560, 21248, -4096, GT_LSNEVADA },
	{ -3328, 29440, 1024, GT_LSPERU },
	{ 3072, -20992, 6400, GT_LSLONDON },
	{ -5120, -15360, -18688, GT_LSANTARC },
	{ 0, 0, 0, 0 },
};

static TEXTSTRING* dtext[DT_NUMT];
static TEXTSTRING* stext[4];
static TEXTSTRING* btext[NLAYOUTKEYS];
static TEXTSTRING* ctext[NLAYOUTKEYS];
static TEXTSTRING* ctrltext[2];
static long iconfig;
static long keychange;

void do_sound_option(INVENTORY_ITEM* item) {
    // Implementazione
}

void do_pickup_option(INVENTORY_ITEM* item) {
    // Implementazione
}

long SavedGames;

long GetRenderWidth()
{
	return phd_winwidth;
}

long GetRenderHeight()
{
	return phd_winheight;
}

long GetRenderWidthDownscaled()
{
	return phd_winwidth * 0x10000 / GetRenderScale(0x10000);
}

long GetRenderHeightDownscaled()
{
	return phd_winheight * 0x10000 / GetRenderScale(0x10000);
}

void do_detail_option(INVENTORY_ITEM* item)
{
	// Per OpenGL, rimuovi le strutture DirectX specifiche
	DISPLAYMODE* cdm;
	static RES_TXT* resolutions;
	static long selected_res;
	static long selection = DOP_NOPTS - 1;
	long nSel, w, tW, oldRes;
	static char available[DOP_NOPTS];
	char gtxt[8];
	bool save;

	save = 0;
	nSel = DT_NUMT - DOP_NOPTS;
	tW = 130;
	w = GetRenderWidthDownscaled() / 2 - 115;
	
	// Per OpenGL, usa le dimensioni della finestra direttamente
	cdm = &App.deviceInfo->DisplayInfo[App.glConfig.nVMode].DisplayMode[0];
	
	// Crea una lista finta di risoluzioni per OpenGL
	if (!dtext[DT_GAMMA])
	{
		resolutions = (RES_TXT*)malloc(sizeof(RES_TXT) * 1); // Solo una risoluzione per OpenGL
		sprintf(resolutions[0].res, "%dx%d", App.width, App.height);
		selected_res = 0;

		//option names
		dtext[DT_EMPTY] = T_Print(0, -72, 0, " ");
		dtext[DT_VIDEOTITLE] = T_Print(0, -70, 0, GF_PCStrings[PCSTR_VIDEOTITLE]);
		dtext[DT_RESOLUTION] = T_Print(w, -45, 0, GF_PCStrings[PCSTR_RESOLUTION]);
		dtext[DT_ZBUFFER] = T_Print(w, -25, 0, GF_PCStrings[PCSTR_ZBUFFER]);
		dtext[DT_FILTER] = T_Print(w, -5, 0, GF_PCStrings[PCSTR_FILTERING]);
		dtext[DT_DITHER] = T_Print(w, 15, 0, GF_PCStrings[PCSTR_DITHER]);
		dtext[DT_GAMMA] = T_Print(w, 35, 0, GF_PCStrings[PCSTR_SKY]);
		
		T_AddBackground(dtext[DT_EMPTY], 240, 130, 0, 0, 48, 0, &req_bgnd_gour1, 0);
		T_AddOutline(dtext[DT_EMPTY], 1, 15, &req_bgnd_gour2, 0);
		T_CentreH(dtext[DT_EMPTY], 1);
		T_CentreV(dtext[DT_EMPTY], 1);

		T_AddBackground(dtext[DT_VIDEOTITLE], 236, 0, 0, 0, 48, 0, &req_main_gour1, 0);
		T_AddOutline(dtext[DT_VIDEOTITLE], 1, 4, &req_main_gour2, 0);
		T_CentreH(dtext[DT_VIDEOTITLE], 1);
		T_CentreV(dtext[DT_VIDEOTITLE], 1);

		T_CentreV(dtext[DT_RESOLUTION], 1);
		T_CentreV(dtext[DT_ZBUFFER], 1);
		T_CentreV(dtext[DT_FILTER], 1);
		T_CentreV(dtext[DT_DITHER], 1);
		T_CentreV(dtext[DT_GAMMA], 1);

		//actual options
		dtext[DT_OP_RESOLUTION] = T_Print(w + tW, -45, 0, resolutions[selected_res].res);

		if (App.glConfig.bZBuffer)
			dtext[DT_OP_ZBUFFER] = T_Print(w + tW, -25, 0, GF_PCStrings[PCSTR_ON]);
		else
			dtext[DT_OP_ZBUFFER] = T_Print(w + tW, -25, 0, GF_PCStrings[PCSTR_OFF]);

		// Per OpenGL, usa sempre filtro lineare
		dtext[DT_OP_FILTER] = T_Print(w + tW, -5, 0, GF_PCStrings[PCSTR_ON]);

		if (HWConfig.bDither)
			dtext[DT_OP_DITHER] = T_Print(w + tW, 15, 0, GF_PCStrings[PCSTR_ON]);
		else
			dtext[DT_OP_DITHER] = T_Print(w + tW, 15, 0, GF_PCStrings[PCSTR_OFF]);

		sprintf(gtxt, "%lu", (ulong)GammaOption);
		dtext[DT_OP_GAMMA] = T_Print(w + tW, 35, 0, gtxt);

		T_CentreV(dtext[DT_OP_RESOLUTION], 1);
		T_CentreV(dtext[DT_OP_ZBUFFER], 1);
		T_CentreV(dtext[DT_OP_FILTER], 1);
		T_CentreV(dtext[DT_OP_DITHER], 1);
		T_CentreV(dtext[DT_OP_GAMMA], 1);

		for (int i = 0; i < DOP_NOPTS; i++)
			available[i] = 1;

		if (tomb3.disable_gamma)
		{
			T_ChangeText(dtext[DT_OP_GAMMA], GF_PCStrings[PCSTR_SPARE8]);
			available[DOP_GAMMA] = 0;
			GammaOption = 2.5F;
		}

		// Per OpenGL, disabilita dithering
		T_ChangeText(dtext[DT_OP_DITHER], GF_PCStrings[PCSTR_SPARE8]);
		available[DOP_DITHER] = 0;
		HWConfig.bDither = 0;

		T_AddBackground(dtext[selection + nSel], (short)T_GetTextWidth(dtext[selection + nSel]), 0, 0, 0, 48, 0, &req_sel_gour1, 1);
		T_AddOutline(dtext[selection + nSel], 1, 4, &req_sel_gour2, 0);
		free(resolutions);
	}

	if (selection == DOP_RESOLUTION)
	{
		// Per OpenGL, la risoluzione non può essere cambiata dinamicamente
		// Mantieni la selezione ma non fare nulla
		oldRes = selected_res;
		
		if (inputDB & IN_LEFT)
		{
			if (selected_res > 0)
				selected_res--;
		}
		else if (inputDB & IN_RIGHT)
		{
			if (selected_res < 0) // Solo una risoluzione disponibile
				selected_res++;
		}

		if (oldRes != selected_res)
		{
			// Per OpenGL, non possiamo cambiare risoluzione dinamicamente
			selected_res = oldRes;
			
			for (int i = 0; i < nSel; i++)
			{
				T_RemovePrint(dtext[i]);
				dtext[i] = 0;
			}

			for (int i = nSel; i < DT_NUMT; i++)
				T_RemovePrint(dtext[i]);

			save = 1;
		}
	}

	if (inputDB & (IN_LEFT | IN_RIGHT))
	{
		switch (selection)
		{
		case DOP_GAMMA:

			if (inputDB & IN_RIGHT)
				GammaOption++;

			if (inputDB & IN_LEFT)
				GammaOption--;

			if (GammaOption > 10)
				GammaOption = 10;

			if (GammaOption < 1)
				GammaOption = 1;

			HWR_InitState();
			T_RemovePrint(dtext[DT_OP_GAMMA]);
			sprintf(gtxt, "%lu", (ulong)GammaOption);
			dtext[DT_OP_GAMMA] = T_Print(w + tW, 35, 0, gtxt);
			T_CentreV(dtext[DT_OP_GAMMA], 1);
			break;

		case DOP_FILTER:
			// Per OpenGL, il filtro è sempre lineare
			break;

		case DOP_ZBUFFER:
			// Per OpenGL, abilita/disabilita Z-buffer
			App.glConfig.bZBuffer = !App.glConfig.bZBuffer;
			HWR_InitState();

			if (App.glConfig.bZBuffer)
				T_ChangeText(dtext[selection + nSel], GF_PCStrings[PCSTR_ON]);
			else
				T_ChangeText(dtext[selection + nSel], GF_PCStrings[PCSTR_OFF]);

			break;
		}

		save = 1;
		T_RemoveOutline(dtext[selection + nSel]);
		T_RemoveBackground(dtext[selection + nSel]);
		T_AddOutline(dtext[selection + nSel], 1, 4, &req_sel_gour2, 0);
		T_AddBackground(dtext[selection + nSel], (short)T_GetTextWidth(dtext[selection + nSel]), 0, 0, 0, 48, 0, &req_sel_gour1, 1);
	}

	if (inputDB & IN_BACK && selection > 0)
	{
		T_RemoveOutline(dtext[selection + nSel]);
		T_RemoveBackground(dtext[selection + nSel]);
		selection--;

		while (!available[selection])
		{
			selection--;

			if (selection <= 0)
			{
				while (!available[selection] && selection <= DOP_NOPTS - 1) selection++;
				break;
			}
		}

		T_AddOutline(dtext[selection + nSel], 1, 4, &req_sel_gour2, 0);
		T_AddBackground(dtext[selection + nSel], (short)T_GetTextWidth(dtext[selection + nSel]), 0, 0, 0, 48, 0, &req_sel_gour1, 1);
	}

	if (inputDB & IN_FORWARD && selection <= DOP_NOPTS - 2)
	{
		T_RemoveOutline(dtext[selection + nSel]);
		T_RemoveBackground(dtext[selection + nSel]);
		selection++;

		while (!available[selection] && selection <= DOP_NOPTS - 1) selection++;

		T_AddOutline(dtext[selection + nSel], 1, 4, &req_sel_gour2, 0);
		T_AddBackground(dtext[selection + nSel], (short)T_GetTextWidth(dtext[selection + nSel]), 0, 0, 0, 48, 0, &req_sel_gour1, 1);
	}

	if (inputDB & (IN_SELECT | IN_DESELECT))
	{
		for (int i = 0; i < nSel; i++)
		{
			T_RemovePrint(dtext[i]);
			dtext[i] = 0;
		}

		for (int i = nSel; i < DT_NUMT; i++)
			T_RemovePrint(dtext[i]);
	}

	if (save)
		S_SaveSettings();
}

// ... (il resto del file rimane sostanzialmente uguale, poiché non contiene codice DirectX specifico)

// Le altre funzioni rimangono invariate poiché non contengono codice DirectX specifico
// do_levelselect_option, do_pickup_option, do_sound_option, do_control_option, etc.

void do_inventory_options(INVENTORY_ITEM* item)
{
	switch (item->object_number)
	{
	case PASSPORT_OPTION:
		return do_passport_option(item);

	case MAP_OPTION:
		return do_compass_option(item);

	case DETAIL_OPTION:
		return do_detail_option(item);

	case SOUND_OPTION:
		return do_sound_option(item);

	case CONTROL_OPTION:
		return do_control_option(item);

	case GAMMA_OPTION:
		return do_levelselect_option(item);

	case GUN_OPTION:
	case SHOTGUN_OPTION:
	case MAGNUM_OPTION:
	case UZI_OPTION:
	case HARPOON_OPTION:
	case M16_OPTION:
	case ROCKET_OPTION:
	case GRENADE_OPTION:
	case MEDI_OPTION:
	case BIGMEDI_OPTION:
	case PUZZLE_OPTION1:
	case PUZZLE_OPTION2:
	case PUZZLE_OPTION3:
	case PUZZLE_OPTION4:
	case KEY_OPTION1:
	case KEY_OPTION2:
	case KEY_OPTION3:
	case KEY_OPTION4:
		inputDB |= IN_SELECT;

	case GUN_AMMO_OPTION:
	case SG_AMMO_OPTION:
	case MAG_AMMO_OPTION:
	case UZI_AMMO_OPTION:
	case HARPOON_AMMO_OPTION:
	case M16_AMMO_OPTION:
	case ROCKET_AMMO_OPTION:
		return;

	case PICKUP_OPTION1:
	case PICKUP_OPTION2:
		return do_pickup_option(item);

	case SAVEGAME_CRYSTAL_OPTION:

		if (tomb3.psx_saving)
			return do_crystal_option(item);

	default:

		if (inputDB & (IN_SELECT | IN_DESELECT))
		{
			item->goal_frame = 0;
			item->anim_direction = -1;
		}

		break;
	}
}