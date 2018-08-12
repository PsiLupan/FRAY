#ifndef _unknown_h_
#define _unknown_h_

#include "scene.h"

struct unk_80432078{
	void* unk970;
};

extern struct unk_8043207C dword_8043207C;

struct unk_8046B0F0 {
	u32 unk00;
	u32 unk04;
	u32 unk08;
	u32 unk0C;
};

extern struct unk_8046B0F0 dword_8046B0F0;

extern void* r13_x77C0;

s8 sub_8000AD8C();
s32 sub_8000AD98(s32);
bool sub_8000ADB4();
bool sub_8000ADD4();

#endif