#include "scripter.h"
#include "interpreter.h"
#include "../tomb3/pch.h"
#include "../game/gameflow.h"
int main(int argc, char** argv)
{
    printf("Scripter started.\n");
    GF_titlefilenames[0] = strdup("TOMB3");
    OutputScript();
    printf("Script exported.\n");
    return 0;
}