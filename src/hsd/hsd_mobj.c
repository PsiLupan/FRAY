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
	HSD_TExp *diff, *spec, *ext, *alpha;
	HSD_TExp *exp, *cnst;
	HSD_TObj *tobj, *toon = NULL, *bump = NULL;
	u32 done = 0;
	u32 diffuse_mode;
	u32 alpha_mode;

	assert(list);

	*list = NULL;

	for (tobj = tobj_top; tobj; tobj = tobj->next){
		if (tobj_coord(tobj) == TEX_COORD_TOON){
			toon = tobj;
		}
	}

	diffuse_mode = mobj->rendermode & RENDER_DIFFUSE_BITS;
	if (diffuse_mode == RENDER_DIFFUSE_MAT0){
		diffuse_mode = RENDER_DIFFUSE_MAT;
	}

	alpha_mode = mobj->rendermode & RENDER_ALPHA_BITS;
	if (alpha_mode == RENDER_ALPHA_COMPAT){
		alpha_mode = diffuse_mode << (RENDER_ALPHA_SHIFT - RENDER_DIFFUSE_SHIFT);
	}

	exp = HSD_TExpTev(list);
	if (mobj->rendermode & RENDER_DIFFUSE){
		static u8 one = 255;

		switch (diffuse_mode){
			case RENDER_DIFFUSE_VTX:
			HSD_TExpColorOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
			HSD_TExpColorIn(exp, HSD_TE_0, HSD_TEXP_ZERO,
							HSD_TE_0, HSD_TEXP_ZERO,
							HSD_TE_0, HSD_TEXP_ZERO,
							HSD_TE_1, HSD_TEXP_ZERO);
			break;

			default:
			cnst = HSD_TExpCnst(&(mobj->mat->diffuse), HSD_TE_RGB, HSD_TE_U8, list);
			HSD_TExpColorOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
			HSD_TExpColorIn(exp, HSD_TE_0, HSD_TEXP_ZERO,
							HSD_TE_0, HSD_TEXP_ZERO,
							HSD_TE_0, HSD_TEXP_ZERO,
							HSD_TE_RGB, cnst);
			break;
		}

		switch (alpha_mode){
			case RENDER_ALPHA_VTX:
			cnst = HSD_TExpCnst(&(one), HSD_TE_X, HSD_TE_U8, list);
			HSD_TExpAlphaOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
			HSD_TExpAlphaIn(exp, HSD_TE_0, HSD_TEXP_ZERO,
							HSD_TE_0, HSD_TEXP_ZERO,
							HSD_TE_0, HSD_TEXP_ZERO,
							HSD_TE_X, cnst);
			break;

			default:
			cnst = HSD_TExpCnst(&(mobj->mat->alpha), HSD_TE_X, HSD_TE_F32, list);
			HSD_TExpAlphaOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
			HSD_TExpAlphaIn(exp, HSD_TE_0, HSD_TEXP_ZERO,
							HSD_TE_0, HSD_TEXP_ZERO,
							HSD_TE_0, HSD_TEXP_ZERO,
							HSD_TE_X, cnst);
			break;
		}
	}
	else{
		switch (diffuse_mode){
			case RENDER_DIFFUSE_MAT:
			cnst = HSD_TExpCnst(&(mobj->mat->diffuse), HSD_TE_RGB, HSD_TE_U8, list);
			HSD_TExpColorOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
			HSD_TExpColorIn(exp, HSD_TE_0, HSD_TEXP_ZERO,
							HSD_TE_0, HSD_TEXP_ZERO,
							HSD_TE_0, HSD_TEXP_ZERO,
							HSD_TE_RGB, cnst);
			break;

			case RENDER_DIFFUSE_VTX:
			HSD_TExpOrder(exp, toon, GX_COLOR0A0);
			HSD_TExpColorOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
			HSD_TExpColorIn(exp, HSD_TE_0, HSD_TEXP_ZERO,
							HSD_TE_0, HSD_TEXP_ZERO,
							HSD_TE_0, HSD_TEXP_ZERO,
							HSD_TE_RGB, (toon) ? HSD_TEXP_TEX : HSD_TEXP_RAS);
			break;

			default:
			cnst = HSD_TExpCnst(&(mobj->mat->diffuse), HSD_TE_RGB, HSD_TE_U8, list);
			HSD_TExpOrder(exp, toon, GX_COLOR0A0);
			HSD_TExpColorOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
			HSD_TExpColorIn(exp, HSD_TE_RGB, (toon) ? HSD_TEXP_TEX : HSD_TEXP_RAS,
							HSD_TE_0, HSD_TEXP_ZERO,
							HSD_TE_0, HSD_TEXP_ZERO,
							HSD_TE_RGB, cnst);
			break;
		}

		switch (alpha_mode){
			case RENDER_ALPHA_MAT:
			cnst = HSD_TExpCnst(&(mobj->mat->alpha), HSD_TE_X, HSD_TE_F32, list);
			HSD_TExpAlphaOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
			HSD_TExpAlphaIn(exp, HSD_TE_0, HSD_TEXP_ZERO,
							HSD_TE_0, HSD_TEXP_ZERO,
							HSD_TE_0, HSD_TEXP_ZERO,
							HSD_TE_X, cnst);
			break;

			case RENDER_ALPHA_VTX:
			HSD_TExpOrder(exp, toon, GX_COLOR0A0);
			HSD_TExpAlphaOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
			HSD_TExpAlphaIn(exp, HSD_TE_0, HSD_TEXP_ZERO,
							HSD_TE_0, HSD_TEXP_ZERO,
							HSD_TE_0, HSD_TEXP_ZERO,
							HSD_TE_A, HSD_TEXP_RAS);
			break;

			default:
			cnst = HSD_TExpCnst(&(mobj->mat->alpha), HSD_TE_X, HSD_TE_F32, list);
			HSD_TExpOrder(exp, toon, GX_COLOR0A0);
			HSD_TExpAlphaOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
			HSD_TExpAlphaIn(exp, HSD_TE_0, HSD_TEXP_ZERO,
							HSD_TE_A, HSD_TEXP_RAS,
							HSD_TE_X, cnst,
							HSD_TE_0, HSD_TEXP_ZERO);
			break;
		}
	}

	diff = exp;
	alpha = exp;

	for (tobj = tobj_top; tobj != NULL; tobj = tobj->next){
		if ((tobj_lightmap(tobj) & (TEX_LIGHTMAP_DIFFUSE | TEX_LIGHTMAP_AMBIENT)) && tobj->id != GX_TEXMAP_NULL){
			HSD_TOBJ_METHOD(tobj)->make_texp(tobj, TEX_LIGHTMAP_DIFFUSE | TEX_LIGHTMAP_AMBIENT, done, &diff, &alpha, list);
		}
	}
	done |= TEX_LIGHTMAP_DIFFUSE | TEX_LIGHTMAP_AMBIENT;

	if (mobj->rendermode & RENDER_DIFFUSE){
		if (alpha_mode & RENDER_ALPHA_VTX){
			exp = HSD_TExpTev(list);

			HSD_TExpOrder(exp, NULL, GX_COLOR1A1);

			HSD_TExpColorOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
			HSD_TExpColorIn(exp, HSD_TE_0, HSD_TEXP_ZERO,
							HSD_TE_0, HSD_TEXP_ZERO,
							HSD_TE_0, HSD_TEXP_ZERO,
							HSD_TE_RGB, diff);

			HSD_TExpAlphaOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
			HSD_TExpAlphaIn(exp, HSD_TE_A, alpha,
							HSD_TE_0, HSD_TEXP_ZERO,
							HSD_TE_A, HSD_TEXP_RAS,
							HSD_TE_0, HSD_TEXP_ZERO);

			diff = exp;
			alpha = exp;
		}

		exp = HSD_TExpTev(list);

		if (toon){
			HSD_TExpOrder(exp, toon, GX_COLOR0A0);
			HSD_TExpColorOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
			HSD_TExpColorIn(exp, HSD_TE_0, HSD_TEXP_ZERO,
							HSD_TE_RGB, diff,
							HSD_TE_RGB, HSD_TEXP_TEX,
							HSD_TE_0, HSD_TEXP_ZERO);
		}
		else{
			HSD_TExpOrder(exp, NULL, GX_COLOR0A0);
			HSD_TExpColorOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
			HSD_TExpColorIn(exp, HSD_TE_0, HSD_TEXP_ZERO,
							HSD_TE_RGB, diff,
							HSD_TE_RGB, HSD_TEXP_RAS,
							HSD_TE_0, HSD_TEXP_ZERO);
		}
		diff = exp;

		if (alpha_mode & RENDER_ALPHA_VTX){
			HSD_TExpAlphaOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
			HSD_TExpAlphaIn(exp, HSD_TE_0, HSD_TEXP_ZERO,
							HSD_TE_A, alpha,
							HSD_TE_A, HSD_TEXP_RAS,
							HSD_TE_0, HSD_TEXP_ZERO);
		}else{
			HSD_TExpAlphaOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
			HSD_TExpAlphaIn(exp, HSD_TE_A, alpha,
							HSD_TE_0, HSD_TEXP_ZERO,
							HSD_TE_A, HSD_TEXP_RAS,
							HSD_TE_0, HSD_TEXP_ZERO);
		}
		alpha = exp;
	}

	if (mobj->rendermode & RENDER_SPECULAR){
		cnst = HSD_TExpCnst(&(mobj->mat->specular), HSD_TE_RGB, HSD_TE_U8, list);
		exp = HSD_TExpTev(list);
		HSD_TExpColorOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
		HSD_TExpColorIn(exp, HSD_TE_0, HSD_TEXP_ZERO,
						HSD_TE_0, HSD_TEXP_ZERO,
						HSD_TE_0, HSD_TEXP_ZERO,
						HSD_TE_RGB, cnst);

		spec = exp;

		for (tobj = tobj_top; tobj != NULL; tobj = tobj->next){
			if ((tobj_lightmap(tobj) & TEX_LIGHTMAP_SPECULAR) && tobj->id != GX_TEXMAP_NULL){
				HSD_TOBJ_METHOD(tobj)->make_texp(tobj, TEX_LIGHTMAP_SPECULAR, done, &spec, &alpha, list);
			}
		}
		done |= TEX_LIGHTMAP_SPECULAR;

		exp = HSD_TExpTev(list);
		HSD_TExpOrder(exp, NULL, GX_COLOR1A1);
		HSD_TExpColorOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
		HSD_TExpColorIn(exp, HSD_TE_0, HSD_TEXP_ZERO,
						HSD_TE_RGB, spec,
						HSD_TE_RGB, HSD_TEXP_RAS,
						HSD_TE_0, HSD_TEXP_ZERO);
		spec = exp;

		exp = HSD_TExpTev(list);
		HSD_TExpColorOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
		HSD_TExpColorIn(exp, HSD_TE_RGB, spec,
						HSD_TE_0, HSD_TEXP_ZERO,
						HSD_TE_0, HSD_TEXP_ZERO,
						HSD_TE_RGB, diff);
		diff = exp;
	}

	ext = diff;

	for (tobj = tobj_top; tobj != NULL; tobj = tobj->next){
		if ((tobj_lightmap(tobj) & TEX_LIGHTMAP_EXT) && tobj->id != GX_TEXMAP_NULL){
			HSD_TOBJ_METHOD(tobj)->make_texp(tobj, TEX_LIGHTMAP_EXT, done, &ext, &alpha, list);
		}
	}

	if (ext != alpha || HSD_TExpGetType(ext) != HSD_TE_TEV || HSD_TExpGetType(alpha) != HSD_TE_TEV){
		exp = HSD_TExpTev(list);
		HSD_TExpColorOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
		HSD_TExpColorIn(exp, HSD_TE_0, HSD_TEXP_ZERO,
						HSD_TE_0, HSD_TEXP_ZERO,
						HSD_TE_0, HSD_TEXP_ZERO,
						HSD_TE_RGB, ext);
		HSD_TExpAlphaOp(exp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
		HSD_TExpAlphaIn(exp, HSD_TE_0, HSD_TEXP_ZERO,
						HSD_TE_0, HSD_TEXP_ZERO,
						HSD_TE_0, HSD_TEXP_ZERO,
						HSD_TE_A, alpha);
		return exp;
	}else{
		return ext;
	}
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
	if(mobj != NULL){
		HSD_CLASS_METHOD(mobj)->release((HSD_Class*)mobj);
		HSD_CLASS_METHOD(mobj)->destroy((HSD_Class*)mobj);
	}
}

//80363CA4
HSD_MObj* HSD_MObjAlloc(){
  HSD_MObj *mobj = (HSD_MObj*)hsdNew((HSD_ClassInfo*)HSD_MObjGetDefaultClass());
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
	hsdFreeMemPiece(mobj->mat, sizeof(HSD_Material)); //hsdFreeMemPiece
	HSD_TObjRemoveAll(mobj->tobj);
	
	if (mobj->tevdesc != NULL)
		HSD_TExpFreeTevDesc(mobj->tevdesc);
	if (mobj->texp != NULL)
		HSD_TExpFreeList(mobj->texp, HSD_TE_ALL, 1);
	if (mobj->pe != NULL)
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
