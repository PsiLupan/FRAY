#ifndef _hsd_fobj_h_
#define _hsd_fobj_h_

//Frame Object
typedef struct _HSD_FObj {
	struct _HSD_FObj* next;
	u32* curr_parse;
	u32* parse_start;
	u32 parse_len;
	u32 unk10;
	u32 unk14;
	u32 unk18;
	u32 unk1C;
	u32 unk20;
	u32 unk24;
	u32 unk28;
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