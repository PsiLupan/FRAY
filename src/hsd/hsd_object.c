#include "hsd_object.h"

//8037ABC8
void* HSD_ObjAlloc(void* obj_def){
	
}

//8037AD20
void HSD_ObjFree(void* init_obj, void* obj){
	obj[0] = init_obj[1];
	init_obj[1] = obj;
	init_obj[3] += 1;
	init_obj[2] -= 1;
}

//8037AD48
void* HSD_ObjAllocInit(void* obj_def, u32 size, u32 count){}