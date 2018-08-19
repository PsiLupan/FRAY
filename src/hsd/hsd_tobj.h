#ifndef _hsd_tobj_h_
#define _hsd_tobj_h_

#include <gctypes.h>

#include "../../include/mtx/GeoTypes.h"

#include "hsd_aobj.h"

#define TOBJ_ANIM 0x10

#define	HSD_A_T_TIMG	     1
#define	HSD_A_T_TRAU	     2
#define	HSD_A_T_TRAV	     3
#define	HSD_A_T_SCAU	     4
#define	HSD_A_T_SCAV	     5
#define HSD_A_T_ROTX	     6
#define HSD_A_T_ROTY	     7
#define HSD_A_T_ROTZ	     8
#define HSD_A_T_BLEND	     9
#define HSD_A_T_TCLT	    10
#define HSD_A_T_LOD_BIAS    11
#define HSD_A_T_KONST_R     12
#define HSD_A_T_KONST_G     13
#define HSD_A_T_KONST_B     14
#define HSD_A_T_KONST_A     15
#define HSD_A_T_TEV0_R      16
#define HSD_A_T_TEV0_G      17
#define HSD_A_T_TEV0_B      18
#define HSD_A_T_TEV0_A      19
#define HSD_A_T_TEV1_R      20
#define HSD_A_T_TEV1_G      21
#define HSD_A_T_TEV1_B      22
#define HSD_A_T_TEV1_A      23
#define HSD_A_T_TS_BLEND    24

#define TOBJ_TEV_CC_KONST_RGB (0x01<<7|0)
#define TOBJ_TEV_CC_KONST_RRR (0x01<<7|1)
#define TOBJ_TEV_CC_KONST_GGG (0x01<<7|2)
#define TOBJ_TEV_CC_KONST_BBB (0x01<<7|3)
#define TOBJ_TEV_CC_KONST_AAA (0x01<<7|4)
#define TOBJ_TEV_CC_TEX0_RGB (0x01<<7|5)
#define TOBJ_TEV_CC_TEX0_AAA (0x01<<7|6)
#define TOBJ_TEV_CC_TEX1_RGB (0x01<<7|7)
#define TOBJ_TEV_CC_TEX1_AAA (0x01<<7|8)

#define TOBJ_TEV_CA_KONST_R (0x01<<6|0)
#define TOBJ_TEV_CA_KONST_G (0x01<<6|1)
#define TOBJ_TEV_CA_KONST_B (0x01<<6|2)
#define TOBJ_TEV_CA_KONST_A (0x01<<6|3)
#define TOBJ_TEV_CA_TEX0_A (0x01<<6|4)
#define TOBJ_TEV_CA_TEX1_A (0x01<<6|5)

#define	TOBJ_TEVREG_ACTIVE_KONST_R	(0x01<<0)
#define	TOBJ_TEVREG_ACTIVE_KONST_G	(0x01<<1)
#define	TOBJ_TEVREG_ACTIVE_KONST_B	(0x01<<2)
#define	TOBJ_TEVREG_ACTIVE_KONST_A	(0x01<<3)
#define	TOBJ_TEVREG_ACTIVE_KONST \
(TOBJ_TEVREG_ACTIVE_KONST_R|TOBJ_TEVREG_ACTIVE_KONST_G \
 |TOBJ_TEVREG_ACTIVE_KONST_B|TOBJ_TEVREG_ACTIVE_KONST_A)
#define	TOBJ_TEVREG_ACTIVE_TEV0_R	(0x01<<4)
#define	TOBJ_TEVREG_ACTIVE_TEV0_G	(0x01<<5)
#define	TOBJ_TEVREG_ACTIVE_TEV0_B	(0x01<<6)
#define	TOBJ_TEVREG_ACTIVE_TEV0_A	(0x01<<7)
#define	TOBJ_TEVREG_ACTIVE_TEV0 \
(TOBJ_TEVREG_ACTIVE_TEV0_R|TOBJ_TEVREG_ACTIVE_TEV0_G \
 |TOBJ_TEVREG_ACTIVE_TEV0_B|TOBJ_TEVREG_ACTIVE_TEV0_A)
#define	TOBJ_TEVREG_ACTIVE_TEV1_R	(0x01<<8)
#define	TOBJ_TEVREG_ACTIVE_TEV1_G	(0x01<<9)
#define	TOBJ_TEVREG_ACTIVE_TEV1_B	(0x01<<10)
#define	TOBJ_TEVREG_ACTIVE_TEV1_A	(0x01<<11)
#define	TOBJ_TEVREG_ACTIVE_TEV1 \
(TOBJ_TEVREG_ACTIVE_TEV1_R|TOBJ_TEVREG_ACTIVE_TEV1_G \
 |TOBJ_TEVREG_ACTIVE_TEV1_B|TOBJ_TEVREG_ACTIVE_TEV1_A)
#define	TOBJ_TEVREG_ACTIVE_COLOR_TEV	(0x01<<30)
#define	TOBJ_TEVREG_ACTIVE_ALPHA_TEV	(0x01<<31)

#define TEX_COORD_UV 0
#define TEX_COORD_REFLECTION   1
#define TEX_COORD_HILIGHT      2
#define TEX_COORD_SHADOW 3
#define TEX_COORD_TOON 4
#define TEX_COORD_GRADATION	5
#define TEX_COORD_BACKLIGHT	6
#define TEX_COORD_MASK 	(0x0f)
#define tobj_coord(T) ((T)->flag & TEX_COORD_MASK)

#define TEX_BUMP (0x1<<24)
#define tobj_bump(T) ((T)->flag & TEX_BUMP)
#define TEX_MTX_DIRTY (1<<31)

//Texture Object
typedef struct _HSD_TObj {
	HSD_Obj	object;
	struct _HSD_TObj* next;
	GXTexMapID id;
	GXTexGenSrc src;
	u32 mtxid;
	Quaternion rotation;
	Vec scale;
	Vec translation;
	GXTexWrapMode wrap_s;
	GXTexWrapMode wrap_t;
	u8 repeat_s;
	u8 repeat_t;
	u16 anim_id;
	u32 flags;
	f32 blending;
	GXTexFilter magFilt;
	HSD_ImageDesc* imagedesc;
	HSD_Tlut* tlut;
	HSD_TexLODDesc* lod;
	struct _HSD_AObj* aobj;
	HSD_ImageDesc **imagetbl;
	Tlut **tluttbl;
	u8 tlut_no;
	Mtx mtx;
	GXTexCoordID coord;
	HSD_TObjTev *tev;
} HSD_TObj;

typedef struct _HSD_TObjDesc {
	char* class_name;
	HSD_TObjDesc* next;
	GXTexMapID id;
	GXTexGenSrc src;
	Vec rotation;
	Vec scale; 
	Vec translation;
	GXTexWrapMode wrap_s;
	GXTexWrapMode wrap_t;
	u8 repeat_s;
	u8 repeat_t;
	u32 flags;
	f32 blending;
	GXTexFilter magFilt;
	HSD_ImageDesc* imagedesc;
	HSD_TlutDesc* tlutdesc;
	HSD_TexLODDesc* lod;
	HSD_TObjTevDesc* tev;
} HSD_TObjDesc;

typedef struct _HSD_Tlut {
	void *lut;
	GXTlutFmt fmt;
	u32 tlut_name;
	u16 n_entries;
} HSD_Tlut;

typedef struct _HSD_TlutDesc {
	void *lut;
	GXTlutFmt fmt;
	u32 tlut_name;
	u16 n_entries;
} HSD_TlutDesc;

typedef struct _HSD_TexLODDesc {
	GXTexFilter minFilt;
	f32 LODBias;
	GXBool bias_clamp;
	GXBool edgeLODEnable;
	GXAnisotropy max_anisotropy;
} HSD_TexLODDesc;

typedef struct _HSD_ImageDesc {
	void *img_ptr;
	u16 width;
	u16 height;
	GXTexFmt format;
	u32 mipmap;
	f32 minLOD;
	f32 maxLOD;
} HSD_ImageDesc;

typedef struct {
	u8 color_op;
	u8 alpha_op;
	u8 color_bias;
	u8 alpha_bias;
	u8 color_scale;
	u8 alpha_scale;
	u8 color_clamp;
	u8 alpha_clamp;
	u8 color_a, color_b, color_c, color_d;
	u8 alpha_a, alpha_b, alpha_c, alpha_d;
	GXColor constant, tev0, tev1;
	u32 active;
} HSD_TObjTev;

typedef struct {
	u8 color_op;
	u8 alpha_op;
	u8 color_bias;
	u8 alpha_bias;
	u8 color_scale;
	u8 alpha_scale;
	u8 color_clamp;
	u8 alpha_clamp;
	u8 color_a, color_b, color_c, color_d;
	u8 alpha_a, alpha_b, alpha_c, alpha_d;
	GXColor constant, tev0, tev1;
	u32 active;
} HSD_TObjTevDesc;

typedef struct _HSD_TObjInfo {
	HSD_ObjInfo		parent;
	void (*make_mtx)(HSD_TObj *tobj);
	int (*load)(HSD_TObj *tobj, HSD_TObjDesc *desc);
	void (*make_texp)(HSD_TObj *tobj, u32 lightmap, u32 lightmap_done, HSD_TExp **c, HSD_TExp **a, HSD_TExp **list);
	void (*update)(void *obj, u32 type, FObjData *val);
} HSD_TObjInfo;

typedef struct _HSD_TexAnim {
	struct _HSD_TexAnim *next;
	GXTexMapID id;
	struct _HSD_AObjDesc *aobjdesc;
	HSD_ImageDesc **imagetbl;
	HSD_TlutDesc **tluttbl;
	u16 n_imagetbl;
	u16 n_tluttbl;
} HSD_TexAnim;

#define HSD_TOBJ(o)		((HSD_TObj *)(o))
#define HSD_TOBJ_INFO(i)	((HSD_TObjInfo *)(i))
#define HSD_TOBJ_METHOD(o)	HSD_TOBJ_INFO(HSD_CLASS_METHOD(o))

extern HSD_TObjInfo hsdTObj;

#endif