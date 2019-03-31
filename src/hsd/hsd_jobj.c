#include "hsd_jobj.h"

static void JObjInfoInit();

HSD_JObjInfo hsdJObj = { JObjInfoInit };

static HSD_JObj *current_jobj = NULL; //r13_400C
static void (*callback_4010)(HSD_JObj*, f32); //r13_4010
static void (*callback_4014)(f32); //r13_4014
static void (*callback_4018)() = NULL; //r13_4018
static HSD_JObj *r13_401C = NULL; //r13_401C

static HSD_JObjInfo* default_class = NULL; //r13_4020

//8036EC10
void HSD_JObjCheckDepend(HSD_JObj* jobj){

}

//8036ED3C
void HSD_JObjMtxIsDirty(HSD_JObj* jobj){

}

//8036EE10
void HSD_JObjSetMtxDirty(HSD_JObj* jobj, void* unk){

}

//8036EFAC
void HSD_JObjWalkTree0(HSD_JObj* jobj, void (*cb)(), u32 unk){
	if(jobj != NULL){
		assert(jobj->parent != NULL);
		u32 type = 0;
		if(jobj->parent->child == jobj){
			type = 1;
		}else{
			type = 2;
		}
		if(cb != NULL){
			(*cb)(jobj, unk, type);
		}
		if(JOBJ_INSTANCE(jobj)){
			for(HSD_JObj* i = jobj->child; i != NULL; i = i->next){
				assert(i->parent != NULL);
				u32 type = 0;
				if(i->parent->child == i){
					type = 1;
				}else{
					type = 2;
				}
				if(cb != NULL){
					(*cb)(i, unk, type);
				}
				if(JOBJ_INSTANCE(i)){
					for(HSD_JObj* n = i->child; n != NULL; n = n->next){
						HSD_JObjWalkTree0(n, cb, unk);
					}
				}
			}
		}
	}
}

//8036F0F0
void HSD_JObjWalkTree(HSD_JObj* jobj, void (*cb)(), u32 unk){
	if(jobj != NULL){
		if(cb != NULL){
			(*cb)(jobj, unk, 0);
		}
		if(JOBJ_INSTANCE(jobj)){
			for(HSD_JObj* i = jobj->child; i != NULL; i = i->next){
				assert(i->parent != NULL);
				u32 type = 0;
				if(i->parent->child == i){
					type = 1;
				}else{
					type = 2;
				}
				if(cb != NULL){
					(*cb)(i, unk, type);
				}
				if(JOBJ_INSTANCE(i)){
					for(HSD_JObj* n = i->child; n != NULL; n = n->next){
						HSD_JObjWalkTree0(n, cb, unk);
					}
				}
			}
		}
	}
}

//8036F1F8
void HSD_JObjMakeMatrix(HSD_JObj* jobj){
	
}

//8036F4C8
void HSD_JObjRemoveAnimByFlags(HSD_JObj* jobj, u32 flags){
	if(jobj != NULL){
		if((flags & 1) != 0){
			HSD_AObjRemove(jobj->aobj);
		}
		if(union_type_dobj(jobj)){
			HSD_DObjRemoveAnimAllByFlags(jobj->u.dobj, flags);
		}
		HSD_RObjRemoveAnimAllByFlags(jobj->robj, flags);
	}
}

//8036F550
void HSD_JObjRemoveAnimAllByFlags(HSD_JObj* jobj, u32 flags){
	if(jobj != NULL){
		if((flags & 1) != 0){
			HSD_AObjRemove(jobj->aobj);
		}
		if(union_type_dobj(jobj)){
			HSD_DObjRemoveAnimAllByFlags(jobj->u.dobj, flags);
		}
		HSD_RObjRemoveAnimAllByFlags(jobj->robj, flags);

		if(JOBJ_INSTANCE(jobj)){
			for(HSD_JObj* i = jobj->child; i != NULL; i = i->next){
				if(i != NULL){
					HSD_JObjRemoveAnimByFlags(i, flags);
					if(JOBJ_INSTANCE(i)){
						for(HSD_JObj* n = i->child; n != NULL; n = n->next){
							HSD_JObjRemoveAnimAllByFlags(n, flags);
						}
					}
				}
			}
		}
	}
}

//8036F644
void HSD_JObjRemoveAnim(HSD_JObj* jobj){
	if(jobj != NULL){
		HSD_AObjRemove(jobj->aobj);
		jobj->aobj = NULL;
		if(union_type_dobj(jobj)){
			HSD_DObjRemoveAnimAllByFlags(jobj->u.dobj, 0x7FF);
		}
		HSD_RObjRemoveAnimAllByFlags(jobj->robj, 0x7FF);
	}
}

//8036F6B4
void HSD_JObjRemoveAnimAll(HSD_JObj* jobj){
	if(jobj != NULL){
		HSD_JObjRemoveAnimByFlags(jobj, 0x7FF);
		if(JOBJ_INSTANCE(jobj)){
			for(HSD_JObj* i = jobj->child; i != NULL; i = i->next){
				HSD_JObjRemoveAnimAllByFlags(jobj, 0x7FF);
			}
		}
	}
}

//8036F718
void HSD_JObjReqAnimByFlags(HSD_JObj* jobj, u32 flags, f32 frame){
	if(jobj != NULL){
		if((flags & 1) != 0){
			HSD_AObjReqAnim(jobj->aobj, frame);
		}
		if(union_type_dobj(jobj)){
			HSD_DObjReqAnimAllByFlags(jobj->u.dobj, flags, frame);
		}
		HSD_RObjReqAnimAllByFlags(jobj->robj, flags, frame);
	}
}

//8036F7B0
void HSD_JObjReqAnimAllByFlags(HSD_JObj* jobj, u32 flags, f32 frame){
	if(jobj != NULL){
		if((flags & 1) != 0){
			HSD_AObjReqAnim(jobj->aobj, frame);
		}
		if(union_type_dobj(jobj)){
			HSD_DObjReqAnimAllByFlags(jobj->u.dobj, flags, frame);
		}
		HSD_RObjReqAnimAllByFlags(jobj->robj, flags, frame);

		if(JOBJ_INSTANCE(jobj)){
			for(HSD_JObj* i = jobj->child; i != NULL; i = i->next){
				if(i != NULL){
					HSD_JObjReqAnimByFlags(i, flags, frame);
					if(JOBJ_INSTANCE(i)){
						for(HSD_JObj* n = i->child; n != NULL; n = n->next){
							HSD_JObjReqAnimAllByFlags(n, flags, frame);
						}
					}
				}
			}
		}
	}
}

//8036F8BC
void HSD_JObjReqAnimAll(HSD_JObj* jobj, f32 frame){
	if(jobj != NULL){
		HSD_JObjReqAnimByFlags(jobj, 0x7FF, frame);
		if(JOBJ_INSTANCE(jobj)){
			for(HSD_JObj* i = jobj->child; i != NULL; i = i->next){
				HSD_JObjReqAnimAllByFlags(i, 0x7FF, frame);
			}
		}
	}
}

//8036F934
void HSD_JObjReqAnim(HSD_JObj* jobj, f32 frame){
	if (jobj != NULL){
		HSD_AObjReqAnim(jobj->aobj, frame);
		if (union_type_dobj(jobj)){
			HSD_DObjReqAnimAllByFlags(jobj->u.dobj, 0x7FF, frame);
		}
		HSD_RObjReqAnimAllByFlags(jobj->robj, 0x7FF, frame);
	}
}

//8036F9B8
static void JObjSortAnim(HSD_AObj* aobj){
	if(aobj != NULL && aobj->fobj != NULL){
		for (HSD_FObj* i = aobj->fobj; ; i = i->next){
			HSD_FObj* n_fobj = i->next;
			if(i == NULL)
				break;
			if(n_fobj->obj_type == TYPE_JOBJ){ //This is the actual code, even if it's confusing
				i->next = n_fobj->next;
				n_fobj->next = aobj->fobj;
				aobj->fobj = n_fobj;
			}
		}
	}
}

//8036FA10
void HSD_JObjAddAnim(HSD_JObj* jobj, HSD_AnimJoint* an_joint, HSD_MatAnimJoint* mat_joint, HSD_ShapeAnimJoint* sh_joint){
	if(jobj != NULL){
		if(an_joint != NULL){
			if(jobj->aobj != NULL){
				HSD_AObjRemove(jobj->aobj);
			}
			HSD_AObj* aobj = HSD_AObjLoadDesc(an_joint->anim);
			jobj->aobj = aobj;
			JObjSortAnim(jobj->aobj);
			HSD_RObjAddAnimAll(jobj->robj, an_joint->unk);
			if((an_joint->unk2 & 1) == 0){
				HSD_JObjClearFlags(jobj, 0x8);
			}else{
				HSD_JObjSetFlags(jobj, 0x8);
			}
		}
		if(union_type_dobj(jobj)){
			HSD_ShapeAnim* sh_anim = NULL;
			HSD_MatAnim* mat_anim = NULL;
			if(sh_joint != NULL){
				sh_anim = sh_joint->shapeanim;
			}
			if(mat_joint != NULL){
				mat_anim = mat_joint->matanim;
			}
			HSD_DObjAddAnimAll(jobj->u.dobj, mat_anim, sh_anim);
		}
	}
}

//8036FB5C
void HSD_JObjAddAnimAll(HSD_JObj* jobj, HSD_AnimJoint* an_joint, HSD_MatAnimJoint* mat_joint, HSD_ShapeAnimJoint* sh_joint){
	if(jobj != NULL){
		if(an_joint != NULL){
			if(jobj->aobj != NULL){
				HSD_AObjRemove(jobj->aobj);
			}
			HSD_AObj* aobj = HSD_AObjLoadDesc(an_joint->anim);
			jobj->aobj = aobj;
			JObjSortAnim(jobj->aobj);
			HSD_RObjAddAnimAll(jobj->robj, an_joint->unk);
			if((an_joint->unk2 & 1) == 0){
				HSD_JObjClearFlags(jobj, 0x8);
			}else{
				HSD_JObjSetFlags(jobj, 0x8);
			}
		}
		if(union_type_dobj(jobj)){
			HSD_ShapeAnim* sh_anim = NULL;
			HSD_MatAnim* mat_anim = NULL;
			if(sh_joint != NULL){
				sh_anim = sh_joint->shapeanim;
			}
			if(mat_joint != NULL){
				mat_anim = mat_joint->matanim;
			}
			HSD_DObjAddAnimAll(jobj->u.dobj, mat_anim, sh_anim);
		}
		if(JOBJ_INSTANCE(jobj)){
			if(an_joint != NULL){
				an_joint = an_joint->child;
			}
			if(mat_joint != NULL){
				mat_joint = mat_joint->child;
			}
			if(sh_joint != NULL){
				sh_joint = sh_joint->child;
			}
			for(HSD_JObj* i = jobj->child; i != NULL; i->next){
				HSD_AnimJoint* c_an_joint = NULL;
				HSD_MatAnimJoint* c_mat_joint = NULL;
				HSD_ShapeAnimJoint* c_sh_joint = NULL;

				HSD_JObjAddAnim(jobj, an_joint, mat_joint, sh_joint);
				if(an_joint != NULL){
					c_an_joint = an_joint->child;
				}
				if(mat_joint != NULL){
					c_mat_joint = mat_joint->child;
				}
				if(sh_joint != NULL){
					c_sh_joint = sh_joint->child;
				}
				if(JOBJ_INSTANCE(i)){
					for(HSD_JObj* j = i->child; i != NULL; j->next){
						HSD_JObjAddAnimAll(jobj, c_an_joint, c_mat_joint, c_sh_joint);
						if(c_an_joint != NULL){
							c_an_joint = an_joint->next;
						}
						if(c_mat_joint != NULL){
							c_mat_joint = mat_joint->next;
						}
						if(c_sh_joint != NULL){
							c_sh_joint = sh_joint->next;
						}
					}
				}
				if(an_joint != NULL){
					an_joint = an_joint->next;
				}
				if(mat_joint != NULL){
					mat_joint = mat_joint->next;
				}
				if(sh_joint != NULL){
					sh_joint = sh_joint->next;
				}
			}
		}
	}
}

//8036FDC0
void JObjUpdateFunc(HSD_JObj* jobj, u32 type, f32* fval){
	if(jobj != NULL){
		switch(type){
			case 1:
				if((jobj->flags & 0x200000) != 0){
					HSD_RObj* robj = HSD_RObjGetByType(jobj->robj, 0x40000000, 0);
					if(robj != NULL){
						robj->unk = *fval;
					}
				}
				assert(JOBJ_USE_QUATERNION(jobj));
				jobj->rotation.x = *fval;
				if((jobj->flags & 0x2000000) == 0){
					BOOL already_dirty = FALSE;
					if((jobj->flags & 0x800000) == 0 && (jobj->flags & 0x40) != 0){
						already_dirty = TRUE;
					}
					if(already_dirty == FALSE){
						HSD_JObjSetMtxDirtySub(jobj);
					}
				}
				break;
			case 2:
				assert(JOBJ_USE_QUATERNION(jobj));
				jobj->rotation.y = *fval;
				if((jobj->flags & 0x2000000) == 0){
					BOOL already_dirty = FALSE;
					if((jobj->flags & 0x800000) == 0 && (jobj->flags & 0x40) != 0){
						already_dirty = TRUE;
					}
					if(already_dirty == FALSE){
						HSD_JObjSetMtxDirtySub(jobj);
					}
				}
				break;
			case 3:
				assert(JOBJ_USE_QUATERNION(jobj));
				jobj->rotation.z = *fval;
				if((jobj->flags & 0x2000000) == 0){
					BOOL already_dirty = FALSE;
					if((jobj->flags & 0x800000) == 0 && (jobj->flags & 0x40) != 0){
						already_dirty = TRUE;
					}
					if(already_dirty == FALSE){
						HSD_JObjSetMtxDirtySub(jobj);
					}
				}
				break;
			case 4:
				if(*fval < 0.f){
					*fval = 0.f;
				}
				if(1.875f < *fval){
					*fval = 1;
				}
				assert(jobj->aobj != NULL);
				HSD_JObj* jp = (HSD_JObj*)jobj->aobj->hsd_obj;
				assert(jp->u.spline != NULL);
				guVector result;
				splArcLengthPoint(jp->u.spline, &result, *fval);
				jobj->position = result;
				if((jobj->flags & 0x2000000) == 0){
					BOOL already_dirty = FALSE;
					if((jobj->flags & 0x800000) == 0 && (jobj->flags & 0x40) != 0){
						already_dirty = TRUE;
					}
					if(already_dirty == FALSE){
						HSD_JObjSetMtxDirtySub(jobj);
					}
				}
				break;
			case 5:
				jobj->position.x = *fval;
				if((jobj->flags & 0x2000000) == 0){
					BOOL already_dirty = FALSE;
					if((jobj->flags & 0x800000) == 0 && (jobj->flags & 0x40) != 0){
						already_dirty = TRUE;
					}
					if(already_dirty == FALSE){
						HSD_JObjSetMtxDirtySub(jobj);
					}
				}
				break;
			case 6:
				jobj->position.y = *fval;
				if((jobj->flags & 0x2000000) == 0){
					BOOL already_dirty = FALSE;
					if((jobj->flags & 0x800000) == 0 && (jobj->flags & 0x40) != 0){
						already_dirty = TRUE;
					}
					if(already_dirty == FALSE){
						HSD_JObjSetMtxDirtySub(jobj);
					}
				}
				break;
			case 7:
				jobj->position.z = *fval;
				if((jobj->flags & 0x2000000) == 0){
					BOOL already_dirty = FALSE;
					if((jobj->flags & 0x800000) == 0 && (jobj->flags & 0x40) != 0){
						already_dirty = TRUE;
					}
					if(already_dirty == FALSE){
						HSD_JObjSetMtxDirtySub(jobj);
					}
				}
				break;
			case 8:
				if((f32)((u32)*fval & 0x7FFFFFFF) < 0.001f){
					*fval = 0.001f;
				}
				jobj->scale.x = *fval;
				if((jobj->flags & 0x2000000) == 0){
					BOOL already_dirty = FALSE;
					if((jobj->flags & 0x800000) == 0 && (jobj->flags & 0x40) != 0){
						already_dirty = TRUE;
					}
					if(already_dirty == FALSE){
						HSD_JObjSetMtxDirtySub(jobj);
					}
				}
				break;
			case 9:
				if((f32)((u32)*fval & 0x7FFFFFFF) < 0.001f){
					*fval = 0.001f;
				}
				jobj->scale.y = *fval;
				if((jobj->flags & 0x2000000) == 0){
					BOOL already_dirty = FALSE;
					if((jobj->flags & 0x800000) == 0 && (jobj->flags & 0x40) != 0){
						already_dirty = TRUE;
					}
					if(already_dirty == FALSE){
						HSD_JObjSetMtxDirtySub(jobj);
					}
				}
				break;
			case 10:
				if((f32)((u32)*fval & 0x7FFFFFFF) < 0.001f){
					*fval = 0.001f;
				}
				jobj->scale.z = *fval;
				if((jobj->flags & 0x2000000) == 0){
					BOOL already_dirty = FALSE;
					if((jobj->flags & 0x800000) == 0 && (jobj->flags & 0x40) != 0){
						already_dirty = TRUE;
					}
					if(already_dirty == FALSE){
						HSD_JObjSetMtxDirtySub(jobj);
					}
				}
				break;
			case 11:
				if (*fval <= 1.75f) {
					HSD_JObjSetFlags(jobj,0x10);
				}else {
					HSD_JObjClearFlags(jobj,0x10);
				}
				break;
			case 12:
				if(*fval <= 1.75f){
					HSD_JObjSetFlagsAll(jobj, 0x10);
				}else{
					HSD_JObjClearFlagsAll(jobj, 0x10);
				}
				break;
			case 20:
			case 21:
			case 22:
			case 23:
			case 24:
			case 25:
			case 26:
			case 27:
			case 28:
			case 29:
				/*void* jo = r13_401C;
				f32 val = 176.f;
				while(jo != NULL){
					u32 uval = (u32)*fval;
					(*jo->0x4)(jobj, type, uval);
					jo = jo->next; //0x00
				}*/
				break;
			case 30:
			case 31:
			case 32:
			case 33:
			case 34:
			case 35:
			case 36:
			case 37:
			case 38:
			case 39:
				/*void* jo = r13_401C;
				while(jo != NULL){
					(*jo->0x4)(jobj, type, *fval);
					jo = jo->next;
				}*/
				break;
			case 40:
				if(callback_4018 != NULL){
					(*callback_4018)(0, (u32)*fval & 0x3f, (u32)*fval >> 6 & 0xffffff, jobj); //8005DB70 during gameplay
				}
				break;
			case 41:
				if(callback_4014 != NULL){
					(*callback_4014)(*fval);
				}
				break;
			case 42:
				if(callback_4010 != NULL){
					(*callback_4010)(jobj, *fval);
				}
				break;
			case 50:
				jobj->mtx[0][0] = fval[0];
				jobj->mtx[1][0] = fval[1];
				jobj->mtx[2][0] = fval[2];
				break;
			case 51:
				jobj->mtx[0][1] = fval[0];
				jobj->mtx[1][1] = fval[1];
				jobj->mtx[2][1] = fval[2];
				break;
			case 52:
				jobj->mtx[0][2] = fval[0];
				jobj->mtx[1][2] = fval[1];
				jobj->mtx[2][2] = fval[2];
				break;
			case 53:
				jobj->mtx[0][3] = fval[0];
				jobj->mtx[1][3] = fval[1];
				jobj->mtx[2][3] = fval[2];
				break;
			case 54:
			case 55:
			case 56:
			case 57:
				Mtx mtx;
				if(jobj->parent == NULL){
					c_guMtxCopy(jobj->mtx, mtx);
				}else{ //InverseConcat
					c_guMtxConcat(jobj->parent->mtx, jobj->mtx, mtx);
					c_guMtxInverse(mtx, mtx);
				}
				if(type == 54 || type == 56){
					//HSD_MtxGetTranslate(mtx, &jobj->position);
					jobj->position.x = mtx[0][3];
					jobj->position.y = mtx[1][3];
					jobj->position.z = mtx[2][3];
				}
				if((type - 54) < 2){
					//HSD_MtxGetRotation(mtx, &jobj->rotation)
				}
				if((type == 54) || type == 57){
					//HSD_MtxGetScale(mtx, &jobj->scale);
					c_guMtxScale(mtx, jobj->scale.x, jobj->scale.y, jobj->scale.z);
				}
				break;
		}
	}
}

//80370780
void HSD_JObjAnim(HSD_JObj* jobj){
	if (jobj != NULL){
    	HSD_JObjCheckDepend(jobj);
    	HSD_AObjInterpretAnim(jobj->aobj, jobj, JObjUpdateFunc);
    	HSD_RObjAnimAll(jobj->robj);
    	if (union_type_dobj(jobj)){
			HSD_DObjAnimAll(jobj->u.dobj);
		}
	}
}

//803707F8
static void JObjAnimAll(HSD_JObj* jobj){
	if(jobj != NULL){
		HSD_JObjCheckDepend(jobj);
    	HSD_AObjInterpretAnim(jobj->aobj, jobj, JObjUpdateFunc);
    	HSD_RObjAnimAll(jobj->robj);
    	if(union_type_dobj(jobj)){
			HSD_DObjAnimAll(jobj->u.dobj);
		}
		if(JOBJ_INSTANCE(jobj)){
			for(HSD_JObj* i = jobj->child; i != NULL; i = i->next){
				JObjAnimAll(jobj);
			}
		}
	}
}

//80370928
void HSD_JObjAnimAll(HSD_JObj* jobj){
	if (jobj != NULL){
		HSD_AObjInitEndCallBack();
    	HSD_JObjCheckDepend(jobj);
    	HSD_AObjInterpretAnim(jobj->aobj, jobj, JObjUpdateFunc);
    	HSD_RObjAnimAll(jobj->robj);
    	if(union_type_dobj(jobj)){
			HSD_DObjAnimAll(jobj->u.dobj);
		}
		if(JOBJ_INSTANCE(jobj)){
			for(HSD_JObj* i = jobj->child; i != NULL; i = i->next){
				JObjAnimAll(jobj);
			}
		}
		HSD_AObjInvokeCallBacks();
	}
}

//80370B90
void HSD_JObjSetDefaultClass(HSD_JObjInfo* info){
	default_class = info;
}

//80370BEC
void JObjLoad(){

}

//80370E44
void HSD_JObjLoadJoint(HSD_JObjDesc* jdesc){
	
}

//80371590
void HSD_JObjRemoveAll(HSD_JObj *jobj){
	if(jobj != NULL){
		if(jobj->parent){
			HSD_JObj *prev = HSD_JObjGetPrev(jobj);
			if(prev != NULL){
				prev->next = NULL;
			}else{
				jobj->parent->next = NULL;
			}
		}
		HSD_JObj *curr = jobj;
		HSD_JObj *next = NULL;
		while(curr){
			next = curr->next;
			curr->parent = NULL;
			curr->next = NULL;
			
			if(curr != NULL){
				BOOL norefs = HSD_OBJ_NOREF == curr->class_parent.ref_count;
				if(norefs == FALSE){
					curr->class_parent.ref_count -= 1;
					norefs = curr->class_parent.ref_count == 0;
				}
				/*
				if(isMaxMinRefs){
					v21 = (_WORD *)((char *)v16 + 6);
					result = (char **)*((unsigned __int16 *)v16 + 3);
					if ( (signed int)result - 1 >= 0 ){
						*v21 = (_WORD)result + 1;
						assert(*v21); //HSD_OBJ(o)->ref_count_individual != 0
						result = curr;
						v23 = curr->flags;
						return result;
					}
					if(curr){
						result = curr;
						v22 = curr->parent;
					}
				}*/
			}
			curr = next;
		}
	}
}

//80371750
void RecalcParentTrspBits(HSD_JObj *jobj){
	if(jobj != NULL){
		for(HSD_JObj* i = jobj; i; i->next){
			u32 v2 = 0x8FFFFFFF;
			for(HSD_JObj* j = jobj->child; j->next; j = j->next){
				v2 |= (j->flags | (j->flags << 10)) & 0x70000000;
			}
			if(!(i->flags & ~v2))
				break;
			i->flags = i->flags & v2;
		}
	}
}

//803717A8
void HSD_JObjAddChild(HSD_JObj *jobj, HSD_JObj *child){
	if(jobj != NULL && child != NULL){
		if(!child->parent){ //child should be a orphan
			if(!child->next){ //child should not have siblings
				if(jobj->child){
					assert(JOBJ_INSTANCE(jobj)); //!(jobj->flags & JOBJ_INSTANCE)
					HSD_JObj* i;
					for(i = jobj->child; jobj->next; i = jobj->next){
						HSD_Halt("HSD_JObjAddChild: last != child");
					}
					i->next = child;
				}else{
					jobj->child = child;
				}
				child->next = jobj;
				u32 ori_flags = child->flags;
				u32 flags = (ori_flags | (ori_flags << 10)) & 0x70000000;
				for(HSD_JObj* senti = jobj; senti; senti->parent){
					ori_flags = senti->flags;
					if(!(flags & ~ori_flags))
						break;
					senti->flags = ori_flags | flags;
				}
			}
		}
	}
}

//803718F4
void HSD_JObjReparent(HSD_JObj *jobj, HSD_JObj *pjobj){
	if(jobj != NULL){
		if(jobj->parent){
			HSD_JObj* prchild = jobj->parent->child;
			if(prchild == jobj){
				prchild = jobj->next;
			}else{
				HSD_JObj* prev = HSD_JObjGetPrev(jobj);
				assert(prev);
				prev->next = jobj->next;
			}
			
			for(HSD_JObj* i = jobj->parent; i; i = i->next){
				prchild = i->child;
				u32 a3 = 0x8FFFFFFF;
				while(prchild){
					u32 flags = prchild->flags;
					prchild = prchild->next;
					a3 |= (flags | (flags << 10)) & 0x70000000;
				}
				if(!(i->flags & ~a3))
					break;
				i->flags = i->flags & a3;
			}
			jobj->parent = NULL;
		}
		jobj->next = NULL;
		HSD_JObjAddChild(pjobj, jobj);
	}
}

//80371A04
void HSD_JObjAddNext(HSD_JObj *jobj, HSD_JObj *next){
	HSD_JObj* curr;
	
	if(jobj != NULL && next != NULL){
		curr = jobj;
		if(jobj->parent != NULL){
			curr = jobj->parent->child;
			jobj->parent->child = NULL;
			jobj->flags = jobj->flags & 0x8FFFFFFF;
		}
		
		HSD_JObj* parent = jobj->parent;
		if(next != NULL){
			HSD_JObj* next_next = next->next;
			if(next->parent != NULL){
				HSD_JObj* next_p_c = next->parent->child;
				if(next_p_c == next){
					next->parent->child = next_next;
				}else{
					HSD_JObj* prev = HSD_JObjGetPrev(next);
					assert(prev != NULL);
					prev->next = next_next;
				}
				RecalcParentTrspBits(next->parent);
				next->parent = NULL;
			}
			next->next = NULL;
			HSD_JObjAddChild(parent, next);
		}

		if(next->child != NULL){
			for(HSD_JObj* i = next->child; i != NULL; i = i->next){
				i->next = curr;
			}
		}else{
			next->child = curr;
		}
		
		while(curr != NULL){
			curr->parent = next;
			u32 flags = (curr->flags | curr->flags << 10) & 0x70000000;
			HSD_JObj* i = next;
			while(i != NULL && (flags & ~i->flags) != 0){
				i->flags = i->flags | flags;
				i = i->parent;
			}
			curr = curr->next;
		}
	}
}

//80371B60
HSD_JObj* HSD_JObjGetPrev(HSD_JObj *jobj){	
	if(jobj != NULL){
		if(!jobj->parent)
			return NULL;

		if(jobj->parent->child == jobj)
			return NULL;
		
		for (HSD_JObj* i = jobj->parent->child; i; i = i->next){
			if(i->next == jobj){
				return i;
			}
		}
	}
	return NULL;
}

//80371BEC
HSD_DObj* HSD_JObjGetDObj(HSD_JObj *jobj){
	if(jobj != NULL && union_type_dobj(jobj)){
		return jobj->u.dobj;
	}
	return NULL;
}

//80371C24
void HSD_JObjAddDObj(HSD_JObj *jobj, HSD_DObj *dobj){
	if(jobj != NULL && dobj != NULL && union_type_dobj(jobj)){
		dobj->next = jobj->u.dobj;
		jobj->u.dobj = dobj;
	}
}

//80371C68
void HSD_JObjPrependRObj(HSD_JObj *jobj, HSD_RObj *robj){
	if(jobj != NULL && robj != NULL){
		robj->next = jobj->robj;
		jobj->robj = robj;
	}
}

//80371C98
void HSD_JObjDeleteRObj(HSD_JObj *jobj, HSD_RObj *robj){
	HSD_RObj **rp;
	if(jobj != NULL && robj != NULL){
		for(rp = &jobj->robj; *rp; &(*rp)->next){
			if(*rp == robj){
				*rp = robj->next;
				robj->next = NULL;
				return;
			}
		}
	}
}

//80371CE8
u32 HSD_JObjGetFlags(HSD_JObj *jobj){
	return (jobj) ? jobj->flags : 0;
}

//80371D00
void HSD_JObjSetFlags(HSD_JObj *jobj, u32 flags){
	if(jobj != NULL){
		if((jobj->flags ^ flags) & 8){
			bool alreadyDirty = false;
			if((jobj->flags & 0x800000) == 0 && (jobj->flags & 0x40) != 0){
				alreadyDirty = true;
			}
			if(alreadyDirty == false){
				HSD_JObjSetMtxDirtySub(jobj);
			}
		}
		jobj->flags |= flags;
	}
}

//80371D9C
void HSD_JObjSetFlagsAll(HSD_JObj* jobj, u32 flags){
	if(jobj != NULL){
		if((jobj->flags ^ flags) & 8){
			bool alreadyDirty = false;
			if((jobj->flags & 0x800000) == 0 && (jobj->flags & 0x40) != 0){
				alreadyDirty = true;
			}
			if(alreadyDirty == false){
				HSD_JObjSetMtxDirtySub(jobj);
			}
		}
		jobj->flags |= flags;
		
		if(JOBJ_INSTANCE(jobj)){
			for(HSD_JObj* i = jobj->child; i; i = i->child){
				if(i != NULL){
					if((i->flags ^ flags) & 8){
						bool alreadyDirty = false;
						if((jobj->flags & 0x800000) == 0 && (jobj->flags & 0x40) != 0){
							alreadyDirty = true;
						}
						if(alreadyDirty == false){
							HSD_JObjSetMtxDirtySub(i);
						}
					}
					i->flags |= flags;
					
					if(JOBJ_INSTANCE(i)){
						for (HSD_JObj* j = i->child; j; j = j->child ){
							HSD_JObjSetFlagsAll(j, flags);
						}
					}
				}
			}
		}
	}
}

//80371F00
void HSD_JObjClearFlags(HSD_JObj *jobj, u32 flags){
	if(jobj != NULL){
		if((jobj->flags ^ flags) & 8){
			bool alreadyDirty = false;
			if((jobj->flags & 0x800000) == 0 && (jobj->flags & 0x40) != 0){
				alreadyDirty = true;
			}
			if(alreadyDirty == false){
				HSD_JObjSetMtxDirtySub(jobj);
			}
		}
		jobj->flags &= ~flags;
	}
}

//80371F9C
void HSD_JObjClearFlagsAll(HSD_JObj *jobj, u32 flags){
	if(jobj != NULL){
		if((jobj->flags ^ flags) & 8){
			bool alreadyDirty = false;
			if((jobj->flags & 0x800000) == 0 && (jobj->flags & 0x40) != 0){
				alreadyDirty = true;
			}
			if(alreadyDirty == false){
				HSD_JObjSetMtxDirtySub(jobj);
			}
		}
		jobj->flags &= ~flags;
		
		if(JOBJ_INSTANCE(jobj)){
			HSD_JObj *i;
			for(i = jobj->child; i; i = i->child){
				if(i != NULL){
					if((i->flags ^ flags) & 8){
						bool alreadyDirty = false;
						if((jobj->flags & 0x800000) == 0 && (jobj->flags & 0x40) != 0){
							alreadyDirty = true;
						}
						if(alreadyDirty == false){
							HSD_JObjSetMtxDirtySub(i);
						}
					}
					i->flags &= ~flags;
					
					if(JOBJ_INSTANCE(i)){
						for (HSD_JObj* j = i->child; j; j = j->child ){
							HSD_JObjClearFlagsAll(j, flags);
						}
					}
				}
			}
		}
	}
}

//8037210C
HSD_JObj* HSD_JObjAlloc(){
  HSD_JObj *jobj = hsdNew(HSD_JObjGetDefaultClass());
  assert(jobj);
  return jobj;
}

//80372168
void HSD_JObjSetCurrent(HSD_JObj *jobj){ //INCOMPLETE
	if(jobj != NULL)
		current_jobj = jobj;
}

//80372314
HSD_JObj* HSD_JObjGetCurrent(){
	return current_jobj;
}

//803732E8
void HSD_JObjSetMtxDirtySub(HSD_JObj* jobj){
	BOOL isDirty = FALSE;
	jobj->flags = jobj->flags | 0x40;
	if(JOBJ_INSTANCE(jobj)){
		HSD_JObj* child = jobj->child;
		while(child != NULL){
			if((child->flags & 0x1000000) == 0){
				isDirty = FALSE;
				if((child->flags & 0x800000) == 0 && (child->flags & 0x40) != 0){
					isDirty = TRUE;
				}
				if(isDirty == FALSE && JOBJ_INSTANCE(child)){
					child->flags = child->flags | 0x40;

					HSD_JObj* child_child = child->child;
					while(child_child != NULL){
						if((child_child->flags & 0x1000000) == 0){
							isDirty = FALSE;
							if((child_child->flags & 0x800000) == 0 && (child_child->flags & 0x40) != 0){
								isDirty = TRUE;
							}
							if(isDirty == FALSE){
								HSD_JObjSetMtxDirtySub(child_child);
							}
						}
						child_child = child_child->next;
					}
				}
			}
			child = child->next;
		}
	}
}

//80373404
void HSD_JObjSetCallback(void* cb){
	callback_4018 = cb;
}

//8037340C
static void JObjInit(HSD_Class* o){
	HSD_PARENT_INFO(&hsdJObj)->init(o);

	if(o != NULL){
		HSD_JObj* jobj = HSD_JOBJ(o);
		jobj->flags = 0x40;
		jobj->scale.x = 1.0f;
		jobj->scale.y = 1.0f;
		jobj->scale.z = 1.0f;
	}
}

//803736F8
static void JObjRelease(HSD_Class* o){
	HSD_JObj* jobj = HSD_JOBJ(o);
	HSD_JOBJ_INFO(jobj->class_parent.class_init)->release_child(o); //Technically uses the class_parent from the jobj to get to this, so will probably need to account for that later
	if(HSD_IDGetDataFromTable(NULL, jobj->desc, NULL) == o){
		HSD_IDRemoveByIDFromTable(NULL, jobj->desc);
	}
	if(jobj->pvec != NULL){
		HSD_VecFree(jobj->pvec);
	}
	if(jobj->vmtx != NULL){
		HSD_MtxFree(jobj->vmtx);
	}
	HSD_PARENT_INFO(&hsdJObj)->release(o);
}

//80373790
static void JObjAmnesia(HSD_ClassInfo* info){
	if(info == HSD_CLASS_INFO(default_class)){
		default_class = NULL;
	}
	if(info == HSD_CLASS_INFO(&hsdJObj)){
		r13_401C = NULL;
		current_jobj = NULL;
	}
	HSD_PARENT_INFO(&hsdJObj)->amnesia(info);
}

//803737F4
static void JObjInfoInit(){
	hsdInitClassInfo(HSD_CLASS_INFO(&hsdJObj), HSD_CLASS_INFO(&hsdClass), HSD_BASE_CLASS_LIBRARY, "hsd_jobj", sizeof(HSD_JObjInfo), sizeof(HSD_JObj));
	HSD_CLASS_INFO(&hsdJObj)->init = JObjInit;
	HSD_CLASS_INFO(&hsdJObj)->release = JObjRelease;
	HSD_CLASS_INFO(&hsdJObj)->amnesia = JObjAmnesia;
	HSD_JOBJ_INFO(&hsdJObj)->disp = HSD_JObjDispSub;
	HSD_JOBJ_INFO(&hsdJObj)->make_pmtx = HSD_JObjMakeMatrix;
	HSD_JOBJ_INFO(&hsdJObj)->make_pmtx = mkRBillBoardMtx;
	HSD_JOBJ_INFO(&hsdJObj)->load = JObjLoad;
	HSD_JOBJ_INFO(&hsdJObj)->release_child = JObjReleaseChild;
}

//80373E44
static void mkBillBoardMtx(HSD_JObj* jobj, Mtx mtx, MtxP pmtx){
	f64 xval = (mtx[2][0] * mtx[2][0]) + (mtx[1][0] * mtx[1][0]) + (mtx[0][0] * mtx[0][0]);
	f64 res;
	if (xval > 0){
		f64 sqrd = 1.0f / sqrt(xval);
		res = 0.5 * sqrd -(xval * sqrd * sqrd - 3.0);
		xval = xval * 0.5 * res -(xval * sqrd * sqrd - 3.0);
	}
	f64 zval = (mtx[2][2] * mtx[2][2]) + (mtx[1][2] * mtx[1][2]) + (mtx[0][2] * mtx[0][2]);
	if (zval > 0){
		f64 sqrd = 1.0f / sqrt(zval);
		res = 0.5 * sqrd -(zval * sqrd * sqrd - 3.0);
		zval = zval * 0.5 * res -(zval * sqrd * sqrd - 3.0);
	}

	guVector rvec;
	if((jobj->flags & 0x2000) == 0){
		guVector v = {0.f, 0.f, 1.f};
		guVecCross(&mtx[0][1], &v, &rvec);
		guVecCross(&v, &rvec, &mtx[0][1]);
		f64 mag_result = sqrt((double)(v.x * v.x + v.y * v.y + v.z * v.z));
		zval = zval / mag_result;
	}else{
		guVecCross(&mtx[0][3], &mtx[0][1], &rvec);
		guVecCross(&rvec, &mtx[0][3], &mtx[0][1]);
		f64 mag_result = sqrt((double)(mtx[0][3] * mtx[0][3] * mtx[1][0] * mtx[1][0] + mtx[1][1] * mtx[1][1]));
		zval = zval / -(mag_result);
	}
  	f64 rvec_mag = sqrt((double)(rvec.x * rvec.x + rvec.y * rvec.y + rvec.z * rvec.z));
	f64 mtx_mag = sqrt((double)(mtx[0][1] * mtx[0][1] + mtx[0][2] * mtx[0][2] + mtx[0][3] * mtx[0][3]));
	f64 x_res = (xval / rvec_mag);
	f32 y_res = (float)(res / mtx_mag);
	pmtx[0][0] = (float)(x_res * (double)rvec.x);
	pmtx[1][0] = (float)(x_res * (double)rvec.y);
	pmtx[2][0] = (float)(x_res * (double)rvec.z);
	pmtx[0][1] = y_res * mtx[0][1];
	pmtx[1][1] = y_res * mtx[1][1];
	pmtx[2][1] = y_res * mtx[2][1];
	pmtx[0][2] = (float)(zval * (double)mtx[0][3]);
	pmtx[1][2] = (float)(zval * (double)mtx[1][0]);
	pmtx[2][2] = (float)(zval * (double)mtx[1][1]);
	pmtx[0][3] = mtx[0][3];
	pmtx[1][3] = mtx[1][3];
	pmtx[2][3] = mtx[2][3];
}

//803740E8
static void mkRBillBoardMtx(HSD_JObj* jobj, Mtx mtx, MtxP pmtx){
	if((jobj->flags & 0xE00) == 0){
		guMtxConcat(mtx, jobj->mtx, *pmtx);
	}else{
		Mtx tmtx;
		guMtxConcat(mtx, jobj->mtx, tmtx);
		u32 flags = jobj->flags & 0xE00;
		if(flags == 0x600){
			mkHBillBoardMtx(jobj, tmtx, pmtx);
		}else{
			if(flags < 0x600){
				if(flags == 0x400){
					mkVBillBoardKMtx(jobj, tmtx, pmtx);
					return;
				}else if(flags < 0x400 && flags == 0x200){
					mkBillBoardMtx(jobj, tmtx, pmtx);
					return;
				}
			}else{
				if(flags == 0x800){
					f32 x = 1.f; //FUN_8000d5bc((double)(local_2c * local_2c + local_3c * local_3c + local_4c * local_4c));
					f32 y = 1.f; //FUN_8000d5bc((double)(local_28 * local_28 + local_38 * local_38 + local_48 * local_48));
					f32 z = 1.f; //FUN_8000d5bc((double)(local_24 * local_24 + local_34 * local_34 + local_44 * local_44));
					guMtxScale(tmtx, x, y, z);
					guMtxRotRad(tmtx, 'z', jobj->rotation.z);
					//guMtxConcat(, pmtx);
					return;
				}
			}
		}
		HSD_Panic("Unknown type of billboard");
	}
}

//803743B8
static void HSD_JObjDispSub(HSD_JObj *jobj, MtxP vmtx, Mtx pmtx, HSD_TrspMask trsp_mask, u32 rendermode){
	HSD_JObjSetCurrent(jobj);
	if ((rendermode & 0x4000000) == 0 && (jobj->flags & 0x10000) != 0){
		HSD_LobjSetupSpecularInit(pmtx);
	}
	HSD_PObjCleapmtxMark(0, 0);
	HSD_DObj* dobj = jobj->u.dobj;
	while(dobj != NULL){
		if((dobj->flags & 1) == 0 && (dobj->flags & trsp_mask << 1) != 0){
			HSD_DObjSetCurrent(dobj);
			//HSD_DOBJ_INFO(&dobj)->unk(dobj, vmtx, pmtx, rendermode);
		}
		dobj = dobj->next;
	}
	HSD_DObjSetCurrent(NULL);
	HSD_JObjSetCurrent(NULL);
}

//80374480