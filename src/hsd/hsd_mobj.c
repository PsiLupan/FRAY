#include "hsd_mobj.h"

#include "hsd_aobj.h"
#include "hsd_state.h"

static void MObjInfoInit();

HSD_MObjInfo hsdMObj = { MObjInfoInit };

static HSD_MObjInfo *default_class = NULL;

static HSD_MObj *current_mobj = NULL;

static HSD_TObj *tobj_toon    = NULL;
static HSD_TObj *tobj_shadows = NULL;

//80362D30
void HSD_MObjSetCurrent(HSD_MObj *mobj){
	current_mobj = mobj;
}

//80362D38
void HSD_MObjSetFlags(HSD_MObj *mobj, u32 flags){
	if (mobj)
		mobj->rendermode |= flags;
}

//80362D50
void HSD_MObjClearFlags(HSD_MObj *mobj, u32 flags){
	if (mobj) 
		mobj->rendermode &= ~flags;
}

//80362D68
void HSD_MObjRemoveAnimByFlags(HSD_MObj *mobj, u32 flags){
	if (mobj){
		if (flags & MOBJ_ANIM) {
			HSD_AObjRemove(mobj->aobj);
			mobj->aobj = NULL;
		}
		
		if (flags & TOBJ_ANIM) {
			HSD_TObjRemoveAnimAll(mobj->tobj);
		}
	}
}

//80362DC8
void HSD_MObjAddAnim(HSD_MObj *mobj, HSD_MatAnim *matanim){
	if (mobj != NULL && matanim != NULL){
		if (mobj->aobj != NULL) {
			HSD_AObjRemove(mobj->aobj);
		}
		mobj->aobj = HSD_AObjLoadDesc(matanim->aobjdesc);
		HSD_TObjAddAnimAll(mobj->tobj, matanim->texanim); 
	}
}

//80362E30
void HSD_MObjReqAnimByFlags(HSD_MObj *mobj, u32 flags, f32 startframe){
	if (mobj){
		if (flags & MOBJ_ANIM) {
			HSD_AObjReqAnim(mobj->aobj, startframe);
		}
		HSD_TObjReqAnimAllByFlags(mobj->tobj, startframe, flags);
	}
}

//80362E98
void HSD_MObjReqAnim(HSD_MObj *mobj, f32 startframe){
	if (mobj){
		HSD_AObjReqAnim(mobj->aobj, startframe);
		HSD_TObjReqAnimAllByFlags(mobj->tobj, startframe, ALL_ANIM);
	}
}

//80362EEC
static void MObjUpdateFunc(void *obj, u32 type, FObjData *val){
	HSD_MObj *mobj = obj;
	
	if (mobj){
		switch (type) {
			case HSD_A_M_AMBIENT_R:
			if (mobj->mat) {
				mobj->mat->ambient.r = (u8)(255.0F * HSD_ClampFloat(val->fv, 0, 1));
			}
			break;
			
			case HSD_A_M_AMBIENT_G:
			if (mobj->mat) {
				mobj->mat->ambient.g = (u8)(255.0F * HSD_ClampFloat(val->fv, 0, 1));
			}
			break;
			
			case HSD_A_M_AMBIENT_B:
			if (mobj->mat) {
				mobj->mat->ambient.b = (u8)(255.0F * HSD_ClampFloat(val->fv, 0, 1));
			}
			break;
			
			case HSD_A_M_DIFFUSE_R:
			if (mobj->mat) {
				mobj->mat->diffuse.r = (u8)(255.0F * HSD_ClampFloat(val->fv, 0, 1));
			}
			break;
			
			case HSD_A_M_DIFFUSE_G:
			if (mobj->mat) {
				mobj->mat->diffuse.g = (u8)(255.0F * HSD_ClampFloat(val->fv, 0, 1));
			}
			break;
			
			case HSD_A_M_DIFFUSE_B:
			if (mobj->mat) {
				mobj->mat->diffuse.b = (u8)(255.0F * HSD_ClampFloat(val->fv, 0, 1));
			}
			break;
			
			case HSD_A_M_ALPHA:
			if (mobj->mat) {
				mobj->mat->alpha = HSD_ClampFloat(1.0F - val->fv, 0, 1);
			}
			break;
			
			case HSD_A_M_SPECULAR_R:
			if (mobj->mat) {
				mobj->mat->specular.r = (u8)(255.0F * HSD_ClampFloat(val->fv, 0, 1));
			}
			break;
			
			case HSD_A_M_SPECULAR_G:
			if (mobj->mat) {
				mobj->mat->specular.g = (u8)(255.0F * HSD_ClampFloat(val->fv, 0, 1));
			}
			break;
			
			case HSD_A_M_SPECULAR_B:
			if (mobj->mat) {
				mobj->mat->specular.b = (u8)(255.0F * HSD_ClampFloat(val->fv, 0, 1));
			}
			break;
			
			case HSD_A_M_PE_REF0:
			if (mobj->pe) {
				mobj->pe->ref0 = (u8)(255.0F * HSD_ClampFloat(val->fv, 0, 1));
			}
			break;
			
			case HSD_A_M_PE_REF1:
			if (mobj->pe) {
				mobj->pe->ref1 = (u8)(255.0F * HSD_ClampFloat(val->fv, 0, 1));
			}
			break;
			
			case HSD_A_M_PE_DSTALPHA:
			if (mobj->pe) {
				mobj->pe->dst_alpha = (u8)(255.0F * HSD_ClampFloat(val->fv, 0, 1));
			}
			break;
		}
	}
}

//803630FC
void HSD_MObjAnim(HSD_MObj *mobj){
	if (mobj){
		HSD_AObjInterpretAnim(mobj->aobj, mobj, MObjUpdateFunc);
		HSD_TObjAnimAll(mobj->tobj);
	}
}

//80363144
static int MObjLoad(HSD_MObj *mobj, HSD_MObjDesc *desc){
	mobj->rendermode = desc->rendermode;
	mobj->tobj = HSD_TObjLoadDesc(desc->texdesc);
	mobj->mat = HSD_MaterialAlloc();
	memcpy(mobj->mat, desc->mat, sizeof(HSD_Material));
	mobj->rendermode |= RENDER_TOON;
	if (desc->pedesc) {
		mobj->pe = hsdAllocMemPiece(sizeof(HSD_PEDesc));
		memcpy(mobj->pe, desc->pedesc, sizeof(HSD_PEDesc));
	}
	mobj->aobj = NULL;
	return 0;
}

void HSD_MObjSetDefaultClass(HSD_MObjInfo *info){
	default_class = info;
}

HSD_MObjInfo* HSD_MObjGetDefaultClass(){
	return default_class ? default_class : &hsdMObj;
}

//803631E4
HSD_MObj* HSD_MObjLoadDesc(HSD_MObjDesc *mobjdesc){
	if (mobjdesc) {
		HSD_MObj *mobj;
		HSD_ClassInfo *info;
		if (!mobjdesc->class_name || !(info = hsdSearchClassInfo(mobjdesc->class_name))){
			mobj = HSD_MObjAlloc();
		} else {
			mobj = hsdNew(info);
			assert(mobj);
		}
		HSD_MOBJ_METHOD(mobj)->load(mobj, mobjdesc);
		HSD_MObjCompileTev(mobj);
		return mobj;
	}
	return NULL;
}

//80363284
static HSD_TExp* MObjMakeTExp(HSD_MObj *mobj, HSD_TObj *tobj_top, HSD_TExp **list){
}

//8036388C
void HSD_MObjCompileTev(HSD_MObj *mobj){
	HSD_TObj *tobj, **tail = NULL;
	HSD_TExp *texp;
	
	if (mobj){
		if (mobj->tevdesc) {
			HSD_TExpFreeTevDesc(mobj->tevdesc);
			mobj->tevdesc = NULL;
		}
		if (mobj->texp) {
			HSD_TExpFreeList(mobj->texp, HSD_TE_ALL, 1);
			mobj->texp = NULL;
		}
		
		tobj = mobj->tobj;
		
		if ((mobj->rendermode & RENDER_SHADOW) && tobj_shadows) {
			for (tail=&tobj; *tail; tail=&(*tail)->next) {}
			*tail = tobj_shadows;
		}
		
		if ((mobj->rendermode & RENDER_TOON) && tobj_toon && tobj_toon->imagedesc) {
			tobj_toon->next = tobj;
			tobj = tobj_toon;
		}
		HSD_TObjAssignResources(tobj);
		texp = HSD_MOBJ_METHOD(mobj)->make_texp(mobj, tobj, &mobj->texp);
		HSD_TExpCompile(texp, &mobj->tevdesc, &mobj->texp);
		
		if (tail) {
			*tail = NULL;
		}
	}
}

//803639AC
static void MObjSetupTev(HSD_MObj *mobj, HSD_TObj *tobj, u32 rendermode){
  assert(mobj->tevdesc);
  HSD_TExpSetupTev(mobj->tevdesc, mobj->texp);
  HSD_TObjSetupVolatileTev(tobj, rendermode);
}

//80363A24
void HSD_MObjSetup(HSD_MObj *mobj, u32 rendermode){
	HSD_TObj *tobj, **tail;
	
	HSD_StateInitTev();
	rendermode = mobj->rendermode;
	HSD_SetMaterialColor(mobj->mat->ambient, mobj->mat->diffuse, mobj->mat->specular, mobj->mat->alpha);
	
	if (rendermode & RENDER_SPECULAR) {
		HSD_SetMaterialShininess(mobj->mat->shininess);
	}
	
	tobj = mobj->tobj;
	tail = NULL;
	
	if ((rendermode & RENDER_SHADOW) && tobj_shadows) {
		for (tail=&tobj; *tail; tail = &((*tail)->next)) {}
		*tail = tobj_shadows;
	}
	if (rendermode & RENDER_TOON && tobj_toon && tobj_toon->imagedesc != NULL) {
		tobj_toon->next = tobj;
		tobj = tobj_toon;
	}
	HSD_TObjSetup(tobj);
	HSD_TObjSetupTextureCoordGen(tobj);
	
	HSD_MOBJ_METHOD(mobj)->setup_tev(mobj, tobj, rendermode);
	HSD_SetupRenderModeWithCustomPE(rendermode, mobj->pe);
	if (tail) {
		*tail = NULL;
	}
}

//80363B68
void HSD_MObjUnset(HSD_MObj *mobj, u32 rendermode){
#pragma unused(mobj, rendermode)
	HSD_TObjSetup(NULL);
}

//80363B8C
void HSD_MObjSetToonTextureImage(HSD_ImageDesc *toonimage){
	if (!tobj_toon) {
		static HSD_TObjDesc desc = {
			NULL,
			NULL,
			GX_TEXMAP7,
			GX_TG_COLOR0,
			{ 0.0F, 0.0F, 0.0F },
			{ 0.0F, 0.0F, 0.0F },  
			{ 0.0F, 0.0F, 0.0F },  
			GX_CLAMP,
			GX_CLAMP,
			0,
			0,
			TEX_COORD_TOON,
			1.0F,
			GX_LINEAR,
			NULL,
			NULL,
			NULL
		};
		tobj_toon = HSD_TObjLoadDesc(&desc);
		assert(tobj_toon);
	}
	tobj_toon->imagedesc = toonimage;
}

//80363C10
void HSD_MObjSetDiffuseColor(HSD_MObj *mobj, u8 r, u8 g, u8 b){
	if (!mobj || !mobj->mat) {
		return;
	}
	mobj->mat->diffuse.r = r;
	mobj->mat->diffuse.g = g;
	mobj->mat->diffuse.b = b;
}

//80363C2C
void HSD_MObjSetAlpha(HSD_MObj *mobj, f32 alpha){
	if (!mobj || !mobj->mat) {
		return;
	}
	mobj->mat->alpha = alpha;
}

//80363C38
HSD_TObj* HSD_MObjGetTObj(HSD_MObj *mobj){
	if (mobj){
		return mobj->tobj;
	}
	return NULL;
}

//80363C50
void HSD_MObjRemove(HSD_MObj *mobj){
	if(mobj){
		HSD_INFO_METHOD(mobj)->release(mobj);
		HSD_INFO_METHOD(mobj)->destroy(mobj);
	}
}

//80363CA4
HSD_MObj* HSD_MObjAlloc(){
  HSD_MObj *mobj = hsdNew(HSD_MObjGetDefaultClass());
  assert(mobj);
  return mobj;
}

//80363D00
HSD_Material* HSD_MaterialAlloc(){
	HSD_Material *mat = hsdAllocMemPiece(sizeof(HSD_Material));
	assert(mat);
	memset(mat, 0, sizeof(HSD_Material));
	mat->alpha = 1.0F;
	return mat;
}

//80363D60
void HSD_MObjAddShadowTexture(HSD_TObj *tobj){
	HSD_TObj *tp;
	assert(tobj);
	for (tp=tobj_shadows; tp; tp=tp->next){
		if (tp == tobj)
			return;
	}
	tobj->next = tobj_shadows;
	tobj_shadows = tobj;
}

//80363DC4
void HSD_MObjDeleteShadowTexture(HSD_TObj *tobj){
	HSD_TObj **tp, *next;
	if (tobj) {
		for (tp = &tobj_shadows; *tp; tp = &(*tp)->next) {
			if (*tp == tobj) {
				*tp = tobj->next;
				tobj->next = NULL;
				return;
			}
		}
	} else {
		for (; tobj_shadows; tobj_shadows=next) {
			next = tobj_shadows->next;
			tobj_shadows->next = NULL;
		}
	}
}

//80363E28
static void MObjRelease(HSD_Class *o){
	HSD_MObj *mobj = HSD_MOBJ(o);
	
	HSD_AObjRemove(mobj->aobj);
	HSD_MaterialRemove(mobj->mat); //hsdFreeMemPiece
	HSD_TObjRemoveAll(mobj->tobj);
	
	if (mobj->tevdesc)
		HSD_TExpFreeTevDesc(mobj->tevdesc);
	if (mobj->texp)
		HSD_TExpFreeList(mobj->texp, HSD_TE_ALL, 1);
	if (mobj->pe)
		hsdFreeMemPiece(mobj->pe, sizeof(HSD_PEDesc));
	
	HSD_PARENT_INFO(&hsdMObj)->release(o);
}

//80363EC4
static void MObjAmnesia(HSD_ClassInfo *info){
	if (info == HSD_CLASS_INFO(default_class)) {
		default_class = NULL;
	}
	if (info == HSD_CLASS_INFO(&hsdMObj)) {
		tobj_toon = NULL;
		tobj_shadows = NULL;
	}
	HSD_PARENT_INFO(&hsdMObj)->amnesia(info);
}

//80363F28
static void MObjInfoInit(){
	hsdInitClassInfo(HSD_CLASS_INFO(&hsdMObj), HSD_CLASS_INFO(&hsdClass), HSD_BASE_CLASS_LIBRARY, "hsd_mobj", sizeof(HSD_MObjInfo), sizeof(HSD_MObj));
	
	HSD_CLASS_INFO(&hsdMObj)->release  = MObjRelease;
	HSD_CLASS_INFO(&hsdMObj)->amnesia  = MObjAmnesia;
	HSD_MOBJ_INFO(&hsdMObj)->setup     = HSD_MObjSetup;
	HSD_MOBJ_INFO(&hsdMObj)->unset     = HSD_MObjUnset;
	HSD_MOBJ_INFO(&hsdMObj)->load      = MObjLoad;
	HSD_MOBJ_INFO(&hsdMObj)->make_texp = MObjMakeTExp;
	HSD_MOBJ_INFO(&hsdMObj)->setup_tev = MObjSetupTev;
	HSD_MOBJ_INFO(&hsdMObj)->update    = MObjUpdateFunc;
}