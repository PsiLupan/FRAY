#ifndef _hsd_fobj_h_
#define _hsd_fobj_h_

#include <string.h>

#include <gctypes.h>

//Frame Object
typedef struct _HSD_FObj {
	struct _HSD_FObj* next;
	u32* curr_parse;
	u32* parse_start;
	u32 parse_len;
	u8 flags; //0x10
	u8 unk11;
	u8 unk12;
	u8 unk13;
	u32 unk14;
	u16 unk18;
	u16 unk1A;
	u32 unk1C;
	f32 unk20;
	f32 unk24;
	f32 unk28;
	u32 unk2C;
	u32 unk30;
} HSD_FObj;

typedef struct _HSD_FObjDesc {
	struct _HSD_FObjDesc* next;
	u32 str_len;
	u32 unk08;
	u32 flags;
	char* string;
} HSD_FObjDesc;

#endif