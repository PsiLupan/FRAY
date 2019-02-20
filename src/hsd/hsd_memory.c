#include "hsd_memory.h"

static HSD_MemoryEntry** entrybysz;
static u32 sz_entries = 32;

static u32 hsd_iddata[4][12];
static u8 hsd_idtable[404]; //804C23EC

//8037CD80
u32* HSD_IDGetAllocData(){
	return &hsd_iddata;
}

//8037CD8C
void HSD_IDInitAllocData(){
	HSD_ObjAllocInit(&hsd_iddata, 12, 4);
}

//8037CDBC
void* HSD_IDSetup(){
	return memset(&hsd_idtable, 0, 404);
}

//8037D020
void _HSD_IDForgetMemory(){
	return memset(&hsd_idtable, 0, 404);
}

//8037F1B0
void HSD_Free(void* ptr){
	//OSFreeToHeap(HSD_GetHeap(), ptr);
	free(ptr);
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
HSD_MemoryEntry* GetMemoryEntry(u32 size){
	int v47; // r4@16
	int v51; // r5@20
	BOOL v52; // r6@20
	int v53; // ctr@20
	u32 *v55; // r3@20
	int v58; // r6@25
	int v59; // r5@25
	int v60; // ctr@25
	u32 *v61; // r3@25

	assert(size >= 0);
	if ( size >= sz_entries )
	{
		if ( sz_entries > 0 ){ //Basically amounts to resizing the array by doubling the size and copying entries to a new array
			u32 i = 2 * sz_entries;
			while(size >= i){
				i *= 2;
			}
			HSD_MemoryEntry* entry = (HSD_MemoryEntry*)HSD_MemAlloc(4 * i);
			if ( entry == NULL ){
				return NULL;
			}
			memcpy(entry, entrybysz, 4 * sz_entries);
			memset(&entry[sz_entries], 0, 4 * (i - sz_entries)); //You start *after* existing ptrs and make sure memory is zero'd
			HSD_MemoryEntry** org_entrybysz = entrybysz;
			u32 tmp = sz_entries;
			u32 offset = 4 * tmp & 0xFFFFFFE0;
			sz_entries = i;
			entrybysz = &entry;
			hsdFreeMemPiece(org_entrybysz, sz_entries);
			v47 = *(u32 *)(*entrybysz + (((u32)(offset + 31) >> 3) & 0x1FFFFFFC) - 4);
			v47->x4_unk += 1;
		} else { //In this case, it's uninitialized and allocs the array
			u32 j = 32;
			while(size >= j){
				j *= 2;
			}
			entry = (HSD_MemoryEntry*)HSD_MemAlloc(4 * j);
			entrybysz = &entry;
			if ( entry == NULL ){
				return NULL;
			}
			memset(entry, 0, 4 * j);
			sz_entries = j;
		}
	}
	if ( entrybysz[size] == NULL ) {
		HSD_MemoryEntry* entry = (HSD_MemoryEntry*)HSD_MemAlloc(0x14u);
		if ( entry == NULL ){
			return NULL;
		}
		memset(entry, 0, 0x14u);
		entry->x0_unk = 32 * (size + 1);
		v51 = size - 1;
		v52 = FALSE;
		entrybysz[size] = entry;
		v53 = size;
		v55 = entrybysz[size - 1];
		if ( size - 1 >= 0 ){
			while ( !*v55 ){
				--v55;
				--v51;
				if ( !--v53 )
					goto JUMP_OUT;
			}
			v52 = TRUE;
			entry->x10_unk = entrybysz[v51]->x10_unk;
			entrybysz[v51]->x10_unk = entry;
		}
JUMP_OUT:
		if ( v52 == FALSE ){
			v58 = size + 1;
			v59 = *(u32 *)(v16 - 16288);
			v60 = sz_entries - (size + 1);
			v61 = entrybysz[size + 1];
			if ( size + 1 < sz_entries ){
				while ( !*v61 ){
					++v61;
					++v58;
					if ( !--v60 )
						return entrybysz[size];
				}
				entry->x10_unk = entrybysz[v58];
			}
		}
	}
	return entrybysz[size];
}

//80381FA8
void hsdAllocMemPiece(){

}


//8038216C
void hsdFreeMemPiece(void* mem, u32 size){
	HSD_MemoryEntry* entry;
	HSD_MemPiece* piece = (HSD_MemPiece*)mem;

	if ( mem != NULL ){
		entry = GetMemoryEntry(((size + 31) >> 5) - 1);
		piece->prev = entry->data;
		entry->data = piece;
		entry->free_pieces += 1;
	}
	return entry;
}