#ifndef _menu_h_
#define _menu_h_

#include <gctypes.h>

#include "match.h"
#include "scene.h"

u8 menu_804D6730[6]; //804D6730

extern u32 sub_8022BFBC(u32 result);
extern s32 sub_8022C010(s32 result, s32 a2);

u8* Menu_GmRst_StartupInit();
u32 Menu_GmTrain_StartupInit();
u32 __InitStartMeleeData(s8*);
u32 Menu_InitStartMeleeData(s8*);

u8* Menu_ZeroFillPtr();
u32 Menu_InitUsableStructs(s8*);

#endif