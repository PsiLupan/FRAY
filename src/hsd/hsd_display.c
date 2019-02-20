#include <ogc/gx.h>

#include "hsd_cobj.h"
#include "hsd_jobj.h"
#include "hsd_state.h"

typedef struct _HSD_ZList {
	Mtx pmtx;
	MtxP vmtx;
	HSD_JObj *jobj;
	u32 rendermode;
	
	struct {
		HSD_ZList *texedge;
		HSD_ZList *xlu;
	} sort;
	HSD_ZList *next;
} HSD_ZList;

static HSD_PtclCallback sptcl_callback = NULL;

static GXColor hsd_background_color = {0, 0, 0, 0};

static HSD_ObjAllocData zlist_alloc_data;

static int	 zsort_listing = 0;
static int	 zsort_sorting = 0;

static HSD_ZList *zlist_top = NULL;
static HSD_ZList **zlist_bottom = &zlist_top;

static HSD_ZList *zlist_texedge_top = NULL;
static HSD_ZList **zlist_texedge_bottom = &zlist_texedge_top;
static int	 zlist_texedge_nb = 0;

static HSD_ZList *zlist_xlu_top = NULL;
static HSD_ZList **zlist_xlu_bottom = &zlist_xlu_top;
static int	 zlist_xlu_nb = 0;

#define ZLIST_NEXT(list,offset) (*(HSD_ZList **)(((u8 *)(list)) + (offset)))

#define MTXRowCol(pmtx, row, col) (pmtx[row][col])

//803738A0
void HSD_ZListInitAllocData(){
	HSD_ObjAllocInit(&zlist_alloc_data, 0x48, 4);
}

//80374680
static HSD_ZList* zlist_sort(HSD_ZList *list, int nb, int offset){
	HSD_ZList *fore, *hind, **ptr;
	int nb_fore, nb_hind;
	
	if (nb <= 1) {
		if (list) {
			ZLIST_NEXT(list, offset) = NULL;
		}
		return list;
	}
	
	nb_fore = nb / 2;
	nb_hind = nb - nb_fore;
	
	hind = list;
	for (int i = 0; i < nb_fore; i++) {
		hind = ZLIST_NEXT(hind, offset);
	}
	
	fore = zlist_sort(list, nb_fore, offset);
	hind = zlist_sort(hind, nb_hind, offset);
	
	list = NULL;
	ptr = &list;
	
	while (fore && hind) {
		if (MTXRowCol(fore->pmtx, 2, 3) <= MTXRowCol(hind->pmtx, 2, 3)) {
			*ptr = fore;
			fore = ZLIST_NEXT(fore, offset);
		} else {
			*ptr = hind;
			hind = ZLIST_NEXT(hind, offset);
		}
		ptr = &ZLIST_NEXT(*ptr, offset);
	}
	
	if (fore) {
		*ptr = fore;
	} else if (hind) {
		*ptr = hind;
	}
	
	return list;
}

#define OFFSET(stru, member)	((int)(&((stru *) 0)->member))

//803747F8
void _HSD_ZListSort(void){
	if (zsort_sorting) {
		zlist_texedge_top = zlist_sort(zlist_texedge_top, zlist_texedge_nb, OFFSET(HSD_ZList, sort.texedge));
		zlist_xlu_top = zlist_sort(zlist_xlu_top, zlist_xlu_nb, OFFSET(HSD_ZList, sort.xlu));
	}
}

//80374848
void _HSD_ZListDisp(){
	HSD_ZList *list;
	MtxP vmtx;
	HSD_CObj *cobj;
	
	cobj = HSD_CObjGetCurrent();
	vmtx = HSD_CObjGetViewingMtxPtrDirect(cobj);
	
	list = zlist_texedge_top;
	while (list) {
		HSD_JOBJ_METHOD(list->jobj)->disp(list->jobj, (list->vmtx) ? list->vmtx : vmtx, list->pmtx, HSD_TRSP_TEXEDGE, list->rendermode);
		list = list->sort.texedge;
	}
	
	list = zlist_xlu_top;
	while (list) {
		HSD_JOBJ_METHOD(list->jobj)->disp(list->jobj, (list->vmtx)? list->vmtx : vmtx, list->pmtx, HSD_TRSP_XLU, list->rendermode);
		list = list->sort.xlu;
	}
	
	_HSD_ZListClear();
}

//80374910
void _HSD_ZListClear(void){
	HSD_ZList *list;

	for(list = zlist_top; list; list = list->next){
		if (list->vmtx) {
			HSD_MtxFree(list->vmtx);
		}
		HSD_ZListFree(list);
	}
	zlist_top = NULL;
	zlist_bottom = &zlist_top;
	
	zlist_texedge_top = NULL;
	zlist_texedge_bottom = &zlist_texedge_top;
	zlist_texedge_nb = 0;
	
	zlist_xlu_top = NULL;
	zlist_xlu_bottom = &zlist_xlu_top;
	zlist_xlu_nb = 0;
}

//803749B0
void HSD_JObjDisp(HSD_JObj *jobj, MtxP vmtx, HSD_TrspMask trsp_mask, u32 rendermode){
	if (jobj){
		if (union_type_dobj(jobj)) {
			HSD_JObjDispDObj(jobj, vmtx, trsp_mask, rendermode);
		} else if (union_type_ptcl(jobj)) {
			HSD_SList *sp;
			if (sptcl_callback) {
				for (sp = jobj->u.ptcl; sp; sp = sp->next) {
					if (((u32)sp->data) & JOBJ_PTCL_ACTIVE) {
						u32 bank = JOBJ_PTCL_BANK_MASK & ((u32)sp->data);
						u32 offset = (JOBJ_PTCL_OFFSET_MASK & ((u32)sp->data)) >> JOBJ_PTCL_OFFSET_SHIFT;
						(*sptcl_callback)(0, bank, offset, jobj);
					}
					sp->data = (void *)((u32)sp->data & ~JOBJ_PTCL_ACTIVE);
				}
			}
		}
	}
}

//80374A80
void HSD_JObjSetSPtclCallback(HSD_PtclCallback func){
	sptcl_callback = func;
}

//80374A88
void HSD_SetEraseColor(u8 r, u8 g, u8 b, u8 a){
	hsd_background_color.r = r;
	hsd_background_color.g = g;
	hsd_background_color.b = b;
	hsd_background_color.a = a;
}

//80374AA0
void HSD_EraseRect(float top, float bottom, float left, float right, float z, int enable_color, int enable_alpha, int enable_depth){
	GXTexObj texobj;
	static u8 depth_image[] = {
		255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255,
		};
	GXColor color;
	
	if (!enable_color && !enable_alpha && !enable_depth)
		return;
	
	if (enable_depth) {
		GX_InitTexObj(&texobj, depth_image, 4, 4, GX_TF_Z8, GX_REPEAT, GX_REPEAT, GX_DISABLE);
		GX_LoadTexObj(&texobj, GX_TEXMAP0);
		GX_SetNumTexGens(1);
		GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
		GX_SetNumTevStages(1);
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
		GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
		GX_SetZTexture(GX_ZT_REPLACE, GX_TF_Z8, 0);
	} else {
		GX_SetNumTexGens(0);
		GX_SetNumTevStages(1);
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
		GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
	}
	
	GX_SetCullMode(GX_CULL_NONE);
	
	GX_SetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);
	GX_SetZCompLoc(GX_TRUE);
	GX_SetZMode(GX_ENABLE, GX_ALWAYS, enable_depth? GX_ENABLE : GX_DISABLE);
	GX_SetBlendMode(GX_BM_LOGIC, GX_BL_ONE, GX_BL_ZERO, GX_LO_COPY);
	GX_SetColorUpdate(enable_color? GX_ENABLE : GX_DISABLE);
	GX_SetAlphaUpdate(enable_alpha? GX_ENABLE : GX_DISABLE);
	
	GX_SetNumChans(1);
	GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHTNULL, GX_DF_NONE, GX_AF_NONE);
	
	GX_ClearVtxDesc();
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_U8, 0);
	GX_LoadPosMtxImm(HSD_identityMtx, GX_PNMTX0);
	GX_SetCurrentMtx(GX_PNMTX0);
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	
	color = HSD_GetEraseColor();
	GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
	GX_Position3f32(left, top, z);
	GX_Color4u8(color.r, color.g, color.b, color.a);
	GX_TexCoord2u8(0, 0);
	GX_Position3f32(right, top, z);
	GX_Color4u8(color.r, color.g, color.b, color.a);
	GX_TexCoord2u8(1, 0);
	GX_Position3f32(right, bottom, z);
	GX_Color4u8(color.r, color.g, color.b, color.a);
	GX_TexCoord2u8(1, 1);
	GX_Position3f32(left, bottom, z);
	GX_Color4u8(color.r, color.g, color.b, color.a);
	GX_TexCoord2u8(0, 1);
	GX_End();
	
	GX_SetZTexture(GX_ZT_DISABLE, GX_TF_Z8, 0);
	
	HSD_StateInvalidate(HSD_STATE_ALL);
}

//80374E14
extern void _HSD_DispForgetMemory(void); 
void _HSD_DispForgetMemory(void) {
  zlist_top = NULL;
  zlist_bottom = &zlist_top;

  zlist_texedge_top = NULL;
  zlist_texedge_bottom = &zlist_texedge_top;
  zlist_texedge_nb = 0;

  zlist_xlu_top = NULL;
  zlist_xlu_bottom = &zlist_xlu_top;
  zlist_xlu_nb = 0;
}
