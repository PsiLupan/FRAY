#ifndef _hsd_pobj_h_
#define _hsd_pobj_h_

#include <gctypes.h>

#include "hsd_object.h"
#include "hsd_util.h"

#define POBJ_ANIM 1<<3
#define POBJ_SHAPEANIM 1<<12
#define POBJ_ENVELOPE 1<<13

#define SHAPESET_ADDITIVE 1<<1

//Polygon Object
typedef struct _HSD_PObj {
	HSD_Class parent;
	struct _HSD_PObj* next;
	struct _HSD_VertAttr* verts;
	u16 flags;
	u16 n_display;
	struct _HSD_Display* display;
	struct _HSD_Weight* weight;
	union {
		HSD_ShapeSet shape_set;
		HSD_SList* envelope_list;
	} u;
	struct _HSD_AObj* aobj;
} HSD_PObj;

typedef struct _HSD_PObjDesc {
	char* class_name;
	struct _HSD_PObjDesc* next;
	HSD_VertAttr* verts;
	u16 flags;
	u16 n_display;
	HSD_Display* display;
	HSD_Weight* weight;
	union {
		HSD_ShapeSetDesc* shape_set;
		EnvelopeDesc** envelope_p;
	} u;
} HSD_PObjDesc;

typedef struct _HSD_Display {
	u8 unk;
	u8 primitive;
	u16 indices;
} HSD_Display;

typedef struct _HSD_Weight {
	struct _HSD_JObj* jobj;
	f32 weight;
} HSD_Weight;

typedef struct _HSD_VertAttr {
	u32 ident;
	u32 usage;
	u32 format;
	u32 type;
	u8 scale;
	u8 unk;
	u16 stride;
	Vertex* vertex;
} HSD_VertAttr;

typedef struct _Vertex {

} Vertex;

typedef struct _HSD_PObjInfo {
	HSD_ClassInfo class_info;
} HSD_PObjInfo;

extern HSD_PObjInfo hsdPObj;

#define HSD_POBJ(o) ((HSD_PObj*)(o))

#endif