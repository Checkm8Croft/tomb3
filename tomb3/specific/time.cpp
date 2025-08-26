#include "../tomb3/pch.h"
#include "time.h"
#include <SDL2/SDL.h>
static Uint64 frequency, ticks;
static void UpdateTicks()
{
	Uint64 counter;

	counter = SDL_GetPerformanceCounter();
	ticks = counter;
}

bool TIME_Init()
{
	Uint64 pfq;

	pfq = SDL_GetPerformanceFrequency();
	if (!pfq)
		return 0;

    frequency = pfq;
	UpdateTicks();
	return 1;
}

ulong Sync()
{
	Uint64 last;

	last = ticks;
	UpdateTicks();
	return ulong(double(ticks - last) / frequency);
}

ulong SyncTicks(long skip)
{
	double passed, dskip;
	Uint64 last;

	passed = 0;
	dskip = (double)skip;
	last = ticks;

	do
	{
		UpdateTicks();
		passed = double(ticks - last) / frequency;
	}
	while (passed < dskip);

	return (ulong)passed;
}
