#ifndef _hsd_object_h_
#define _hsd_object_h_

#include "hsd_init.h"

#include <gctypes.h>

#include <string.h>

#define HSD_BASE_CLASS_LIBRARY "sysdolphin_base_library"

#define HSD_OBJ_NOREF -1

#define HSD_CLASS_INFO(o) ((HSD_ClassInfo*)o)
#define HSD_CLASS_METHOD(o) (o->parent.class_info)
#define HSD_PARENT_INFO(o) ((o)->parent.head.parent)
#define HSD_OBJECT_INFO(o) (o.parent.parent)
#define HSD_OBJECT_METHOD(o) (o->parent.parent.class_info)
#define HSD_OBJECT_PARENT_INFO(o) (o.parent.parent.head.parent)

typedef struct _HSD_Class {
	struct _HSD_ClassInfo* class_info;
} HSD_Class;

typedef struct _HSD_ClassInfoHead {
	void (*info_init)(); //0x00
	u32 flags; //0x04
	char* library_name; //0x08
	char* class_name; //0x0C
	s16 obj_size; //0x10
	s16 info_size; //0x12
	struct _HSD_ClassInfo* parent; //0x14
	struct _HSD_ClassInfo* next; //0x18
	struct _HSD_ClassInfo* child; //0x1C
	u32 nb_exist; //0x20
	u32 nb_peak; //0x24
} HSD_ClassInfoHead;

typedef struct _HSD_ClassInfo {
	struct _HSD_ClassInfoHead head;
	HSD_Class* (*alloc)(struct _HSD_ClassInfo* o); //0x28
	void (*init)(struct _HSD_Class* o); //0x2C
	void (*release)(struct _HSD_Class* o); //0x30
	void (*destroy)(struct _HSD_Class* o); //0x34
	void (*amnesia)(struct _HSD_ClassInfo* info); //0x38
} HSD_ClassInfo;

typedef struct _HSD_Object {
	struct _HSD_Class parent;
	u16 ref_count;
	u16 ref_count_individual;
} HSD_Obj;

typedef struct _HSD_ObjInfo {
	struct _HSD_ClassInfo parent;
} HSD_ObjInfo;

typedef struct _HSD_ObjAllocLink {
	struct _HSD_ObjAllocLink* next;
} HSD_ObjAllocLink;

typedef struct _HSD_ObjAllocData {
	u32 flags; //0x00 - Technically 2 diff flags
	HSD_ObjAllocLink* freehead; //0x04
	u32 used; //0x08
	u32 free; //0x0C
	u32 peak; //0x10
	u32 num_limit; //0x14
	u32 heap_limit_size; //0x18
	u32 heap_limit_num; //0x1C
	u32 size; //0x20
	u32 align; //0x24
	struct _HSD_ObjAllocData* next; //0x28
} HSD_ObjAllocData;

struct unk_80406E48 {
	void* unk00;
	u32 unk0C;
};

extern HSD_ObjInfo hsdObj;
extern HSD_ClassInfo hsdClass;

s32 HSD_ObjAllocAddFree(HSD_ObjAllocData *, u32);
void* HSD_ObjAlloc(HSD_ObjAllocData *);
void HSD_ObjFree(HSD_ObjAllocData *, HSD_ObjAllocLink *);
void HSD_ObjAllocInit(HSD_ObjAllocData *, u32, u32);

void hsdInitClassInfo(HSD_ClassInfo *, HSD_ClassInfo *, char *, char*, s32, s32);
void* hsdNew(HSD_ClassInfo *);
HSD_ClassInfo* hsdSearchClassInfo(char *);

#endif
