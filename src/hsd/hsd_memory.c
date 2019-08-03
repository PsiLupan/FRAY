#include "hsd_memory.h"

#include <stdint.h>
#include <stdlib.h>

static HSD_MemoryEntry** memory_list;
static u32 nb_memory_list = 0;

HSD_ObjAllocData free_alloc; //804C233C
HSD_ObjAllocData hsd_iddata; //804C23C0

HSD_IDTable default_table; //804C23EC

//8037CD80
HSD_ObjAllocData* HSD_IDGetAllocData(){
	return &hsd_iddata;
}

//8037CD8C
void HSD_IDInitAllocData(){
	HSD_ObjAllocInit(&hsd_iddata, 12, 4);
}

//8037CDBC
void* HSD_IDSetup(){
	return memset(&default_table, 0, 404);
}

//8037CDEC
void HSD_IDInsertToTable(HSD_IDTable* id_table, u32 id, void* data){
	if(id_table == NULL){
		id_table = &default_table;
	}
	IDEntry* t_entry = &id_table->table[id % 65]; 
	IDEntry* entry = t_entry; 
	while(entry != NULL && entry->id == id){
		entry = entry->next;
	}
	if(entry == NULL){
		entry = HSD_ObjAlloc(&hsd_iddata);
		assert(entry != NULL);
		memset(entry, 0, sizeof(IDEntry));
		entry->id = id;
		entry->data = data;
		entry->next = t_entry->next;
		t_entry->next = entry;

	}else{
		entry->id = id;
		entry->data = data;
	}
}

//8037CEE8
void HSD_IDRemoveByIDFromTable(HSD_IDTable* id_table, u32 id){
	if(id_table == NULL){
		id_table = &default_table;
	}
	IDEntry* entry = &id_table->table[id % 65];
	IDEntry* curr = NULL;
	IDEntry* prev = NULL;
	while(true) {
		curr = entry;
		if (curr == NULL) {
			return;
		}
		if (curr->id == id) 
			break;
		entry = curr->next;
		prev = curr;
	}
	if (prev == NULL) {
		entry = curr->next;
	}else {
		prev->next = curr->next;
	}
	HSD_ObjFree(&hsd_iddata, (HSD_ObjAllocLink*)curr);
}

//8037CF98
void* HSD_IDGetDataFromTable(HSD_IDTable* id_table, u32 id, u8* success){
	if(id_table == NULL){
		id_table = &default_table;
	}
	IDEntry* entry = &id_table->table[id % 65];
	while(true){
		if(entry == NULL){
			if(success != NULL){
				*success = 0;
			}
			return NULL;
		}
		if(entry->id = id)
			break;
		entry = entry->next;
	}
	if(success != NULL){
		*success = 1;
	}
	return entry->data;
}

//8037D020
void _HSD_IDForgetMemory(){
	memset(&default_table, 0, 101*sizeof(u32));
}

//8037F1B0
void HSD_Free(void* ptr){
	//OSFreeToHeap(HSD_GetHeap(), ptr);
	if(ptr != NULL){
		HSD_ObjFree(&free_alloc, (HSD_ObjAllocLink*)ptr);
	}
}

//8037F1E4
void* HSD_MemAlloc(u32 size){
	void* result = NULL;
	
	if(size > 0){	
		//result = OSAllocFromHeap(HSD_GetHeap(), size);
		result = malloc(size);
		assert(result != NULL);
	}
	return result;
}

//80381D58
HSD_MemoryEntry* GetMemoryEntry(u32 idx){
	assert(idx >= 0);
	if ( idx >= nb_memory_list )
	{
		if ( nb_memory_list > 0 ){ //Basically amounts to resizing the array by doubling the size and copying entries to a new array
			u32 i;
			u32 nb = nb_memory_list;
			do {
				i = nb;
				nb *= 2;
			}while(nb <= idx);
			HSD_MemoryEntry* entry = (HSD_MemoryEntry*)HSD_MemAlloc(8 * i);
			if ( entry == NULL ){
				return NULL;
			}
			memcpy(entry, memory_list, 4 * nb_memory_list);
			memset(&entry[nb_memory_list * 4], 0, 4 * (nb - nb_memory_list)); //You start *after* existing ptrs and make sure memory is zero'd
			
			HSD_MemoryEntry** mem_list = memory_list;
			u32 tmp = nb_memory_list;
			nb_memory_list = nb;
			i = 4 * (tmp & 0x3FFFFFF8);
			memory_list = &entry;
			hsdFreeMemPiece(mem_list, i);
			HSD_MemoryEntry* unk_entry = (HSD_MemoryEntry*)(*memory_list + (i + 31 >> 3 & 0x1FFFFFFC) - 4);
			unk_entry->nb_alloc += 1;
		} else { //In this case, it's uninitialized and allocs the array
			u32 j = 32;
			while(idx >= j){
				j *= 2;
			}
			HSD_MemoryEntry* entry = (HSD_MemoryEntry*)HSD_MemAlloc(4 * j);
			memory_list = &entry;
			if ( entry == NULL ){
				return NULL;
			}
			memset(entry, 0, 4 * j);
			nb_memory_list = j;
		}
	}

	s32 midx = idx;
	if ( memory_list[midx] == NULL ) {
		HSD_MemoryEntry* t_entry = (HSD_MemoryEntry*)HSD_MemAlloc(0x14u);
		if ( t_entry == NULL ){
			return NULL;
		}
		memset(t_entry, 0, 0x14u);
		t_entry->total_bits = 32 * (idx + 1);
		memory_list[midx] = t_entry;
		
		s32 i = idx - 1;
		u32* entry = (u32*)memory_list[i];
		u32 t_idx = idx;
		if ( i >= 0 ){
			do {
				if ( *entry != 0){
					t_entry->next = memory_list[i]->next;
					memory_list[i]->next = t_entry->next;
					return memory_list[midx];
				}
				entry = entry - 4;
				--i;
				--t_idx;
			} while(t_idx > 0);
		}
		idx += 1;
		t_idx = nb_memory_list - idx;
		u32* f_entry = (u32*)memory_list[idx];
		if ( idx < nb_memory_list ){
			do {
				if(*f_entry == 0){
					t_entry->next = memory_list[idx];
					break;
				}
				f_entry = f_entry + 4;
				++idx;
				--t_idx;
			} while(t_idx > 0);
		}
	}
	return memory_list[midx];
}

//80381FA8
void* hsdAllocMemPiece(u32 size){
	HSD_MemoryEntry* entry = NULL;
	HSD_MemoryEntry* entry_2 = NULL;
	HSD_FreeList* piece = NULL;

	u32 adj_size = size + 0x1F;
	u32 entry_sz = ((adj_size >> 5) + (u32)(adj_size < 0 && (adj_size & 0x1F) != 0) - 1);
	entry = GetMemoryEntry(entry_sz);
	if(entry == NULL){
		return NULL;
	}
	piece = entry->data;
	if(piece != NULL){
		entry->data = piece->next;
		entry->nb_free -= 1;
	}else{
		for(HSD_MemoryEntry* i = entry->next; i != NULL; i = i->next){
			if(i->data != NULL){
				adj_size = (i->total_bits - entry->total_bits) + 0x1F;
				entry_2 = GetMemoryEntry(adj_size >> 5) + (u32)(adj_size < 0 && (adj_size & 0x1F) != 0) - 1;
				if(entry_2 == NULL){
					return NULL;
				}
				piece = i->data;
				i->data = piece->next;
				i->nb_free -= 1;
				i->nb_alloc -= 1;
				
				HSD_FreeList* np = (HSD_FreeList*)((u8*)piece->next + entry->total_bits);
				np->next = entry_2->data;
				entry_2->data = np;
				entry_2->nb_alloc += 1;
				entry_2->nb_free += 1;
				entry->nb_alloc += 1;
				return (void*)piece;
			}
		}
		u32 tmp_size = (nb_memory_list - entry_sz) - 2;
		if(tmp_size < 0 || (entry_2 = GetMemoryEntry(tmp_size)) != NULL){
			piece = (HSD_FreeList*)HSD_MemAlloc(nb_memory_list * 32);
			if(piece != NULL){
				if(tmp_size >= 0){
					HSD_FreeList* np = (HSD_FreeList*)((u8*)piece->next + entry->total_bits);
					np->next = entry_2->data;
					entry_2->data = np;
					entry_2->nb_alloc += 1;
					entry_2->nb_free += 1;
				}
				entry->nb_alloc += 1;
			}else{
				return NULL;
			}
		}else{
			return NULL;
		}
	}
	return (void*)piece;
}

//8038216C
void hsdFreeMemPiece(void* mem, u32 size){
	HSD_MemoryEntry* entry;
	HSD_FreeList* piece = (HSD_FreeList*)mem;

	if ( mem != NULL ){
		entry = GetMemoryEntry(((size + 31) >> 5) - 1);
		piece->next = entry->data;
		entry->data = piece;
		entry->nb_free += 1;
	}
}
