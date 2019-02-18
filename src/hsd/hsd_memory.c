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
		assert(result);
	}
	return result;
}

//80381D58
void* GetMemoryEntry(u32 idx){
	int v16; // r13@0
	int v17; // r29@1
	int v18; // r0@3
	s32 j; // r31@5
	int v20; // r3@8
	int v21; // r13@8
	int *v22; // r3@8
	int i; // r31@11
	int v25; // r13@14
	int v26; // r3@14
	int v27; // r30@14
	int v28; // r13@16
	int v29; // r13@16
	int v30; // r0@16
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
	int *v49; // r3@18
	int *v50; // r30@18
	int v51; // r5@20
	s32 v52; // r6@20
	int v53; // ctr@20
	int v54; // r4@20
	u32 *v55; // r3@20
	int v56; // r5@22
	int v57; // r4@25
	int v58; // r6@25
	int v59; // r5@25
	int v60; // ctr@25
	u32 *v61; // r3@25

	v17 = idx;
	assert(idx >= 0);
	v18 = alignment_sz;
	if ( v17 >= v18 )
	{
		if ( v18 ){
			for ( i = 2 * v18; v17 >= i; i *= 2 );
			v26 = HSD_MemAlloc(4 * i);
			v27 = v26;
			if ( !v26 )
				return NULL;
			memcpy(v26, *(u32 *)(v25 - 16288), 4 * *(u32 *)(v25 - 16284));
			memset((int *)(v27 + 4 * *(u32 *)(v28 - 16284)), 0, 4 * (i - *(u32 *)(v28 - 16284)));
			v30 = *(u32 *)(v29 - 16284);
			v31 = *(u32 *)(v29 - 16288);
			*(u32 *)(v29 - 16284) = i;
			v32 = 4 * v30 & 0xFFFFFFE0;
			*(u32 *)(v29 - 16288) = v27;
			hsdFreeMemPiece(v31, v32, v38, v37, v36, v35, v34, v33, v46, v45, v44, v43, v42, v41, v40, v39);
			v47 = *(u32 *)(*(u32 *)(v16 - 16288) + (((unsigned int)(v32 + 31) >> 3) & 0x1FFFFFFC) - 4);
			++*(u32 *)(v47 + 4);
		} else {
			for ( j = 32; v17 >= j; j *= 2 );
			v20 = HSD_MemAlloc(4 * j);
			*(u32 *)(v21 - 16288) = v20;
			v22 = *(int **)(v21 - 16288);
			if ( !v22 )
				return NULL;
			memset(v22, 0, 4 * j);
			*(u32 *)(v16 - 16284) = j;
		}
	}
	v48 = 4 * v17;
	if ( !*(u32 *)(*(u32 *)(v16 - 16288) + 4 * v17) ) {
		v49 = (int *)HSD_MemAlloc(20);
		v50 = v49;
		if ( !v49 )
			return NULL;
		memset(v49, 0, 0x14u);
		*v50 = 32 * (v17 + 1);
		v51 = v17 - 1;
		v52 = 0;
		*(u32 *)(*(u32 *)(v16 - 16288) + v48) = v50;
		v53 = v17;
		v54 = *(u32 *)(v16 - 16288);
		v55 = (u32 *)(v54 + 4 * (v17 - 1));
		if ( v17 - 1 >= 0 ){
			while ( !*v55 ){
				--v55;
				--v51;
				if ( !--v53 )
					goto LABEL_24;
			}
			v56 = 4 * v51;
			v52 = 1;
			v50[4] = *(u32 *)(*(u32 *)(v54 + v56) + 16);
			*(u32 *)(*(u32 *)(*(u32 *)(v16 - 16288) + v56) + 16) = v50;
		}
LABEL_24:
		if ( !v52 ){
			v57 = *(u32 *)(v16 - 16284);
			v58 = v17 + 1;
			v59 = *(u32 *)(v16 - 16288);
			v60 = v57 - (v17 + 1);
			v61 = (u32 *)(v59 + 4 * (v17 + 1));
			if ( v17 + 1 < v57 ){
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


//8038216C
void hsdFreeMemPiece(void* addr, u32 len){
	void* v16; // r31@1
	u8 v17; // xer_ca@2
	
	v16 = addr;
	if ( addr ){
		addr = GetMemoryEntry(((len + 31) >> 5) + v17 - 1);
		*v16 = *(void *)(addr + 12);
		*(void *)(addr + 12) = v16;
		++*(u32 *)(addr + 8);
	}
	return addr;
}