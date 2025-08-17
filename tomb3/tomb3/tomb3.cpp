#include "../tomb3/pch.h"
#include "tomb3.h"
#include "../newstuff/registry.h"
#include "../specific/specific.h"
#include "../specific/option.h"
#include "../specific/input.h"
#include "../specific/winmain.h"
#include "../specific/hwrender.h"
#include "../game/inventry.h"
#if defined(__APPLE__)
    #include <sys/types.h>
    #include <sys/time.h>
    #include <time.h>
	#include <unistd.h>
#endif
TOMB3_OPTIONS tomb3;
TOMB3_SAVE tomb3_save;
ulong tomb3_save_size;

ulong water_color[24] =
{
	//home
	0xFFCCFF80,

	//India
	0xFFCCFF80,
	0xFFCCFF80,
	0xFFCCFF80,
	0xFFCCFF80,

	//South Pacific
	0xFF80FFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFF80E0FF,		//puna has no water

	//London
	0xFFFFFFFF,
	0xFFCCFF80,
	0xFFCCFF80,
	0xFFCCFF80,

	//Nevada
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,

	//Antarctica
	0xFF80FFFF,
	0xFFCCFFCC,
	0xFF80E0FF,
	0xFF80E0FF,		//cavern has no water

	//Hallows
	0xFFB2E6E6,

	//
	0xFF80E0FF,
	0xFF80E0FF,
	0xFF80E0FF
};

static void T3_InitSettings()
{
	Option_Music_Volume = 7;
	Option_SFX_Volume = 10;
	App.windowed = 0;

	tomb3.footprints = 1;
	tomb3.pickup_display = 1;
	tomb3.improved_rain = 1;
	tomb3.improved_lasers = 1;
	tomb3.uwdust = 1;
	tomb3.flexible_crawl = 1;
	tomb3.duck_roll = 1;
	tomb3.flexible_sprint = 1;
	tomb3.slide_to_run = 1;
	tomb3.kayak_mist = 1;
	tomb3.dozy = 0;
	tomb3.disable_gamma = 1;
	tomb3.disable_ckey = 0;
	tomb3.crawl_tilt = 1;
	tomb3.improved_poison_bar = 1;
	tomb3.custom_water_color = 1;
	tomb3.psx_text_colors = 0;
	tomb3.upv_wake = 1;
	tomb3.psx_fov = 0;
	tomb3.psx_boxes = 0;
	tomb3.psx_mono = 0;
	tomb3.psx_saving = 0;
	tomb3.psx_crystal_sfx = 0;
	tomb3.blue_crystal_light = 0;
	tomb3.improved_electricity = 1;
	tomb3.psx_contrast = 0;
	tomb3.shadow_mode = SHADOW_PSX;
	tomb3.bar_mode = BAR_PSX;
	tomb3.sophia_rings = SRINGS_PSX;
	tomb3.bar_pos = BPOS_ORIGINAL;
	tomb3.ammo_counter = ACTR_PC;
	tomb3.GUI_Scale = 1.0F;
	tomb3.INV_Scale = 0.5F;
	tomb3.unwater_music_mute = 0.8F;
	tomb3.inv_music_mute = 0.8F;
}

void T3_SaveSettings()
{
    OpenConfig(CONFIG_FILE);

    CFG_WriteLong("VM", App.glConfig.nVMode);
    CFG_WriteLong("zbuffer", App.glConfig.bZBuffer);
    CFG_WriteLong("dither", App.glConfig.Dither);
    CFG_WriteLong("filter", App.glConfig.Filter);
    CFG_WriteLong("sound", App.glConfig.sound);
    CFG_WriteLong("SFXVolume", Option_SFX_Volume);
    CFG_WriteLong("MusicVolume", Option_Music_Volume);
    CFG_WriteBool("Window", App.windowed);
    CFG_WriteLong("WindowX", App.rScreen.left);
    CFG_WriteLong("WindowY", App.rScreen.top);
    CFG_WriteFloat("Gamma", GammaOption);
    // CFG_WriteBlock("keyLayout", &layout[1][0], sizeof(layout) / 2); // Implementa se serve

    // Nuove opzioni
    CFG_WriteBool("footprints", tomb3.footprints);
    CFG_WriteBool("pickup_display", tomb3.pickup_display);
    CFG_WriteBool("improved_rain", tomb3.improved_rain);
    CFG_WriteBool("improved_lasers", tomb3.improved_lasers);
    CFG_WriteBool("uwdust", tomb3.uwdust);
    CFG_WriteBool("flexible_crawl", tomb3.flexible_crawl);
    CFG_WriteBool("duck_roll", tomb3.duck_roll);
    CFG_WriteBool("flexible_sprint", tomb3.flexible_sprint);
    CFG_WriteBool("slide_to_run", tomb3.slide_to_run);
    CFG_WriteBool("kayak_mist", tomb3.kayak_mist);
    CFG_WriteBool("dozy", tomb3.dozy);
    CFG_WriteBool("disable_gamma", tomb3.disable_gamma);
    CFG_WriteBool("disable_ckey", tomb3.disable_ckey);
    CFG_WriteBool("crawl_tilt", tomb3.crawl_tilt);
    CFG_WriteBool("improved_poison_bar", tomb3.improved_poison_bar);
    CFG_WriteBool("custom_water_color", tomb3.custom_water_color);
    CFG_WriteBool("psx_text_colors", tomb3.psx_text_colors);
    CFG_WriteBool("upv_wake", tomb3.upv_wake);
    CFG_WriteBool("psx_fov", tomb3.psx_fov);
    CFG_WriteBool("psx_boxes", tomb3.psx_boxes);
    CFG_WriteBool("psx_mono", tomb3.psx_mono);

    if (!tomb3.gold)
        CFG_WriteBool("psx_saving", tomb3.psx_saving);

    CFG_WriteBool("psx_crystal_sfx", tomb3.psx_crystal_sfx);
    CFG_WriteBool("blue_crystal_light", tomb3.blue_crystal_light);
    CFG_WriteBool("improved_electricity", tomb3.improved_electricity);
    CFG_WriteBool("psx_contrast", tomb3.psx_contrast);
    CFG_WriteLong("shadow_mode", tomb3.shadow_mode);
    CFG_WriteLong("bar_mode", tomb3.bar_mode);
    CFG_WriteLong("sophia_rings", tomb3.sophia_rings);
    CFG_WriteLong("bar_pos", tomb3.bar_pos);
    CFG_WriteLong("ammo_counter", tomb3.ammo_counter);
    CFG_WriteFloat("GUI_Scale", tomb3.GUI_Scale);
    CFG_WriteFloat("INV_Scale", tomb3.INV_Scale);
    CFG_WriteFloat("unwater_music_mute", tomb3.unwater_music_mute);
    CFG_WriteFloat("inv_music_mute", tomb3.inv_music_mute);

    CloseConfig();
}

bool T3_LoadSettings()
{
    if (!OpenConfig(CONFIG_FILE))
    {
        T3_InitSettings();
        return false;
    }

    ulong tmp;
    CFG_ReadLong("VM", &tmp, 0); App.glConfig.nVMode = (long)tmp;
    CFG_ReadLong("zbuffer", &tmp, 0); App.glConfig.bZBuffer = (long)tmp;
    CFG_ReadLong("dither", &tmp, 0); App.glConfig.Dither = (long)tmp;
    CFG_ReadLong("filter", &tmp, 0); App.glConfig.Filter = (long)tmp;
    CFG_ReadLong("sound", &tmp, 0); App.glConfig.sound = (long)tmp;
    CFG_ReadLong("SFXVolume", &tmp, 0); Option_SFX_Volume = (short)tmp;
    CFG_ReadLong("MusicVolume", &tmp, 0); Option_Music_Volume = (short)tmp;
    CFG_ReadBool("Window", &App.windowed, 0);
    CFG_ReadLong("WindowX", &tmp, 0); App.rScreen.left = (int)tmp;
    CFG_ReadLong("WindowY", &tmp, 0); App.rScreen.top = (int)tmp;
    CFG_ReadFloat("Gamma", &GammaOption, 0);

    // CFG_ReadBlock("keyLayout", &layout[1][0], sizeof(layout) / 2, 0); // Implementa se serve
    DefaultConflict();

    CFG_ReadBool("footprints", &tomb3.footprints, 1);
    CFG_ReadBool("pickup_display", &tomb3.pickup_display, 1);
    CFG_ReadBool("improved_rain", &tomb3.improved_rain, 1);
    CFG_ReadBool("improved_lasers", &tomb3.improved_lasers, 1);
    CFG_ReadBool("uwdust", &tomb3.uwdust, 1);
    CFG_ReadBool("flexible_crawl", &tomb3.flexible_crawl, 1);
    CFG_ReadBool("duck_roll", &tomb3.duck_roll, 1);
    CFG_ReadBool("flexible_sprint", &tomb3.flexible_sprint, 1);
    CFG_ReadBool("slide_to_run", &tomb3.slide_to_run, 1);
    CFG_ReadBool("kayak_mist", &tomb3.kayak_mist, 1);
    CFG_ReadBool("dozy", &tomb3.dozy, 0);
    CFG_ReadBool("disable_gamma", &tomb3.disable_gamma, 1);
    CFG_ReadBool("disable_ckey", &tomb3.disable_ckey, 0);
    CFG_ReadBool("crawl_tilt", &tomb3.crawl_tilt, 1);
    CFG_ReadBool("improved_poison_bar", &tomb3.improved_poison_bar, 1);
    CFG_ReadBool("custom_water_color", &tomb3.custom_water_color, 1);
    CFG_ReadBool("psx_text_colors", &tomb3.psx_text_colors, 0);
    CFG_ReadBool("upv_wake", &tomb3.upv_wake, 1);
    CFG_ReadBool("psx_fov", &tomb3.psx_fov, 0);
    CFG_ReadBool("psx_boxes", &tomb3.psx_boxes, 0);
    CFG_ReadBool("psx_mono", &tomb3.psx_mono, 0);

    if (tomb3.gold)
        tomb3.psx_saving = 0;
    else
        CFG_ReadBool("psx_saving", &tomb3.psx_saving, 0);

    CFG_ReadBool("psx_crystal_sfx", &tomb3.psx_crystal_sfx, 0);
    CFG_ReadBool("blue_crystal_light", &tomb3.blue_crystal_light, 0);
    CFG_ReadBool("improved_electricity", &tomb3.improved_electricity, 1);
    CFG_ReadBool("psx_contrast", &tomb3.psx_contrast, 0);

    CFG_ReadLong("shadow_mode", &tmp, SHADOW_PSX); tomb3.shadow_mode = (long)tmp;
    CFG_ReadLong("bar_mode", &tmp, BAR_PSX); tomb3.bar_mode = (long)tmp;
    CFG_ReadLong("sophia_rings", &tmp, SRINGS_PSX); tomb3.sophia_rings = (long)tmp;
    CFG_ReadLong("bar_pos", &tmp, BPOS_ORIGINAL); tomb3.bar_pos = (long)tmp;
    CFG_ReadLong("ammo_counter", &tmp, ACTR_PC); tomb3.ammo_counter = (long)tmp;

    CFG_ReadFloat("GUI_Scale", &tomb3.GUI_Scale, 1.0F);
    CFG_ReadFloat("INV_Scale", &tomb3.INV_Scale, 1.0F);
    CFG_ReadFloat("unwater_music_mute", &tomb3.unwater_music_mute, 0.8F);
    CFG_ReadFloat("inv_music_mute", &tomb3.inv_music_mute, 0.8F);

    CloseConfig();
    return true;
}

void T3_GoldifyString(char* string)
{
	char str[128];
	char buf[4];

	buf[0] = string[0];
	buf[1] = string[1];
	buf[2] = string[2];

	if (strstr(buf, "pix"))	//pix
	{
		strcpy(&str[4], &string[3]);	//the rest of the string
		str[0] = 'p';
		str[1] = 'i';
		str[2] = 'x';
		str[3] = 'g';
		strcpy(string, str);	//becomes pixg
		return;
	}

	buf[3] = string[3];

	if (strstr(buf, "data"))	//data
	{
		strcpy(&str[5], &string[4]);
		str[0] = 'd';
		str[1] = 'a';
		str[2] = 't';
		str[3] = 'a';
		str[4] = 'g';
		strcpy(string, str);	//becomes datag
		return;
	}
}

int main(){
    printf("tomb3 initialized");
    return 0;
}