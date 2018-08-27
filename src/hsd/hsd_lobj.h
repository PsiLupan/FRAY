#ifndef _hsd_lobj_h_
#define _hsd_lobj_h_

#include "hsd_aobj.h"
#include "hsd_cobj.h"
#include "hsd_jobj.h"
#include "hsd_object.h"

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

#define LOBJ_HIDDEN 0x20
#define LOBJ_RAW_PARAM 0x40
#define LOBJ_DIFF_DIRTY 0x80
#define LOBJ_SPEC_DIRTY 0x100

typedef struct _HSD_LObj{
	HSD_Obj obj;
	u16 flags;
	struct _HSD_LObj* next;
	GXColor color;
	GXColor hw_color;
	union {
		struct {
			f32 cutoff;
			f32 ref_dist;
			f32 ref_br;
			GXSpotFn spot_func;
			GXDistAttnFn dist_func;
		} spot;
		struct {
			f32 a0;
			f32 a1;
			f32 a2;
			f32 k0;
			f32 k1;
			f32 k2;
		} attn;
	} u;
	f32 shininess;
	Vec lvec;
	HSD_AObj* aobj;
	GXLightID id;
	GXLightObj* lightobj; //0x50
	GXLightID spec_id; //0x90
	GXLightObj* spec_lightobj;
} HSD_LObj;

typedef struct _HSD_LObjDesc {
	
} HSD_LObjDesc;

typedef struct _HSD_LObjInfo {
	
} HSD_LObjInfo;

extern HSD_LObjInfo hsdLObj;

#define HSD_LOBJ(o)		((HSD_LObj *)(o))
#define HSD_LOBJ_INFO(i)	((HSD_LObjInfo *)(i))
#define HSD_LOBJ_METHOD(o)	HSD_LOBJ_INFO(HSD_CLASS_METHOD(o))



#endif