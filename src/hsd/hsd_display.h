#ifndef _hsd_display_h_
#define _hsd_display_h_

#include <ogc/gx.h>

#include "hsd_cobj.h"
#include "hsd_state.h"
#include "hsd_jobj.h"

#define JOBJ_PTCL_ACTIVE 0x7FFFFFFF
#define JOBJ_PTCL_OFFSET_MASK 0xFFFFFF
#define JOBJ_PTCL_OFFSET_SHIFT 6
#define JOBJ_PTCL_BANK_MASK 0x3F

#define JOBJ_BILLBOARD_FIELD 0xE00
#define JOBJ_BILLBOARD 0x200
#define JOBJ_VBILLBOARD 0x400
#define JOBJ_HBILLBOARD 0x600
#define JOBJ_RBILLBOARD 0x800
#define JOBJ_PBILLBOARD 0x2000

#define HSD_JOBJ_INFO(i)	((HSD_JObjInfo *)(i))
#define HSD_JOBJ_METHOD(o)	HSD_JOBJ_INFO(HSD_CLASS_METHOD(o))

typedef void (*HSD_PtclCallback)(u32, u32, u32, HSD_JObj*);

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
void HSD_JObjDispSub(HSD_JObj*, MtxP, MtxP, HSD_TrspMask, u32);

#endif