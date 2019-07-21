#include "hsd_lobj.h"

#include "hsd_jobj.h"

static void LObjInfoInit(void);

HSD_LObjInfo hsdLObj = { LObjInfoInit };

static HSD_LObjInfo *default_class = NULL;

static HSD_SList *current_lights = NULL;
static HSD_LObj  *active_lights[MAX_GXLIGHT];
static s32 nb_active_lights = 0;

static u8 lightmask_diffuse  = GX_LIGHTNULL;
static u8 lightmask_specular = GX_LIGHTNULL;
static u8 lightmask_attnfunc = GX_LIGHTNULL;
static u8 lightmask_alpha    = GX_LIGHTNULL;

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
u8 HSD_LObjGetLightMaskDiffuse(){
	return lightmask_diffuse;
}

//803653EC
u8 HSD_LObjGetLightMaskAttnFunc(){
	return lightmask_attnfunc;
}

//803653F4
u8 HSD_LObjGetLightMaskAlpha(){
	return lightmask_alpha;
}

//803653FC
u8 HSD_LObjGetLightMaskSpecular(){
	return lightmask_specular;
}

void HSD_LObjSetActive(HSD_LObj *lobj){
	s32 idx;
  if (HSD_LObjGetType(lobj) == LOBJ_AMBIENT) {
    idx = MAX_GXLIGHT - 1;
    if (active_lights[idx]){
      return;
		}
	} else {
			idx = nb_active_lights ++;
  }
  active_lights[idx] = lobj;
  lobj->id = idx;
}

//80365404
s32 HSD_LObjGetNbActive(){
	return nb_active_lights;
}

//8036540C - Modified due to Libogc
HSD_LObj* HSD_LObjGetActiveByID(u32 idx){
	if(idx == GX_MAXLIGHT){
		idx = MAX_GXLIGHT - 1;
	}
	if (0 <= idx && idx < MAX_GXLIGHT) {
		return active_lights[idx];
	} else {
		return NULL;
	}
}

//80365458
HSD_LObj* HSD_LObjGetActiveByIndex(u8 idx){
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
void HSD_LObjReqAnimAll(HSD_LObj *lobj, f32 startframe){
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
void HSD_LObjGetLightVector(HSD_LObj *lobj, guVector* dir){
	guVector position = {0.0F, 0.0F, 0.0F};
	guVector interest = {0.0F, 0.0F, 0.0F};
	
	if (lobj){
		HSD_LObjGetPosition(lobj, &position);
		HSD_LObjGetInterest(lobj, &interest);
		guVecSub(&interest, &position, dir);
		guVecNormalize(dir);
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
      GX_InitLightColor(lobj->lightobj, color);
      lobj->hw_color = color;
      lobj->flags |= LOBJ_DIFF_DIRTY;
    }

    if (lobj->flags & LOBJ_DIFF_DIRTY) {
      GX_LoadLightObj(lobj->lightobj, lobj->id);
      lobj->flags &= ~LOBJ_DIFF_DIRTY;
    }
  }

  if (lobj->spec_id != GX_LIGHTNULL) {
    if (lobj->shininess != shininess) {
      lobj->shininess = shininess;
      GX_InitLightShininess(lobj->spec_lightobj, shininess);
      lobj->flags |= LOBJ_SPEC_DIRTY;
    }

    if (lobj->flags & LOBJ_SPEC_DIRTY) {
      GX_LoadLightObj(lobj->spec_lightobj, lobj->spec_id);
      lobj->flags &= ~LOBJ_SPEC_DIRTY;
    }
  }
}

//8036597C
void HSD_LObjSetupSpecularInit(MtxP pmtx){
	int i, num;
	guVector cdir;
	guVector jpos = {pmtx[0][3], pmtx[1][3], pmtx[2][3]};
	cdir = jpos;
	guVecNormalize(&cdir);
	
	num = HSD_LObjGetNbActive();
	for (i = 0; i < num; i++) {
		guVector half, ldir;	
		HSD_LObj *lobj = HSD_LObjGetActiveByIndex(i);
		
		if (lobj->spec_id == GX_LIGHTNULL) {
			continue;
		}
		
		switch (HSD_LObjGetType(lobj)) {
			case LOBJ_POINT:
			case LOBJ_SPOT:
			guVecSub(&jpos, &lobj->lvec, &ldir);
			guVecNormalize(&ldir);
			guVecAdd(&ldir, &cdir, &half);
			break;
			
			case LOBJ_INFINITE:
			guVecAdd(&lobj->lvec, &cdir, &half);
			break;
			
			default:
			assert(0);
		}
		guVecNormalize(&half);
		GX_InitLightDirv(lobj->spec_lightobj, &half);
		lobj->flags |= LOBJ_SPEC_DIRTY;
	}
}

static void setup_diffuse_lightobj(HSD_LObj *lobj){
	GX_InitLightColor(lobj->lightobj, lobj->color);
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
static void setup_spec_lightobj(HSD_LObj *lobj, Mtx vmtx, u8 spec_id){
	lobj->spec_id = spec_id;
	if (spec_id != GX_LIGHTNULL) {
		GX_InitLightColor(lobj->spec_lightobj, lobj->color);
		lobj->shininess = 50.0F;
		GX_InitLightShininess(lobj->spec_lightobj, lobj->shininess);
		
		switch (HSD_LObjGetType(lobj)) {
			case LOBJ_POINT:
			case LOBJ_SPOT:
			HSD_LObjGetPosition(lobj, &lobj->lvec);
			guVecMultiply(vmtx, &lobj->lvec, &lobj->lvec);
			break;
			
			case LOBJ_INFINITE:
			HSD_LObjGetLightVector(lobj, &lobj->lvec);
			guVecMultiplySR(vmtx, &lobj->lvec, &lobj->lvec);
			guVecNormalize(&lobj->lvec);
			break;
			
			default:
			assert(0);
		}
		
		lobj->flags |= LOBJ_SPEC_DIRTY;
		lightmask_specular |= spec_id;
	}
}

//80365C50
static void setup_point_lightobj(HSD_LObj *l, MtxP vmtx){
	guVector lpos;
	
	GX_InitLightColor(l->lightobj, l->color);
	l->hw_color = l->color;
	HSD_LObjGetPosition(l, &lpos);
	guVecMultiply(vmtx, &lpos, &lpos);
	GX_InitLightPosv(l->lightobj, &lpos);
	GX_InitLightPosv(l->spec_lightobj, &lpos);
	
	if (l->flags & LOBJ_RAW_PARAM) {
		GX_InitLightAttn(l->lightobj,
			1.0F, 0.0F, 0.0F,
			l->u.attn.k0,
		    l->u.attn.k1,
		    l->u.attn.k2);
	} else {
		f32 ref_br = l->u.spot.ref_br;
		f32 ref_dist = l->u.spot.ref_dist;
		u8 dist_func = l->u.spot.dist_func;
		
		if (ref_dist < FLT_EPSILON) {
			ref_dist = FLT_EPSILON;
		}
		if (ref_br < FLT_EPSILON) {
			ref_br = FLT_EPSILON;
		}
		
		GX_InitLightDistAttn(l->lightobj, ref_dist, ref_br, dist_func);
		GX_InitLightSpot(l->lightobj, 0.0F, GX_SP_OFF);
		GX_InitLightDistAttn(l->spec_lightobj, ref_dist, ref_br, dist_func);
	}
}

//80365D6C
static void setup_spot_lightobj(HSD_LObj *l, MtxP vmtx){
	guVector lpos, ldir;
	
	HSD_LObjGetPosition(l, &lpos);
	guVecMultiply(vmtx, &lpos, &lpos);
	HSD_LObjGetLightVector(l, &ldir);
	guVecMultiplySR(vmtx, &ldir, &ldir);
	guVecNormalize(&ldir);
	GX_InitLightPosv(l->lightobj, &lpos);
	GX_InitLightPosv(l->spec_lightobj, &lpos);
	GX_InitLightDirv(l->lightobj, &ldir);
	if (l->flags & LOBJ_RAW_PARAM) {
		GX_InitLightAttn(l->lightobj,
		    l->u.attn.a0,
		    l->u.attn.a1,
		    l->u.attn.a2,
		    l->u.attn.k0,
		    l->u.attn.k1,
		    l->u.attn.k2);
	} else {
		f32 ref_br = l->u.spot.ref_br;
		f32 ref_dist = l->u.spot.ref_dist;
		f32 cutoff = l->u.spot.cutoff;
		u8 spot_func = l->u.spot.spot_func;
		u8 dist_func = l->u.spot.dist_func;
		
		if (ref_dist < FLT_EPSILON) {
			ref_dist = FLT_EPSILON;
		}
		if (ref_br < FLT_EPSILON) {
			ref_br = FLT_EPSILON;
		}
		
		GX_InitLightDistAttn(l->lightobj, ref_dist, ref_br, dist_func);
		GX_InitLightDistAttn(l->spec_lightobj, ref_dist, ref_br, dist_func);
		GX_InitLightSpot(l->lightobj, cutoff, spot_func);
	}
}

//80365F28
void HSD_LObjSetupInit(HSD_CObj *cobj){
  MtxP vmtx;
  u32 i, num, idx;
  HSD_SList *list;

  lightmask_diffuse  = GX_LIGHTNULL;
  lightmask_specular = GX_LIGHTNULL;
  lightmask_attnfunc = GX_LIGHTNULL;
  lightmask_alpha    = GX_LIGHTNULL;
  vmtx = (MtxP)HSD_CObjGetViewingMtxPtrDirect(cobj);

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

    lobj->spec_id = GX_LIGHTNULL;

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

  if (!HSD_LObjGetActiveByID(GX_MAXLIGHT)) {
    for (; list; list = list->next) {
      HSD_LObj *lobj = list->data;
      if (!lobj || lobj->flags & LOBJ_HIDDEN) {
				continue;
      }
      if (HSD_LObjGetType(lobj) == LOBJ_AMBIENT && (lobj->flags & (LOBJ_DIFFUSE|LOBJ_ALPHA))){
				HSD_LObjSetActive(lobj);
				break;
      }
    }
  }
  
  num = HSD_LObjGetNbActive();
  for (i = 0; idx < MAX_GXLIGHT - 1 && i < num; i++) {
    HSD_LObj *lobj;
    u32 flags;
    
    if (!(lobj = HSD_LObjGetActiveByIndex(i))) {
      continue;
    }

    flags = lobj->flags;

    if ((flags & LOBJ_SPECULAR) && (flags & (LOBJ_DIFFUSE|LOBJ_ALPHA))) {
      setup_spec_lightobj(lobj, vmtx, idx++);
    }
  }

  for (i = 0; i < num; i++) {
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
		
		lobj->parent.ref_count += 1; 
		assert(lobj->parent.ref_count != HSD_OBJ_NOREF);
		for (listp = &current_lights; *listp; listp = &(*listp)->next) {
			if (((HSD_LObj*)((*listp)->data))->priority > lobj->priority) {
				break;
			}
		}
		*listp = (HSD_SList*)HSD_SListPrepend(*listp, lobj);
	}
}

//803664D4
void HSD_LObjDeleteCurrent(HSD_LObj *lobj){
	HSD_SList **p;
	int i;
	
	if (!lobj)
		return;
	
	for (p = &current_lights; *p; p = &(*p)->next) {
		if ((*p)->data == lobj) {
			for (i = 0; i < MAX_GXLIGHT; i++) {
				if (lobj == active_lights[i]) {
					active_lights[i] = NULL;
				}
			}
			(*p) = (HSD_SList*)HSD_SListRemove(*p);
			HSD_LObjUnref(lobj);
			return;
		}
	}
}

//80366654
void HSD_LObjDeleteCurrentAll(HSD_LObj *lobj){
	if (lobj) {
		for (; lobj; lobj=lobj->next) {
			HSD_LObjDeleteCurrent(lobj);
		}
	} else {
		HSD_LObjClearActive();
		while (current_lights) {
			HSD_LObjUnref((HSD_LObj *)(current_lights->data));
			current_lights = (HSD_SList*)HSD_SListRemove(current_lights);
		}
	}
}

//803667A8
void HSD_LObjSetCurrent(HSD_LObj *lobj){
	HSD_LObjDeleteCurrentAll(NULL);
	HSD_LObjAddCurrent(lobj);
}

//803668EC
void HSD_LObjSetCurrentAll(HSD_LObj *lobj){
	HSD_LObjDeleteCurrentAll(NULL);
	
	HSD_LObj *l;
	for(l = lobj; l; l = l->next) {
		HSD_LObjAddCurrent(l);
	}
}

//80366A44
HSD_LObj* HSD_LObjGetCurrentByType(u32 type){
	HSD_SList *list;
	
	type &= LOBJ_TYPE_MASK;
	for (list = current_lights; list; list=list->next) {
		HSD_LObj* lobj = (HSD_LObj*)list->data;
		if (HSD_LObjGetType(lobj) == type) {
			return list->data;
		}
	}
	return NULL;
}

//80366A78 - Unused due to Libogc in our version
/*u32 HSD_LightID2Index(u8 id){
	u32 index;
	switch (id) {
		case GX_LIGHT0: index = 0; break;
		case GX_LIGHT1: index = 1; break;
		case GX_LIGHT2: index = 2; break;
		case GX_LIGHT3: index = 3; break;
		case GX_LIGHT4: index = 4; break;
		case GX_LIGHT5: index = 5; break;
		case GX_LIGHT6: index = 6; break;
		case GX_LIGHT7: index = 7; break;
		case GX_MAXLIGHT: index = MAX_GXLIGHT - 1; break;
		default: assert(0);
	}
	return index;
}*/

//80366B64 - Unused due to Libogc in our version
/*u8 HSD_Index2LightID(u32 index){
	u8 id;
	switch (index) {
		case 0: id = GX_LIGHT0; break;
		case 1: id = GX_LIGHT1; break;
		case 2: id = GX_LIGHT2; break;
		case 3: id = GX_LIGHT3; break;
		case 4: id = GX_LIGHT4; break;
		case 5: id = GX_LIGHT5; break;
		case 6: id = GX_LIGHT6; break;
		case 7: id = GX_LIGHT7; break;
		case MAX_GXLIGHT-1: id = GX_MAXLIGHT; break;
		default: id = GX_LIGHTNULL;
	}
	return id;
}*/

//80366BD4
void HSD_LObjRemoveAll(HSD_LObj *lobj){
	while (lobj != NULL) {
		HSD_LObj *next = lobj->next;
		HSD_LObjDeleteCurrent(lobj);
		HSD_LObjUnref(lobj);
		lobj = next;
	}
}

//80366CA4
void HSD_LObjSetColor(HSD_LObj *lobj, GXColor color){
	lobj->color = color;
}

//80366CB0
void HSD_LObjGetColor(HSD_LObj *lobj, GXColor *color){
	*color = lobj->color;
}

//80366CBC
void HSD_LObjSetSpot(HSD_LObj *lobj, f32 cutoff, u8 spot_func){
	if (!lobj) 
		return;
	
	lobj->u.spot.cutoff = cutoff;
	lobj->u.spot.spot_func = spot_func;
}

//80366CD0
void HSD_LObjSetDistAttn(HSD_LObj *lobj, f32 ref_distance, f32 ref_brightness, u8 dist_func){
	if(!lobj)
		return;
	
	lobj->u.spot.ref_dist  = ref_distance;
	lobj->u.spot.ref_br    = ref_brightness;
	lobj->u.spot.dist_func = dist_func;
}

//80366CE8
void HSD_LObjSetPosition(HSD_LObj *lobj, guVector* position){
	assert(lobj);
	if (!lobj->position) {
		lobj->position = HSD_WObjAlloc();
		assert(lobj->position);
	}
	HSD_WObjSetPosition(lobj->position, position);
}

//80366D70
BOOL HSD_LObjGetPosition(HSD_LObj *lobj, guVector* position){
	if (lobj && lobj->position) {
		HSD_WObjGetPosition(lobj->position, position);
		return TRUE;
	}
	return FALSE;
}

//80366DB0
void HSD_LObjSetInterest(HSD_LObj *lobj, guVector* interest){
	assert(lobj);
	if(!lobj->interest){
		lobj->interest = HSD_WObjAlloc();
		assert(lobj->interest);
	}
	HSD_WObjSetPosition(lobj->interest, interest);
}

//80366E38
BOOL HSD_LObjGetInterest(HSD_LObj *lobj, guVector* interest){
	if (lobj && lobj->interest) {
		HSD_WObjGetPosition(lobj->interest, interest);
		return TRUE;
	}
	return FALSE;
}

//80366E78
HSD_WObj* HSD_LObjGetPositionWObj(HSD_LObj *lobj){
	return (lobj) ? lobj->position : NULL;
}

//80366E90
HSD_WObj* HSD_LObjGetInterestWObj(HSD_LObj *lobj){
	return (lobj) ? lobj->interest : NULL;
}

//80366EA8
static int LObjLoad(HSD_LObj *lobj, HSD_LightDesc *ldesc){
	HSD_LObjSetColor(lobj, ldesc->color);
	HSD_LObjSetFlags(lobj, ldesc->flags);
	
	switch (ldesc->flags & LOBJ_TYPE_MASK) {
		case GX_MAXLIGHT:
		break;
		
		case LOBJ_INFINITE:
		if(lobj){
			HSD_WObjUnref(lobj->position);
			lobj->position = HSD_WObjLoadDesc(ldesc->position);
		}
		break;

		case LOBJ_POINT:
		if(lobj){
			HSD_WObjUnref(lobj->position);
			lobj->position = HSD_WObjLoadDesc(ldesc->position);
		}
		if (ldesc->attnflags & LOBJ_LIGHT_ATTN) {
			HSD_LObjSetFlags(lobj, LOBJ_RAW_PARAM);
			if(lobj){
				lobj->u.attn.k0 = ldesc->u.attn->k0;
				lobj->u.attn.k1 = ldesc->u.attn->k1;
				lobj->u.attn.k2 = ldesc->u.attn->k2;
			}
		} else {
			if(lobj){
				lobj->u.spot.ref_dist  = ldesc->u.point->ref_dist;
				lobj->u.spot.ref_br    = ldesc->u.point->ref_br;
				lobj->u.spot.dist_func = ldesc->u.point->dist_func;
			}
		}
		break;
		
		case LOBJ_SPOT:
		if(lobj){
			HSD_WObjUnref(lobj->position);
			lobj->position = HSD_WObjLoadDesc(ldesc->position);
			
			HSD_WObjUnref(lobj->interest);
			lobj->interest = HSD_WObjLoadDesc(ldesc->interest);
		}
		if (ldesc->attnflags & LOBJ_LIGHT_ATTN) {
			HSD_LObjSetFlags(lobj, LOBJ_RAW_PARAM);
			if(lobj){
				lobj->u.attn.a0 = ldesc->u.attn->a0;
				lobj->u.attn.a1 = ldesc->u.attn->a1;
				lobj->u.attn.a2 = ldesc->u.attn->a2;
				lobj->u.attn.k0 = ldesc->u.attn->k0;
				lobj->u.attn.k1 = ldesc->u.attn->k1;
				lobj->u.attn.k2 = ldesc->u.attn->k2;
			}
		} else {
			if(lobj){
				lobj->u.spot.ref_dist  = ldesc->u.spot->ref_dist;
				lobj->u.spot.ref_br    = ldesc->u.spot->ref_br;
				lobj->u.spot.dist_func = ldesc->u.spot->dist_func;
				
				lobj->u.spot.cutoff = ldesc->u.spot->cutoff;
				lobj->u.spot.spot_func = ldesc->u.spot->spot_func;
			}
		}
		break;
		
		default:
		HSD_Halt("Unexpected lightdesc flags\n");
	}
	return 0;
}

//803672DC
HSD_LObj* HSD_LObjLoadDesc(HSD_LightDesc *ldesc){
	HSD_LObj *top, **p = &top;
	for (; ldesc; ldesc = ldesc->next) {
		HSD_ClassInfo *info;
		if (!ldesc->class_name || !(info = (HSD_ClassInfo*)hsdSearchClassInfo(ldesc->class_name))){
			info = (HSD_ClassInfo*)&hsdLObj;
			if(default_class != NULL){
				info = (HSD_ClassInfo*)default_class;
			}
			*p = (HSD_LObj*)hsdNew(info);
		} else {
			*p = (HSD_LObj*)hsdNew(info);
		}
		assert(*p);
		HSD_LOBJ_INFO((*p)->parent.parent.class_info)->load(*p, ldesc);
		p = &(*p)->next;
	}
	*p = NULL;
	return top;
}

//803673D8
void HSD_LObjAddAnim(HSD_LObj *lobj, HSD_LightAnim *lanim){
	HSD_LObj *lp;
	HSD_LightAnim *la;
	
	for (lp = lobj, la = lanim; lp; lp = lp->next, la = la->next) {
		if (lp){	
			if (la) {
				if (lp->aobj) {
					HSD_AObjRemove(lp->aobj);
				}
				lp->aobj = HSD_AObjLoadDesc(la->aobjdesc);
				HSD_WObjAddAnim(HSD_LObjGetPositionWObj(lp), la->position_anim);
				HSD_WObjAddAnim(HSD_LObjGetInterestWObj(lp), la->interest_anim);
			}
		}
	}
}

//803674B4
static void LObjRelease(HSD_Class *o){
	HSD_LObj *lobj = HSD_LOBJ(o);
	
	HSD_AObjRemove(lobj->aobj);
	HSD_WObjUnref(HSD_LObjGetPositionWObj(lobj));
	HSD_WObjUnref(HSD_LObjGetInterestWObj(lobj));
	
	HSD_OBJECT_INFO(&hsdLObj)->release(o);
}

//80367628
static void LObjAmnesia(HSD_ClassInfo *info){
	if (info == HSD_CLASS_INFO(default_class)) {
		default_class = NULL;
	}
	if (info == HSD_CLASS_INFO(&hsdLObj)) {
		current_lights = NULL;
	}
	HSD_OBJECT_INFO(&hsdLObj)->amnesia(info);
}

//80367688
static void LObjInfoInit(void){
	hsdInitClassInfo(HSD_CLASS_INFO(&hsdLObj),
		HSD_CLASS_INFO(&hsdClass),
		HSD_BASE_CLASS_LIBRARY,
		"hsd_lobj",
		sizeof(HSD_LObjInfo),
		sizeof(HSD_LObj));
		
	HSD_OBJECT_INFO(&hsdLObj)->release = LObjRelease;
	HSD_OBJECT_INFO(&hsdLObj)->amnesia = LObjAmnesia;
	
	HSD_LOBJ_INFO(&hsdLObj)->load     = LObjLoad;
	HSD_LOBJ_INFO(&hsdLObj)->update   = LObjUpdateFunc;
}

static void LObjUnref(HSD_LObj* lobj){
	if(lobj != NULL){
		u16 ref_count = lobj->parent.ref_count;
		u32 lz = __builtin_clz(0xFFFF - ref_count);
		lz = lz >> 5;
		if(lz == 0){
			lobj->parent.ref_count -= 1;
      lz = __builtin_clz(-ref_count);
			lz = lz >> 5;
		}
    if(lz != 0){
      HSD_OBJECT_METHOD(lobj)->release((HSD_Class*)lobj);
    }
  }
}
