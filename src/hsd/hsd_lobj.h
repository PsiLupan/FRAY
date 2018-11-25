#ifndef _hsd_lobj_h_
#define _hsd_lobj_h_

#include <ogc/gx.h>

#include "hsd_aobj.h"
#include "hsd_cobj.h"
#include "hsd_jobj.h"
#include "hsd_object.h"
#include "hsd_util.h"
#include "hsd_wobj.h"

#define MAX_GXLIGHT 9

#define HSD_A_L_LITC_R 0
#define HSD_A_L_LITC_G 1
#define HSD_A_L_LITC_B 2
#define HSD_A_L_VIS 3
#define HSD_A_L_A0 4
#define HSD_A_L_A1 5
#define HSD_A_L_A2 6
#define HSD_A_L_K0 7
#define HSD_A_L_K1 8
#define HSD_A_L_K2 9
#define HSD_A_L_CUTOFF 10
#define HSD_A_L_REFDIST 11
#define HSD_A_L_REFBRIGHT 12
#define HSD_A_L_LITC_A 13

#define LOBJ_INFINITE 1
#define LOBJ_POINT 2
#define LOBJ_SPOT 3
#define LOBJ_AMBIENT 4
#define LOBJ_DIFFUSE 6
#define LOBJ_ALPHA 7
#define LOBJ_SPECULAR 8

#define LOBJ_TYPE_MASK 3

#define LOBJ_LIGHT_ATTN 1 //TODO: Figure this shit out

#define LOBJ_HIDDEN 0x20
#define LOBJ_RAW_PARAM 0x40
#define LOBJ_DIFF_DIRTY 0x80
#define LOBJ_SPEC_DIRTY 0x100

struct spot {
	f32 cutoff;
	u8 spot_func;
	f32 ref_br;
	f32 ref_dist;
	u8 dist_func;
};

struct attn {
	f32 a0;
	f32 a1;
	f32 a2;
	f32 k0;
	f32 k1;
	f32 k2;
};

typedef struct _HSD_LObj {
	struct _HSD_Obj parent_class;
	u16 flags;
	struct _HSD_LObj* next;
	GXColor color; //0x10
	GXColor hw_color;
	HSD_WObj* position; //0x18
	HSD_WObj* interest; //0x1C
	union {
		struct spot spot;
		struct attn attn;
	} u;
	f32 shininess;
	guVector lvec;
	HSD_AObj* aobj;
	u8 id; //GXLightID
	GXLightObj* lightobj; //0x50
	u8 spec_id; //0x90 GXLightID
	GXLightObj* spec_lightobj;
} HSD_LObj;

typedef struct _HSD_LightDesc {
	char* class_name; //0x00
	u16 flags; //0x08
	GXColor color; //0x0C
	struct _HSD_LightDesc* next;
	u16 attnflags; //TODO: verify size
	union {
		struct spot point;
		struct spot spot;
		struct attn attn;
	} u;
	HSD_WObjDesc* position;
	HSD_WObjDesc* interest;
} HSD_LightDesc;

typedef struct _HSD_LightAnim {
	struct _HSD_LightAnim *next;
	struct _HSD_AObjDesc *aobjdesc;
	struct _HSD_AObj position_anim;
	struct _HSD_AObj interest_anim;
} HSD_LightAnim;

typedef struct _HSD_LObjInfo {
	HSD_ClassInfo parent;
	void    (*release)(HSD_Class *o);
	void    (*amnesia)(HSD_ClassInfo *info);
	int	    (*load)(HSD_LObj *lobj, HSD_LightDesc *ldesc);
	void	(*update)(void *obj, u32 type, FObjData *val);
} HSD_LObjInfo;

extern HSD_LObjInfo hsdLObj;

#define HSD_LOBJ(o)		((HSD_LObj *)(o))
#define HSD_LOBJ_INFO(i)	((HSD_LObjInfo *)(i))
#define HSD_LOBJ_METHOD(o)	HSD_LOBJ_INFO(HSD_CLASS_METHOD(o))



#endif