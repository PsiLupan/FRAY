#ifndef _gobj_h_
#define _gobj_h_

#include <assert.h>

#include <gctypes.h>

typedef struct _HSD_GObj {
	u16 classifier;
	u8 unkflag02;
	u8 unkflag03;
	u16 padding;
	u8 unkflag06;
	u8 unkflag07;
	struct _HSD_GObj* next;
	struct _HSD_GObj* prev;
	void* unk_linkedlist;
	void* commonmove_tbl;
	void* unk_struct;
	void* data;
	void* destructor;
	void* unk_linkedlist2;
	void* unk_linkedlist3;
} HSD_GObj;

bool GObj_IsPlayer(HSD_GObj* gobj);

#endif