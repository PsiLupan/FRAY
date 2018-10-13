#include "gobj.h"

//80390228
void GObj_Free(HSD_GObj* gobj){
	assert(gobj);
	
	
}

//80390B0C
void GObj_CallHSDDestructor(HSD_GObj* gobj){
	if(gobj->obj_kind != GOBJ_NOREF){
		void (*destructor)(void*) = /*r13_3E90 +*/ 4 * (gobj->obj_kind);
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

//???? - Fuck me where was it
bool GObj_IsPlayer(HSD_GObj* gobj){
	if(gobj->classifier == 0x04){
		return true;
	}
	return false;
}