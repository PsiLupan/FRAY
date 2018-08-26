#include "hsd_init.h"

#define HSD_OBJ_NOREF 0xFFFF

typedef struct _HSD_ObjDef {
	u32 flags;
	void* unk04;
	u32 unk08;
	u32 unk0C;
	u32 unk10;
	u32 unk14;
} HSD_ObjDef;

typedef struct _HSD_Obj {
	u16 unk00;
	u16 unk02;
	u16 ref_count;
} HSD_Obj;

struct unk_80406E48 {
	void* unk00;
	u32 unk0C;
};