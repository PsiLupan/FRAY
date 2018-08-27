#include "gobj.h"

bool GObj_IsPlayer(HSD_GObj* gobj){
	if(gobj->classifier == 0x04){
		return true;
	}
	return false;
}