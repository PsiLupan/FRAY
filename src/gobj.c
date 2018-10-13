#include "gobj.h"

//80390BE4
void GObj_CallDestructor(HSD_GObj* gobj){
	if(gobj->ref_flag != GOBJ_NODATA){
		assert(gobj->user_data_remove_func);
		(*gobj->user_data_remove_func)(gobj->data);
		
		gobj->ref_flag = GOBJ_NODATA;
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