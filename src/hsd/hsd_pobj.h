#ifndef _hsd_pobj_h_
#define _hsd_pobj_h_

#include "hsd_jobj.h"

//Polygon Object
typedef struct _HSD_PObj {
	HSD_Class parent;
	struct _HSD_PObj* next;
	HSD_VertAttr* verts;
	u16 flags;
	u16 n_display;
	HSD_Display* display;
	HSD_Weight* weight;
} HSD_PObj;

typedef struct _HSD_PObjDesc {
	char* class_name;
	struct _HSD_PObjDesc* next;
	HSD_VertAttr* verts;
	u16 flags;
	u16 n_display;
	HSD_Display* display;
	HSD_Weight* weight;
} HSD_PObjDesc;

typedef struct {
	u8 unk;
	u8 primitive;
	u16 indices;
} HSD_Display;

typedef struct {
	struct _HSD_JObj* jobj;
	f32 weight;
} HSD_Weight;

typedef struct {
	u32 ident;
	u32 usage;
	u32 format;
	u32 type;
	u8 scale;
	u8 unk;
	u16 stride;
	Vertex* vertex;
} HSD_VertAttr;

typedef struct {

} Vertex;

typedef _HSD_PObjInfo {
	
} HSD_PObjInfo;

extern PObjInfo hsdPObj;

#endif