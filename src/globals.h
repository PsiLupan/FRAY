#ifndef _globals_h_
#define _globals_h_

#include <gctypes.h>

extern void* r13_x77C0;

extern s8 sub_8000AD8C();
extern s32 sub_8000AD98(s32);
extern bool sub_8000ADB4();
extern bool sub_8000ADD4();

extern u8* Globals_Get1850();
extern u32* Globals_Get1868();
extern u8* Globals_GetCurrentLanguage();
extern u16* Globals_Get1CD0();
extern u16* Globals_Get1CD2();
extern u16* Globals_Get1CD4();
extern s32* Globals_Get2FF8();

#endif