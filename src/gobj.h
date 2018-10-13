#ifndef _gobj_h_
#define _gobj_h_

#include <assert.h>

#include <gctypes.h>

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
	u8 unk04;
	u8 unk05;
	u8 obj_kind;
	u8 unk07;
	struct _HSD_GObj* next;
	struct _HSD_GObj* prev;
	struct _HSD_GObj* next_gx;
	struct _HSD_GObj* prev_gx;
	GObj_LinkedList* unk_linkedlist;
	void (*render_cb)(struct _HSD_GObj *gobj, int code);
	void* unk20;
	void* unk24;
	void* data;
	void* destructor;
	GObj_LinkedList* unk_linkedlist2;
	GObj_LinkedList* unk_linkedlist3;
} HSD_GObj;

bool GObj_IsPlayer(HSD_GObj* gobj);

#endif