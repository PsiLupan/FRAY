#ifndef _hsd_fobj_h_
#define _hsd_fobj_h_

#include <string.h>

#include <gctypes.h>
#include <ogc/gu.h>

#include "hsd_object.h"

#define TYPE_ROBJ 1
#define TYPE_JOBJ 12

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

HSD_ObjDef* HSD_FObjGetAllocData();
void HSD_FObjInitAllocData();
void HSD_FObjRemove(HSD_FObj *);
void HSD_FObjRemoveAll(HSD_FObj *);
u8 HSD_FObjSetState(HSD_FObj *, u8);
u8 HSD_FObjGetState(HSD_FObj *);
void HSD_FObjReqAnimAll(HSD_FObj *, f32);
void HSD_FObjStopAnim(HSD_FObj *, void *, void(*)(), f32);
void HSD_FObjStopAnimAll(HSD_FObj *, void *, void(*)(), f32);
void FObjUpdateAnim(HSD_FObj *, void *, void (*)(void*, u32, FObjData));
void HSD_FObjInterpretAnim(HSD_FObj *, void *, void (*)(), f32);
void HSD_FObjInterpretAnimAll(HSD_FObj *, void *, void (*)(), f32);
HSD_FObj* HSD_FObjAlloc();
void HSD_FObjFree(HSD_FObj *);

#endif