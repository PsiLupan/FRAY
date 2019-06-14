#ifndef _hsd_mobj_h_
#define _hsd_mobj_h_

#include <gctypes.h>
#include <ogc/gx.h>

#include "hsd_object.h"

#include "hsd_texp.h"
#include "hsd_tobj.h"
#include "hsd_util.h"

#define MOBJ_ANIM 0x4
#define TOBJ_ANIM 0x10
#define ALL_ANIM 0x7FF

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
	HSD_Class class_parent;
	u32 rendermode;
	struct _HSD_TObj* tobj;
	struct _HSD_Material *mat;
	struct _HSD_PEDesc* pe;
	struct _HSD_AObj* aobj;
	struct _HSD_TObj* ambient_tobj;
	struct _HSD_TObj* specular_tobj;
	struct _HSD_TExpTevDesc *tevdesc;
	struct _HSD_TExp *texp;

	struct _HSD_TObj* tobj_toon;
	struct _HSD_TObj* tobj_gradation;
	struct _HSD_TObj* tobj_backlight;
	f32 z_offset;
} HSD_MObj;

typedef struct _HSD_Material {
	GXColor ambient;
	GXColor diffuse;
	GXColor specular;
	f32 alpha;
	f32 shininess;
} HSD_Material;

typedef struct _HSD_PEDesc {
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

typedef struct _HSD_MObjDesc {
	char* class_name;
	u32 rendermode;
	struct _HSD_TObjDesc* texdesc;
	struct _HSD_Material* mat;
	void* renderdesc;
	struct _HSD_PEDesc* pedesc;
} HSD_MObjDesc;

typedef struct _HSD_MatAnim {
	struct _HSD_MatAnim *next;
	struct _HSD_AObjDesc *aobjdesc;
	struct _HSD_TexAnim *texanim;
	void* renderanim; //struct _HSD_RenderAnim *renderanim; - Commented out until I actually run into an instance of it to test w/
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

void HSD_MObjSetFlags(HSD_MObj *mobj, u32 flags);
void HSD_MObjClearFlags(HSD_MObj *mobj, u32 flags);
void HSD_MObjRemoveAnimByFlags(HSD_MObj *mobj, u32 flags);
void HSD_MObjAddAnim(HSD_MObj *mobj, HSD_MatAnim *matanim);
void HSD_MObjReqAnimByFlags(HSD_MObj *mobj, u32 flags, f32 startframe);
void HSD_MObjReqAnim(HSD_MObj *mobj, f32 startframe);
void HSD_MObjAnim(HSD_MObj *mobj);
HSD_MObj* HSD_MObjLoadDesc(HSD_MObjDesc *mobjdesc);
void HSD_MObjCompileTev(HSD_MObj *mobj);
void HSD_MObjSetToonTextureImage(HSD_ImageDesc *toonimage);
void HSD_MObjSetDiffuseColor(HSD_MObj *mobj, u8 r, u8 g, u8 b);
void HSD_MObjSetAlpha(HSD_MObj *mobj, f32 alpha);
HSD_TObj* HSD_MObjGetTObj(HSD_MObj *mobj);
void HSD_MObjRemove(HSD_MObj *mobj);
HSD_MObj* HSD_MObjAlloc();
HSD_Material* HSD_MaterialAlloc();
void HSD_MObjAddShadowTexture(HSD_TObj *tobj);
void HSD_MObjDeleteShadowTexture(HSD_TObj *tobj);


void HSD_MObjSetCurrent(HSD_MObj *mobj);
void HSD_MObjSetup(HSD_MObj *mobj, u32 rendermode);
void HSD_MObjUnset(HSD_MObj *mobj, u32 rendermode);

#endif
