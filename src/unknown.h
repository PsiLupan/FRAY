#ifndef _unknown_h_
#define _unknown_h_

#include "scene.h"

typedef struct _unk_80432078{
	void* unk970;
} unk_80432078;

extern unk_80432078 dword_80432078;

typedef struct _unk_8046B0F0 {
	u32 unk00;
	u32 unk04;
	u32 unk08;
	u32 unk0C;
} unk_8046B0F0;

extern unk_8046B0F0 dword_8046B0F0;

void unkstatestruct_set970(void* scene_ptr);

#endif