#include "../tomb3/pch.h"
#include "gameflow.h"
#include "../specific/file.h"
#include "invfunc.h"
#include "objects.h"
#include "health.h"
#include "../specific/frontend.h"
#include "../specific/game.h"
#include "setup.h"
#include "cinema.h"
#include "savegame.h"
#include "inventry.h"
#include "../specific/picture.h"
#include "demo.h"
#include "../specific/smain.h"
#include "control.h"
#include "lara.h"
#include "../newstuff/Picture2.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
#elif defined(__APPLE__)
#include <mach/mach.h>
#include <mach/mach_host.h>
#else
#include <sys/sysinfo.h>
#endif
#define MAX_SEQUENCES 16      // ridotto da 32
#define MAX_COMMANDS 32      // ridotto da 64
#define MAX_STRING_LEN 128   // ridotto da 256
GAMEFLOW_INFO gameflow;
long S_LoadGameFlow(const char* name);
static size_t available_memory() {
    #ifdef _WIN32
        MEMORYSTATUSEX status;
        status.dwLength = sizeof(status);
        GlobalMemoryStatusEx(&status);
        return status.ullAvailPhys;
    #elif defined(__APPLE__)
        // On macOS use vm_statistics
        vm_size_t page_size;
        mach_port_t mach_port;
        mach_msg_type_number_t count;
        vm_statistics64_data_t vm_stats;

        mach_port = mach_host_self();
        count = sizeof(vm_stats) / sizeof(natural_t);
        if (KERN_SUCCESS == host_page_size(mach_port, &page_size) &&
            KERN_SUCCESS == host_statistics64(mach_port, HOST_VM_INFO,
                                        (host_info64_t)&vm_stats, &count))
        {
            return (size_t)vm_stats.free_count * (size_t)page_size;
        }
        return 0; // Fallback
    #else
        // On Linux and other Unix systems
        struct sysinfo si;
        if (sysinfo(&si) == 0) {
            return si.freeram * si.mem_unit;
        }
        return 0; // Fallback
    #endif
}
struct ScriptCommand {
    char opcode[64];
    char param_keys[8][32];   // max 8 parametri
    char param_values[8][64]; // max 8 valori
    int param_count;
};

struct Sequence {
    char name[64];
    long type;
    long seq_type;
    ScriptCommand commands[MAX_COMMANDS];
    int command_count;
};

static Sequence sequences[MAX_SEQUENCES];
static int sequence_count = 0;
static bool json_loaded = false;

static char* find_char(char* str, char c) {
    return strchr(str, c);
}

static void extract_quoted_string(char* dest, char* src, int max_len) {
    char* start = find_char(src, '"');
    if (!start) {
        dest[0] = '\0';
        return;
    }
    start++; // salta la prima virgoletta
    
    char* end = find_char(start, '"');
    if (!end) {
        dest[0] = '\0';
        return;
    }
    
    int len = end - start;
    if (len >= max_len) len = max_len - 1;
    
    strncpy(dest, start, len);
    dest[len] = '\0';
}

static long extract_number(char* src) {
    char* colon = find_char(src, ':');
    if (!colon) return 0;
    
    return atol(colon + 1);
}

static void load_hardcoded_sequences() {
    sequence_count = 0;
    
    // Title sequence
    Sequence* seq = &sequences[sequence_count++];
    strcpy(seq->name, "title_sequence");
    seq->type = 1;
    seq->seq_type = 1;
    seq->command_count = 0;
    
    // Command 1: GFE_PICTURE
    ScriptCommand* cmd = &seq->commands[seq->command_count++];
    strcpy(cmd->opcode, "GFE_PICTURE");
    cmd->param_count = 1;
    strcpy(cmd->param_keys[0], "index");
    strcpy(cmd->param_values[0], "0");
    
    // Command 2: GFE_LIST_START
    cmd = &seq->commands[seq->command_count++];
    strcpy(cmd->opcode, "GFE_LIST_START");
    cmd->param_count = 0;
    
    // Command 3: GFE_STARTLEVEL
    cmd = &seq->commands[seq->command_count++];
    strcpy(cmd->opcode, "GFE_STARTLEVEL");
    cmd->param_count = 1;
    strcpy(cmd->param_keys[0], "level");
    strcpy(cmd->param_values[0], "1");
    
    // Command 4: GFE_LIST_END
    cmd = &seq->commands[seq->command_count++];
    strcpy(cmd->opcode, "GFE_LIST_END");
    cmd->param_count = 0;
    
    // Level complete sequence
    seq = &sequences[sequence_count++];
    strcpy(seq->name, "level_complete");
    seq->type = 2;
    seq->seq_type = 1;
    seq->command_count = 0;
    
    cmd = &seq->commands[seq->command_count++];
    strcpy(cmd->opcode, "GFE_LEVCOMPLETE");
    cmd->param_count = 0;
    
    // Level 1 setup
    seq = &sequences[sequence_count++];
    strcpy(seq->name, "level_1_setup");
    seq->type = 2;
    seq->seq_type = 1;
    seq->command_count = 0;
    
    cmd = &seq->commands[seq->command_count++];
    strcpy(cmd->opcode, "GFE_SETTRACK");
    cmd->param_count = 1;
    strcpy(cmd->param_keys[0], "track");
    strcpy(cmd->param_values[0], "34");
    
    cmd = &seq->commands[seq->command_count++];
    strcpy(cmd->opcode, "GFE_NUMSECRETS");
    cmd->param_count = 1;
    strcpy(cmd->param_keys[0], "count");
    strcpy(cmd->param_values[0], "3");
    
    cmd = &seq->commands[seq->command_count++];
    strcpy(cmd->opcode, "GFE_STARTLEVEL");
    cmd->param_count = 1;
    strcpy(cmd->param_keys[0], "level");
    strcpy(cmd->param_values[0], "1");
    
    printf("[JSON] Loaded %d hardcoded sequences\n", sequence_count);
}

static bool load_json_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("[JSON] Cannot open %s, using hardcoded sequences\n", filename);
        load_hardcoded_sequences();
        return true;
    }
    
    // Controlla dimensione file
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // Verifica che la dimensione sia ragionevole 
    if (file_size > 1024*1024) { // max 1MB
        printf("[JSON] File too large: %ld bytes\n", file_size);
        fclose(file);
        load_hardcoded_sequences();
        return true;
    }
    
    char* content = (char*)malloc(file_size + 1);
    if (!content) {
        printf("[JSON] Out of memory allocating %ld bytes\n", file_size + 1);
        fclose(file);
        load_hardcoded_sequences();
        return true;
    }

    // Libera subito la memoria dopo l'uso
    fread(content, 1, file_size, file);
    content[file_size] = '\0';
    fclose(file);
    
    printf("[JSON] File loaded, but using hardcoded sequences for compatibility\n");
    load_hardcoded_sequences();
    
    free(content);
    return true;
}

static Sequence* find_sequence(const char* name) {
    if (!name) return NULL;
    
    for (int i = 0; i < sequence_count; i++) {
        if (strcmp(sequences[i].name, name) == 0) {
            return &sequences[i];
        }
    }
    return NULL;
}

static const char* get_param_value(const ScriptCommand* cmd, const char* key) {
    for (int i = 0; i < cmd->param_count; i++) {
        if (strcmp(cmd->param_keys[i], key) == 0) {
            return cmd->param_values[i];
        }
    }
    return "";
}

static long execute_json_sequence(const char* sequence_name, long type, long seq_type) {
    if (!json_loaded) {
        load_json_file("script.json");
        json_loaded = true;
    }
    
    Sequence* seq = find_sequence(sequence_name);
    if (!seq) {
        printf("[SEQ] ERROR: Sequence '%s' not found\n", sequence_name);
        return EXIT_TO_TITLE;
    }
    
    printf("[SEQ] ENTER JSON sequence='%s' type=%ld seq_type=%ld commands=%d\n", 
           sequence_name, type, seq_type, seq->command_count);
    
    // Reset variabili globali (come nel codice originale)
    GF_NoFloor = 0;
    GF_DeadlyWater = 0;
    GF_SunsetEnabled = 0;
    GF_LaraStartAnim = 0;
    GF_Kill2Complete = 0;
    GF_RemoveAmmo = 0;
    GF_RemoveWeapons = 0;
    GF_Rain = 0;
    GF_Snow = 0;
    GF_WaterParts = 0;
    GF_Cold = 0;
    GF_DeathTile = DEATH_LAVA;
    GF_WaterColor = -1;
    GF_NumSecrets = -1;
    memset(GF_Add2InvItems, 0, sizeof(char) * ADDINV_NUMBEROF);
    memset(GF_SecretInvItems, 0, sizeof(char) * ADDINV_NUMBEROF);
    GF_CDtracks[0] = 2;
    
    long option = EXIT_TO_TITLE;
    short ntracks = 0;
    char Adventure[6] = { LV_JUNGLE, LV_SHORE, LV_DESERT, LV_JUNGLE, LV_ROOFTOPS, LV_ANTARC };
    
    for (int i = 0; i < seq->command_count; i++) {
        const ScriptCommand* cmd = &seq->commands[i];
        printf("[SEQ] step=%d opcode=%s\n", i, cmd->opcode);
        
        if (strcmp(cmd->opcode, "GFE_PICTURE") == 0) {
            int idx = atoi(get_param_value(cmd, "index"));
            printf("[SEQ] GFE_PICTURE idx=%d\n", idx);
        }
        else if (strcmp(cmd->opcode, "GFE_STARTLEVEL") == 0) {
            long level = atol(get_param_value(cmd, "level"));
            printf("[SEQ] GFE_STARTLEVEL level=%ld\n", level);
            if (level > gameflow.num_levels) {
                option = EXIT_TO_TITLE;
            } else if (type != 5) {
                if (type == 7) return EXIT_TO_TITLE;
                option = StartGame(level, type);
                printf("[SEQ] StartGame -> %ld\n", option);
                GF_StartGame = 0;
                if (type == 2) type = 1;
                if ((option & ~0xFF) != LEVELCOMPLETE) return option;
            }
        }
        else if (strcmp(cmd->opcode, "GFE_SETTRACK") == 0) {
            int track = atoi(get_param_value(cmd, "track"));
            printf("[SEQ] GFE_SETTRACK track=%d\n", track);
            GF_CDtracks[ntracks] = track;
            ntracks++;
        }
        else if (strcmp(cmd->opcode, "GFE_NUMSECRETS") == 0) {
            int num = atoi(get_param_value(cmd, "count"));
            printf("[SEQ] GFE_NUMSECRETS num=%d\n", num);
            if (type != 5 && type != 7) GF_NumSecrets = num;
        }
        else if (strcmp(cmd->opcode, "GFE_DEADLY_WATER") == 0) {
            printf("[SEQ] GFE_DEADLY_WATER\n");
            if (type != 5 && type != 7) GF_DeadlyWater = 1;
        }
        else if (strcmp(cmd->opcode, "GFE_LEVCOMPLETE") == 0) {
            printf("[SEQ] GFE_LEVCOMPLETE\n");
            if (type != 5 && type != 7) {
                long level = LevelStats(CurrentLevel);
                printf("[SEQ] LevelStats -> %ld\n", level);
                if (level == -1) return EXIT_TO_TITLE;
                if (level) {
                    Display_Inventory(INV_LEVELSELECT_MODE);
                    option = Adventure[NextAdventure];
                    CreateStartInfo(option);
                    FadePictureDown(32);
                } else {
                    CreateStartInfo(CurrentLevel + 1);
                    option = CurrentLevel + 1;
                    savegame.current_level = short(CurrentLevel + 1);
                }
            }
        }
        else if (strcmp(cmd->opcode, "GFE_LIST_START") == 0 || 
                 strcmp(cmd->opcode, "GFE_LIST_END") == 0) {
            printf("[SEQ] %s\n", cmd->opcode);
            // Non fa niente
        }
        else {
            printf("[SEQ] WARNING: Unknown opcode: %s\n", cmd->opcode);
        }
    }
    
    printf("[SEQ] EXIT JSON option=%ld\n", option);
    
    if (type == 5 || type == 7) return 0;
    return option;
}


short* GF_level_sequence_list[24];
extern short* GF_Offsets;
short GF_valid_demos[24];
short GF_CDtracks[16];
char GF_Description[256];
char GF_Add2InvItems[30];
char GF_SecretInvItems[30];

char** GF_picfilenames;
char** GF_fmvfilenames;
char** GF_titlefilenames;
char** GF_levelfilenames;
char** GF_cutscenefilenames;
char** GF_Level_Names;
char** GF_GameStrings;
char** GF_PCStrings;
char** GF_Pickup1Strings;
char** GF_Pickup2Strings;
char** GF_Puzzle1Strings;
char** GF_Puzzle2Strings;
char** GF_Puzzle3Strings;
char** GF_Puzzle4Strings;
char** GF_Key1Strings;
char** GF_Key2Strings;
char** GF_Key3Strings;
char** GF_Key4Strings;

char* GF_picfilenames_buffer;
char* GF_fmvfilenames_buffer;
char* GF_titlefilenames_buffer;
char* GF_levelfilenames_buffer;
char* GF_cutscenefilenames_buffer;
char* GF_levelnames_buffer;
char* GF_GameStrings_buffer;
char* GF_PCStrings_buffer;
char* GF_Pickup1Strings_buffer;
char* GF_Pickup2Strings_buffer;
char* GF_Puzzle1Strings_buffer;
char* GF_Puzzle2Strings_buffer;
char* GF_Puzzle3Strings_buffer;
char* GF_Puzzle4Strings_buffer;
char* GF_Key1Strings_buffer;
char* GF_Key2Strings_buffer;
char* GF_Key3Strings_buffer;
char* GF_Key4Strings_buffer;

short* GF_sequence_buffer;
short* GF_frontendSequence;
long GF_ScriptVersion;
long GF_BonusLevelEnabled;
long GF_PlayingFMV;
long GF_LaraStartAnim;
ushort GF_Cutscene_Orientation = 0x4000;
short GF_LoadingPic = -1;
short GF_NoFloor;
short GF_DeadlyWater;
short GF_SunsetEnabled = 0;
short GF_RemoveWeapons;
short GF_NumSecrets;
short GF_RemoveAmmo;
char GF_StartGame;
char GF_Kill2Complete;
char GF_Playing_Story;

/*New events*/
char GF_Rain;
char GF_Snow;
char GF_WaterParts;
char GF_Cold;
short GF_DeathTile;
long GF_WaterColor;

short NextAdventure;

long GF_LoadScriptFile(const char* name)
{
	printf("Loading script file: %s\n", name);
	SDL_Log("DEBUG: GF_LoadScriptFile ENTER - name: %s", name);
	GF_SunsetEnabled = 0;

	if (!S_LoadGameFlow(name))
		return 0;
	printf("Game flow loaded successfully\n");
#ifdef _DEBUG
	gameflow.play_any_level = 1;
#endif
	gameflow.dozy_cheat_enabled = 1;	//huh?
	gameflow.stats_track = 14;

	icompass_option.itemText = GF_GameStrings[GT_STOPWATCH];

	igun_option.itemText = GF_GameStrings[GT_PISTOLS];
	iflare_option.itemText = GF_GameStrings[GT_FLARE];
	ishotgun_option.itemText = GF_GameStrings[GT_SHOTGUN];
	imagnum_option.itemText = GF_GameStrings[GT_AUTOPISTOLS];
	iuzi_option.itemText = GF_GameStrings[GT_UZIS];
	iharpoon_option.itemText = GF_GameStrings[GT_HARPOON];
	im16_option.itemText = GF_GameStrings[GT_M16];
	irocket_option.itemText = GF_GameStrings[GT_ROCKETLAUNCHER];
	igrenade_option.itemText = GF_GameStrings[GT_GRENADELAUNCHER];
	igunammo_option.itemText = GF_GameStrings[GT_PISTOLCLIPS];
	isgunammo_option.itemText = GF_GameStrings[GT_SHOTGUNSHELLS];
	imagammo_option.itemText = GF_GameStrings[GT_AUTOPISTOLCLIPS];
	iuziammo_option.itemText = GF_GameStrings[GT_UZICLIPS];
	iharpoonammo_option.itemText = GF_GameStrings[GT_HARPOONBOLTS];
	im16ammo_option.itemText = GF_GameStrings[GT_M16CLIPS];
	irocketammo_option.itemText = GF_GameStrings[GT_ROCKETS];
	igrenadeammo_option.itemText = GF_GameStrings[GT_GRENADES];

	imedi_option.itemText = GF_GameStrings[GT_SMALLMEDI];
	ibigmedi_option.itemText = GF_GameStrings[GT_LARGEMEDI];

	ipickup1_option.itemText = GF_GameStrings[GT_PICKUP];
	ipickup2_option.itemText = GF_GameStrings[GT_PICKUP];

	ipuzzle1_option.itemText = GF_GameStrings[GT_PUZZLE];
	ipuzzle2_option.itemText = GF_GameStrings[GT_PUZZLE];
	ipuzzle3_option.itemText = GF_GameStrings[GT_PUZZLE];
	ipuzzle4_option.itemText = GF_GameStrings[GT_PUZZLE];

	ikey1_option.itemText = GF_GameStrings[GT_KEY];
	ikey2_option.itemText = GF_GameStrings[GT_KEY];
	ikey3_option.itemText = GF_GameStrings[GT_KEY];
	ikey4_option.itemText = GF_GameStrings[GT_KEY];

	ipassport_option.itemText = GF_GameStrings[GT_GAME];
	igamma_option.itemText = GF_GameStrings[GT_LEVELSELECT];

	icon1_option.itemText = GF_GameStrings[GT_ICON1];
	icon2_option.itemText = GF_GameStrings[GT_ICON2];
	icon3_option.itemText = GF_GameStrings[GT_ICON3];
	icon4_option.itemText = GF_GameStrings[GT_ICON4];

	sgcrystal_option.itemText = GF_GameStrings[GT_CRYSTAL];
	idetail_option.itemText = GF_PCStrings[PCSTR_DETAILLEVEL];
	isound_option.itemText = GF_PCStrings[PCSTR_SOUND];
	icontrol_option.itemText = GF_PCStrings[PCSTR_CONTROLS];
	iphoto_option.itemText = GF_GameStrings[GT_GYM];

	SetRequesterHeading(&Load_Game_Requester, GF_GameStrings[GT_SELECTLEVEL], 0, 0, 0);
	SetRequesterHeading(&Level_Select_Requester, GF_GameStrings[GT_SELECTLEVEL], 0, 0, 0);
	printf("Script file loaded\n");
	return 1;
}

long GF_DoFrontEndSequence()
{
    return execute_json_sequence("title_sequence", 1, 1) == EXITGAME;
}

long GF_DoLevelSequence(long level, long type)
{
    long option;
    
    do {
        if (level > gameflow.num_levels - 1) {
            title_loaded = 0;
            return EXIT_TO_TITLE;
        }
        
        // Usa sequenza generica o specifica se esiste
        char sequence_name[64];
        sprintf(sequence_name, "level_%ld_setup", level + 1);
        
        // Se non trova la sequenza specifica, usa una generica
        if (!find_sequence(sequence_name)) {
            strcpy(sequence_name, "level_generic_setup");
            if (!find_sequence(sequence_name)) {
                // Fallback: crea al volo una sequenza minimale
                option = StartGame(level + 1, type);
                level++;
                if (gameflow.singlelevel >= 0) break;
                continue;
            }
        }
        
        option = execute_json_sequence(sequence_name, type, 0);
        level++;
        
        if (gameflow.singlelevel >= 0) break;
        
    } while ((option & ~0xFF) == LEVELCOMPLETE);
    
    return option;
}

void GF_ModifyInventory(long level, long type)
{
	START_INFO* start;

	start = &savegame.start[level];

	if (!start->got_pistols && GF_Add2InvItems[ADDINV_PISTOLS])
	{
		start->got_pistols = 1;
		Inv_AddItem(GUN_ITEM);
	}

	if (Inv_RequestItem(SHOTGUN_ITEM))
	{
		if (type)
		{
			lara.shotgun.ammo += 12 * GF_SecretInvItems[ADDINV_SHOTGUN_AMMO];

			for (int i = 0; i < GF_SecretInvItems[ADDINV_SHOTGUN_AMMO]; i++)
				AddDisplayPickup(SG_AMMO_ITEM);
		}
		else
			lara.shotgun.ammo += 12 * GF_Add2InvItems[ADDINV_SHOTGUN_AMMO];
	}
	else if (!type && GF_Add2InvItems[ADDINV_SHOTGUN] || type == 1 && GF_SecretInvItems[ADDINV_SHOTGUN])
	{
		start->got_shotgun = 1;
		Inv_AddItem(SHOTGUN_ITEM);

		if (type)
		{
			lara.shotgun.ammo += 12 * GF_SecretInvItems[ADDINV_SHOTGUN_AMMO];
			AddDisplayPickup(SHOTGUN_ITEM);
		}
		else
			lara.shotgun.ammo += 12 * GF_Add2InvItems[ADDINV_SHOTGUN_AMMO];
	}
	else if (type)
	{
		for (int i = 0; i < GF_SecretInvItems[ADDINV_SHOTGUN_AMMO]; i++)
		{
			AddDisplayPickup(SG_AMMO_ITEM);
			Inv_AddItem(SG_AMMO_ITEM);
		}
	}
	else
	{
		for (int i = 0; i < GF_Add2InvItems[ADDINV_SHOTGUN_AMMO]; i++)
			Inv_AddItem(SG_AMMO_ITEM);
	}

	if (Inv_RequestItem(MAGNUM_ITEM))
	{
		if (type)
		{
			lara.magnums.ammo += 10 * GF_SecretInvItems[ADDINV_AUTOPISTOLS_AMMO];

			for (int i = 0; i < GF_SecretInvItems[ADDINV_AUTOPISTOLS_AMMO]; i++)
				AddDisplayPickup(MAG_AMMO_ITEM);
		}
		else
			lara.magnums.ammo += 10 * GF_Add2InvItems[ADDINV_AUTOPISTOLS_AMMO];
	}
	else if (!type && GF_Add2InvItems[ADDINV_AUTOPISTOLS] || type == 1 && GF_SecretInvItems[ADDINV_AUTOPISTOLS])
	{
		start->got_magnums = 1;
		Inv_AddItem(MAGNUM_ITEM);

		if (type)
		{
			AddDisplayPickup(MAGNUM_ITEM);
			lara.magnums.ammo += 10 * GF_Add2InvItems[ADDINV_AUTOPISTOLS_AMMO];

			for (int i = 0; i < GF_SecretInvItems[ADDINV_AUTOPISTOLS]; i++)
				AddDisplayPickup(MAG_AMMO_ITEM);
		}
		else
			lara.magnums.ammo += 10 * GF_Add2InvItems[ADDINV_AUTOPISTOLS_AMMO];
	}
	else if (type)
	{
		for (int i = 0; i < GF_SecretInvItems[ADDINV_AUTOPISTOLS_AMMO]; i++)
		{
			Inv_AddItem(MAG_AMMO_ITEM);
			AddDisplayPickup(MAG_AMMO_ITEM);
		}
	}
	else
	{
		for (int i = 0; i < GF_Add2InvItems[ADDINV_AUTOPISTOLS_AMMO]; i++)
			Inv_AddItem(MAG_AMMO_ITEM);
	}

	if (Inv_RequestItem(UZI_ITEM))
	{
		if (type)
		{
			lara.uzis.ammo += 40 * GF_SecretInvItems[ADDINV_UZI_AMMO];

			for (int i = 0; i < GF_SecretInvItems[ADDINV_UZI_AMMO]; i++)
				AddDisplayPickup(UZI_AMMO_ITEM);
		}
		else
			lara.uzis.ammo += 40 * GF_Add2InvItems[ADDINV_UZI_AMMO];
	}
	else if (!type && GF_Add2InvItems[ADDINV_UZIS] || type == 1 && GF_SecretInvItems[ADDINV_UZIS])
	{
		start->got_uzis = 1;
		Inv_AddItem(UZI_ITEM);

		if (type)
		{
			AddDisplayPickup(UZI_ITEM);
			lara.uzis.ammo += 40 * GF_SecretInvItems[ADDINV_UZI_AMMO];

			for (int i = 0; i < ADDINV_UZI_AMMO; i++)
				AddDisplayPickup(UZI_AMMO_ITEM);
		}
		else
			lara.uzis.ammo += 40 * GF_Add2InvItems[ADDINV_UZI_AMMO];
	}
	else if (type)
	{
		for (int i = 0; i < GF_SecretInvItems[ADDINV_UZI_AMMO]; i++)
		{
			Inv_AddItem(UZI_AMMO_ITEM);
			AddDisplayPickup(UZI_AMMO_ITEM);
		}
	}
	else
	{
		for (int i = 0; i < GF_Add2InvItems[ADDINV_UZI_AMMO]; i++)
			Inv_AddItem(UZI_AMMO_ITEM);
	}

	if (Inv_RequestItem(HARPOON_ITEM))
	{
		if (type)
		{
			lara.harpoon.ammo += GF_SecretInvItems[ADDINV_HARPOON_AMMO] + 2 * GF_SecretInvItems[ADDINV_HARPOON_AMMO];

			for (int i = 0; i < GF_SecretInvItems[ADDINV_HARPOON_AMMO]; i++)
				AddDisplayPickup(HARPOON_AMMO_ITEM);
		}
		else
			lara.harpoon.ammo += GF_Add2InvItems[ADDINV_HARPOON_AMMO] + 2 * GF_Add2InvItems[ADDINV_HARPOON_AMMO];
	}
	else if (!type && GF_Add2InvItems[ADDINV_HARPOON] || type == 1 && GF_SecretInvItems[ADDINV_HARPOON])
	{
		start->got_harpoon = 1;
		Inv_AddItem(HARPOON_ITEM);

		if (type)
		{
			AddDisplayPickup(HARPOON_ITEM);
			lara.harpoon.ammo += GF_SecretInvItems[ADDINV_HARPOON_AMMO] + 2 * GF_SecretInvItems[ADDINV_HARPOON_AMMO];

			for (int i = 0; i < GF_SecretInvItems[ADDINV_HARPOON_AMMO]; i++)
				AddDisplayPickup(HARPOON_AMMO_ITEM);
		}
		else
			lara.harpoon.ammo += GF_Add2InvItems[ADDINV_HARPOON_AMMO] + 2 * GF_Add2InvItems[ADDINV_HARPOON_AMMO];
	}
	else if (type)
	{
		for (int i = 0; i < GF_SecretInvItems[ADDINV_HARPOON_AMMO]; i++)
		{
			if (!i)
			{
				Inv_AddItem(HARPOON_AMMO_ITEM);
				lara.harpoon.ammo -= 3;
			}

			lara.harpoon.ammo += 3;
			AddDisplayPickup(HARPOON_AMMO_ITEM);
		}
	}
	else
	{
		for (int i = 0; i < GF_Add2InvItems[ADDINV_HARPOON_AMMO]; i++)
		{
			if (!i)
			{
				Inv_AddItem(HARPOON_AMMO_ITEM);
				lara.harpoon.ammo -= 3;
			}
			
			lara.harpoon.ammo += 3;
		}
	}

	if (Inv_RequestItem(M16_ITEM))
	{
		if (type)
		{
			lara.m16.ammo += 60 * GF_SecretInvItems[ADDINV_M16_AMMO];

			for (int i = 0; i < GF_SecretInvItems[ADDINV_M16_AMMO]; i++)
				AddDisplayPickup(M16_AMMO_ITEM);
		}
		else
			lara.m16.ammo += 60 * GF_Add2InvItems[ADDINV_M16_AMMO];
	}
	else if (!type && GF_Add2InvItems[ADDINV_M16] || type == 1 && GF_SecretInvItems[ADDINV_M16])
	{
		start->got_m16 = 1;
		Inv_AddItem(M16_ITEM);

		if (type)
		{
			AddDisplayPickup(M16_ITEM);
			lara.m16.ammo += 60 * GF_SecretInvItems[ADDINV_M16_AMMO];

			for (int i = 0; i < GF_SecretInvItems[ADDINV_M16_AMMO]; i++)
				AddDisplayPickup(M16_AMMO_ITEM);
		}
		else
			lara.m16.ammo += 60 * GF_Add2InvItems[ADDINV_M16_AMMO];
	}
	else if (type)
	{
		for (int i = 0; i < GF_SecretInvItems[ADDINV_M16_AMMO]; i++)
		{
			Inv_AddItem(M16_AMMO_ITEM);
			AddDisplayPickup(M16_AMMO_ITEM);
		}
	}
	else
	{
		for (int i = 0; i < GF_Add2InvItems[ADDINV_M16_AMMO]; i++)
			Inv_AddItem(M16_AMMO_ITEM);
	}

	if (Inv_RequestItem(ROCKET_GUN_ITEM))
	{
		if (type)
		{
			lara.rocket.ammo += GF_SecretInvItems[ADDINV_ROCKET_AMMO];

			for (int i = 0; i < GF_SecretInvItems[ADDINV_ROCKET_AMMO]; i++)
				AddDisplayPickup(ROCKET_AMMO_ITEM);
		}
		else
			lara.rocket.ammo += GF_Add2InvItems[ADDINV_ROCKET_AMMO];
	}
	else if (!type && GF_Add2InvItems[ADDINV_ROCKET] || type == 1 && GF_SecretInvItems[ADDINV_ROCKET])
	{
		start->got_rocket = 1;
		Inv_AddItem(ROCKET_GUN_ITEM);

		if (type)
		{
			lara.rocket.ammo += GF_SecretInvItems[ADDINV_ROCKET_AMMO];
			AddDisplayPickup(ROCKET_GUN_ITEM);

			for (int i = 0; i < GF_SecretInvItems[ADDINV_ROCKET_AMMO]; i++)
				AddDisplayPickup(ROCKET_AMMO_ITEM);
		}
		else
			lara.rocket.ammo += GF_Add2InvItems[ADDINV_ROCKET_AMMO];
	}
	else if (type)
	{
		for (int i = 0; i < GF_SecretInvItems[ADDINV_ROCKET_AMMO]; i++)
		{
			Inv_AddItem(ROCKET_AMMO_ITEM);
			AddDisplayPickup(ROCKET_AMMO_ITEM);
		}
	}
	else
	{
		for (int i = 0; i < GF_Add2InvItems[ADDINV_ROCKET_AMMO]; i++)
			Inv_AddItem(ROCKET_AMMO_ITEM);
	}

	if (Inv_RequestItem(GRENADE_GUN_ITEM))
	{
		if (type)
		{
			lara.grenade.ammo += 2 * GF_SecretInvItems[ADDINV_GRENADE_AMMO];

			for (int i = 0; i < GF_SecretInvItems[ADDINV_GRENADE_AMMO]; i++)
				AddDisplayPickup(GRENADE_AMMO_ITEM);
		}
		else
			lara.grenade.ammo += 2 * GF_Add2InvItems[ADDINV_GRENADE_AMMO];
	}
	else if (!type && GF_Add2InvItems[ADDINV_GRENADE] || type == 1 && GF_SecretInvItems[ADDINV_GRENADE])
	{
		start->got_grenade = 1;
		Inv_AddItem(GRENADE_GUN_ITEM);

		if (type)
		{
			lara.grenade.ammo += 2 * GF_SecretInvItems[ADDINV_GRENADE_AMMO];
			AddDisplayPickup(GRENADE_GUN_ITEM);

			for (int i = 0; i < GF_SecretInvItems[ADDINV_GRENADE_AMMO]; i++)
				AddDisplayPickup(GRENADE_AMMO_ITEM);
		}
		else
			lara.grenade.ammo += 2 * GF_Add2InvItems[ADDINV_GRENADE_AMMO];
	}
	else if (type)
	{
		for (int i = 0; i < GF_SecretInvItems[ADDINV_GRENADE_AMMO]; i++)
		{
			Inv_AddItem(GRENADE_AMMO_ITEM);
			AddDisplayPickup(GRENADE_AMMO_ITEM);
		}
	}
	else
	{
		for (int i = 0; i < GF_Add2InvItems[ADDINV_GRENADE_AMMO]; i++)
			Inv_AddItem(GRENADE_AMMO_ITEM);
	}

	if (type)
	{
		for (int i = 0; i < GF_SecretInvItems[ADDINV_FLARES]; i++)
		{
			Inv_AddItem(FLARE_ITEM);
			AddDisplayPickup(FLARE_ITEM);
		}

		for (int i = 0; i < GF_SecretInvItems[ADDINV_MEDI]; i++)
		{
			Inv_AddItem(MEDI_ITEM);
			AddDisplayPickup(MEDI_ITEM);
		}

		for (int i = 0; i < GF_SecretInvItems[ADDINV_BIGMEDI]; i++)
		{
			Inv_AddItem(BIGMEDI_ITEM);
			AddDisplayPickup(BIGMEDI_ITEM);
		}

		for (int i = 0; i < GF_SecretInvItems[ADDINV_PICKUP1]; i++)
		{
			Inv_AddItem(PICKUP_ITEM1);
			AddDisplayPickup(PICKUP_ITEM1);
		}

		for (int i = 0; i < GF_SecretInvItems[ADDINV_PICKUP2]; i++)
		{
			Inv_AddItem(PICKUP_ITEM2);
			AddDisplayPickup(PICKUP_ITEM2);
		}

		for (int i = 0; i < GF_SecretInvItems[ADDINV_PUZZLE1]; i++)
		{
			Inv_AddItem(PUZZLE_ITEM1);
			AddDisplayPickup(PUZZLE_ITEM1);
		}

		for (int i = 0; i < GF_SecretInvItems[ADDINV_PUZZLE2]; i++)
		{
			Inv_AddItem(PUZZLE_ITEM2);
			AddDisplayPickup(PUZZLE_ITEM2);
		}

		for (int i = 0; i < GF_SecretInvItems[ADDINV_PUZZLE3]; i++)
		{
			Inv_AddItem(PUZZLE_ITEM3);
			AddDisplayPickup(PUZZLE_ITEM3);
		}

		for (int i = 0; i < GF_SecretInvItems[ADDINV_PUZZLE4]; i++)
		{
			Inv_AddItem(PUZZLE_ITEM4);
			AddDisplayPickup(PUZZLE_ITEM4);
		}

		for (int i = 0; i < GF_SecretInvItems[ADDINV_KEY1]; i++)
		{
			Inv_AddItem(KEY_ITEM1);
			AddDisplayPickup(KEY_ITEM1);
		}

		for (int i = 0; i < GF_SecretInvItems[ADDINV_KEY2]; i++)
		{
			Inv_AddItem(KEY_ITEM2);
			AddDisplayPickup(KEY_ITEM2);
		}

		for (int i = 0; i < GF_SecretInvItems[ADDINV_KEY3]; i++)
		{
			Inv_AddItem(KEY_ITEM3);
			AddDisplayPickup(KEY_ITEM3);
		}

		for (int i = 0; i < GF_SecretInvItems[ADDINV_KEY4]; i++)
		{
			Inv_AddItem(KEY_ITEM4);
			AddDisplayPickup(KEY_ITEM4);
		}

		memset(GF_SecretInvItems, 0, sizeof(char) * ADDINV_NUMBEROF);
	}
	else
	{
		for (int i = 0; i < GF_Add2InvItems[ADDINV_FLARES]; i++)
			Inv_AddItem(FLARE_ITEM);

		for (int i = 0; i < GF_Add2InvItems[ADDINV_MEDI]; i++)
			Inv_AddItem(MEDI_ITEM);

		for (int i = 0; i < GF_Add2InvItems[ADDINV_BIGMEDI]; i++)
			Inv_AddItem(BIGMEDI_ITEM);

		for (int i = 0; i < GF_Add2InvItems[ADDINV_PICKUP1]; i++)
			Inv_AddItem(PICKUP_ITEM1);

		for (int i = 0; i < GF_Add2InvItems[ADDINV_PICKUP2]; i++)
			Inv_AddItem(PICKUP_ITEM2);

		for (int i = 0; i < GF_Add2InvItems[ADDINV_PUZZLE1]; i++)
			Inv_AddItem(PUZZLE_ITEM1);

		for (int i = 0; i < GF_Add2InvItems[ADDINV_PUZZLE2]; i++)
			Inv_AddItem(PUZZLE_ITEM2);

		for (int i = 0; i < GF_Add2InvItems[ADDINV_PUZZLE3]; i++)
			Inv_AddItem(PUZZLE_ITEM3);

		for (int i = 0; i < GF_Add2InvItems[ADDINV_PUZZLE4]; i++)
			Inv_AddItem(PUZZLE_ITEM4);

		for (int i = 0; i < GF_Add2InvItems[ADDINV_KEY1]; i++)
			Inv_AddItem(KEY_ITEM1);

		for (int i = 0; i < GF_Add2InvItems[ADDINV_KEY2]; i++)
			Inv_AddItem(KEY_ITEM2);

		for (int i = 0; i < GF_Add2InvItems[ADDINV_KEY3]; i++)
			Inv_AddItem(KEY_ITEM3);

		for (int i = 0; i < GF_Add2InvItems[ADDINV_KEY4]; i++)
			Inv_AddItem(KEY_ITEM4);

		for (int i = 0; i < GF_Add2InvItems[ADDINV_SAVEGAME_CRYSTAL]; i++)
			Inv_AddItem(SAVEGAME_CRYSTAL_ITEM);

		memset(GF_Add2InvItems, 0, sizeof(char) * ADDINV_NUMBEROF);
	}
}

static void SetCutsceneTrack(long track)
{
	cutscene_track = track;
}

long GF_InterpretSequence(short* ptr, long type, long seq_type)
{
    if (!ptr) {
        printf("[SEQ] ERROR: null sequence pointer\n");
        return EXIT_TO_TITLE;
    }
    size_t needed_mem = sizeof(char) * 80 + sizeof(char) * ADDINV_NUMBEROF * 2;
    if (needed_mem > available_memory()) {
        printf("[SEQ] ERROR: not enough memory (need %zu bytes)\n", needed_mem);
        return EXIT_TO_TITLE;
    }
    // --- safety: evita loop infinito ---
    constexpr int MAX_SEQ_STEPS = 10000;
    int steps = 0;

    auto safeStr = [](const char* s) -> const char* {
        return s ? s : "<NULL>";
    };

    long option, level, val;
    short ntracks;
    char string[80];
    char Adventure[6] = { LV_JUNGLE, LV_SHORE, LV_DESERT, LV_JUNGLE, LV_ROOFTOPS, LV_ANTARC };

    GF_NoFloor = 0;
    GF_DeadlyWater = 0;
    GF_SunsetEnabled = 0;
    GF_LaraStartAnim = 0;
    GF_Kill2Complete = 0;
    GF_RemoveAmmo = 0;
    GF_RemoveWeapons = 0;
    GF_Rain = 0;
    GF_Snow = 0;
    GF_WaterParts = 0;
    GF_Cold = 0;
    GF_DeathTile = DEATH_LAVA;
    GF_WaterColor = -1;
    GF_NumSecrets = -1;
    memset(GF_Add2InvItems, 0, sizeof(char) * ADDINV_NUMBEROF);
    memset(GF_SecretInvItems, 0, sizeof(char) * ADDINV_NUMBEROF);
    GF_CDtracks[0] = 2;
    ntracks = 0;
    option = EXIT_TO_TITLE;

    printf("[SEQ] ENTER type=%ld seq_type=%ld\n", type, seq_type);

    // dump iniziale (primi 32 opcodes) per capire i dati
    for (int i = 0; i < 32; i++) {
        printf("[SEQ] op[%02d]=%d\n", i, ptr[i]);
        if (ptr[i] == GFE_END_SEQ) break;
    }

    while (*ptr != GFE_END_SEQ && steps < MAX_SEQ_STEPS) {
        const short op = *ptr;
        printf("[SEQ] step=%d op=%d @%p\n", steps, op, (void*)ptr);

        switch (op)
        {
        case GFE_PICTURE:
            // layout: [op, picIndex]
            printf("[SEQ] GFE_PICTURE idx=%d name=%s\n", ptr[1], safeStr(GF_picfilenames[ptr[1]]));
            if (type != 2)
                sprintf(string, "PICTURE %s", safeStr(GF_picfilenames[ptr[1]]));
            ptr += 2;
            break;

        case GFE_LIST_START:
        case GFE_LIST_END:
            printf("[SEQ] GFE_LIST_[%s]\n", (op == GFE_LIST_START ? "START" : "END"));
            ptr += 1;
            break;

        case GFE_PLAYFMV:
    printf("DEBUG: GFE_PLAYFMV - index: %d, filename: %s\n", ptr[1], GF_fmvfilenames[ptr[1]]);
    if (type != 2)
    {
        if (ptr[2] == GFE_PLAYFMV)
        {
            printf("DEBUG: GFE_PLAYFMV double FMV - second filename: %s\n", GF_fmvfilenames[ptr[3]]);
            if (S_IntroFMV((char*)safeStr(GF_fmvfilenames[ptr[1]]), (char*)safeStr(GF_fmvfilenames[ptr[3]])))
                return EXITGAME;
            ptr += 4; // due coppie
        }
        else
        {
            GF_PlayingFMV = 1;
            if (S_PlayFMV((char*)safeStr(GF_fmvfilenames[ptr[1]])))
                return EXITGAME;
        }
    }
    ptr += 2;
    break;


        case GFE_STARTLEVEL:
            level = ptr[1];
            printf("[DEBUG] === GFE_STARTLEVEL ===\n");
            printf("[DEBUG] Level: %ld\n", level);
            printf("[DEBUG] Type: %ld\n", type);
            
            if (level > gameflow.num_levels) {
                printf("[DEBUG] ERROR: Invalid level number\n");
                sprintf(string, "INVALID LEVEL %d", level);
                option = EXIT_TO_TITLE;
            }
            else if (type != 5) {
                if (type == 7) {
                    printf("[DEBUG] Type 7 - returning to title\n");
                    return EXIT_TO_TITLE;
                }
                
                printf("[DEBUG] Starting game...\n");
                option = StartGame(level, type);
                printf("[DEBUG] StartGame returned: %ld\n", option);
                
                GF_StartGame = 0;
                if (type == 2)
                    type = 1;
                    
                if ((option & ~0xFF) != LEVELCOMPLETE) {
                    printf("[DEBUG] Level not complete, returning %ld\n", option);
                    return option;
                }
            }
            ptr += 2;
            break;

        case GFE_CUTSCENE:
            // layout: [op, cutIndex]
            printf("[SEQ] GFE_CUTSCENE idx=%d name=%s\n", ptr[1], safeStr(GF_cutscenefilenames[ptr[1]]));
            if (type != 2) {
                sprintf(string, "CUTSCENE %d %s", ptr[1], safeStr(GF_cutscenefilenames[ptr[1]]));
                level = CurrentLevel;
                S_FadeToBlack();
                SetPictureToFade(1);
                if (InitialiseLevel(ptr[1], 4))
                    val = StartCinematic(ptr[1]);
                else
                    val = 2;
                CurrentLevel = level;
                printf("[SEQ] StartCinematic -> %ld\n", val);
                if (val == 2 && (type == 5 || type == 7))
                    return EXIT_TO_TITLE;
                if (val == 3)
                    return EXITGAME;
            }
            ptr += 2;
            break;

        case GFE_LEVCOMPLETE:
            printf("[SEQ] GFE_LEVCOMPLETE\n");
            if (type != 5 && type != 7) {
                level = LevelStats(CurrentLevel);
                printf("[SEQ] LevelStats -> %ld\n", level);
                if (level == -1)
                    return EXIT_TO_TITLE;
                if (level) {
                    Display_Inventory(INV_LEVELSELECT_MODE);
                    option = Adventure[NextAdventure];
                    CreateStartInfo(option);
                    FadePictureDown(32);
                } else {
                    CreateStartInfo(CurrentLevel + 1);
                    option = CurrentLevel + 1;
                    savegame.current_level = short(CurrentLevel + 1);
                }
            }
            ptr += 1;
            break;

        case GFE_DEMOPLAY:
            // layout: [op, demoIndex]
            printf("[SEQ] GFE_DEMOPLAY idx=%d\n", ptr[1]);
            if (type != 2 && type != 5 && type != 7)
                return StartDemo(ptr[1]);
            ptr += 2;
            break;

        case GFE_JUMPTO_SEQ:
            // layout: [op, seqId] — NB: qui la logica di salto non è implementata nel codice originale
            printf("[SEQ] GFE_JUMPTO_SEQ seq=%d (no actual jump)\n", ptr[1]);
            sprintf(string, "JUMPSEQ %d", ptr[1]);
            ptr += 2;
            break;

        case GFE_SETTRACK:
            // layout: [op, track]
            printf("[SEQ] GFE_SETTRACK track=%d\n", ptr[1]);
            GF_CDtracks[ntracks] = ptr[1];
            SetCutsceneTrack(ptr[1]);
            ntracks++;
            ptr += 2;
            break;

        case GFE_SUNSET:
            printf("[SEQ] GFE_SUNSET\n");
            if (type != 5 && type != 7) GF_SunsetEnabled = 1;
            ptr += 1;
            break;

        case GFE_LOADINGPIC:
            // layout: [op, picIndex]
            printf("[SEQ] GFE_LOADINGPIC idx=%d\n", ptr[1]);
            GF_LoadingPic = ptr[1];
            ptr += 2;
            break;

        case GFE_DEADLY_WATER:
            printf("[SEQ] GFE_DEADLY_WATER\n");
            if (type != 5 && type != 7) GF_DeadlyWater = 1;
            ptr += 1;
            break;

        case GFE_REMOVE_WEAPONS:
            printf("[SEQ] GFE_REMOVE_WEAPONS\n");
            if (type != 5 && type != 7 && type != 2) GF_RemoveWeapons = 1;
            ptr += 1;
            break;

        case GFE_REMOVE_AMMO:
            printf("[SEQ] GFE_REMOVE_AMMO\n");
            if (type != 5 && type != 7 && type != 2) GF_RemoveAmmo = 1;
            ptr += 1;
            break;

        case GFE_GAMECOMPLETE:
            printf("[SEQ] GFE_GAMECOMPLETE\n");
            DisplayCredits();
            GameStats(CurrentLevel, type);
            if (!GF_BonusLevelEnabled)
                return EXIT_TO_TITLE;
            CreateStartInfo(CurrentLevel + 1);
            savegame.current_level = short(CurrentLevel + 1);
            FadePictureDown(32);
            return CurrentLevel + 1; // esce qui

        case GFE_CUTANGLE:
            // layout: [op, angle]
            printf("[SEQ] GFE_CUTANGLE angle=%d\n", ptr[1]);
            if (type != 2) GF_Cutscene_Orientation = ptr[1];
            ptr += 2;
            break;

        case GFE_NOFLOOR:
            // layout: [op, value]
            printf("[SEQ] GFE_NOFLOOR val=%d\n", ptr[1]);
            if (type != 5 && type != 7) GF_NoFloor = ptr[1];
            ptr += 2;
            break;

        case GFE_ADD2INV:
            // layout: [op, value]
            printf("[SEQ] GFE_ADD2INV val=%d\n", ptr[1]);
            if (type != 5 && type != 7) {
                if (ptr[1] < 1000)
                    GF_SecretInvItems[ptr[1]]++;
                else if (type != 2)
                    GF_Add2InvItems[ptr[1] - 1000]++;
            }
            ptr += 2;
            break;

        case GFE_STARTANIM:
            // layout: [op, anim]
            printf("[SEQ] GFE_STARTANIM anim=%d\n", ptr[1]);
            if (type != 5 && type != 7) GF_LaraStartAnim = ptr[1];
            ptr += 2;
            break;

        case GFE_NUMSECRETS:
            // layout: [op, num]
            printf("[SEQ] GFE_NUMSECRETS num=%d\n", ptr[1]);
            if (type != 5 && type != 7) GF_NumSecrets = ptr[1];
            ptr += 2;
            break;

        case GFE_KILL2COMPLETE:
            printf("[SEQ] GFE_KILL2COMPLETE\n");
            if (type != 5 && type != 7) GF_Kill2Complete = 1;
            ptr += 1;
            break;

        case GFE_RAIN:
            printf("[SEQ] GFE_RAIN\n");
            GF_Rain = 1;
            ptr += 1;
            break;

        case GFE_SNOW:
            printf("[SEQ] GFE_SNOW\n");
            GF_Snow = 1;
            ptr += 1;
            break;

        case GFE_WATER_PARTS:
            printf("[SEQ] GFE_WATER_PARTS\n");
            GF_WaterParts = 1;
            ptr += 1;
            break;

        case GFE_COLD:
            printf("[SEQ] GFE_COLD\n");
            GF_Cold = 1;
            ptr += 1;
            break;

        case GFE_DEATHTILE:
            // layout: [op, tile]
            printf("[SEQ] GFE_DEATHTILE tile=%d\n", ptr[1]);
            GF_DeathTile = ptr[1];
            ptr += 2;
            break;

        case GFE_WATERCLR:
            // layout: [op, lo16, hi16]
            printf("[SEQ] GFE_WATERCLR lo=%d hi=%d\n", ptr[1], ptr[2]);
            GF_WaterColor = (ushort)ptr[1] | (ushort)ptr[2] << 16;
            ptr += 3;
            break;

        default:
            // Consuma opcodes imprevisti per evitare il loop
            printf("[SEQ] WARNING: unknown opcode %d, skipping\n", op);
            ptr += 1;
            break;
        }

        steps++;
    }

    if (steps >= MAX_SEQ_STEPS) {
        printf("[SEQ] ERROR: runaway sequence (no END or bad ptr advance). Forcing exit to title.\n");
        return EXIT_TO_TITLE;
    }

    printf("[SEQ] EXIT option=%ld\n", option);

    if (type == 5 || type == 7)
        return 0;

    return option;
}