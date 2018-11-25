#include "hsd_object.h"

HSD_ClassInfo hsdClass = { ObjInfoInit };

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
void* HSD_ObjAllocInit(void* obj_def, u32 size, u32 count){
	
}

//80381C18
void hsdInitClassInfo(HSD_ClassInfo* class_info, HSD_ClassInfo* parent_info, char* base_class_library, char* type, u64 info_size, u64 class_size){
	class_info->initialized = TRUE;
	class_info->base_class_library = base_class_library;
	class_info->obj_size = (u16)class_size;
	class_info->info_size = (u16)info_size;
	class_info->parent_info = parent_info;
	class_info->next_info = NULL;
	class_info->child_info = NULL;
	class_info->unk20 = NULL;
	class_info->unk24 = NULL;
	
	if(parent_info != NULL){
		if(parent_info->initialized == TRUE){
			assert(class_info->obj_size >= parent_info->obj_size);
			assert(class_info->info_size >= parent_info->info_size);
			memcpy(&class_info->unk28, &parent_info->unk28, parent_info->info_size - 0x28);
			class_info->next_info = parent_info->child_info;
			parent_info->child_info = class_info;
		}else{
			parent_info->ObjInfoInit();
		}
	}
}