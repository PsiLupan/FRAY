#ifndef _hsd_mobj_h_
#define _hsd_mobj_h_

#include "hsd_aobj.h"
#include "hsd_texp.h"
#include "hsd_tobj.h"

#define MOBJ_ANIM 0x4
#define TOBJ_ANIM 0x10

#define	HSD_A_M_AMBIENT_R   1
#define	HSD_A_M_AMBIENT_G   2
#define	HSD_A_M_AMBIENT_B   3
#define	HSD_A_M_DIFFUSE_R   4
#define	HSD_A_M_DIFFUSE_G   5
#define	HSD_A_M_DIFFUSE_B   6
#define	HSD_A_M_SPECULAR_R  7
#define	HSD_A_M_SPECULAR_G  8
#define	HSD_A_M_SPECULAR_B  9
#define	HSD_A_M_ALPHA      10
#define	HSD_A_M_PE_REF0    	11
#define	HSD_A_M_PE_REF1    	12
#define	HSD_A_M_PE_DSTALPHA	13

#define RENDER_TOON	(1<<12)
#define RENDER_SPECULAR (1<<3)
#define RENDER_SHADOW (1<<26)

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
	GXColor ambient;
	GXColor diffuse;
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

typedef struct _HSD_MatAnim {
	struct _HSD_MatAnim *next;
	struct _HSD_AObjDesc *aobjdesc;
	struct _HSD_TexAnim *texanim;
	struct _HSD_RenderAnim *renderanim;
} HSD_MatAnim;

typedef struct _HSD_MatAnimJoint {
	struct _HSD_MatAnimJoint *child;
	struct _HSD_MatAnimJoint *next;
	struct _HSD_MatAnim *matanim;
} HSD_MatAnimJoint;

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

#define HSD_MOBJ(o)		((HSD_MObj *)(o))
#define HSD_MOBJ_INFO(i)	((HSD_MObjInfo *)(i))
#define HSD_MOBJ_METHOD(o)	HSD_MOBJ_INFO(HSD_CLASS_METHOD(o))

#endif