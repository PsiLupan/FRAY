#include "hsd_init.h"

#include <gctypes.h>

#define HSD_OBJ_NOREF 0xFFFF

typedef struct _HSD_Obj {
	void* class_init;
	u16 ref_count;
} HSD_Obj;

struct unk_80406E48 {
	void* unk00;
	u32 unk0C;
};