#include "hsd_memory.h"

static u32 alignment_sz = 0x20;

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
	OSFreeToHeap(HSD_GetHeap(), ptr);
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
	int v16; // r13@0
	int v21; // r13@8
	int v25; // r13@14
	void* addr;
	int v28; // r13@16
	int v29; // r13@16
	int v31; // r3@16
	int v32; // r31@16
	int v33; // r10@16
	int v34; // r9@16
	int v35; // r8@16
	int v36; // r7@16
	int v37; // r6@16
	int v38; // r5@16
	double v39; // fp8@16
	double v40; // fp7@16
	double v41; // fp6@16
	double v42; // fp5@16
	double v43; // fp4@16
	double v44; // fp3@16
	double v45; // fp2@16
	double v46; // fp1@16
	int v47; // r4@16
	int v48; // r31@17
	void* entry_addr;
	int v51; // r5@20
	BOOL v52; // r6@20
	int v53; // ctr@20
	int v54; // r4@20
	u32 *v55; // r3@20
	int v56; // r5@22
	int v58; // r6@25
	int v59; // r5@25
	int v60; // ctr@25
	u32 *v61; // r3@25

	assert(size >= 0);
	if ( size >= alignment_sz )
	{
		if ( alignment_sz > 0 ){
			u32 i = 2 * alignment_sz;
			while(size >= i){
				i *= 2;
			}
			addr = HSD_MemAlloc(4 * i);
			if ( addr == NULL )
				return NULL;
			memcpy(addr, *(u32 *)(v25 - 16288), 4 * alignment_sz);
			memset((u32 *)(addr + 4 * alignment_sz), 0, 4 * (i - alignment_sz));
			v31 = *(u32 *)(v29 - 16288);
			alignment_sz = i;
			v32 = 4 * alignment_sz & 0xFFFFFFE0;
			*(u32 *)(v29 - 16288) = addr;
			hsdFreeMemPiece(v31, v32);
			v47 = *(u32 *)(*(u32 *)(v16 - 16288) + (((u32)(v32 + 31) >> 3) & 0x1FFFFFFC) - 4);
			++*(u32 *)(v47 + 4);
		} else {
			u32 j = 32;
			while(size >= j){
				j *= 2;
			}
			addr = HSD_MemAlloc(4 * j);
			*(u32 *)(v21 - 16288) = addr;
			if ( addr == NULL )
				return NULL;
			memset(addr, 0, 4 * j);
			alignment_sz = j;
		}
	}
	v48 = 4 * size;
	if ( !*(u32 *)(*(u32 *)(v16 - 16288) + 4 * size) ) {
		entry_addr = HSD_MemAlloc(0x14u);
		if ( !entry_addr )
			return NULL;
		memset(entry_addr, 0, 0x14u);
		*entry_addr = 32 * (size + 1);
		v51 = size - 1;
		v52 = FALSE;
		*(u32 *)(*(u32 *)(v16 - 16288) + v48) = entry_addr;
		v53 = size;
		v54 = *(u32 *)(v16 - 16288);
		v55 = (u32 *)(v54 + 4 * (size - 1));
		if ( size - 1 >= 0 ){
			while ( !*v55 ){
				--v55;
				--v51;
				if ( !--v53 )
					goto LABEL_24;
			}
			v56 = 4 * v51;
			v52 = TRUE;
			entry_addr[4] = *(u32 *)(*(u32 *)(v54 + v56) + 16);
			*(u32 *)(*(u32 *)(*(u32 *)(v16 - 16288) + v56) + 16) = entry_addr;
		}
LABEL_24:
		if ( v52 == FALSE ){
			v58 = size + 1;
			v59 = *(u32 *)(v16 - 16288);
			v60 = alignment_sz - (size + 1);
			v61 = (u32 *)(v59 + 4 * (size + 1));
			if ( size + 1 < alignment_sz ){
				while ( !*v61 ){
				++v61;
				++v58;
				if ( !--v60 )
					return *(u32 *)(*(u32 *)(v16 - 16288) + v48);
				}
				v50[4] = *(u32 *)(v59 + 4 * v58);
			}
		}
	}
	return *(u32 *)(*(u32 *)(v16 - 16288) + v48);
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
	return addr;
}