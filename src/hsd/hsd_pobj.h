#ifndef _hsd_pobj_h_
#define _hsd_pobj_h_

#include <gctypes.h>

#include "hsd_object.h"
#include "hsd_util.h"

#include "hsd_aobj.h"
#include "hsd_fobj.h"

#define HSD_A_S_W0 8
#define HSD_DEFAULT_MAX_SHAPE_VERTICES 2000
#define HSD_DEFAULT_MAX_SHAPE_NORMALS 2000

#define POBJ_ANIM 1<<3
#define POBJ_SKIN 3<<12
#define POBJ_SHAPEANIM 1<<12
#define POBJ_ENVELOPE 1<<13

#define pobj_type(o)	(o->flags & 0x3000)

#define POBJ_CULLFRONT 0x4000
#define POBJ_CULLBACK 0x8000

#define SHAPESET_AVERAGE 1
#define SHAPESET_ADDITIVE 1<<1

#define GX_NOP 0
#define GX_VAT_MASK 0x7
#define GX_OPCODE_MASK 0xF8

//Polygon Object
typedef struct _HSD_PObj {
	HSD_Class parent;
	struct _HSD_PObj* next;
	struct _HSD_VtxDescList* verts;
	u16 flags;
	u16 n_display;
	struct _HSD_Display* display;
	union {
		struct _HSD_JObj* jobj;
		struct _HSD_ShapeSet* shape_set;
		HSD_SList* envelope_list;
	} u;
} HSD_PObj;

typedef struct _HSD_PObjDesc {
	char* class_name;
	struct _HSD_PObjDesc* next;
	struct _HSD_VtxDescList** verts;
	u16 flags;
	u16 n_display;
	struct _HSD_Display* display;
	union {
		struct _HSD_JObjDesc* joint;
		struct _HSD_ShapeSetDesc* shape_set;
		struct _HSD_EnvelopeDesc** envelope_p;
	} u;
} HSD_PObjDesc;

typedef struct _HSD_Display {
	u8 primitive;
	u8 vtxcnt;
	u16* indices;
} HSD_Display;

typedef struct _HSD_VtxDescList {
	u32 attr;
	u32 attr_type;
	u32 comp_cnt;
	u32 comp_type;
	u8 frac;
	u8 unk;
	u16 stride;
	f32* vertex;
} HSD_VtxDescList;

typedef struct _HSD_Envelope {
	struct _HSD_Envelope* next;
	struct _HSD_JObj* jobj;
	f32 weight;
} HSD_Envelope;

typedef struct _HSD_EnvelopeDesc {
	struct _HSD_JObjDesc* joint;
	f32 weight;
} HSD_EnvelopeDesc;

typedef struct _HSD_ShapeSet {
	u16 flags;
	u16 nb_shape;
	u32 nb_vertex_index;
	struct _HSD_VtxDescList* vertex_desc;
	u8* vertex_idx_list;
	u32 nb_normal_index;
	struct _HSD_VtxDescList* normal_desc;
	u8* normal_idx_list;
	struct {
		f32* bp;
		f32 bl;
	} blend;
	struct _HSD_AObj* aobj;
	u32 x24_unk;
} HSD_ShapeSet;

typedef struct _HSD_ShapeSetDesc {
	u16 flags;
	u16 nb_shape;
	u32 nb_vertex_index;
	struct _HSD_VertexDesc* vertex_desc;
	HSD_SList* vertex_idx_list;
	u32 nb_normal_index;
	void* normal_desc;
	HSD_SList* normal_idx_list;
} HSD_ShapeSetDesc;

typedef struct _HSD_ShapeAnim {
	struct _HSD_ShapeAnim* next;
	HSD_SList* aobjdescs_list;
} HSD_ShapeAnim;

typedef struct _HSD_ShapeAnimJoint {
	struct _HSD_ShapeAnimJoint* child;
	struct _HSD_ShapeAnimJoint* next;
	struct _HSD_ShapeAnim* shapeanim;
} HSD_ShapeAnimJoint;

typedef struct _HSD_PObjInfo {
	HSD_ClassInfo parent;
	void (*disp)(HSD_PObj* pobj, Mtx vmtx, Mtx pmtx, u32 rendermode);
	void (*setup_mtx)(HSD_PObj *pobj, Mtx vmtx, Mtx pmtx, u32 rendermode);
	void (*load)(HSD_PObj* pobj, HSD_PObjDesc *desc);
	void (*update)(void *obj, u32 type, FObjData* val);
} HSD_PObjInfo;

extern HSD_PObjInfo hsdPObj;

#define HSD_POBJ(o) ((HSD_PObj*)(o))
#define HSD_POBJ_INFO(i)	((HSD_PObjInfo *)(i))
#define HSD_POBJ_METHOD(o)	HSD_POBJ_INFO(HSD_CLASS_METHOD(o))


u16 HSD_PObjGetFlags(HSD_PObj *);
void HSD_PObjRemoveAnimAllByFlags(HSD_PObj *, u32);
void HSD_PObjRemoveAnimAll(HSD_PObj *, HSD_SList *);
void HSD_PObjReqAnimAllByFlags(HSD_PObj *, f32, u32);
void HSD_PObjAnimAll(HSD_PObj *);
HSD_PObj* HSD_PObjLoadDesc(HSD_PObjDesc *);
void HSD_PObjRemoveAll(HSD_PObj *);
void HSD_PObjSetDefaultClass(HSD_PObjInfo *);
HSD_PObj* HSD_PObjAlloc();
void HSD_PObjResolveRefsAll(HSD_PObj *, HSD_PObjDesc *);
void HSD_ClearVtxDesc();
void HSD_PObjClearMtxMark(void *, u32);
void HSD_PObjSetMtxMark(s32,  void *, u32);
void HSD_PObjGetMtxMark(s32,  void **, u32 *);
void HSD_PObjDisp(HSD_PObj *, Mtx, Mtx, u32);

#endif
