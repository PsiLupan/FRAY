#include "hsd_tobj.h"

#include "hsd_lobj.h"

static void TObjInfoInit();

HSD_TObjInfo hsdTObj = { TObjInfoInit };

static HSD_TObjInfo *default_class = NULL;

#define GX_MAX_TEXMTX 10
static HSD_TObj *tobj_head;

//8035E560
void HSD_TObjRemoveAnimAll(HSD_TObj *tobj){
	HSD_TObj *tp;
	if(tobj){
		for (tp = tobj; tp; tp = tp->next) {
			HSD_AObjRemove(tobj->aobj);
			tobj->aobj = NULL;
		}
	}
}

static HSD_TexAnim* lookupTextureAnim(s32 id, HSD_TexAnim *texanim){
	HSD_TexAnim *ta;
	for (ta = texanim; ta; ta = ta->next) {
		if (ta->id == id) 
			return ta;
	}
	return NULL;
}


//8035E5C0
void HSD_TObjAddAnim(HSD_TObj *tobj, HSD_TexAnim *texanim){
	HSD_TexAnim *ta;
	if (tobj == NULL){
		if ((ta = lookupTextureAnim(tobj->anim_id, texanim)) != NULL) {
			int i;
			if (tobj->aobj) {
				HSD_AObjRemove(tobj->aobj);
			}
			tobj->aobj = HSD_AObjLoadDesc(ta->aobjdesc);
			tobj->imagetbl = ta->imagetbl;
			
			if (tobj->tluttbl) {
				for (i=0; tobj->tluttbl[i]; i++) {
					HSD_TlutFree(tobj->tluttbl[i]);
				}
				HSD_Free(tobj->tluttbl);
			}
			
			if (ta->n_tluttbl) {
				tobj->tluttbl = HSD_MemAlloc((s32) sizeof(HSD_Tlut*) * (ta->n_tluttbl + 1));
				for (i = 0; i < ta->n_tluttbl; i++) {
					tobj->tluttbl[i] = HSD_TlutLoadDesc(ta->tluttbl[i]);
				}
				tobj->tluttbl[i] = NULL;
			} else {
				tobj->tluttbl = NULL;
			}
			tobj->tlut_no = (u8)-1;
		}
	}
}

//8035E708
void HSD_TObjAddAnimAll(HSD_TObj *tobj, HSD_TexAnim *texanim){
	HSD_TObj *tp;
	if (tobj){
		for (tp = tobj; tp; tp = tp->next) {
			HSD_TObjAddAnim(tp, texanim);
		}
	}
}

//8035E760
void HSD_TObjReqAnimAllByFlags(HSD_TObj *tobj, f32 startframe, u32 flags){
	HSD_TObj *tp;
	if (tobj){
		for (tp = tobj; tp; tp = tp->next) {
			if (flags & TOBJ_ANIM) {
				HSD_AObjReqAnim(tobj->aobj, startframe);
			}
		}
	}
}

//8035E7D4
void HSD_TObjReqAnim(HSD_TObj *tobj, f32 startframe){
	if(tobj)
		HSD_AObjReqAnim(tobj->aobj, startframe);
}

//8035E800
void HSD_TObjReqAnimAll(HSD_TObj *tobj, f32 startframe){
	HSD_TObj *tp;
	if (tobj){
		for (tp = tobj; tp; tp = tp->next) {
			HSD_AObjReqAnim(tobj->aobj, startframe);
		}
	}
}

//8035E860
static void TObjUpdateFunc(void *obj, u32 type, FObjData *val){
	HSD_TObj *tobj = obj;
	if (tobj){
		switch (type) {
			case HSD_A_T_TIMG:
			{
				int n;
				assert(tobj->imagetbl);
				n = (int)val->fv;
				if (tobj->imagetbl[n]) {
					tobj->imagedesc = tobj->imagetbl[n];
				}
			}
			break;
			
			case HSD_A_T_TCLT:
			{
				if (tobj->tluttbl) {
					tobj->tlut_no = (u8) val->fv;
				}
			}
			break;
			
			case HSD_A_T_BLEND:
			tobj->blending = HSD_ClampFloat(val->fv, 0, 1);
			break;
			
			case HSD_A_T_ROTX:
			tobj->rotate.x = val->fv;
			goto mtxdirty;
			
			case HSD_A_T_ROTY:
			tobj->rotate.y = val->fv;
			goto mtxdirty;
			
			case HSD_A_T_ROTZ:
			tobj->rotate.z = val->fv;
			goto mtxdirty;
			
			case HSD_A_T_TRAU:
			tobj->translate.x = val->fv;
			goto mtxdirty;
			
			case HSD_A_T_TRAV:
			tobj->translate.y = val->fv;
			goto mtxdirty;
			
			case HSD_A_T_SCAU:
			tobj->scale.x = val->fv;
			goto mtxdirty;
			
			case HSD_A_T_SCAV:
			tobj->scale.y = val->fv;
		mtxdirty:
			tobj->flags |= TEX_MTX_DIRTY;
			break;
			
			case HSD_A_T_LOD_BIAS:
			if (tobj->lod) {
				tobj->lod->LODBias = val->fv;
			}
			break;
			
			case HSD_A_T_KONST_R:
			if (tobj->tev) {
				tobj->tev->constant.r = (u8)(255.0F * HSD_ClampFloat(val->fv, 0, 1));
			}
			break;
			
			case HSD_A_T_KONST_G:
			if (tobj->tev) {
				tobj->tev->constant.g = (u8)(255.0F * HSD_ClampFloat(val->fv, 0, 1));
			}
			break;
			
			case HSD_A_T_KONST_B:
			if (tobj->tev) {
				tobj->tev->constant.b = (u8)(255.0F * HSD_ClampFloat(val->fv, 0, 1));
			}
			break;
			
			case HSD_A_T_KONST_A:
			if (tobj->tev) {
				tobj->tev->constant.a = (u8)(255.0F * HSD_ClampFloat(val->fv, 0, 1));
			}
			break;
			
			case HSD_A_T_TEV0_R:
			if (tobj->tev) {
				tobj->tev->tev0.r = (u8)(255.0F * HSD_ClampFloat(val->fv, 0, 1));
			}
			break;
			
			case HSD_A_T_TEV0_G:
			if (tobj->tev) {
				tobj->tev->tev0.g = (u8)(255.0F * HSD_ClampFloat(val->fv, 0, 1));
			}
			break;
			
			case HSD_A_T_TEV0_B:
			if (tobj->tev) {
				tobj->tev->tev0.b = (u8)(255.0F * HSD_ClampFloat(val->fv, 0, 1));
			}
			break;
			
			case HSD_A_T_TEV0_A:
			if (tobj->tev) {
				tobj->tev->tev0.a = (u8)(255.0F * HSD_ClampFloat(val->fv, 0, 1));
			}
			break;
			
			case HSD_A_T_TEV1_R:
			if (tobj->tev) {
				tobj->tev->tev1.r = (u8)(255.0F * HSD_ClampFloat(val->fv, 0, 1));
			}
			break;
			
			case HSD_A_T_TEV1_G:
			if (tobj->tev) {
				tobj->tev->tev1.g = (u8)(255.0F * HSD_ClampFloat(val->fv, 0, 1));
			}
			break;
			
			case HSD_A_T_TEV1_B:
			if (tobj->tev) {
				tobj->tev->tev1.b = (u8)(255.0F * HSD_ClampFloat(val->fv, 0, 1));
			}
			break;
			
			case HSD_A_T_TEV1_A:
			if (tobj->tev) {
				tobj->tev->tev1.a = (u8)(255.0F * HSD_ClampFloat(val->fv, 0, 1));
			}
			break;
			
			case HSD_A_T_TS_BLEND:
			tobj->blending = HSD_ClampFloat(val->fv, 0, 1);
			break;
		}
	}
}

//8035EB6C
void HSD_TObjAnim(HSD_TObj *tobj){
	if (tobj){
		HSD_AObjInterpretAnim(tobj->aobj, tobj, TObjUpdateFunc);
	}
}

//8035EBA0
void HSD_TObjAnimAll(HSD_TObj *tobj){
	HSD_TObj *tp;
	if (tobj){
		for (tp = tobj; tp; tp = tp->next) {
			HSD_AObjInterpretAnim(tp->aobj, tp, TObjUpdateFunc);
		}
	}
}

//8035EC08
static int TObjLoad(HSD_TObj *tobj, HSD_TObjDesc *td){
	tobj->next = HSD_TObjLoadDesc(td->next);
	tobj->anim_id = (u16) td->id;
	tobj->src = td->src;
	tobj->mtxid = GX_IDENTITY;
	tobj->rotate.x = td->rotate.x;
	tobj->rotate.y = td->rotate.y;
	tobj->rotate.z = td->rotate.z;
	tobj->scale = td->scale;
	tobj->translate = td->translate;
	tobj->wrap_s = td->wrap_s;
	tobj->wrap_t = td->wrap_t;
	tobj->repeat_s = td->repeat_s;
	tobj->repeat_t = td->repeat_t;
	tobj->blend_flags = td->blend_flags;
	tobj->blending = td->blending;
	tobj->magFilt = td->magFilt;
	tobj->imagedesc = td->imagedesc;
	tobj->tlut = HSD_TlutLoadDesc(td->tlutdesc);
	tobj->lod = td->lod;
	tobj->aobj = NULL;
	tobj->flags |= TEX_MTX_DIRTY;
	tobj->tlut_no = (u8) -1;
	tobj->tev = HSD_TObjTevLoadDesc(td->tev);
	
	return 0;
}

//8035ED28
HSD_TObj* HSD_TObjLoadDesc(HSD_TObjDesc *td){
	if (td != NULL) {
		HSD_TObj *tobj;
		HSD_ClassInfo *info;
		if (!td->class_name || !(info = hsdSearchClassInfo(td->class_name))){
			tobj = HSD_TObjAlloc();
		} else {
			tobj = (HSD_TObj*)hsdNew(info);
			assert(tobj);
		}
		HSD_TOBJ_METHOD(tobj)->load(tobj, td);
		return tobj;
	}
	return NULL;
}

//8035EDC0
HSD_Tlut* HSD_TlutLoadDesc(HSD_TlutDesc *tlutdesc){
	if (tlutdesc != NULL)  {
		HSD_Tlut *new = HSD_TlutAlloc();
		memcpy(new, tlutdesc, sizeof(HSD_Tlut));
		return new;
	}
	return NULL;
}

//8035EE14
HSD_TObjTev* HSD_TObjTevLoadDesc(HSD_TObjTevDesc *tevdesc){
	if (tevdesc != NULL)  {
		HSD_TObjTev *new = HSD_TObjTevAlloc();
		memcpy(new, tevdesc, sizeof(HSD_TObjTev));
		return new;
	}
	return NULL;
}

//8035EE68
HSD_TObj* _HSD_TObjGetCurrentByType(HSD_TObj *from, u32 mapping){
	HSD_TObj *tp;
	if (from == NULL) {
		tp = tobj_head;
	} else {
		tp = from->next;
	}
	
	for (; tp; tp=tp->next) {
		if (tobj_coord(tp) == mapping)
			return tp;
	}
	return NULL;
}

//8035EEA4
static u32 HSD_TexMapID2PTTexMtx(u32 id){
	switch (id) {
		case GX_TEXMAP0:
		return 64;
		
		case GX_TEXMAP1:
		return 67;
		
		case GX_TEXMAP2:
		return 70;
		
		case GX_TEXMAP3:
		return 73;
		
		case GX_TEXMAP4:
		return 76;
		
		case GX_TEXMAP5:
		return 79;
		
		case GX_TEXMAP6:
		return 82;
		
		case GX_TEXMAP7:
		return 85;
		
		default:
		assert(0); //HSD_Halt
	}
	return 0;
}

static int current_TexCoordID_index;

#define initCurrentTexCoordID()	(current_TexCoordID_index = 0)
#define getCurrentTexCoord() 	HSD_Index2TexCoord(current_TexCoordID_index++)

//8035EF38
static void MakeTextureMtx(HSD_TObj *tobj){
	guVector scale;
	assert(tobj->repeat_s && tobj->repeat_t);
	{
		Mtx m;
		guVector trans;
		guQuaternion rot;
		
		scale.x = fabsf(tobj->scale.x) < FLT_EPSILON ? 0.0F : (f32)tobj->repeat_s/tobj->scale.x;
		scale.y = fabsf(tobj->scale.y) < FLT_EPSILON ? 0.0F : (f32)tobj->repeat_t/tobj->scale.y;
		scale.z = tobj->scale.z;
		rot.x = tobj->rotate.x;
		rot.y = tobj->rotate.y;
		rot.z = -tobj->rotate.z;
		trans.x = -tobj->translate.x;
		trans.y = -(tobj->translate.y + (tobj->wrap_t == GX_MIRROR ? 1.0F / (tobj->repeat_t / tobj->scale.y) : 0.0F));
		trans.z = tobj->translate.z;
		
		guMtxTrans(tobj->mtx, trans.x, trans.y, trans.z);
		HSD_MkRotationMtx(m, &rot);
		guMtxConcat(m, tobj->mtx, tobj->mtx);
		guMtxScale(m, scale.x, scale.y, scale.z);
		guMtxConcat(m, tobj->mtx, tobj->mtx);
	}
}

//3035F0EC
static void TObjSetupMtx(HSD_TObj *tobj){
	if (tobj_coord(tobj) == TEX_COORD_TOON)
		return;
	
	if (tobj->flags & TEX_MTX_DIRTY) {
		HSD_TOBJ_METHOD(tobj)->make_mtx(tobj);
		tobj->flags &= ~TEX_MTX_DIRTY;
	}
	
	switch (tobj_coord(tobj)) {
		case TEX_COORD_REFLECTION:
		{
			Mtx mtx;
			
			for (u32 i = 0; i < 3; i++) {
				mtx[i][0] =  0.5F * tobj->mtx[i][0];
				mtx[i][1] = -0.5F * tobj->mtx[i][1];
				mtx[i][2] =  0.0F;
				mtx[i][3] =  0.5F * tobj->mtx[i][0] + 0.5F * tobj->mtx[i][1] + tobj->mtx[i][2] + tobj->mtx[i][3];
			}
			GX_LoadTexMtxImm(mtx, tobj->mtxid, GX_MTX3x4);
		}
		break;
		
		case TEX_COORD_HILIGHT:
		{
			HSD_LObj *lobj;
			
			if ((lobj = HSD_LObjGetCurrentByType(LOBJ_INFINITE)) != NULL) {
				HSD_CObj *cobj;
				guVector ldir, half;
				Mtx mtx;
				MtxP vmtx;
				
				cobj = HSD_CObjGetCurrent();
				assert(cobj);
				vmtx = HSD_CObjGetViewingMtxPtrDirect(cobj);
				HSD_LObjGetLightVector(lobj, &ldir);
				guVecMultiplySR(vmtx, &ldir, &ldir);
				ldir.z += -1.0F;
				
				half = ldir; //libogc implements normalize as just returning the result in the ptr, rather than DSDK's (*src, *rslt);
				guVecNormalize(&half);
				
				half.x *= -0.5;
				half.y *= -0.5;
				half.z *= -0.5;
				
				mtx[0][0] = tobj->mtx[0][0] * half.x;
				mtx[0][1] = tobj->mtx[0][0] * half.y;
				mtx[0][2] = tobj->mtx[0][0] * half.z;
				mtx[0][3] = tobj->mtx[0][0] * 0.5F + tobj->mtx[0][3]; 
				mtx[1][0] = tobj->mtx[1][0] * half.x;
				mtx[1][1] = tobj->mtx[1][0] * half.y;
				mtx[1][2] = tobj->mtx[1][0] * half.z;
				mtx[1][3] = tobj->mtx[1][0] * 0.5F + tobj->mtx[1][3];         
				mtx[2][0] = mtx[2][1] = mtx[2][2] = 0.0F;
				mtx[2][3] = 1.0F;
				
				GX_LoadTexMtxImm(mtx, tobj->mtxid, GX_MTX3x4);
			} else {
				static Mtx zero = {
					0.0F, 0.0F, 0.0F, 1.0F,
					0.0F, 0.0F, 0.0F, 1.0F,
					0.0F, 0.0F, 0.0F, 0.0F
				};
				GX_LoadTexMtxImm(zero, tobj->mtxid, GX_MTX3x4);
			}
		}
		break;
		
		case TEX_COORD_SHADOW:
		{
			HSD_CObj *cobj = HSD_CObjGetCurrent();
			Mtx mtx;
			
			guMtxConcat(tobj->mtx, HSD_CObjGetInvViewingMtxPtrDirect(cobj), mtx);
			GX_LoadTexMtxImm(mtx, tobj->mtxid, GX_MTX3x4);
		}
		break;
		
		default:
		if (tobj_bump(tobj)) {
			GX_LoadTexMtxImm(tobj->mtx, tobj->mtxid, GX_MTX2x4);
		} else {
			GX_LoadTexMtxImm(tobj->mtx, tobj->mtxid, GX_MTX3x4);
		}
		break;
	}
}

//8035F418
static void HSD_TObjSetupTextureCoordGen(HSD_TObj *tobj){
	switch (tobj_coord(tobj)) {
		case TEX_COORD_SHADOW:
			GX_SetTexCoordGen2(tobj->coord, GX_TG_MTX3x4, GX_TG_POS, GX_PNMTX0, GX_DISABLE, tobj->mtxid);
			break;
		
		case TEX_COORD_REFLECTION:
		case TEX_COORD_HILIGHT:
			GX_SetTexCoordGen2(tobj->coord, GX_TG_MTX3x4, GX_TG_NRM, GX_TEXMTX0, GX_ENABLE, tobj->mtxid);
			break;
		
		default:
			if (tobj_bump(tobj)) {
				GX_SetTexCoordGen(tobj->coord, GX_TG_MTX2x4, tobj->src, tobj->mtxid);
			} else {
				GX_SetTexCoordGen2(tobj->coord, GX_TG_MTX2x4, tobj->src, GX_IDENTITY, GX_FALSE, tobj->mtxid);
			}
	}
}

//8035F6B4
void HSD_TObjSetupVolatileTev(HSD_TObj *tobj, u32 rendermode){ //TODO: Verify - Very different from decompilation
	#pragma unused(rendermode)
	for (; tobj; tobj=tobj->next) {
		if (tobj->id == GX_TEXMAP_NULL)
			continue;
		if (tobj_lightmap(tobj) & TEX_LIGHTMAP_SHADOW) {
			TObjSetupTevModulateShadow(tobj);
			break;
		}
	}
}

//8035F7D0
static void MakeColorGenTExp(u32 lightmap, HSD_TObj *tobj, HSD_TExp **c, HSD_TExp **a, HSD_TExp **list, int repeat){
	#pragma unused(lightmap)
	#pragma unused(repeat)
	
	HSD_TObjTev *tev = tobj->tev;
	u8 *in;
	HSD_TExp *e0, *tmp;
	
	HSD_TExp *konst_rgb;
	HSD_TExp *konst_r;
	HSD_TExp *konst_g;
	HSD_TExp *konst_b;
	HSD_TExp *konst_a;
	HSD_TExp *reg0_rgb;
	HSD_TExp *reg0_a;
	HSD_TExp *reg1_rgb;
	HSD_TExp *reg1_a;
	
	s32 use_k_rgb = 0;
	s32 use_k_r = 0;
	s32 use_k_g = 0;
	s32 use_k_b = 0;
	s32 use_k_a = 0;
	s32 use_reg0_rgb = 0;
	s32 use_reg0_a = 0;
	s32 use_reg1_rgb = 0;
	s32 use_reg1_a = 0;
	
	in = &tev->color_a;
	for (u32 i = 0; i < 4; i++) {
		switch (in[i]) {
			case TOBJ_TEV_CC_KONST_RGB: use_k_rgb = 1; break;
			case TOBJ_TEV_CC_KONST_RRR: use_k_r = 1; break;
			case TOBJ_TEV_CC_KONST_GGG: use_k_g = 1; break;
			case TOBJ_TEV_CC_KONST_BBB: use_k_b = 1; break;
			case TOBJ_TEV_CC_KONST_AAA: use_k_a = 1; break;
			case TOBJ_TEV_CC_TEX0_RGB:  use_reg0_rgb = 1; break;
			case TOBJ_TEV_CC_TEX0_AAA: use_reg0_a = 1; break;
			case TOBJ_TEV_CC_TEX1_RGB: use_reg1_rgb = 1; break;
			case TOBJ_TEV_CC_TEX1_AAA: use_reg1_a = 1; break;
			default:
			break;
		}
	}
	
	in = &tev->alpha_a;
	for (u32 i = 0; i < 4; i++) {
		switch (in[i]) {
			case TOBJ_TEV_CA_KONST_R: use_k_r = 1; break;
			case TOBJ_TEV_CA_KONST_G: use_k_g = 1; break;
			case TOBJ_TEV_CA_KONST_B: use_k_b = 1; break;
			case TOBJ_TEV_CA_KONST_A: use_k_a = 1; break;
			case TOBJ_TEV_CA_TEX0_A: use_reg0_a = 1; break;
			case TOBJ_TEV_CA_TEX1_A: use_reg1_a = 1; break;
			default: break;
		}
	}
	
	if (use_k_rgb) {
		konst_rgb = HSD_TExpCnst(&tobj->tev->constant, HSD_TE_RGB, HSD_TE_U8, list);
	}
	if (use_k_r) {
		konst_r = HSD_TExpCnst(&tobj->tev->constant.r, HSD_TE_X, HSD_TE_U8, list);
	}
	if (use_k_g) {
		konst_g = HSD_TExpCnst(&tobj->tev->constant.g, HSD_TE_X, HSD_TE_U8, list);
	}
	if (use_k_b) {
		konst_b = HSD_TExpCnst(&tobj->tev->constant.b, HSD_TE_X, HSD_TE_U8, list);
	}
	if (use_k_a) {
		konst_a = HSD_TExpCnst(&tobj->tev->constant.a, HSD_TE_X, HSD_TE_U8, list);
	}
	if (use_reg0_rgb) {
		reg0_rgb = HSD_TExpCnst(&tobj->tev->tev0, HSD_TE_RGB, HSD_TE_U8, list);
	}
	if (use_reg0_a) {
		reg0_a = HSD_TExpCnst(&tobj->tev->tev0.a, HSD_TE_X, HSD_TE_U8, list);
	}
	if (use_reg1_rgb) {
		reg1_rgb = HSD_TExpCnst(&tobj->tev->tev1, HSD_TE_RGB, HSD_TE_U8, list);
	}
	if (use_reg1_a) {
		reg1_a = HSD_TExpCnst(&tobj->tev->tev1.a, HSD_TE_X, HSD_TE_U8, list);
	}
	
	e0 = HSD_TExpTev(list);
	HSD_TExpOrder(e0, tobj, GX_COLORNULL);

  if (tev->active & TOBJ_TEVREG_ACTIVE_COLOR_TEV) {
    HSD_TEInput sel[4];
    HSD_TExp *exp[4];

    in = &tev->color_a;
    for (u32 i = 0; i < 4; i++) {
      switch (in[i]) {
		case GX_CC_ZERO:
		sel[i] = HSD_TE_0;
		exp[i] = HSD_TEXP_ZERO;
		break;
		
		case GX_CC_ONE:
		sel[i] = HSD_TE_1;
		exp[i] = HSD_TEXP_ZERO;
		break;
		
		case GX_CC_HALF:
		sel[i] = HSD_TE_4_8;
		exp[i] = HSD_TEXP_ZERO;
		break;
      
		case GX_CC_TEXC:
		sel[i] = HSD_TE_RGB;
		exp[i] = HSD_TEXP_TEX;
		break;
		
		case GX_CC_TEXA:
		sel[i] = HSD_TE_A;
		exp[i] = HSD_TEXP_TEX;
		break;
		
		case TOBJ_TEV_CC_KONST_RGB:
		sel[i] = HSD_TE_RGB;
		exp[i] = konst_rgb;
		break;
		
		case TOBJ_TEV_CC_KONST_RRR:
		sel[i] = HSD_TE_X;
		exp[i] = konst_r;
		break;
		
		case TOBJ_TEV_CC_KONST_GGG:
		sel[i] = HSD_TE_X;
		exp[i] = konst_g;
		break;
		
		case TOBJ_TEV_CC_KONST_BBB:
		sel[i] = HSD_TE_X;
		exp[i] = konst_b;
		break;
		
		case TOBJ_TEV_CC_KONST_AAA:
		sel[i] = HSD_TE_X;
		exp[i] = konst_a;
		break;
		
		case TOBJ_TEV_CC_TEX0_RGB:
		tmp = HSD_TExpTev(list);
		HSD_TExpOrder(tmp, NULL, GX_COLORNULL);
		HSD_TExpColorOp(tmp, GX_TEV_ADD, GX_TB_ZERO,
			GX_CS_SCALE_1, GX_ENABLE);
		HSD_TExpColorIn(tmp, HSD_TE_0, HSD_TEXP_ZERO,
			HSD_TE_0, HSD_TEXP_ZERO,
			HSD_TE_0, HSD_TEXP_ZERO,
			HSD_TE_RGB, reg0_rgb);
		sel[i] = HSD_TE_RGB;
		exp[i] = tmp;
		break;
		
		case TOBJ_TEV_CC_TEX0_AAA:
		tmp = HSD_TExpTev(list);
		HSD_TExpOrder(tmp, NULL, GX_COLORNULL);
		HSD_TExpColorOp(tmp, GX_TEV_ADD, GX_TB_ZERO,
			GX_CS_SCALE_1, GX_ENABLE);
		HSD_TExpColorIn(tmp, HSD_TE_0, HSD_TEXP_ZERO,
			HSD_TE_0, HSD_TEXP_ZERO,
			HSD_TE_0, HSD_TEXP_ZERO,
			HSD_TE_X, reg0_a);
		sel[i] = HSD_TE_RGB;
		exp[i] = tmp;
		break;
		
		case TOBJ_TEV_CC_TEX1_RGB:
		tmp = HSD_TExpTev(list);
		HSD_TExpOrder(tmp, NULL, GX_COLORNULL);
		HSD_TExpColorOp(tmp, GX_TEV_ADD, GX_TB_ZERO,
			GX_CS_SCALE_1, GX_ENABLE);
		HSD_TExpColorIn(tmp, HSD_TE_0, HSD_TEXP_ZERO,
			HSD_TE_0, HSD_TEXP_ZERO,
			HSD_TE_0, HSD_TEXP_ZERO,
			HSD_TE_RGB, reg1_rgb);
		sel[i] = HSD_TE_RGB;
		exp[i] = tmp;
		break;
		
		case TOBJ_TEV_CC_TEX1_AAA:
		tmp = HSD_TExpTev(list);
		HSD_TExpOrder(tmp, NULL, GX_COLORNULL);
		HSD_TExpColorOp(tmp, GX_TEV_ADD, GX_TB_ZERO,
			GX_CS_SCALE_1, GX_ENABLE);
		HSD_TExpColorIn(tmp, HSD_TE_0, HSD_TEXP_ZERO,
			HSD_TE_0, HSD_TEXP_ZERO,
			HSD_TE_0, HSD_TEXP_ZERO,
			HSD_TE_X, reg1_a);
		sel[i] = HSD_TE_RGB;
		exp[i] = tmp;
		break;
		
		default:
		assert(0);
		break;
		}
    }

    HSD_TExpColorOp(e0, tev->color_op,
		tev->color_bias,
		tev->color_scale,
		tev->color_clamp);
    HSD_TExpColorIn(e0, sel[0], exp[0],
		sel[1], exp[1],
		sel[2], exp[2],
		sel[3], exp[3]);
    *c = e0;
  }
  
	if (tev->active & TOBJ_TEVREG_ACTIVE_ALPHA_TEV) {
		s32 sel[4];
		HSD_TExp *exp[4];

    	in = &tev->alpha_a;
    	for (u32 i = 0; i < 4; i++) {
			switch (in[i]) {
				case GX_CA_ZERO:
				sel[i] = HSD_TE_0;
				exp[i] = HSD_TEXP_ZERO;
				break;
			
				case GX_CA_TEXA:
        		sel[i] = HSD_TE_A;
				exp[i] = HSD_TEXP_TEX;
				break;
    
				case TOBJ_TEV_CA_KONST_R:
				sel[i] = HSD_TE_X;
				exp[i] = konst_r;
				break;

    			case TOBJ_TEV_CA_KONST_G:
				sel[i] = HSD_TE_X;
				exp[i] = konst_g;
				break;
      
	  			case TOBJ_TEV_CA_KONST_B:
				sel[i] = HSD_TE_X;
				exp[i] = konst_b;
				break;
      
				case TOBJ_TEV_CA_KONST_A:
				sel[i] = HSD_TE_X;
				exp[i] = konst_a;
				break;

    			case TOBJ_TEV_CA_TEX0_A:
				tmp = HSD_TExpTev(list);
				HSD_TExpOrder  (tmp, NULL, GX_COLORNULL);
				HSD_TExpAlphaOp(tmp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
				HSD_TExpAlphaIn(tmp, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_X, reg0_a);
				sel[i] = HSD_TE_A;
				exp[i] = tmp;
				break;
      
	  			case TOBJ_TEV_CA_TEX1_A:
				tmp = HSD_TExpTev(list);
				HSD_TExpOrder  (tmp, NULL, GX_COLORNULL);
				HSD_TExpAlphaOp(tmp, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
				HSD_TExpAlphaIn(tmp, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_0, HSD_TEXP_ZERO, HSD_TE_X, reg1_a);
				sel[i] = HSD_TE_A;
				exp[i] = tmp;
				break;
			
				default:
				assert(0);
				break;
			}
		}
		
		HSD_TExpAlphaOp(e0, tev->alpha_op, tev->alpha_bias, tev->alpha_scale, tev->alpha_clamp);
		HSD_TExpAlphaIn(e0, sel[0], exp[0], sel[1], exp[1],	sel[2], exp[2],	sel[3], exp[3]);
		*a = e0;
	}
}

//803600A0
static void TObjMakeTExp(HSD_TObj *tobj, u32 lightmap, u32 lightmap_done, HSD_TExp **c, HSD_TExp **a, HSD_TExp **list){
	HSD_TExp *e0, *e1;
	HSD_TExp *c_src, *a_src;
	s32 c_sel, a_sel;
	s32 repeat = (lightmap_done & tobj_lightmap(tobj)) != 0;

	c_src = HSD_TEXP_TEX;
	c_sel = HSD_TE_RGB;

	a_src = HSD_TEXP_TEX;
	a_sel = HSD_TE_A;

	e0 = HSD_TExpTev(list);

	if (tobj->tev && (tobj->tev->active & (TOBJ_TEVREG_ACTIVE_COLOR_TEV | TOBJ_TEVREG_ACTIVE_ALPHA_TEV))){
		MakeColorGenTExp(lightmap, tobj, &c_src, &a_src, list, repeat);
	}

	HSD_TExpOrder(e0, tobj, GX_COLORNULL);

	switch (tobj_colormap(tobj)) {
		case TEX_COLORMAP_ALPHA_MASK:
		HSD_TExpColorOp(e0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
		HSD_TExpColorIn(e0, HSD_TE_RGB, *c,
			c_sel, c_src,
			a_sel, a_src,
			HSD_TE_0, HSD_TEXP_ZERO);
		break;
		
		case TEX_COLORMAP_RGB_MASK:
    	HSD_TExpColorOp(e0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
    	HSD_TExpColorIn(e0, HSD_TE_RGB, *c,
			c_sel, c_src,
			c_sel, c_src,
			HSD_TE_0, HSD_TEXP_ZERO);
    	break;
  		
		case TEX_COLORMAP_BLEND:
    	e1 = HSD_TExpCnst(&tobj->blending, HSD_TE_X, HSD_TE_F32, list);
    	HSD_TExpColorOp(e0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
    	HSD_TExpColorIn(e0, HSD_TE_RGB, *c,
			c_sel, c_src,
			HSD_TE_X, e1,
			HSD_TE_0, HSD_TEXP_ZERO);
    	break;
		
		case TEX_COLORMAP_MODULATE:
    	HSD_TExpColorOp(e0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
    	HSD_TExpColorIn(e0, HSD_TE_0, HSD_TEXP_ZERO,
			HSD_TE_RGB, *c,
			c_sel, c_src,
			HSD_TE_0, HSD_TEXP_ZERO);
    	break;
  		
		case TEX_COLORMAP_REPLACE:
    	HSD_TExpColorOp(e0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
    	HSD_TExpColorIn(e0, HSD_TE_0, HSD_TEXP_ZERO,
			HSD_TE_0, HSD_TEXP_ZERO,
			HSD_TE_0, HSD_TEXP_ZERO,
			c_sel, c_src);
    	break;
  
  		case TEX_COLORMAP_NONE:
  		case TEX_COLORMAP_PASS:
    	HSD_TExpColorOp(e0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
    	HSD_TExpColorIn(e0, HSD_TE_0, HSD_TEXP_ZERO,
			HSD_TE_0, HSD_TEXP_ZERO,
			HSD_TE_0, HSD_TEXP_ZERO,
			HSD_TE_RGB, *c);
    	break;
  
  		case TEX_COLORMAP_ADD:
    	HSD_TExpColorOp(e0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
    	HSD_TExpColorIn(e0, c_sel, c_src,
			HSD_TE_0, HSD_TEXP_ZERO,
			HSD_TE_0, HSD_TEXP_ZERO,
			HSD_TE_RGB, *c);
    	break;
  
  		case TEX_COLORMAP_SUB:
    	HSD_TExpColorOp(e0, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
    	HSD_TExpColorIn(e0, c_sel, c_src,
			HSD_TE_0, HSD_TEXP_ZERO,
			HSD_TE_0, HSD_TEXP_ZERO,
			HSD_TE_RGB, *c);
    	break;
  		
		default:
   		assert(0);
	}
	*c = e0;

	if (!repeat) {
		switch (tobj_alphamap(tobj)) {
			case TEX_ALPHAMAP_ALPHA_MASK:
			HSD_TExpAlphaOp(e0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
			HSD_TExpAlphaIn(e0, HSD_TE_A, *a,
				a_sel, a_src,
				a_sel, a_src,
				HSD_TE_0, HSD_TEXP_ZERO);
			break;
    		
			case TEX_ALPHAMAP_BLEND:
      		e1 = HSD_TExpCnst(&tobj->blending, HSD_TE_X, HSD_TE_F32, list);
			HSD_TExpAlphaOp(e0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
			HSD_TExpAlphaIn(e0, HSD_TE_A, *a,
				a_sel, a_src,
			  	HSD_TE_X, e1,
			  	HSD_TE_0, HSD_TEXP_ZERO);
      		break;
			  
			case TEX_ALPHAMAP_MODULATE:
			HSD_TExpAlphaOp(e0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
			HSD_TExpAlphaIn(e0, HSD_TE_0, HSD_TEXP_ZERO,
				HSD_TE_A, *a,
			  	a_sel, a_src,
			  	HSD_TE_0, HSD_TEXP_ZERO);
      		break;

    		case TEX_ALPHAMAP_REPLACE:
      		HSD_TExpAlphaOp(e0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
      		HSD_TExpAlphaIn(e0, HSD_TE_0, HSD_TEXP_ZERO,
			  	HSD_TE_0, HSD_TEXP_ZERO,
			  	HSD_TE_0, HSD_TEXP_ZERO,
			  	a_sel, a_src);
      		break;
    		
			case TEX_ALPHAMAP_NONE:
    		case TEX_ALPHAMAP_PASS:
      		HSD_TExpAlphaOp(e0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
      		HSD_TExpAlphaIn(e0, HSD_TE_0, HSD_TEXP_ZERO,
			  	HSD_TE_0, HSD_TEXP_ZERO,
			 	HSD_TE_0, HSD_TEXP_ZERO,
			 	HSD_TE_A, *a);
      		break;
    
			case TEX_ALPHAMAP_ADD:
      		HSD_TExpAlphaOp(e0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
      		HSD_TExpAlphaIn(e0, a_sel, a_src,
			  	HSD_TE_0, HSD_TEXP_ZERO,
			  	HSD_TE_0, HSD_TEXP_ZERO,
			  	HSD_TE_A, *a);
      		break;
    
			case TEX_ALPHAMAP_SUB:
      		HSD_TExpAlphaOp(e0, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE);
      		HSD_TExpAlphaIn(e0, a_sel, a_src,
			  	HSD_TE_0, HSD_TEXP_ZERO,
			  	HSD_TE_0, HSD_TEXP_ZERO,
			  	HSD_TE_A, *a);
      		break;
    		
			default:
      		assert(0);
    	}
    	*a = e0;
	}
}

//803606F0
s32 HSD_TObjAssignResources(HSD_TObj* tobj_top){
	HSD_TObj *tobj;
	u32 texmap_no = 0;
	u32 texcoord_no = 0;
	u32 limit = 8;
	HSD_TObj *bump = NULL;
	HSD_TObj *toon = NULL;

	if (tobj_top == NULL)
    	return 0;

	for (tobj = tobj_top; tobj; tobj = tobj->next) {
		if (tobj_coord(tobj) == TEX_COORD_TOON) {
			toon = tobj;
		} else if (tobj_bump(tobj)) {
			bump = tobj;
		}
	}

	if (toon != NULL)
		limit -= 1;
	if (bump != NULL)
		limit -= 2;

	for (tobj = tobj_top; tobj; tobj = tobj->next) {
    	if (tobj_coord(tobj) == TEX_COORD_TOON) {
      		if (tobj != toon) {
				tobj->id = GX_TEXMAP_NULL;
      		}
    	} else if (tobj_bump(tobj)) {
      		if (tobj != bump) {
				tobj->id = GX_TEXMAP_NULL;
      		}
		} else if (texmap_no < limit) {
			tobj->id = HSD_Index2TexMap(texmap_no ++);
			tobj->mtxid = HSD_TexMapID2PTTexMtx(tobj->id);
			switch (tobj_coord(tobj)) {
				case TEX_COORD_REFLECTION:
				case TEX_COORD_HILIGHT:
				case TEX_COORD_SHADOW:
					tobj->coord = HSD_Index2TexCoord(texcoord_no ++);
				break;
				
				default:
				break;
			}
		}
	}

	for (tobj = tobj_top; tobj; tobj = tobj->next) {
		if (tobj->id != GX_TEXMAP_NULL && !tobj_bump(tobj)) {
			switch (tobj_coord(tobj)) {
				case TEX_COORD_UV:
				tobj->coord = HSD_Index2TexCoord(texcoord_no ++);
				break;
				
				default:
				break;
			}
		}
	}

	if (bump) {
		bump->id = HSD_Index2TexMap(texmap_no ++);
		bump->mtxid = GX_TEXMTX9;
		bump->coord = HSD_Index2TexCoord(texcoord_no);
		texcoord_no += 2;
	}
	
	if (toon) {
		toon->id = HSD_Index2TexMap(texmap_no ++);
		toon->coord = HSD_Index2TexCoord(texcoord_no ++);
	}

  	return (s32) texcoord_no;
}

static int DifferentTluts(HSD_Tlut *t0, HSD_Tlut *t1) {
	return (t0->lut != t0->lut) || (t0->fmt != t1->fmt) || (t0->n_entries != t1->n_entries);
}

//80360950
void HSD_TObjSetup(HSD_TObj *tobj){
	GXTlutObj tlutobj;
	GXTexObj texobj;
	s32 num;
	HSD_Tlut *tluts[8];
	s32 nb_tluts = 0;
	u32 tlut_name = GX_TLUT0;
	u32 big_tlut_name = GX_BIGTLUT0;
	s32 i;

	tobj_head = tobj;

	if (tobj == NULL){
		return;
	}

	num = HSD_TObjAssignResources(tobj);
	if (num > 0){
		HSD_StateRegisterTexGen(HSD_Index2TexCoord(num - 1u));
	}

	for (; tobj; tobj = tobj->next)
	{
		static HSD_TexLODDesc default_lod = {
			GX_LIN_MIP_LIN,
			0.0F,
			GX_FALSE,
			GX_FALSE,
			GX_ANISO_1,
		};
		HSD_TexLODDesc *lod;
		HSD_ImageDesc *imagedesc = tobj->imagedesc;
		u8 min_filter;

		if (tobj->id == GX_TEXMAP_NULL)
			continue;

		TObjSetupMtx(tobj);
		assert(imagedesc);
		assert(imagedesc->img_ptr);

		lod = tobj->lod != NULL ? tobj->lod : &default_lod;
		min_filter = lod->minFilt;

		switch (imagedesc->format){
			case GX_TF_CI4:
			case GX_TF_CI8:
			case GX_TF_CI14:{
				HSD_Tlut *tlut;

				if (tobj->tlut_no != (u8)-1){
					tlut = tobj->tluttbl[tobj->tlut_no];
				}else{
					tlut = tobj->tlut;
				}

				assert(tlut);

				for (i = 0; i < nb_tluts; i++){
					if (!DifferentTluts(tluts[i], tlut)){
						break;
					}
				}

				if (i < nb_tluts){
					tlut->tlut_name = tluts[i]->tlut_name;
				}else if (nb_tluts < 8){
					if (tlut->n_entries > 256){
						tlut->tlut_name = big_tlut_name++;
					}else{
						tlut->tlut_name = tlut_name++;
					}
					GX_InitTlutObj(&tlutobj, tlut->lut, tlut->fmt, tlut->n_entries);
					GX_LoadTlut(&tlutobj, tlut->tlut_name);
					tluts[nb_tluts++] = tlut;
				}else{
					tlut->tlut_name = GX_TLUT0;
				}
				
				GX_InitTexObjCI(&texobj, imagedesc->img_ptr,
						imagedesc->width, imagedesc->height,
						imagedesc->format,
						tobj->wrap_s, tobj->wrap_t,
						imagedesc->mipmap ? GX_TRUE : GX_FALSE,
						tlut->tlut_name);
				
				if (min_filter == GX_LIN_MIP_LIN){
					min_filter = GX_LIN_MIP_NEAR;
				}
			}
			break;

			case GX_TF_I4:
			case GX_TF_I8:
			case GX_TF_IA4:
			case GX_TF_IA8:
			case GX_TF_RGB565:
			case GX_TF_RGB5A3:
			case GX_TF_RGBA8:
			case GX_TF_CMPR:
				GX_InitTexObj(&texobj, imagedesc->img_ptr,
						imagedesc->width, imagedesc->height, imagedesc->format,
						tobj->wrap_s, tobj->wrap_t,
						imagedesc->mipmap ? GX_TRUE : GX_FALSE);
			break;

			default:
				assert(0);
		}

		if (!imagedesc->mipmap)
		{
			min_filter &= 0x01;
		}
		GX_InitTexObjLOD(&texobj, min_filter, tobj->magFilt,
						imagedesc->minLOD, imagedesc->maxLOD, lod->LODBias,
						lod->bias_clamp, lod->edgeLODEnable, lod->max_anisotropy);
	}
}

//80360C38
u32 HSD_TGTex2Index(u32 tgtex){
	switch (tgtex) {
		case GX_TG_TEX0: return 0;
		case GX_TG_TEX1: return 1;
		case GX_TG_TEX2: return 2;
		case GX_TG_TEX3: return 3;
		case GX_TG_TEX4: return 4;
		case GX_TG_TEX5: return 5;
		case GX_TG_TEX6: return 6;
		case GX_TG_TEX7: return 7;
		default: assert(0);
	}
	return GX_TG_TEX0;
}

//80360CCC
u32 HSD_TexCoordID2TexGenSrc(u16 coord){
	switch (coord) {
		case GX_TEXCOORD0: return GX_TG_TEXCOORD0;
		case GX_TEXCOORD1: return GX_TG_TEXCOORD1;
		case GX_TEXCOORD2: return GX_TG_TEXCOORD2;
		case GX_TEXCOORD3: return GX_TG_TEXCOORD3;
		case GX_TEXCOORD4: return GX_TG_TEXCOORD4;
		case GX_TEXCOORD5: return GX_TG_TEXCOORD5;
		case GX_TEXCOORD6: return GX_TG_TEXCOORD6;
		case GX_TEXCOORD7:
		default: 
		assert(0);
	}
	return GX_TG_TEXCOORD0;
}

//80360D54
u32 HSD_TexCoord2Index(u16 coord_id){
	switch (coord_id) {
		case GX_TEXCOORD0: return 0;
		case GX_TEXCOORD1: return 1;
		case GX_TEXCOORD2: return 2;
		case GX_TEXCOORD3: return 3;
		case GX_TEXCOORD4: return 4;
		case GX_TEXCOORD5: return 5;
		case GX_TEXCOORD6: return 6;
		case GX_TEXCOORD7: return 7;
		default: assert(0);
	}
	return GX_TEXCOORD0;
}

//80360DE4
u16 HSD_Index2TexCoord(u32 index){
	switch (index) {
		case 0: return GX_TEXCOORD0;
		case 1: return GX_TEXCOORD1;
		case 2: return GX_TEXCOORD2;
		case 3: return GX_TEXCOORD3;
		case 4: return GX_TEXCOORD4;
		case 5: return GX_TEXCOORD5;
		case 6: return GX_TEXCOORD6;
		case 7: return GX_TEXCOORD7;
		default: assert(0);
	}
	return GX_TEXCOORD0;
}

//80360E74
u32 HSD_TexMtx2Index(u32 texmtx){
	switch (texmtx) {
		case GX_TEXMTX0: return 0;
		case GX_TEXMTX1: return 1;
		case GX_TEXMTX2: return 2;
		case GX_TEXMTX3: return 3;
		case GX_TEXMTX4: return 4;
		case GX_TEXMTX5: return 5;
		case GX_TEXMTX6: return 6;
		case GX_TEXMTX7: return 7;
		case GX_TEXMTX8: return 8;
		case GX_TEXMTX9: return 9;
		case GX_IDENTITY: return 10;
		default: assert(0); //HSD_Halt
	}
	return (u32) -1;
}

//80360F24
u32 HSD_Index2TexMtx(u32 index){
	switch (index) {
		case 0: return GX_TEXMTX0;
		case 1: return GX_TEXMTX1;
		case 2: return GX_TEXMTX2;
		case 3: return GX_TEXMTX3;
		case 4: return GX_TEXMTX4;
		case 5: return GX_TEXMTX5;
		case 6: return GX_TEXMTX6;
		case 7: return GX_TEXMTX7;
		case 8: return GX_TEXMTX8;
		case 9: return GX_TEXMTX9;
		case 10: return GX_IDENTITY;
		default: assert(0); //HSD_Report and HSD_Halt
	}
	return GX_IDENTITY;
}

//80360FE0
u8 HSD_Index2TexMap(u32 index){
	switch (index) {
		case 0: return GX_TEXMAP0;
		case 1: return GX_TEXMAP1;
		case 2: return GX_TEXMAP2;
		case 3: return GX_TEXMAP3;
		case 4: return GX_TEXMAP4;
		case 5: return GX_TEXMAP5;
		case 6: return GX_TEXMAP6;
		case 7: return GX_TEXMAP7;
		default: assert(0);
	}
	return GX_TEXMAP0;
}

//80361070
u32 HSD_TexMap2Index(u8 mapid){
	switch (mapid) {
		case GX_TEXMAP0: return 0;
		case GX_TEXMAP1: return 1;
		case GX_TEXMAP2: return 2;
		case GX_TEXMAP3: return 3;
		case GX_TEXMAP4: return 4;
		case GX_TEXMAP5: return 5;
		case GX_TEXMAP6: return 6;
		case GX_TEXMAP7: return 7;
		default: assert(0);
	}
	return 0;
}

//80361100
void HSD_TObjRemoveAll(HSD_TObj *tobj){
	while (tobj) {
		HSD_TObj *next = tobj->next;
		HSD_CLASS_METHOD(tobj)->release((HSD_Class*)tobj);
		HSD_CLASS_METHOD(tobj)->destroy((HSD_Class*)tobj);
		tobj = next;
	}
}

//80361174
HSD_TObj* HSD_TObjGetNext(HSD_TObj *tobj){
	if (tobj){
		return tobj->next;
	}
	return NULL;
}

void HSD_TObjSetDefaultClass(HSD_TObjInfo *info){
	/*if (info) {
		assert(hsdIsDescendantOf(info, &hsdTObj));
	}*/
	default_class = info;
}

HSD_TObjInfo* HSD_TObjGetDefaultClass(){
	return default_class ? default_class : &hsdTObj;
}

//8036118C
HSD_TObj* HSD_TObjAlloc(){
	HSD_TObj *new = (HSD_TObj*)hsdNew((HSD_ClassInfo*)HSD_TObjGetDefaultClass());
	assert(new);
	return new;
}

//803611E8
void HSD_TObjFree(HSD_TObj *tobj){
	if (tobj){
		HSD_CLASS_METHOD(tobj)->destroy((HSD_Class*)tobj);
	}
}

//8036121C
HSD_Tlut* HSD_TlutAlloc(){
	HSD_Tlut *tlut = hsdAllocMemPiece(sizeof(HSD_Tlut));
	assert(tlut);
	memset(tlut, 0, sizeof(HSD_Tlut));
	return tlut;
}

//80361274
void HSD_TlutFree(HSD_Tlut *tlut){
	if(tlut)
		hsdFreeMemPiece(tlut, sizeof(HSD_Tlut));
}

//803612A0
HSD_TObjTev* HSD_TObjTevAlloc(){
	HSD_TObjTev *tev = hsdAllocMemPiece(sizeof(HSD_TObjTev));
	assert(tev);
	memset(tev, 0, sizeof(HSD_TObjTev));
	return tev;
}

//803612F8
HSD_ImageDesc* HSD_ImageDescAlloc(){
	HSD_ImageDesc *idesc = hsdAllocMemPiece(sizeof(HSD_ImageDesc));
	assert(idesc);
	memset(idesc, 0, sizeof(HSD_ImageDesc));
	return idesc;
}

//80361350
void HSD_ImageDescFree(HSD_ImageDesc *idesc){
	hsdFreeMemPiece(idesc, sizeof(HSD_ImageDesc));
}

//80361374
void HSD_ImageDescCopyFromEFB(HSD_ImageDesc *idesc, u16 origx, u16 origy, u8 clear, s32 sync){
	if (idesc){
		GX_SetTexCopySrc(origx, origy, idesc->width, idesc->height);
		GX_SetTexCopyDst(idesc->width, idesc->height, idesc->format, idesc->mipmap? GX_TRUE : GX_FALSE);
		if (clear) {
			HSD_StateSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
		}
		GX_CopyTex(idesc->img_ptr, clear);
		if (sync) {
			GX_PixModeSync();
			GX_InvalidateTexAll();
		}
	}
}

//8036142C
static void TObjRelease(HSD_Class *o){
	HSD_TObj *tobj = HSD_TOBJ(o);
	
	HSD_AObjRemove(tobj->aobj);
	HSD_TlutFree(tobj->tlut);
	if(tobj->tev != NULL){
		hsdFreeMemPiece(tobj->tev, sizeof(HSD_TObjTev));
	}
	if (tobj->tluttbl) {
		int i;
		for (i=0; tobj->tluttbl[i]; i++) {
			HSD_TlutFree(tobj->tluttbl[i]);
		}
		HSD_Free(tobj->tluttbl);
	}
	HSD_PARENT_INFO(&hsdTObj)->release(o);
}

//803614E8
static void TObjAmnesia(HSD_ClassInfo *info){
	if (info == HSD_CLASS_INFO(default_class)) {
		default_class = NULL;
	}
	if (info == HSD_CLASS_INFO(&hsdTObj)) {
		tobj_head = NULL;
	}
	HSD_PARENT_INFO(&hsdTObj)->amnesia(info);
}

//80361548
static void TObjInfoInit(){
	hsdInitClassInfo(HSD_CLASS_INFO(&hsdTObj), HSD_CLASS_INFO(&hsdClass), HSD_BASE_CLASS_LIBRARY, "hsd_tobj", sizeof(HSD_TObjInfo),	sizeof(HSD_TObj));
	
	HSD_CLASS_INFO(&hsdTObj)->release  = TObjRelease;
	HSD_CLASS_INFO(&hsdTObj)->amnesia  = TObjAmnesia;
	HSD_TOBJ_INFO(&hsdTObj)->load      = TObjLoad;
	HSD_TOBJ_INFO(&hsdTObj)->make_texp = TObjMakeTExp;
	HSD_TOBJ_INFO(&hsdTObj)->make_mtx = MakeTextureMtx;
	HSD_TOBJ_INFO(&hsdTObj)->update   = TObjUpdateFunc;
}
