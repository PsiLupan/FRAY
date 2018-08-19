#include "hsd_cobj.h"
#include "hsd_jobj.h"

static HSD_ObjAllocData zlist_alloc_data;

typedef struct _HSD_ZList {
	Mtx pmtx;
	MtxPtr vmtx;
	HSD_JObj *jobj;
	u32 rendermode;
	
	struct {
		HSD_ZList *texedge;
		HSD_ZList *xlu;
	} sort;
	HSD_ZList *next;
} HSD_ZList;

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

//80374848
void _HSD_ZListDisp(){
	HSD_ZList *list;
	MtxPtr vmtx;
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