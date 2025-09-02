#include "../tomb3/pch.h"
#include "smain.h"
#include "specific.h"
#include "../game/gameflow.h"
#include "../game/invfunc.h"
#include "../game/objects.h"
#include "../game/sound.h"
#include "../game/missile.h"
#include "../game/text.h"
#include "display.h"
#include "picture.h"
#include "../game/setup.h"
#include "time.h"
#include "../game/inventry.h"
#include "game.h"
#include "../game/savegame.h"
#include "winmain.h"
#include "input.h"
#include "frontend.h"
#include "../game/cinema.h"
#include "../game/demo.h"
#include "../game/lara.h"
#include "init.h"
#include "../newstuff/discord.h"
#include "../game/control.h"
#include "../newstuff/Picture2.h"
#include "../tomb3/tomb3.h"
#include <unistd.h>
bool LoadPicture(const char* name, GLuint* textureID, int* width, int* height);
long HiResFlag;
long title_loaded;
char exit_message[128];
long s = 0;
long lp = 0; 
long level = 0;
bool S_LoadSettings()
{
    printf("[8.1.1] S_LoadSettings started...\n");
    
    FILE* file = fopen("data.bin", "rb");

    if (file)
    {
        printf("[8.1.2] Reading data.bin...\n");
        size_t read1 = fread(&savegame.best_assault_times, sizeof(long), sizeof(savegame.best_assault_times) / sizeof(long), file);
        size_t read2 = fread(&savegame.best_quadbike_times, sizeof(long), sizeof(savegame.best_quadbike_times) / sizeof(long), file);
        size_t read3 = fread(&savegame.QuadbikeKeyFlag, sizeof(long), 1, file);
        fclose(file);
        
        printf("[8.1.3] Read counts: %zu, %zu, %zu\n", read1, read2, read3);
    }
    else
    {
        printf("[8.1.2] data.bin not found, using default values\n");
        // Inizializza savegame con valori di default
        memset(&savegame.best_assault_times, 0, sizeof(savegame.best_assault_times));
        memset(&savegame.best_quadbike_times, 0, sizeof(savegame.best_quadbike_times));
        savegame.QuadbikeKeyFlag = 0;
    }

    printf("[8.1.4] Calling T3_LoadSettings...\n");
    bool result = T3_LoadSettings();
    printf("[8.1.5] T3_LoadSettings returned: %d\n", result);
    return result;
}

void S_SaveSettings()
{
	FILE* file;

	T3_SaveSettings();
	file = fopen("data.bin", "wb+");

	if (file)
	{
		fwrite(savegame.best_assault_times, sizeof(ulong), sizeof(savegame.best_assault_times) / sizeof(long), file);
		fwrite(savegame.best_quadbike_times, sizeof(ulong), sizeof(savegame.best_quadbike_times) / sizeof(long), file);
		fwrite(&savegame.QuadbikeKeyFlag, sizeof(ulong), 1, file);
		fclose(file);
	}
}

void CheckCheatMode()
{
	static long mode, gun, turn;
	static short as, angle;

	as = lara_item->current_anim_state;

	switch (mode)
	{
	case 0:

		if (as == AS_WALK)
			mode = 1;

		break;

	case 1:
		gun = lara.gun_type == LG_PISTOLS;

		if (as != AS_WALK)
		{
			if (as == AS_STOP)
				mode = 2;
			else
				mode = 0;
		}

		break;

	case 2:

		if (as != AS_STOP)
		{
			if (as == AS_DUCK)
				mode = 3;
			else
				mode = 0;
		}

		break;

	case 3:

		if (as != AS_DUCK)
		{
			if (as == AS_STOP)
				mode = 4;
			else
				mode = 0;
		}

		break;

	case 4:

		if (as != AS_STOP)
		{
			angle = lara_item->pos.y_rot;
			turn = 0;

			if (as == AS_TURN_L)
				mode = 5;
			else if (as == AS_TURN_R)
				mode = 6;
			else
				mode = 0;
		}

		break;

	case 5:

		if (as == AS_TURN_L || as == AS_FASTTURN)
		{
			turn += short(lara_item->pos.y_rot - angle);
			angle = lara_item->pos.y_rot;
		}
		else if (turn < -0x17000)
			mode = 7;
		else
			mode = 0;

		break;

	case 6:

		if (as == AS_TURN_R || as == AS_FASTTURN)
		{
			turn += short(lara_item->pos.y_rot - angle);
			angle = lara_item->pos.y_rot;
		}
		else if (turn > 0x17000)
			mode = 7;
		else
			mode = 0;

		break;

	case 7:

		if (as != AS_STOP)
		{
			if (as == AS_COMPRESS)
				mode = 8;
			else
				mode = 0;
		}

		break;

	case 8:

		if (lara_item->fallspeed <= 0)
			break;

		if (gun)
			gun = lara.gun_type == LG_PISTOLS;

		if (gun)
		{
			if (as == AS_FORWARDJUMP)
			{
				if (CurrentLevel != LV_GYM)
					FinishLevelCheat = 1;
			}
			else if (as == AS_BACKJUMP)
			{
				if (CurrentLevel == LV_GYM)
				{
					Inv_AddItem(KEY_ITEM1);

					for (int i = 0; i < 50; i++)
						Inv_AddItem(FLARE_ITEM);
				}
				else
				{
					Inv_AddItem(M16_ITEM);
					Inv_AddItem(SHOTGUN_ITEM);
					Inv_AddItem(UZI_ITEM);
					Inv_AddItem(MAGNUM_ITEM);
					Inv_AddItem(GUN_ITEM);
					Inv_AddItem(ROCKET_GUN_ITEM);
					Inv_AddItem(GRENADE_GUN_ITEM);
					Inv_AddItem(HARPOON_ITEM);
					lara.magnums.ammo = 1000;
					lara.uzis.ammo = 1000;
					lara.shotgun.ammo = 1000;
					lara.harpoon.ammo = 1000;
					lara.rocket.ammo = 1000;
					lara.grenade.ammo = 1000;
					lara.m16.ammo = 1000;

					for (int i = 0; i < 50; i++)
					{
						Inv_AddItem(MEDI_ITEM);
						Inv_AddItem(BIGMEDI_ITEM);
						Inv_AddItem(FLARE_ITEM);

						if (tomb3.psx_saving)
							Inv_AddItem(SAVEGAME_CRYSTAL_ITEM);
					}
				}

				SoundEffect(SFX_LARA_HOLSTER, 0, SFX_ALWAYS);
			}
		}
		else if (as == AS_FORWARDJUMP || as == AS_BACKJUMP)
		{
			if (CurrentLevel != LV_GYM)
			{
				ExplodingDeath(lara.item_number, -1, 1);
				lara_item->hit_points = 0;
				lara_item->flags |= IFL_INVISIBLE;
			}
		}
		
		//fallthrough

	default:
		mode = 0;
	}
}

long TitleSequence()
{
	char name[128];

	T_InitPrint();
	TempVideoAdjust(1, 1.0);
	noinput_count = 0;
	dontFadePicture = 1;

#if (DIRECT3D_VERSION >= 0x900)
	RemoveMonoScreen(0);
#endif
GLuint textureID;
int width, height;

	if (tomb3.gold)
	{
		strcpy(name, GF_titlefilenames[1]);
		T3_GoldifyString(name);
		LoadPicture(name, &textureID, &width, &height);
	}
	else

		LoadPicture(GF_titlefilenames[1], &textureID, &width, &height);
	FadePictureUp(32);

	if (!title_loaded)
	{
		if (!InitialiseLevel(0, 0))
			return EXITGAME;

		title_loaded = 1;
	}

	S_CDStop();

	if (gameflow.title_track)
		S_CDPlay(gameflow.title_track, 1);

	TIME_Init();
	Display_Inventory(INV_TITLE_MODE);
	dontFadePicture = 0;
	FadePictureDown(32);
	S_CDStop();

	if (reset_flag)
	{
		reset_flag = 0;
		return STARTDEMO;
	}

	if (Inventory_Chosen == PHOTO_OPTION)
		return STARTGAME;

	if (Inventory_Chosen == PASSPORT_OPTION)
	{
		if (!Inventory_ExtraData[0])
		{
			Inv_RemoveAllItems();
			S_LoadGame(&savegame, sizeof(SAVEGAME_INFO), Inventory_ExtraData[1]);
			return Inventory_ExtraData[1] | STARTSAVEDGAME;
		}

		if (Inventory_ExtraData[0] == 1)
		{
			InitialiseStartInfo();

			if (gameflow.play_any_level)
				return STARTGAME | (Inventory_ExtraData[1] + 1);

			return STARTGAME | LV_FIRSTLEVEL;
		}
	}

	return EXITGAME;
}

long GameMain()
{
	// In GameMain(), aggiungi:
char cwd[1024];
getcwd(cwd, sizeof(cwd));
printf("Current working directory: %s\n", cwd);
    printf("[16.1] GameMain started...\n");
    
    HiResFlag = 0;
    screen_sizer = 1.0F;
    game_sizer = 1.0F;

    printf("[16.2] Initializing system...\n");
    if (!S_InitialiseSystem()) {
        printf("[16.2.1] S_InitialiseSystem failed\n");
        return 0;
    }
    printf("[16.3] System initialized successfully\n");

    printf("[16.4] Loading script file...\n");
    if (tomb3.gold)
    {
        printf("[16.4.1] Trying datag/trtla.dat...\n");
        if (!GF_LoadScriptFile("datag/trtla.dat"))
        {
            printf("[16.4.2] Trying datag/tombPC.dat...\n");
            if (!GF_LoadScriptFile("datag/tombPC.dat"))
            {
                printf("[16.4.3] Failed to load gold script files\n");
                S_ExitSystem("GameMain: could not load script file");
            }
        }
    }
    else
    {
        printf("[16.4.4] Trying data/tombPC.dat...\n");
        if (!GF_LoadScriptFile("data/tombPC.dat"))
        {
            printf("[16.4.5] Failed to load script file\n");
            S_ExitSystem("GameMain: could not load script file");
			
        }
    }
    printf("[16.5] Script file loaded successfully\n");

    printf("[16.6] Initializing sound...\n");
    SOUND_Init();
    printf("[16.7] Sound initialized\n");

    printf("[16.8] Initializing start info...\n");
    InitialiseStartInfo();
    printf("[16.9] Start info initialized\n");

    printf("[16.10] Front end check...\n");
    S_FrontEndCheck(&savegame, sizeof(SAVEGAME_INFO));
    printf("[16.11] Front end check completed\n");

    HiResFlag = -1;
    printf("[16.12] Allocating memory...\n");
    malloc_buffer = (char*)malloc(MALLOC_SIZE);

    if (!malloc_buffer)
    {
        printf("[16.13] Memory allocation failed\n");
        strcpy(exit_message, "GameMain: could not allocate malloc_buffer");
        return 0;
    }
    printf("[16.14] Memory allocated successfully\n");

    HiResFlag = 0;
    printf("[16.15] Temporary video adjust...\n");
    TempVideoAdjust(1, 1.0);
    printf("[16.16] Video adjust completed\n");

    printf("[16.17] Updating input...\n");
    S_UpdateInput();
    printf("[16.18] Input updated\n");

    printf("[16.19] Loading legal picture...\n");
    GLuint textureID;
    int width, height;
    if (tomb3.gold) {
        printf("[16.19.1] Loading pixg/legal.bmp...\n");
        LoadPicture("pixg/legal.bmp", &textureID, &width, &height);
    } else {
        printf("[16.19.2] Loading pix/legal.bmp...\n");
        LoadPicture("pix/legal.bmp", &textureID, &width, &height);
    }
    printf("[16.20] Legal picture loaded\n");

    printf("[16.21] Fading picture up...\n");
    FadePictureUp(32);
    printf("[16.22] Picture faded up\n");

    printf("[16.25] Forcing fade down...\n");
    ForceFadeDown(1);
    printf("[16.26] Fade down forced\n");

    printf("[16.27] Fading picture down...\n");
    FadePictureDown(32);
    printf("[16.28] Picture faded down\n");

    printf("[16.29] Starting front end sequence...\n");
    GF_DoFrontEndSequence();
    printf("[16.30] Front end sequence completed: %ld\n", s);

    if (GtWindowClosed) {
        printf("[16.31] Window closed, exiting\n");
        return 1;
    }

    if (s == 1)
    {
        printf("[16.32] Front end sequence failed\n");
        strcpy(exit_message, "GameMain: failed in GF_DoFrontEndSequence()");
        return 0;
    }

    printf("[16.33] Starting game loop...\n");
    s = gameflow.firstOption;
    title_loaded = 0;
    lp = 1;
    
    while (lp)
    {
        level = s & 0xFF;
        s &= ~0xFF;

        switch (s)
        {
        case STARTGAME:
            printf("[16.34] Starting game...\n");
            if (gameflow.singlelevel < 0)
            {
                if (level > gameflow.num_levels)
                {
                    sprintf(exit_message, "GameMain: STARTGAME with invalid level number (%d)", level);
                    return 0;
                }
                s = GF_DoLevelSequence(level, 1);
            }
            else
                s = GF_DoLevelSequence(gameflow.singlelevel, 1);
            break;

        case STARTSAVEDGAME:
            printf("[16.35] Starting saved game...\n");
            S_LoadGame(&savegame, sizeof(SAVEGAME_INFO), level);
            if (savegame.current_level > gameflow.num_levels)
            {
                sprintf(exit_message, "GameMain: STARTSAVEDGAME with invalid level number (%d)", savegame.current_level);
                return 0;
            }
            s = GF_DoLevelSequence(savegame.current_level, 2);
            break;

        case STARTCINE:
            printf("[16.36] Starting cinematic...\n");
            StartCinematic(level);
            s = EXIT_TO_TITLE;
            break;

        case STARTDEMO:
            printf("[16.37] Starting demo...\n");
            s = DoDemoSequence(-1);
            break;

        case EXIT_TO_TITLE:
        case EXIT_TO_OPTION:
            printf("[16.38] Exiting to title/options...\n");
            if (gameflow.title_disabled)
            {
                s = gameflow.title_replace;
                if (gameflow.title_replace < 0 || gameflow.title_replace == EXIT_TO_TITLE)
                {
                    strcpy(exit_message, "GameMain Failed: Title disabled & no replacement");
                    return 0;
                }
            }
            else
            {
                s = TitleSequence();
                GF_StartGame = 1;
            }
            break;

        case LEVELCOMPLETE:
            printf("[16.39] Level complete...\n");
            s = LevelCompleteSequence();
            break;

        case EXITGAME:
        default:
            printf("[16.40] Exiting game...\n");
            lp = 0;
            break;
        }
    }

    if (nLoadedPictures)
    {
        printf("[16.41] Cleaning up pictures...\n");
        ForceFadeDown(1);
        FadePictureDown(32);
    }

    printf("[16.42] Saving settings...\n");
    S_SaveSettings();
    
    printf("[16.43] Shutting down game...\n");
    ShutdownGame();
    
    printf("[16.44] GameMain completed successfully\n");
    return 1;
}