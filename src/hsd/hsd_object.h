#ifndef _hsd_object_h_
#define _hsd_object_h_

#include "hsd_init.h"

#include <gctypes.h>

#include <String.h>

#define HSD_BASE_CLASS_LIBRARY "sysdolphin_base_library"

#define HSD_OBJ_NOREF 0

#define HSD_CLASS_INFO(s)	((HSD_ClassInfo*)(s))
#define HSD_PARENT_INFO(s)  (((HSD_ClassInfo*)(s))->parent_info)

typedef struct _HSD_Class {
	HSD_ClassInfo* class_init;
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
	struct _HSD_ClassInfo* next_info;
	struct _HSD_ClassInfo* child_info;
	void* unk20;
	void* unk24;
	void* unk28;
	void (*init)(HSD_Class* o); //0x2C
	void (*release)(HSD_Class* o); //0x30
	void (*destroy)(HSD_Class* o); //0x34
	void (*amnesia)(HSD_ClassInfo* info); //0x38
} HSD_ClassInfo;

struct unk_80406E48 {
	void* unk00;
	u32 unk0C;
};

extern HSD_ClassInfo hsdClass;

#endif