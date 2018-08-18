#ifndef _hsd_tobj_h_
#define _hsd_tobj_h_

#include "../../include/mtx/GeoTypes.h"

#include "hsd_aobj.h"

//Texture Object
typedef struct _HSD_TObj {
	HSD_Class parent;
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
	void (*make_mtx)	(HSD_TObj *tobj);
	int (*load)		(HSD_TObj *tobj, HSD_TObjDesc *desc);
	void (*make_texp)	(HSD_TObj *tobj, u32 lightmap, u32 lightmap_done, HSD_TExp **c, HSD_TExp **a, HSD_TExp **list);
	void (*update)	(void *obj, u32 type, FObjData *val);
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

extern HSD_TObjInfo hsdTObj;

#endif