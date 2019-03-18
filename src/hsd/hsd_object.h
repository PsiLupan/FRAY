#ifndef _hsd_object_h_
#define _hsd_object_h_

#include "hsd_init.h"

#include <gctypes.h>

#include <string.h>

#define HSD_BASE_CLASS_LIBRARY "sysdolphin_base_library"

#define HSD_OBJ_NOREF -1

#define HSD_CLASS_INFO(s)	((HSD_ClassInfo*)(s))
#define HSD_PARENT_INFO(s)  (((HSD_ClassInfo*)(s))->parent_info)

typedef struct _HSD_Class {
	struct _HSD_ClassInfo* class_init;
	u16 ref_count;
} HSD_Class;

typedef struct _HSD_ClassInfo {
	void (*ObjInfoInit)(); //0x00
	bool initialized; //0x04
	char* base_class_library; //0x08
	char* type; //0x0C
	u16 obj_size; //0x10
	u16 info_size; //0x12
	struct _HSD_ClassInfo* parent_info; //0x14
	struct _HSD_ClassInfo* next_info; //0x18
	struct _HSD_ClassInfo* child_info; //0x1C
	u32 active_objs; //0x20
	u32 total_allocs; //0x24
	void (*obj_alloc)(struct _HSD_Class* o); //0x28
	void (*init)(struct _HSD_Class* o); //0x2C
	void (*release)(struct _HSD_Class* o); //0x30
	void (*destroy)(struct _HSD_Class* o); //0x34
	void (*amnesia)(struct _HSD_ClassInfo* info); //0x38
} HSD_ClassInfo;

typedef struct _HSD_ObjDef {
	u32 flags;
	void** obj_ptr;
	u32 unk_ctr;
	u32 unk_ctr2;
	u32 unk_ctr3;
	u32 unk_14;
	u32 obj_size;
	s32 unk_1C;
} HSD_ObjDef;

struct unk_80406E48 {
	void* unk00;
	u32 unk0C;
};

extern HSD_ClassInfo hsdClass;

#endif