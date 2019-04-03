#ifndef _hsd_pobj_h_
#define _hsd_pobj_h_

#include <gctypes.h>

#include "hsd_object.h"
#include "hsd_util.h"

#include "hsd_aobj.h"
#include "hsd_jobj.h"

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
		struct _HSD_ShapeSet* shape_set;
		HSD_SList* envelope_list;
	} u;
	struct _HSD_AObj* aobj;
} HSD_PObj;

typedef struct _HSD_PObjDesc {
	char* class_name;
	struct _HSD_PObjDesc* next;
	struct _HSD_VertAttr* verts;
	u16 flags;
	u16 n_display;
	struct _HSD_Display* display;
	struct _HSD_Weight* weight;
	union {
		struct _HSD_ShapeSetDesc* shape_set;
		struct _HSD_EnvelopeDesc** envelope_p;
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
	struct _HSD_Vertex* vertex;
} HSD_VertAttr;

typedef struct _HSD_Vertex {

} Vertex;

typedef struct _HSD_VertexDesc {

} HSD_VertexDesc;

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
	struct _HSD_VertexDesc* vertex_desc;
	HSD_SList* vertex_idx_list;
	u32 nb_normal_index;
	void* normal_desc;
	HSD_SList* normal_idx_list;
	struct {
		f32* bp;
		f32 bl;
	} blend;
	u32 x20_unk;
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
	u32* unk;
} HSD_ShapeAnim;

typedef struct _HSD_ShapeAnimJoint {
	struct _HSD_ShapeAnimJoint* child;
	struct _HSD_ShapeAnimJoint* next;
	struct _HSD_ShapeAnim* shapeanim;
	u32 unk;
	u32 unk2;
} HSD_ShapeAnimJoint;

typedef struct _HSD_PObjInfo {
	HSD_ClassInfo class_info;
} HSD_PObjInfo;

extern HSD_PObjInfo hsdPObj;

#define HSD_POBJ(o) ((HSD_PObj*)(o))

#endif