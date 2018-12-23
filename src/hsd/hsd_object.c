#include "hsd_object.h"

HSD_ClassInfo hsdClass = { ObjInfoInit };

static struct {
	void* heap_start;
	void* heap_pos;
	u32 size;
	u32 bytes_remaining;
} objheap; //80406E48

//8037A94C
void HSD_ObjSetHeap(u32 size, void* unk){
	objheap.heap_start = unk;
	objheap.heap_pos = unk;
	objheap.size = size;
	objheap.bytes_remaining = size;
}

static void** current_obj;

//8037A968
void HSD_ObjAllocAddFree(HSD_ObjDef* obj_def, u32 unk){
	assert(obj_def != NULL);
}

//8037ABC8
void* HSD_ObjAlloc(HSD_ObjDef* obj_def){
	if( ((obj_def->flags >> 7) & 1) && (obj_def->unk_ctr >= obj_def->unk_14) ){
		return NULL;
	}
	if( ((obj_def->flags >> 6) & 1) ){
		if(obj_def->unk_1C == -1){
			u64 bytes_avail;
			if(objheap.heap_start){
				bytes_avail = objheap.bytes_remaining;
			}else{
				bytes_avail = OSCheckHeap(HSD_GetHeap());
			}
			if(bytes_avail <= obj_def->obj_size){
				obj_def->unk_1C = obj_def->unk_ctr + obj_def->unk_ctr2;
			}
		}else{
			u64 bytes_avail;
			if(objheap.heap_start){
				bytes_avail = objheap.bytes_remaining;
			}else{
				bytes_avail = OSCheckHeap(HSD_GetHeap());
			}
			if(bytes_avail > obj_def->obj_size){
				obj_def->unk_1C = -1;
			}
		}
	}
	if(obj_def->unk_ctr2 == 0){
		HSD_ObjAllocAddFree(obj_def, 1);
		if(obj_def->unk_ctr2 == 0){
			return NULL;
		}
	}
	void* obj_ptr = *obj_def->obj_ptr;
	obj_def->obj_ptr = obj_ptr;
	obj_def->unk_ctr += 1;
	obj_def->unk_ctr2 -= 1;
	if(obj_def->unk_ctr > obj_def->unk_ctr3){
		obj_def->unk_ctr3 = obj_def->unk_ctr;
	}
	return obj_ptr;
}

//8037AD20
void HSD_ObjFree(HSD_ObjDef* init_obj, u32* obj){
	obj[0] = init_obj->obj_ptr;
	init_obj->obj_ptr = obj;
	init_obj->unk_ctr2 += 1;
	init_obj->unk_ctr -= 1;
}

//8037AD48
void HSD_ObjAllocInit(u32* init_obj, u32 size, u32 count){
	assert(init_obj != NULL);
	while(true){
		u32* obj = *current_obj;
		if(!*current_obj){
			break;
		}
		if(obj == init_obj){
			*current_obj = (void*)obj[10];
		}else{
			current_obj = (void**)(obj + 0x28);
		}
	}
	memset(init_obj, 0, 0x2Cu);
	init_obj[5] = -1;
	init_obj[6] = 0;
	init_obj[7] = -1;
	init_obj[9] = count - 1;
	init_obj[8] = (size + init_obj[9]) & ~init_obj[9];
	init_obj[10] = *current_obj;
	*current_obj = init_obj;
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