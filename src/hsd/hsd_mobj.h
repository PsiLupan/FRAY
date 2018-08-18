#ifndef _hsd_mobj_h_
#define _hsd_mobj_h_

#include "hsd_aobj.h"
#include "hsd_tobj.h"

//Material Object
typedef struct _HSD_MObj {
	HSD_Class parent;
	u32 rendermode;
	struct _HSD_TObj* tobj;
	HSD_Material *mat;
	HSD_PEDesc* pedesc;
	struct _HSD_AObj* aobj;
	struct _HSD_TObj* ambient_tobj;
	struct _HSD_TObj* specular_tobj;
	HSD_TExpTevDesc *tevdesc;
	HSD_TExp *texp;

	HSD_TObj* tobj_toon;
	HSD_TObj* tobj_gradation;
	HSD_TObj* tobj_backlight;
	f32 z_offset;
} HSD_MObj;

typedef struct _HSD_MObjDesc {
	char* unused_class;
	u32 rendermode;
	struct _HSD_TObjDesc* tobj;
	HSD_Material* material;
	struct _HSD_RenderDesc renderdesc;
	HSD_PEDesc* pedesc;
} HSD_MObjDesc;

typedef struct _HSD_Material {
	GXColor diffuse;
	GXColor ambience;
	GXColor specular;
	f32 alpha;
	f32 shininess;
} HSD_Material;

typdef struct _HSD_PEDesc {
	u8 flags;
	u8 ref0;
	u8 ref1;
	u8 dst_alpha;
	u8 type;
	u8 src_factor;
	u8 dst_factor;
	u8 logic_op;
	u8 z_comp;
	u8 alpha_comp0;
	u8 alpha_op;
	u8 alpha_comp1;
} HSD_PEDesc;

typedef struct _HSD_TExpTevDesc {

} HSD_TExpTevDesc;

typedef struct _HSD_TExp {

} HSD_TExp;

typedef struct _HSD_MObjInfo {
  HSD_ClassInfo	parent;
  void	    (*setup)(HSD_MObj *mobj, u32 rendermode);
  int	    (*load)(HSD_MObj *mobj, HSD_MObjDesc *desc);
  HSD_TExp* (*make_texp)(HSD_MObj *mobj, HSD_TObj *tobj_top, HSD_TExp **list);
  void	    (*setup_tev)(HSD_MObj *mobj, HSD_TObj *tobj, u32 rendermode);
  void	    (*update)(void *obj, u32 type, FObjData *val);
  void	    (*unset)(HSD_MObj *mobj, u32 rendermode);
} HSD_MObjInfo;

extern HSD_MObjInfo hsdMObj;

#endif