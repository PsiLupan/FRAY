#include "hsd_object.h"

static void _hsdInfoInit();

HSD_ClassInfo hsdClass = { 
	_hsdInfoInit
};

static struct {
	void* heap_start;
	void* heap_pos;
	u32 size;
	u32 bytes_remaining;
} objheap; //80406E48

//8037A94C
void HSD_ObjSetHeap(u32 size, void* start){
	objheap.heap_start = start;
	objheap.heap_pos = start;
	objheap.size = size;
	objheap.bytes_remaining = size;
}

static HSD_ObjDef* current_obj = NULL;

//8037A968
void HSD_ObjAllocAddFree(HSD_ObjDef* obj_def, u32 num){
	assert(obj_def != NULL);
}

//8037ABC8
void* HSD_ObjAlloc(HSD_ObjDef* obj_def){
	if( ((obj_def->flags >> 7) & 1) && (obj_def->used >= obj_def->num_limit) ){
		return NULL;
	}
	if( ((obj_def->flags >> 6) & 1) ){
		if(obj_def->heap_limit_num == -1){
			u64 bytes_avail;
			if(objheap.heap_start){
				bytes_avail = objheap.bytes_remaining;
			}else{
				bytes_avail = SYS_GetArena1Size();
			}
			if(bytes_avail <= obj_def->heap_limit_size){
				obj_def->heap_limit_num = obj_def->used + obj_def->free;
			}
		}else{
			u64 bytes_avail;
			if(objheap.heap_start){
				bytes_avail = objheap.bytes_remaining;
			}else{
				bytes_avail = SYS_GetArena1Size();
			}
			if(bytes_avail > obj_def->heap_limit_size){
				obj_def->heap_limit_num = -1;
			}
		}
	}
	if(obj_def->free == 0){
		HSD_ObjAllocAddFree(obj_def, 1);
		if(obj_def->free == 0){
			return NULL;
		}
	}
	HSD_ObjAllocLink* obj_ptr = obj_def->freehead->next;
	obj_def->freehead->next = obj_ptr;
	obj_def->used += 1;
	obj_def->free -= 1;
	if(obj_def->used > obj_def->peak){
		obj_def->peak = obj_def->used;
	}
	return obj_ptr;
}

//8037AD20
void HSD_ObjFree(HSD_ObjDef* init_obj, u32* obj){
	obj[0] = init_obj->freehead->next;
	init_obj->freehead->next = (HSD_ObjAllocLink*)obj;
	init_obj->free += 1;
	init_obj->used -= 1;
}

//8037AD48
void HSD_ObjAllocInit(HSD_ObjDef* init_obj, u32 size, u32 align){
	assert(init_obj != NULL);
	HSD_ObjDef* obj = current_obj;
	while(obj != NULL){
		if(obj == init_obj){
			current_obj = obj->next;
		}else{
			obj = obj->next;
		}
	}
	memset(init_obj, 0, 0x2Cu);
	init_obj->num_limit = -1;
	init_obj->heap_limit_size = 0;
	init_obj->heap_limit_num = -1;
	init_obj->align = align - 1;
	init_obj->size = (size + init_obj->align) & ~init_obj->align;
	init_obj->next = current_obj;
	current_obj = init_obj;
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
	class_info->active_objs = 0;
	class_info->total_allocs = 0;
	
	if(parent_info != NULL){
		if(parent_info->initialized == TRUE){
			assert(class_info->obj_size >= parent_info->obj_size);
			assert(class_info->info_size >= parent_info->info_size);
			memcpy(&class_info->obj_alloc, &parent_info->obj_alloc, parent_info->info_size - 0x28);
			class_info->next_info = parent_info->child_info;
			parent_info->child_info = class_info;
		}else{
			parent_info->ObjInfoInit();
		}
	}
}

//803821C4
static void _hsdClassAlloc(HSD_ClassInfo* info){
	HSD_FreeList* mem_piece = hsdAllocMemPiece(info->obj_size);
	if(mem_piece != NULL){
		info->active_objs += 1;
		if(info->total_allocs < info->active_objs){
			info->total_allocs = info->active_objs;
		}
	}
}

//8038221C
static void _hsdClassInit(HSD_Class* info){
	return;
}

//80382224
static void _hsdClassRelease(HSD_Class* info){
	return;
}

//80382228
static void _hsdClassDestroy(HSD_ClassInfo* info){
	if(info != NULL){
		info->active_objs -= 1;
		u32 size = info->obj_size + 0x1F;
		HSD_MemoryEntry* entry = GetMemoryEntry((size >> 5) + (u32)((s32)size < 0 && (size & 0x1F) != 0) + -1);
		((HSD_FreeList*)info)->next = entry->data;
		entry->data = (HSD_FreeList*)info;
		entry->nb_free += 1;
	}
}

//80382294
static void _hsdClassAmnesia(HSD_ClassInfo* info){
	info->active_objs = 0;
	info->total_allocs = 0;
	if(info == &hsdClass){
		/*
		-0x3F98(r13) = 0;
		-0x3F9C(r13) = 0;
		-0x3FA0(r13) = 0;
		*/
	}
}

//803822C0
static void _hsdInfoInit(){
	hsdInitClassInfo(&hsdClass, NULL, HSD_BASE_CLASS_LIBRARY, "hsd_class", sizeof(HSD_ClassInfo), 4);
	hsdClass.obj_alloc = _hsdClassAlloc; 
	hsdClass.init = _hsdClassInit; 
	hsdClass.release = _hsdClassRelease; 
	hsdClass.destroy = _hsdClassDestroy; 
	hsdClass.amnesia = _hsdClassAmnesia;
}
