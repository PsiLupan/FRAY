#ifndef _dvd_lookup_
#define _dvd_lookup_

#include <gctypes.h>

typedef struct _entry { 
    const char* name; 
    s32 entrynum; 
} entry;

extern entry files[] = {
    {"/audio/opening.hps", 107},
    {"/audio/us/kirby.ssm", 161},
    {"/audio/us/link.ssm", 167},
    {"/audio/us/luigi.ssm", 168},
    {"/audio/us/main.ssm", 169},
    {"/audio/us/smash2.sem", 190},
    {"GmTtAll.usd", 381},
    {"GmTtAll.usd", 382},
    {"LbRb.dat", 492},
    {"LbRf.dat", 493},
    {"MnExtAll.dat", 494},
    {"MnExtAll.usd", 495},
    {"MnMaAll.dat", 496},
    {"MnMaAll.usd", 497},
    {"MvOpen.mth", 530}
};

#endif