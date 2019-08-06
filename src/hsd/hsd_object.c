#include "hsd_object.h"

#include <stdlib.h>

static void ObjInfoInit();
static void _hsdInfoInit();

HSD_ObjAllocData* alloc_datas;

HSD_ObjInfo hsdObj = { 
	ObjInfoInit
};

HSD_ClassInfo hsdClass = {
	_hsdInfoInit
};

static struct {
	u32 heap_start;
	u32 heap_pos;
	u32 size;
	u32 bytes_remaining;
} objheap; //80406E48

//8037A94C
void HSD_ObjSetHeap(u32 size, void* start){
	objheap.heap_start = (u32)start;
	objheap.heap_pos = (u32)start;
	objheap.size = size;
	objheap.bytes_remaining = size;
}

static HSD_ObjAllocData* current_obj = NULL; //r13_3FC0

//8037A968
s32 HSD_ObjAllocAddFree(HSD_ObjAllocData* obj_def, u32 num){
	assert(obj_def != NULL);
	u32 size = obj_def->size * num;
	u32 obj_pos;
	if(objheap.heap_start == 0){
		void* heap = HSD_MemAlloc(size);
		if(heap == NULL){
			return 0;
		}
		objheap.bytes_remaining -= size;
	}else{
		u32 heap_end = (objheap.heap_start + objheap.size);
		obj_pos = (objheap.heap_pos + obj_def->align & ~obj_def->align);
		if(heap_end < obj_pos){
			return 0;
		}
		if((heap_end - obj_pos) < size){
			size = ((heap_end - obj_pos) / objheap.size) * objheap.size;
		}
		num = size / objheap.size;
		if(num == 0){
			return 0;
		}
		objheap.heap_pos = obj_pos + size;
		objheap.bytes_remaining = heap_end - objheap.heap_pos;
	}

	u32 inum = num - 1;
	u32 offset = 0;
	if(inum != 0){
		u32 snum = inum >> 3;
		if(snum != 0){
			do {
			    *(void **)(obj_pos + obj_def->size * offset) = (void *)(obj_pos + obj_def->size * (offset + 1));
				*(void **)(obj_pos + obj_def->size * (offset + 1)) = (void *)(obj_pos + obj_def->size * (offset + 2));
        		*(void **)(obj_pos + obj_def->size * (offset + 2)) = (void *)(obj_pos + obj_def->size * (offset + 3));
        		*(void **)(obj_pos + obj_def->size * (offset + 3)) = (void *)(obj_pos + obj_def->size * (offset + 4));
				*(void **)(obj_pos + obj_def->size * (offset + 4)) = (void *)(obj_pos + obj_def->size * (offset + 5));
				*(void **)(obj_pos + obj_def->size * (offset + 5)) = (void *)(obj_pos + obj_def->size * (offset + 6));
				*(void **)(obj_pos + obj_def->size * (offset + 6)) = (void *)(obj_pos + obj_def->size * (offset + 7));
				*(void **)(obj_pos + obj_def->size * (offset + 7)) = (void *)(obj_pos + obj_def->size * (offset + 8));
				offset += 8;
				snum -= 1;
			} while(snum > 0);
			inum &= 7;
			if(inum == 0){
				*(void **)(obj_pos + obj_def->size * offset) = obj_def->freehead;
				obj_def->freehead = (void*)obj_pos;
				obj_def->free = obj_def->free + num;
				return num;
			}
		}
		do {
			*(void **)(obj_pos + obj_def->size * offset) = (void *)(obj_pos + obj_def->size * (offset + 1));
			offset += 1;
			inum -= 1;
		} while(inum > 0);
	}
	*(void **)(obj_pos + obj_def->size * offset) = obj_def->freehead;
	obj_def->freehead = (void*)obj_pos;
	obj_def->free = obj_def->free + num;
	return num;
}

//8037ABC8
void* HSD_ObjAlloc(HSD_ObjAllocData* obj_def){
	if (((obj_def->flags >> 7) & 1) == 0 || obj_def->used < obj_def->num_limit){
		if ((obj_def->flags >> 6) & 1){
			if (obj_def->heap_limit_num == -1){
				u64 bytes_avail;
				if (objheap.heap_start != 0){
					bytes_avail = objheap.bytes_remaining;
				}else{
					bytes_avail = SYS_GetArena1Size();
				}
				if (bytes_avail <= obj_def->heap_limit_size){
					obj_def->heap_limit_num = obj_def->used + obj_def->free;
				}
			}else{
				u64 bytes_avail;
				if (objheap.heap_start){
					bytes_avail = objheap.bytes_remaining;
				}
				else{
					bytes_avail = SYS_GetArena1Size();
				}
				if (bytes_avail > obj_def->heap_limit_size){
					obj_def->heap_limit_num = -1;
				}
			}
		}
		if (obj_def->free == 0){
			HSD_ObjAllocAddFree(obj_def, 1);
			if (obj_def->free == 0){
				return NULL;
			}
		}
		HSD_ObjAllocLink *obj_ptr = obj_def->freehead->next;
		obj_def->freehead = obj_ptr;
		obj_def->used += 1;
		obj_def->free -= 1;
		if (obj_def->used > obj_def->peak){
			obj_def->peak = obj_def->used;
		}
		return obj_ptr;
	}
	return NULL;
}

//8037AD20
void HSD_ObjFree(HSD_ObjAllocData* init_obj, HSD_ObjAllocLink* obj){
	free(obj);
	/*obj->next = init_obj->freehead->next;
	init_obj->freehead = obj;
	init_obj->free += 1;
	init_obj->used -= 1;*/
}

//8037AD48
void HSD_ObjAllocInit(HSD_ObjAllocData* init_obj, u32 size, u32 align){
	assert(init_obj != NULL);
	HSD_ObjAllocData* obj = current_obj;
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

//8037E6C4
static void ObjInfoInit(){
	hsdInitClassInfo(HSD_CLASS_INFO(&hsdObj), HSD_CLASS_INFO(&hsdClass), HSD_BASE_CLASS_LIBRARY, "has_obj", sizeof(HSD_ObjInfo), 8);
}

//80381BE4
static void hsdObjInfoInit(HSD_ClassInfo* info){
	if((info->head.flags & 1) == 0){
		(*info->head.info_init)();
	}
}

//80381C18
void hsdInitClassInfo(HSD_ClassInfo* class_info, HSD_ClassInfo* parent_info, char* base_class_library, char* type, s32 info_size, s32 class_size){
	class_info->head.flags = 1;
	class_info->head.library_name = base_class_library;
	class_info->head.obj_size = (s16)class_size;
	class_info->head.info_size = (s16)info_size;
	class_info->head.parent = parent_info;
	class_info->head.next = NULL;
	class_info->head.child = NULL;
	class_info->head.nb_exist = 0;
	class_info->head.nb_peak = 0;
	
	if(parent_info != NULL){
		if((parent_info->head.flags & 1) == 0){
			(*parent_info->head.info_init)();
		}
		assert(class_info->head.obj_size >= parent_info->head.obj_size);
		assert(class_info->head.info_size >= parent_info->head.info_size);
		memcpy(&class_info->alloc, &parent_info->alloc, parent_info->head.info_size - 0x28);
		class_info->head.next = parent_info->head.child;
		parent_info->head.child = class_info;
	}
}

//803821C4
static HSD_Class* _hsdClassAlloc(HSD_ClassInfo* info){
	HSD_FreeList* mem_piece = HSD_MemAlloc(info->head.obj_size); //hsdAllocMemPiece(info->head.obj_size);
	if(mem_piece != NULL){
		info->head.nb_exist += 1;
		if(info->head.nb_peak < info->head.nb_exist){
			info->head.nb_peak = info->head.nb_exist;
		}
	}
	return (HSD_Class*)mem_piece;
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
static void _hsdClassDestroy(HSD_Class* class){
	HSD_ClassInfo* info = (class->class_info ? class->class_info : NULL);
	if(info != NULL){
		info->head.nb_exist -= 1;
		u32 size = info->head.obj_size + 0x1F;
		HSD_MemoryEntry* entry = GetMemoryEntry((size >> 5) + (u32)((s32)size < 0 && (size & 0x1F) != 0) + -1);
		((HSD_FreeList*)info)->next = entry->data;
		entry->data = (HSD_FreeList*)info;
		entry->nb_free += 1;
	}
}

//80382294
static void _hsdClassAmnesia(HSD_ClassInfo* info){
	info->head.nb_exist = 0;
	info->head.nb_peak = 0;
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
	hsdClass.alloc = _hsdClassAlloc; 
	hsdClass.init = _hsdClassInit; 
	hsdClass.release = _hsdClassRelease; 
	hsdClass.destroy = _hsdClassDestroy; 
	hsdClass.amnesia = _hsdClassAmnesia;
}

//80382344
void* hsdNew(HSD_ClassInfo* info){
	if((info->head.flags & 1) == 0){
		(*info->head.info_init)();
	}

	HSD_Class* class = (*info->alloc)(info);
	if(class == NULL){
		return NULL;
	}
	hsdObjInfoInit(info);
	memset(class, 0, info->head.obj_size);
	class->class_info = info;
	(*info->init)(class);
	/*  So, the compiler optimized away hsdClassInit to just return 0, which affects
		every init function that would normally return an int, possibly -1.
		Otherwise, the below would look something like:

		if(info->init(class) < 0){
			(*info->destroy)(class);
			return NULL;
		}
	*/
	return class;
}

//80382814
HSD_ClassInfo* hsdSearchClassInfo(char* class_name){
	return NULL; //class_name is never set, so why the fuck would I write this?
}