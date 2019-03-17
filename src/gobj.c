#include "gobj.h"

#define P_LINK_MAX 63
#define GX_LINK_MAX 63
static HSD_GObj** plinkhigh_gobjs[P_LINK_MAX + 2]; //r13_3E74
static HSD_GObj** plinklow_gobjs[P_LINK_MAX + 2]; //r13_3E78
static HSD_GObj** highestprio_gobjs[GX_LINK_MAX + 2]; //r13_3E7C
static HSD_GObj** lowestprio_gobjs[GX_LINK_MAX + 2]; //r13_3E80
static HSD_GObj* current_gobj = NULL; //r13_3E84 - Really just a guess
static void* r13_3E88 = NULL;
static void* r13_3E8C = NULL;
static void** hsd_destructors[14]; //r13_3E90 - Length is currently made up, TODO: need to explictly assign the functions to this at some point

u32 flag_array[4] = {1, 4, 2, 0}; //804085F0

HSD_ObjDef gobj_def_1; //804CE38C
HSD_ObjDef gobj_def_2; //804CE3B8
HSD_ObjDef gobj_def_3; //804CE3E4

//80086960
BOOL GObj_IsPlayer(HSD_GObj* gobj){
	if(gobj != NULL){
		if(gobj->classifier == GOBJ_CLASS_PLAYER){
			return TRUE;
		}
	}
	return FALSE;
}

//8001CEC0
//80176BCC
//80176D18
void GObj_AnimAll_Callback(HSD_GObj* gobj){
    HSD_JObjAnimAll((HSD_JObj*)gobj->hsd_obj);
}

//80272D1C
BOOL GObj_IsItem(HSD_GObj* gobj){
	if(gobj != NULL){
		if(gobj->classifier == GOBJ_CLASS_ITEM){
			return TRUE;
		}
	}
	return FALSE;
}

//8038FD54
s32 GObj_CreateWithAnimCallback(HSD_GObj* gobj, void* animcb, u8 gx_link){
  s32 iVar1;
  int in_r13;
  
  iVar1 = HSD_ObjAlloc(&gobj_def_2);
  assert(iVar1 != NULL);
  assert(gx_link <= GX_LINK_MAX);
  *(u8 *)(iVar1 + 0xc) = gx_link;
  *(u8 *)(iVar1 + 0xd) = *(u8 *)(iVar1 + 0xd) & 0xbf;
  *(u8 *)(iVar1 + 0xd) = *(u8 *)(iVar1 + 0xd) & 0x7f;
  *(u8 *)(iVar1 + 0xd) = *(u8 *)(iVar1 + 0xd) & 0xcf | 0x30;
  *(u32 *)(iVar1 + 0x10) = gobj;
  *(u32 *)(iVar1 + 0x14) = animcb;
  FUN_8038faa8(iVar1);
  return iVar1;
}

//8038FF5C
void GObj_PReorder(HSD_GObj* gobj, HSD_GObj* hiprio_gobj){
	gobj->prev = hiprio_gobj;
	if(hiprio_gobj != NULL){
		gobj->next = hiprio_gobj->next;
		hiprio_gobj->next = gobj;
	}else{
		gobj->next = plinkhigh_gobjs[gobj->p_link];
		plinkhigh_gobjs[gobj->p_link] = gobj;
	}
	if(gobj->next != NULL){
		gobj->next->prev = gobj;
	}else{
		plinklow_gobjs[gobj->p_link] = gobj;
	}
}

//8038FFB8
static HSD_GObj* CreateGObj(u32 order, u32 class, u32 p_link, u32 p_prio, HSD_GObj* p_gobj){
	assert(p_link < P_LINK_MAX);
	HSD_GObj* gobj = (HSD_GObj*)HSD_ObjAlloc(&gobj_def_1);
	if(gobj != NULL){
		gobj->classifier = class;
		gobj->p_link = p_link;
		gobj->gx_link = GOBJ_NOREF;
		gobj->p_priority = p_prio;
		gobj->render_priority = 0;
		gobj->obj_kind = GOBJ_NOREF;
		gobj->data_kind = GOBJ_NOREF;
		gobj->prev_gx = NULL;
		gobj->next_gx = NULL;
		gobj->unk_linkedlist = NULL;
		gobj->render_cb = NULL;
		gobj->unk24 = NULL;
		gobj->unk20 = NULL;
		gobj->hsd_obj = NULL;
		gobj->data = NULL;
		gobj->user_data_remove_func = NULL;

		switch(order){
			case 0:
			HSD_GObj* g = NULL;
			for(g = plinklow_gobjs[gobj->p_link]; g != NULL && g->p_priority > gobj->p_priority; g = g->prev){
				//Works backwards from lowest to highest priority till it finds the highest priority to be it's parent obj
				//Returns null if nothing is a higher priority than the current object or if there is none of that type
			}
			GObj_PReorder(gobj, g);
			break;

			case 1:
			HSD_GObj* g = NULL;
			for(g = plinkhigh_gobjs[gobj->p_link]; g != NULL && g->p_priority < gobj->p_priority; g = g->next){
			}
			if(g != NULL){
				g = g->prev;
			}else{
				g = plinklow_gobjs[gobj->p_link];
			}
			GObj_PReorder(gobj, g);
			break;

			case 2:
			gobj->prev = p_gobj;
			if(p_gobj != NULL){
				gobj->next = p_gobj->next;
				p_gobj->next = gobj;
			}else{
				gobj->next = plinkhigh_gobjs[gobj->p_link];
				plinkhigh_gobjs[gobj->p_link] = gobj;
			}
			if(gobj->next != NULL){
				gobj->next->prev = gobj;
			}else{
				plinklow_gobjs[gobj->p_link] = gobj;
			}
			break;

			case 3:
			HSD_GObj* prev = p_gobj->prev;
			gobj->prev = prev;
			if(prev != NULL){
				gobj->next = prev->next;
				prev->next = gobj;
			}else{
				gobj->next = plinkhigh_gobjs[gobj->p_link];
				plinkhigh_gobjs[gobj->p_link] = gobj;
			}
			if(gobj->next != NULL){
				gobj->next->prev = gobj;
			}else{
				plinklow_gobjs[gobj->p_link] = gobj;
			}
			break;
		}
	}
	return gobj;
}

//803901F0
HSD_GObj* GObj_Create(u32 class, u32 p_link, u32 p_prio){
	return CreateGObj(0, class, p_link, p_prio, 0);
}

//80390228
void GObj_Free(HSD_GObj* gobj){
	assert(gobj);
	if( ((gobj_def_3.flags >> 7) & 1) != 0 || gobj != current_gobj){
		GObj_CallDestructor(gobj);
		GObj_CallHSDDestructor(gobj);
		sub_8038FED4(gobj);

		if(gobj->gx_link != GOBJ_NOREF){
			GObj_GXLinkDestructor(gobj);
		}

		if(gobj->prev != NULL){
			gobj->prev->next = gobj->next;
		}else{
			plinkhigh_gobjs[gobj->p_link] = gobj->next;
		}
		if(gobj->next != NULL){
			gobj->next->prev = gobj->prev;
		}else{
			plinklow_gobjs[gobj->p_link] = gobj->prev;
		}
		HSD_ObjFree(&gobj_def_1, gobj);
	}else{
		gobj_def_3.flags = (gobj_def_3.flags & 0xBF) | 0x40;
	}
}

//8039063C
void GObj_GXReorder(HSD_GObj* gobj, HSD_GObj* hiprio_gobj){
	gobj->prev_gx = hiprio_gobj;
	if(hiprio_gobj != NULL){ //If there is a higher priority GObj, make gobj the next in GX order
		gobj->next_gx = hiprio_gobj->next_gx;
		hiprio_gobj->next_gx = gobj;
	}else{ //If there's not a higher priority GObj of this type, this gobj becomes the highest priority
		gobj->next_gx = highestprio_gobjs[gobj->gx_link];
		highestprio_gobjs[gobj->gx_link] = gobj;
	}
	if(gobj->next_gx != NULL){
		gobj->next_gx->prev_gx = gobj;
	}else{
		lowestprio_gobjs[gobj->gx_link] = gobj;
	}
}

//8039069C
void GObj_SetupGXLink(HSD_GObj* gobj, void* render_cb, u32 gx_link, u32 priority){
	assert(gx_link < GX_LINK_MAX);
	gobj->render_cb = render_cb;
	gobj->gx_link = gx_link;
	gobj->render_priority = priority;

	HSD_GObj* i = NULL;
	for(i = lowestprio_gobjs[gx_link]; i != NULL && (i->render_priority > gobj->render_priority); i = gobj->prev_gx){ 
		//Works backwards from lowest to highest priority till it finds the highest priority to be it's parent obj
		//Returns null if nothing is a higher priority than the current object or if there is none of that type
	}
	GObj_GXReorder(gobj, i);
}

//8039075C
void GObj_SetupGXLink_Max(HSD_GObj* gobj, void* render_cb, u32 priority){
	gobj->render_cb = render_cb;
	gobj->gx_link = GX_LINK_MAX + 1;
	gobj->render_priority = priority;
	HSD_GObj* i = NULL;
	for(i = lowestprio_gobjs[gobj->gx_link]; i != NULL && (i->render_priority > gobj->render_priority); i = gobj->prev_gx){ 
		//Works backwards from lowest to highest priority till it finds the highest priority to be it's parent obj
		//Returns null if nothing is a higher priority than the current object or if there is none of that type
	}
	GObj_GXReorder(gobj, i);
}

//8039084C
void GObj_GXLinkDestructor(HSD_GObj* gobj){
	assert(gobj->gx_link != GOBJ_NOREF);
	if(gobj->prev_gx != NULL){
		gobj->prev_gx->next_gx = gobj->next_gx;
	}else{
		highestprio_gobjs[gobj->gx_link] = gobj->next_gx;
	}
	if(gobj->next_gx != NULL){
		gobj->next_gx->prev_gx = gobj->prev_gx;
	}else{
		lowestprio_gobjs[gobj->gx_link] = gobj->prev_gx;
	}
	gobj->gx_link = GOBJ_NOREF;
	gobj->render_priority = 0;
	gobj->prev_gx = NULL;
	gobj->next_gx = NULL;
}

//80390A70
void GObj_InitKindObj(HSD_GObj* gobj, s8 obj_kind, void* obj_ptr){
	assert(gobj->obj_kind == GOBJ_NOREF);
	gobj->obj_kind = obj_kind;
	gobj->data = obj_ptr;
}

//80390ADC
void* GObj_NullObj_ReturnPtr(HSD_GObj* gobj){
	if(gobj->obj_kind == GOBJ_NOREF){
		return NULL;
	}
	gobj->obj_kind = GOBJ_NOREF;
	void* obj = gobj->hsd_obj;
	gobj->hsd_obj = NULL;
	return obj;
}

//80390B0C
void GObj_CallHSDDestructor(HSD_GObj* gobj){
	if(gobj->obj_kind != GOBJ_NOREF){
		void (*destructor)(void*) = hsd_destructors[gobj->obj_kind];
		(*destructor)(gobj->hsd_obj);
		
		gobj->obj_kind = GOBJ_NOREF;
		gobj->hsd_obj = NULL;
	}
}

//80390B68
void GObj_InitKindData(HSD_GObj* gobj, s8 data_kind, void* destructor_func, void* data_ptr){
	assert(gobj->data_kind == GOBJ_NOREF);
	gobj->data_kind = data_kind;
	gobj->data = data_ptr;
	gobj->user_data_remove_func = destructor_func;
}

//80390BE4
void GObj_CallDestructor(HSD_GObj* gobj){
	if(gobj->data_kind != GOBJ_NOREF){
		assert(gobj->user_data_remove_func);
		(*gobj->user_data_remove_func)(gobj->data);
		
		gobj->data_kind = GOBJ_NOREF;
		gobj->data = NULL;
	}
}

//80390EB8
u32 GObj_GetFlagFromArray(u32 offset){
	return flag_array[offset];
}