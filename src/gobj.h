#ifndef _gobj_h_
#define _gobj_h_

#include <assert.h>

#include <gctypes.h>

#include "hsd/hsd_jobj.h"

#define GOBJ_KIND_MENU_COBJ 1
#define GOBJ_KIND_JOBJ 3
#define GOBJ_KIND_FOG 4

#define GOBJ_CLASS_PLAYER 0x4
#define GOBJ_CLASS_ITEM 0x6
#define GOBJ_CLASS_TEXT 0x9
#define GOBJ_CLASS_HSD_FOG 0xA
#define GOBJ_CLASS_HSD_LOBJ 0xB

#define GOBJ_NOREF 0xFF

//This has 0x20 aligned size - assuming this is actually something related to HSD_Memory and can be scrapped later
typedef struct _GObj_LinkedList {
	struct _GObj_LinkedList* prev;
	struct _GObj_LinkedList* next;
	int size;
	int data[5];
} GObj_LinkedList;

typedef struct _HSD_GObj {
	u16 classifier;
	u8 p_link;
	u8 gx_link;
	u8 p_priority;
	u8 render_priority;
	s8 obj_kind;
	s8 data_kind;
	struct _HSD_GObj* next;
	struct _HSD_GObj* prev;
	struct _HSD_GObj* next_gx;
	struct _HSD_GObj* prev_gx;
	GObj_LinkedList* unk_linkedlist;
	void (*render_cb)(struct _HSD_GObj *gobj, int code);
	u32 unk20;
	u32 unk24;
	void* data;
	void* hsd_obj; //JObj or CObj known
	void (*user_data_remove_func)(void* data);
	GObj_LinkedList* unk_linkedlist2;
	GObj_LinkedList* unk_linkedlist3;
} HSD_GObj;

BOOL GObj_IsPlayer(HSD_GObj *);
void GObj_AnimAll_Callback(HSD_GObj *);
BOOL GObj_IsItem(HSD_GObj *);


HSD_GObj* GObj_Create(u32, u32, u32);
void GObj_Free(HSD_GObj *);
void GObj_SetupGXLink(HSD_GObj *, void *, u32, u32);
void GObj_SetupGXLink_Max(HSD_GObj *, void *, u32);
void GObj_GXLinkDestructor(HSD_GObj *);
void GObj_InitKindObj(HSD_GObj *, s8, void *);
void GObj_CallHSDDestructor(HSD_GObj *);
void GObj_InitKindData(HSD_GObj *, s8, void *, void *);
void GObj_CallDestructor(HSD_GObj *);

#endif