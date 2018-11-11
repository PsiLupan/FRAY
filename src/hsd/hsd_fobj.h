#ifndef _hsd_fobj_h_
#define _hsd_fobj_h_

#include <string.h>

#include <gctypes.h>
#include <ogc/gu.h>

//Frame Object
typedef struct _HSD_FObj {
	struct _HSD_FObj* next; //0x00
	u8** curr_parse; //0x04
	u8* parse_start; //0x08
	u32 parse_len; //0x0C
	u8 flags; //0x10
	u8 parsed_state; //0x11
	u8 obj_state; //0x12
	u8 obj_type; //0x13
	u8 unk14;
	u8 unk15;
	u16 unk16;
	u16 unk18;
	u16 parse_res; //0x1A
	f32 unk1C;
	f32 unk20;
	f32 unk24;
	f32 unk28;
	f32 unk2C;
	u32 unk30;
} HSD_FObj;

typedef struct _HSD_FObjDesc {
	struct _HSD_FObjDesc* next;
	u32 str_len;
	u32 unk08;
	u32 flags;
	char* string;
} HSD_FObjDesc;

typedef struct _HSD_FObjData {
	f32 fv;
} FObjData;

#endif