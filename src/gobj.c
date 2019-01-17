#include "gobj.h"

#define GX_LINK_MAX 64
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

//80390228
void GObj_Free(HSD_GObj* gobj){
	assert(gobj);
	
	
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

	HSD_GObj* i = lowestprio_gobjs[gx_link];
	for(; i != NULL && (i->render_priority > gobj->render_priority); i = gobj->prev_gx){ 
		//Works backwards from lowest to highest priority till it finds the highest priority to be it's parent obj
		//Returns null if nothing is a higher priority than the current object or if there is none of that type
	}
	GObj_GXReorder(gobj, i);
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

//80390BE4
void GObj_CallDestructor(HSD_GObj* gobj){
	if(gobj->data_kind != GOBJ_NOREF){
		assert(gobj->user_data_remove_func);
		(*gobj->user_data_remove_func)(gobj->data);
		
		gobj->data_kind = GOBJ_NOREF;
		gobj->data = NULL;
	}
}