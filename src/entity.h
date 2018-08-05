#ifndef _entity_h_
#define _entity_h_

#include <assert.h>

#include <gctypes.h>

struct Entity
{
	u16 classifier;
	u8 unkflag02;
	u8 unkflag03;
	u16 padding;
	u8 unkflag06;
	u8 unkflag07;
	struct Entity* next_entity;
	struct Entity* prev_entity;
	void* unk_linkedlist;
	void* commonmove_tbl;
	void* unk_struct;
	void* entity_data;
	void* destructor;
	void* unk_linkedlist2;
	void* unk_linkedlist3;
};

extern bool Entity_IsPlayer(struct Entity*);

#endif