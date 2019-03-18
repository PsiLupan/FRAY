#ifndef _hsd_display_h_
#define _hsd_display_h_

#include <ogc/gx.h>

#include "hsd_cobj.h"
#include "hsd_jobj.h"
#include "hsd_state.h"

typedef void (*HSD_PtclCallback)();

typedef struct _HSD_ZList {
	Mtx pmtx;
	MtxP vmtx;
	struct _HSD_JObj *jobj;
	u32 rendermode;
	
	struct {
		struct _HSD_ZList *texedge;
		struct _HSD_ZList *xlu;
	} sort;
	struct _HSD_ZList *next;
} HSD_ZList;

void _HSD_ZListSort();
void _HSD_ZListDisp();
void _HSD_ZListClear();

Mtx HSD_identityMtx = 
{
	{1,0,0,0},
	{0,1,0,0},
	{0,0,1,0}
};

void HSD_JObjDisp(HSD_JObj*, MtxP, HSD_TrspMask, u32);

#endif