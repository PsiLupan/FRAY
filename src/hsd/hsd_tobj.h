#ifndef _hsd_tobj_h_
#define _hsd_tobj_h_

#include <gctypes.h>
#include <ogc/gu.h>
#include <ogc/gx.h>

#include "hsd_aobj.h"
#include "hsd_lobj.h"
#include "hsd_object.h"

#define TOBJ_ANIM 0x10

#define HSD_A_T_TIMG	     1
#define HSD_A_T_TRAU	     2
#define HSD_A_T_TRAV	     3
#define HSD_A_T_SCAU	     4
#define HSD_A_T_SCAV	     5
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
#define tobj_coord(T) ((T)->flags & TEX_COORD_MASK)

# define TEX_LIGHTMAP_DIFFUSE	(0x1<<4)
# define TEX_LIGHTMAP_SPECULAR	(0x1<<5)
# define TEX_LIGHTMAP_AMBIENT	(0x1<<6)
# define TEX_LIGHTMAP_EXT	(0x1<<7)
# define TEX_LIGHTMAP_SHADOW	(0x1<<8)
# define TEX_LIGHTMAP_MASK \
  (TEX_LIGHTMAP_DIFFUSE \
   |TEX_LIGHTMAP_SPECULAR \
   |TEX_LIGHTMAP_AMBIENT \
   |TEX_LIGHTMAP_EXT \
   |TEX_LIGHTMAP_SHADOW)
# define tobj_lightmap(T) ((T)->flags & TEX_LIGHTMAP_MASK)

#define TEX_BUMP (0x1<<24)
#define tobj_bump(T) ((T)->flags & TEX_BUMP)
#define TEX_MTX_DIRTY (1<<31)

/*typedef enum _GXTlutFmt {
	GX_TL_IA8,
    GX_TL_RGB565,
    GX_TL_RGB5A3,
    GX_MAX_TLUTFMT
} GXTlutFmt;*/

//Texture Object
typedef struct _HSD_TObj {
	HSD_Class class_parent;
	u16 flags;
	struct _HSD_TObj* next;
	u8 id; //GXTexMapID
	u32 src; //GXTexGenSrc
	u32 mtxid;
	guQuaternion rotate;
	guVector scale;
	guVector translate;
	u8 wrap_s; //GXTexWrapMode
	u8 wrap_t; //GXTexWrapMode
	u8 repeat_s;
	u8 repeat_t;
	u16 anim_id;
	u32 flags;
	f32 blending;
	u8 magFilt; //GXTexFilter
	HSD_ImageDesc* imagedesc;
	HSD_Tlut* tlut;
	HSD_TexLODDesc* lod;
	struct _HSD_AObj* aobj;
	HSD_ImageDesc **imagetbl;
	HSD_Tlut **tluttbl;
	u8 tlut_no;
	Mtx mtx;
	u16 coord; //GXTexCoordID
	HSD_TObjTev *tev;
} HSD_TObj;

typedef struct _HSD_TObjDesc {
	char* class_name;
	struct _HSD_TObjDesc* next;
	u8 id; //GXTexMapID
	u32 src; //GXTexGenSrc
	guVector rotate;
	guVector scale; 
	guVector translate;
	u8 wrap_s; //GXTexWrapMode
	u8 wrap_t; //GXTexWrapMode
	u8 repeat_s;
	u8 repeat_t;
	u32 flags;
	f32 blending;
	u8 magFilt; //GXTexFilter
	HSD_ImageDesc* imagedesc;
	HSD_TlutDesc* tlutdesc;
	HSD_TexLODDesc* lod;
	HSD_TObjTevDesc* tev;
} HSD_TObjDesc;

typedef struct _HSD_Tlut {
	void *lut;
	u32 fmt;
	u32 tlut_name;
	u16 n_entries;
} HSD_Tlut;

typedef struct _HSD_TlutDesc {
	void *lut;
	u32 fmt;
	u32 tlut_name;
	u16 n_entries;
} HSD_TlutDesc;

typedef struct _HSD_TexLODDesc {
	u8 minFilt; //GXTexFilter
	f32 LODBias;
	u8 bias_clamp; //GXBool
	u8 edgeLODEnable; //GXBool
	u8 max_anisotropy; //GXAnisotropy
} HSD_TexLODDesc;

typedef struct _HSD_ImageDesc {
	void *img_ptr;
	u16 width;
	u16 height;
	u32 format;
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
	HSD_ClassInfo parent;
	void (*make_mtx)(HSD_TObj *tobj);
	int  (*load)(HSD_TObj *tobj, HSD_TObjDesc *desc);
	void (*make_texp)(HSD_TObj *tobj, u32 lightmap, u32 lightmap_done, HSD_TExp **c, HSD_TExp **a, HSD_TExp **list);
	void (*update)(void *obj, u32 type, FObjData *val);
} HSD_TObjInfo;

typedef struct _HSD_TexAnim {
	struct _HSD_TexAnim *next;
	u8 id; //GXTexMapID
	struct _HSD_AObjDesc *aobjdesc;
	HSD_ImageDesc **imagetbl;
	HSD_TlutDesc **tluttbl;
	u16 n_imagetbl;
	u16 n_tluttbl;
} HSD_TexAnim;

extern HSD_TObjInfo hsdTObj;

#define HSD_TOBJ(o)		((HSD_TObj *)(o))
#define HSD_TOBJ_INFO(i)	((HSD_TObjInfo *)(i))
#define HSD_TOBJ_METHOD(o)	HSD_TOBJ_INFO(HSD_CLASS_METHOD(o))

void HSD_TObjRemoveAnimAll(HSD_TObj *tobj);
void HSD_TObjAddAnim(HSD_TObj *tobj, HSD_TexAnim *texanim);
void HSD_TObjAddAnimAll(HSD_TObj *tobj, HSD_TexAnim *texanim);
void HSD_TObjReqAnimAllByFlags(HSD_TObj *tobj, f32 startframe, u32 flags);
void HSD_TObjReqAnim(HSD_TObj *tobj, f32 startframe);
void HSD_TObjReqAnimAll(HSD_TObj *tobj, f32 startframe);
void HSD_TObjAnim(HSD_TObj *tobj);
void HSD_TObjAnimAll(HSD_TObj *tobj);
void HSD_TObjRemoveAll(HSD_TObj *tobj);
HSD_TObj* HSD_TObjGetNext(HSD_TObj *tobj);
HSD_TObj* HSD_TObjAlloc();
void HSD_TObjFree(HSD_TObj *tobj);

void HSD_TObjSetDefaultClass(HSD_TObjInfo *info);
HSD_TObjInfo* HSD_TObjGetDefaultClass();

HSD_Tlut* HSD_TlutLoadDesc(HSD_TlutDesc *tlutdesc);
HSD_Tlut* HSD_TlutAlloc();
void HSD_TlutFree(HSD_Tlut *tlut);

HSD_TObjTev* HSD_TObjTevLoadDesc(HSD_TObjTevDesc *tevdesc);
HSD_TObjTev* HSD_TObjTevAlloc();

HSD_TObj* HSD_TObjLoadDesc(HSD_TObjDesc *td);

HSD_ImageDesc* HSD_ImageDescAlloc();
void HSD_ImageDescFree(HSD_ImageDesc *idesc);
void HSD_ImageDescCopyFromEFB(HSD_ImageDesc *idesc, u16 origx, u16 origy, u8 clear, int sync);

u32 HSD_TGTex2Index(u32 tgtex);
u32 HSD_TexCoordID2TexGenSrc(u16 coord);
u32 HSD_TexCoord2Index(u16 coord_id);
u16 HSD_Index2TexCoord(u32 index);
u32 HSD_TexMtx2Index(u32 texmtx);
u32 HSD_Index2TexMtx(u32 index);
u8 HSD_Index2TexMap(u32 index);
u32 HSD_TexMap2Index(u8 mapid);


HSD_TObj* _HSD_TObjGetCurrentByType(HSD_TObj *from, u32 mapping);

#endif