#include "hsd_lobj.h"

static void LObjInfoInit(void);

HSD_LObjInfo hsdLObj = { LObjInfoInit };

static HSD_LObjInfo *default_class = NULL;

static HSD_SList *current_lights = NULL;
static HSD_LObj  *active_lights[MAX_GXLIGHT];
static s32 nb_active_lights = 0;

static GXLightID lightmask_diffuse  = GX_LIGHT_NULL;
static GXLightID lightmask_specular = GX_LIGHT_NULL;
static GXLightID lightmask_attnfunc = GX_LIGHT_NULL;
static GXLightID lightmask_alpha    = GX_LIGHT_NULL;

//8036539C
u32 HSD_LObjGetFlags(HSD_LObj *lobj){
	return (lobj) ? lobj->flags : 0;
}

//803653B4
void HSD_LObjSetFlags(HSD_LObj *lobj, u32 flags){
	if (lobj) 
		lobj->flags |= flags;
}

//803653CC
void HSD_LObjClearFlags(HSD_LObj *lobj, u32 flags){
	if (lobj) 
		lobj->flags &= ~flags;
}

//803653E4
GXLightID HSD_LObjGetLightMaskDiffuse(){
	return lightmask_diffuse;
}

//803653EC
GXLightID HSD_LObjGetLightMaskAttnFunc(){
	return lightmask_attnfunc;
}

//803653F4
GXLightID HSD_LObjGetLightMaskAlpha(){
	return lightmask_alpha;
}

//803653FC
GXLightID HSD_LObjGetLightMaskSpecular(){
	return lightmask_specular;
}

//80365404
s32 HSD_LObjGetNbActive(){
	return nb_active_lights;
}

//8036540C
HSD_LObj* HSD_LObjGetActiveByID(GXLightID id){
	s32 idx = HSD_LightID2Index(id);
	if (0 <= idx && idx < MAX_GXLIGHT) {
		return active_lights[idx];
	} else {
		return NULL;
	}
}

//80365458
HSD_LObj* HSD_LObjGetActiveByIndex(s32 idx){
	if (0 <= idx && idx < MAX_GXLIGHT - 1) {
		return active_lights[idx];
	} else {
		return NULL;
	}
}

//80365488
static void LObjUpdateFunc(void *obj, u32 type, FObjData *val){
  HSD_LObj *lobj = obj;

  if (lobj == NULL)
    return;

  switch (type) {
  case HSD_A_L_VIS:
    if (val->fv >= 0.5) 
      lobj->flags &= ~LOBJ_HIDDEN;
    else
      lobj->flags |= LOBJ_HIDDEN;
    break;
  case HSD_A_L_A0:
  case HSD_A_L_CUTOFF:
    if (lobj->flags & LOBJ_RAW_PARAM) {
      lobj->u.attn.a0 = val->fv;
    } else {
      lobj->u.spot.cutoff = val->fv;
    }
    break;
  case HSD_A_L_A1:
  case HSD_A_L_REFDIST:
    if (lobj->flags & LOBJ_RAW_PARAM) {
      lobj->u.attn.a1 = val->fv;
    } else {
      lobj->u.spot.ref_dist = val->fv;
    }
    break;
  case HSD_A_L_A2:
  case HSD_A_L_REFBRIGHT:
    if (lobj->flags & LOBJ_RAW_PARAM) {
      lobj->u.attn.a2 = val->fv;
    } else {
      lobj->u.spot.ref_br = val->fv; 
    }
    break;
  case HSD_A_L_K0:
    if (lobj->flags & LOBJ_RAW_PARAM) {
      lobj->u.attn.k0 = val->fv;
    }
    break;
  case HSD_A_L_K1:
    if (lobj->flags & LOBJ_RAW_PARAM) {
      lobj->u.attn.k1 = val->fv;
    }
    break;
  case HSD_A_L_K2:
    if (lobj->flags & LOBJ_RAW_PARAM) {
      lobj->u.attn.k2 = val->fv;
    }
    break;
  case HSD_A_L_LITC_R:
    lobj->color.r = (u8)(255.0F * HSD_ClampFloat(val->fv, 0, 1));
    break;
  case HSD_A_L_LITC_G:
    lobj->color.g = (u8)(255.0F * HSD_ClampFloat(val->fv, 0, 1));
    break;
  case HSD_A_L_LITC_B:
    lobj->color.b = (u8)(255.0F * HSD_ClampFloat(val->fv, 0, 1));
    break;
  case HSD_A_L_LITC_A:
    lobj->color.a = (u8)(255.0F * HSD_ClampFloat(val->fv, 0, 1));
    break;
  }
}

//80365620
void HSD_LObjAnim(HSD_LObj *lobj){
	if (lobj){
		HSD_AObjInterpretAnim(lobj->aobj, lobj, LObjUpdateFunc);
		HSD_WObjInterpretAnim(HSD_LObjGetPositionWObj(lobj));
		HSD_WObjInterpretAnim(HSD_LObjGetInterestWObj(lobj));
	}
}

//80365678
void HSD_LObjAnimAll(HSD_LObj *lobj){
	HSD_LObj *lp;
	
	if (lobj){
		for (lp = lobj; lp; lp = lp->next) {
			HSD_AObjInterpretAnim(lp->aobj, lp, LObjUpdateFunc);
			HSD_WObjInterpretAnim(HSD_LObjGetPositionWObj(lp));
			HSD_WObjInterpretAnim(HSD_LObjGetInterestWObj(lp));
		}
	}
}

//803656F8
void HSD_LObjReqAnimAll(HSD_LObj *lobj, float startframe){
	HSD_LObj *lp;
	
	if (lobj){
		for (lp = lobj; lp; lp = lp->next) {
			HSD_AObjReqAnim(lp->aobj, startframe);
			HSD_WObjReqAnim(HSD_LObjGetPositionWObj(lp), startframe);
			HSD_WObjReqAnim(HSD_LObjGetInterestWObj(lp), startframe);
		}
	}
}

//80365778
void HSD_LObjGetLightVector(HSD_LObj *lobj, VecPtr dir){
	Vec position = {0.0F, 0.0F, 0.0F};
	Vec interest = {0.0F, 0.0F, 0.0F};
	
	if (lobj){
		HSD_LObjGetPosition(lobj, &position);
		HSD_LObjGetInterest(lobj, &interest);
		VECSubtract(&interest, &position, dir);
		if (vec_normalize_check(dir, dir) != 0) {
			dir->x = 0;
			dir->y = 0;
			dir->z = 1;
		}
	}
}

//80365820
void HSD_LObjSetup(HSD_LObj *lobj, GXColor color, f32 shininess){
  if (lobj->flags & LOBJ_HIDDEN || HSD_LObjGetType(lobj) == LOBJ_AMBIENT)
    return;
    
  if (lobj->flags & 4) { //(LOBJ_DIFFUSE|LOBJ_ALPHA)
    if (lobj->hw_color.r != color.r
	|| lobj->hw_color.g != color.g
	|| lobj->hw_color.b != color.b
	|| lobj->hw_color.a != color.a) {
      GXInitLightColor(&lobj->lightobj, color);
      lobj->hw_color = color;
      lobj->flags |= LOBJ_DIFF_DIRTY;
    }

    if (lobj->flags & LOBJ_DIFF_DIRTY) {
      GXLoadLightObjImm(&lobj->lightobj, lobj->id);
      lobj->flags &= ~LOBJ_DIFF_DIRTY;
    }
  }

  if (lobj->spec_id != GX_LIGHT_NULL) {
    if (lobj->shininess != shininess) {
      lobj->shininess = shininess;
      GXInitLightShininess(&lobj->spec_lightobj, shininess);
      lobj->flags |= LOBJ_SPEC_DIRTY;
    }

    if (lobj->flags & LOBJ_SPEC_DIRTY) {
      GXLoadLightObjImm(&lobj->spec_lightobj, lobj->spec_id);
      lobj->flags &= ~LOBJ_SPEC_DIRTY;
    }
  }
}

//8036597C
void HSD_LObjSetupSpecularInit(MtxPtr pmtx){
	int i, num;
	Vec cdir, jpos;
	
	HSD_MtxColVec(pmtx, 3, &jpos);
	if (vec_normalize_check(&jpos, &cdir) != 0) {
		cdir.x = 0.0F;
		cdir.y = 0.0F;
		cdir.z = -1.0F;
	}
	
	num = HSD_LObjGetNbActive();
	for (i = 0; i < num; i++) {
		Vec half, ldir;	
		HSD_LObj *lobj = HSD_LObjGetActiveByIndex(i);
		
		if (lobj->spec_id == GX_LIGHT_NULL) {
			continue;
		}
		
		switch (HSD_LObjGetType(lobj)) {
			case LOBJ_POINT:
			case LOBJ_SPOT:
			VECSubtract(&jpos, &lobj->lvec, &ldir);
			VECNormalize(&ldir, &ldir);
			VECAdd(&ldir, &cdir, &half);
			break;
			
			case LOBJ_INFINITE:
			VECAdd(&lobj->lvec, &cdir, &half);
			break;
			
			default:
			assert(0);
		}
		VECNormalize(&half, &half);
		GXInitLightDirv(&lobj->spec_lightobj, &half);
		lobj->flags |= LOBJ_SPEC_DIRTY;
	}
}

static void setup_diffuse_lightobj(HSD_LObj *lobj){
	GXInitLightColor(&lobj->lightobj, lobj->color);
	lobj->hw_color = lobj->color;
	lobj->flags |= LOBJ_DIFF_DIRTY;
	
	switch (HSD_LObjGetType(lobj)) {
		case LOBJ_SPOT:
		case LOBJ_POINT:
		lightmask_attnfunc |= lobj->id;
		break;
		
		case LOBJ_INFINITE:
		break;

		default:
		assert(0);
	}
	
	if (lobj->flags & LOBJ_DIFFUSE) {
		lightmask_diffuse |= lobj->id;
	}
	if (lobj->flags & LOBJ_ALPHA) {
		lightmask_alpha |= lobj->id;
	}
}

//80365AC4
static void setup_spec_lightobj(HSD_LObj *lobj, Mtx vmtx, GXLightID spec_id){
	lobj->spec_id = spec_id;
	if (spec_id != GX_LIGHT_NULL) {
		GXInitLightColor(&lobj->spec_lightobj, lobj->color);
		lobj->shininess = 50.0F;
		GXInitLightShininess(&lobj->spec_lightobj, lobj->shininess);
		
		switch (HSD_LObjGetType(lobj)) {
			case LOBJ_POINT:
			case LOBJ_SPOT:
			HSD_LObjGetPosition(lobj, &lobj->lvec);
			MTXMultVec(vmtx, &lobj->lvec, &lobj->lvec);
			break;
			
			case LOBJ_INFINITE:
			HSD_LObjGetLightVector(lobj, &lobj->lvec);
			MTXMultVecSR(vmtx, &lobj->lvec, &lobj->lvec);
			VECNormalize(&lobj->lvec, &lobj->lvec);
			break;
			
			default:
			assert(0);
		}
		
		lobj->flags |= LOBJ_SPEC_DIRTY;
		lightmask_specular |= spec_id;
	}
}

//80365C50
static void setup_point_lightobj(HSD_LObj *l, MtxPtr vmtx){
	Vec lpos;
	
	GXInitLightColor(&l->lightobj, l->color);
	l->hw_color = l->color;
	HSD_LObjGetPosition(l, &lpos);
	MTXMultVec(vmtx, &lpos, &lpos);
	GXInitLightPosv(&l->lightobj, &lpos);
	GXInitLightPosv(&l->spec_lightobj, &lpos);
	
	if (l->flags & LOBJ_RAW_PARAM) {
		GXInitLightAttn(&l->lightobj,
			1.0F, 0.0F, 0.0F,
			l->u.attn.k0,
		    l->u.attn.k1,
		    l->u.attn.k2);
	} else {
		f32 ref_br = l->u.spot.ref_br;
		f32 ref_dist = l->u.spot.ref_dist;
		GXDistAttnFn dist_func = l->u.spot.dist_func;
		
		if (ref_dist < FLT_EPSILON) {
			ref_dist = FLT_EPSILON;
		}
		if (ref_br < FLT_EPSILON) {
			ref_br = FLT_EPSILON;
		}
		
		GXInitLightDistAttn(&l->lightobj, ref_dist, ref_br, dist_func);
		GXInitLightSpot(&l->lightobj, 0.0F, GX_SP_OFF);
		GXInitLightDistAttn(&l->spec_lightobj, ref_dist, ref_br, dist_func);
	}
}

//80365D6C
static void setup_spot_lightobj(HSD_LObj *l, MtxPtr vmtx){
  Vec lpos, ldir;

  HSD_LObjGetPosition(l, &lpos);
  MTXMultVec(vmtx, &lpos, &lpos);
  HSD_LObjGetLightVector(l, &ldir);
  MTXMultVecSR(vmtx, &ldir, &ldir);
  VECNormalize(&ldir, &ldir);
  GXInitLightPosv(&l->lightobj, &lpos);
  GXInitLightPosv(&l->spec_lightobj, &lpos);
  GXInitLightDirv(&l->lightobj, &ldir);
  if (l->flags & LOBJ_RAW_PARAM) {
    GXInitLightAttn(&l->lightobj,
		    l->u.attn.a0,
		    l->u.attn.a1,
		    l->u.attn.a2,
		    l->u.attn.k0,
		    l->u.attn.k1,
		    l->u.attn.k2);
  } else {
    float ref_br = l->u.spot.ref_br;
    float ref_dist = l->u.spot.ref_dist;
    float cutoff = l->u.spot.cutoff;
    GXSpotFn spot_func = l->u.spot.spot_func;
    GXDistAttnFn dist_func = l->u.spot.dist_func;
    
    if (ref_dist < FLT_EPSILON) {
      ref_dist = FLT_EPSILON;
    }
    if (ref_br < FLT_EPSILON) {
      ref_br = FLT_EPSILON;
    }
    GXInitLightDistAttn(&l->lightobj, ref_dist, ref_br, dist_func);
    GXInitLightDistAttn(&l->spec_lightobj, ref_dist, ref_br, dist_func);
    GXInitLightSpot(&l->lightobj, cutoff, spot_func);
  }
}

//80365F28
void HSD_LObjSetupInit(HSD_CObj *cobj){
  MtxPtr vmtx;
  int i, num, idx;
  HSD_SList *list;

  lightmask_diffuse  = GX_LIGHT_NULL;
  lightmask_specular = GX_LIGHT_NULL;
  lightmask_attnfunc = GX_LIGHT_NULL;
  lightmask_alpha    = GX_LIGHT_NULL;
  vmtx = HSD_CObjGetViewingMtxPtrDirect(cobj);

  idx = 0;

  HSD_LObjClearActive();

  for (list = current_lights; idx < MAX_GXLIGHT-1 && list; list = list->next) {
    HSD_LObj *lobj = list->data;
    u32 type;
    u32 ref_type;

    if (!lobj || lobj->flags & LOBJ_HIDDEN) {
      continue;
    }

    type = HSD_LObjGetType(lobj);

    ref_type = lobj->flags & (LOBJ_DIFFUSE|LOBJ_ALPHA|LOBJ_SPECULAR);

    if (!ref_type) {
      continue;
    }

    HSD_LObjSetActive(lobj);
    idx = HSD_LObjGetNbActive();

    lobj->spec_id = GX_LIGHT_NULL;

    switch (type) {
    case LOBJ_INFINITE:
      setup_infinite_lightobj(lobj, vmtx);
      break;
    case LOBJ_POINT:
      setup_point_lightobj(lobj, vmtx);
      break;
    case LOBJ_SPOT:
      setup_spot_lightobj(lobj, vmtx);
      break;
    case LOBJ_AMBIENT:
      continue;
    }

    if (ref_type & (LOBJ_DIFFUSE|LOBJ_ALPHA)) {
      setup_diffuse_lightobj(lobj);
    } else if (ref_type & LOBJ_SPECULAR) {
      setup_spec_lightobj(lobj, vmtx, lobj->id);
    }
  }

  if (!HSD_LObjGetActiveByID(GX_LIGHT_AMBIENT)) {
    for (; list; list=list->next) {
      HSD_LObj *lobj = list->data;
      if (!lobj || lobj->flags & LOBJ_HIDDEN) {
		  continue;
      }
      if (HSD_LObjGetType(lobj) == LOBJ_AMBIENT && (lobj->flags & (LOBJ_DIFFUSE|LOBJ_ALPHA)))
      {
		  HSD_LObjSetActive(lobj);
		  break;
      }
    }
  }
  
  num = HSD_LObjGetNbActive();
  for (i=0; idx<MAX_GXLIGHT-1 && i<num; i++) {
    HSD_LObj *lobj;
    u32 flags;
    
    if (!(lobj = HSD_LObjGetActiveByIndex(i))) {
      continue;
    }

    flags = lobj->flags;

    if ((flags & LOBJ_SPECULAR) && (flags & (LOBJ_DIFFUSE|LOBJ_ALPHA))) {
      setup_spec_lightobj(lobj, vmtx, HSD_Index2LightID(idx++));
    }
  }

  for (i=0; i<num; i++) {
    HSD_LObj *lobj;
    
    if ((lobj = HSD_LObjGetActiveByIndex(i))) {
      HSD_LObjSetup(lobj, lobj->color, lobj->shininess);
    }
  }
}

//803663B4
void HSD_LObjAddCurrent(HSD_LObj *lobj){
	HSD_SList *list, **listp;
	
	if (lobj){
		for (list = current_lights; list; list = list->next) {
			if (list->data == lobj) {
				HSD_LObjDeleteCurrent(lobj);
				break;
			}
		}
		
		HSD_LObjRef(lobj); //lobj->obj.ref_count += 1; assert(lobj->obj.ref_count != 0xFFFF);
		for (listp = &current_lights; *listp; listp = &(*listp)->next) {
			if (HSD_LObjGetPriority((*listp)->data) > HSD_LObjGetPriority(lobj)) {
				break;
			}
		}
		*listp = HSD_SListPrepend(*listp, lobj);
	}
}