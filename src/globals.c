#include "globals.h"

static u8 bss[0xFFFF];
void* r13_x77C0 = &bss; //r13 - 0x77C0; 8045A6C0 in-game, start of the bss

//8000AD8C
s8 sub_8000AD8C(){
	return **(s8 **)r13_x77C0;
}

//8000AD98
s32 sub_8000AD98(s32 result){
	if ( result >= 0 && result < 2 )
		**(s8 **)r13_x77C0 = result;
	return result;
}

//8000ADB4
bool sub_8000ADB4(){
	return **(s8 **)r13_x77C0 == 0;
}

//8000ADD4
bool sub_8000ADD4(){
	return **(s8 **)r13_x77C0 == 1;
}

//8015CC34
u8* Globals_Get1850(){
	return &bss[0x1850];
}

//8015CC40
u32* Globals_Get1868(){
	return &bss[0x1868];
}

//8015CC58
u8* Globals_GetCurrentLanguage(){
	return &bss[0x1CB0];
}

//8015CC90
u16* Globals_Get1CD0(){
	(u16*)&bss[0x1CD0];
}

//8015CC84
u16* Globals_Get1CD2(){
	(u16*)&bss[0x1CD2];
}

//8015CC78
u16* Globals_Get1CD4(){
	(u16*)&bss[0x1CD4];
}

//8015CC4C
s32* Globals_Get2FF8(){
	&bss[0x2FF8];
}