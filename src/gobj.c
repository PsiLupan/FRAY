#include "gobj.h"

#define P_LINK_MAX 64
#define GX_LINK_MAX 64
static HSD_GObj** plinkhigh_gobjs[P_LINK_MAX]; //r13_3E74
static HSD_GObj** plinklow_gobjs[P_LINK_MAX]; //r13_3E78
static HSD_GObj** highestprio_gobjs[GX_LINK_MAX]; //r13_3E7C
static HSD_GObj** lowestprio_gobjs[GX_LINK_MAX]; //r13_3E80
static void** hsd_destructors[14]; //r13_3E90 - Length is currently made up, TODO: need to explictly assign the functions to this at some point

//80086960
bool GObj_IsPlayer(HSD_GObj* gobj){
	if(gobj->classifier == 0x04){
		return true;
	}
	return false;
}

//8001CEC0
//80176BCC
//80176D18
void GObj_AnimAll_Callback(HSD_GObj* gobj){
    HSD_JObjAnimAll((HSD_JObj*)gobj->hsd_obj);
}

//80390228
void GObj_Free(HSD_GObj* gobj){
	assert(gobj);
	if( ((unk_804CE3E4 >> 7) & 1) != 0 || gobj != r13_3E84){
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
		HSD_ObjFree(&unk_804CE38C, gobj);
	}else{
		unk_804CE3E4 = (unk_804CE3E4 & 0xBF) | 0x40;
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
void GObj_InitKindObj(HSD_GObj* gobj, s32 obj_kind, void* obj_ptr){
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