#include "hsd_jobj.h"

static HSD_JObj *current_jobj = NULL;

//80371590
void HSD_JObjRemoveAll(HSD_JObj *jobj){
	if(jobj){
		if(jobj->parent){
			HSD_JObj *prev = HSD_JObjGetPrev(jobj);
			if(prev)
				prev->next = NULL;
			else
				jobj->parent->class_parent = NULL; //TODO: Verify this, probably wrong : *((_DWORD *)v16[3] + 4) = NULL;
		}
		HSD_JObj *curr = jobj;
		HSD_JObj *next = NULL;
		while(curr){
			next = curr->next;
			curr->parent = NULL;
			curr->next = NULL;
			
			if(curr){			
				/*v19 = *((u16*)v16 + 2);
				v20 = 0xFFFF == v19;
				if(!v20){
					*((u16*)v16 + 2) = v19 - 1;
					v20 = v19 == 0;
				}
				if(v20){
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
	if(jobj){
		for(HSD_JObj* i = jobj; i; i->next){
			u32 v2 = 0x8FFFFFFF;
			for(HSD_JObj j = jobj->child; j->next; j = j->next){
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
	if(jobj && child){
		if(!child->parent){ //child should be a orphan
			if(!child->next){ //child should not have siblings
				if(jobj->child){
					assert(jobj->flags & JOBJ_INSTANCE); //!(jobj->flags & JOBJ_INSTANCE)
					HSD_JObj* i;
					for(i = jobj->child; jobj->next; i = jobj->next){
						assert(i == child); //last != child
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
	if(jobj){
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
					a3 |= (flag | (flags << 10)) & 0x70000000;
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
	HSD_JObj* v18;
	
	if(jobj && next){
		if(jobj->parent){
			v18 = jobj->child;
			jobj->child = NULL;
			jobj->flags = jobj->flags & 0x8FFFFFFF;
		}else{
			v18 = jobj;
		}
		
		HSD_JObj* v19 = jobj->parent;
		HSD_JObj* v20 = next->parent;
		HSD_JObj* v21 = next->next;
		if(v20){
			HSD_JObj* v22 = v20->child;
			if(v22 == next){
				v22 = v21;
			}else{
				HSD_JObj* prev = HSD_JObjGetPrev(next);
				assert(prev);
				prev->next = v21;
			}
			RecalcParentTrspBits(next->parent);
			next->parent = NULL;
		}
		next->next = NULL;
		HSD_JObjAddChild(next->parent, next);

		if(next->child){
			for(HSD_JObj* i = next->child; i->next; i = i->next)
				i->next = v18;
		}else{
			next->child = v18;
		}
		
		while(v18){
			/**((_DWORD *)v18 + 3) = next;
			v38 = next;
			result = (char **)*((_DWORD *)v18 + 5);
			v39 = ((unsigned int)result | ((_DWORD)result << 10)) & 0x70000000;
			while ( v38 )
			{
				result = (char **)v38[5];
				if ( !(v39 & ~(unsigned int)result) )
					break;
				v38[5] = (unsigned int)result | v39;
				v38 = (_DWORD *)v38[3];
			}
			v18 = (char *)*((_DWORD *)v18 + 2);
			*/
		}
	}
}

//80371B60
HSD_JObj* HSD_JObjGetPrev(HSD_JObj *jobj){
	HSD_JObj *i;
	
	if(jobj){
		if(!jobj->parent)
			return NULL;

		if(jobj->parent->child == jobj)
			return NULL;
		
		for (i = jobj->parent->child; i; i = i->next){
			if(i->next == jobj){
				return i;
			}
		}
	}
	return NULL;
}

//80371BEC
HSD_DObj* HSD_JObjGetDObj(HSD_JObj *jobj){
	if(jobj && (jobj->flags & 0x4020) == 0){
		return jobj->dobj;
	}
	return NULL;
}

//80371C24
void HSD_JObjAddDObj(HSD_JObj *jobj, HSD_DObj *dobj){
	if(jobj && dobj && ((jobj->flags & 0x4020) == 0){
		dobj->next = jobj->dobj;
		jobj->dobj = dobj;
	}
}

//80371C68
void HSD_JObjPrependRObj(HSD_JObj *jobj, HSD_RObj *robj){
	if(jobj && robj){
		
	}
}

//80371C98
void HSD_JObjDeleteRObj(HSD_JObj *jobj, HSD_RObj *robj){
	HSD_RObj **rp;
	if(jobj && robj){
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
	if(jobj){
		return jobj->flags;
	}
	return 0;
}

//80371D00
bool HSD_JObjSetFlags(HSD_JObj *jobj, u32 flags){
	bool result = false;
	if(jobj){
		if((jobj->flags ^ flags) & 8){
			if(!(jobj->flags & 0x800000) && (jobj->flags & 0x40))
				result = true;
			if(!result)
				result = HSD_JObjSetMtxDirtySub(jobj);
		}
		jobj->flags |= flags;
	}
	return result;
}

//80371D9C
bool HSD_JObjSetFlagsAll(HSD_JObj *jobj, u32 flags){
	bool result = false;
	if(jobj){
		if((jobj->flags ^ flags) & 8){
			if(!(jobj->flags & 0x800000) && (jobj->flags & 0x40))
				result = true;
			if(!result)
				result = HSD_JObjSetMtxDirtySub(jobj);
		}
		jobj->flags |= flags;
		
		if(!(jobj->flags & JOBJ_INSTANCE)){
			HSD_JObj* i;
			for(i = jobj->child; i; i = i->child){
				if(i){
					if((i->flags ^ flags) & 8){
						result = false;
						if(!(i->flags & 0x800000) && (i->flags & 0x40))
							result = true;
						if(!result)
							result = HSD_JObjSetMtxDirtySub(i);
					}
					i->flags |= flags;
					
					if(!(i->flags & JOBJ_INSTANCE)){
						for ( j = i->child; j; j = j->child )
							result = HSD_JObjSetFlagsAll(j, flags);
					}
				}
			}
		}
	}
}

//80371F00
bool HSD_JObjClearFlags(HSD_JObj *jobj, u32 flags){
	bool result = false;
	if(jobj){
		if((jobj->flags ^ flags) & 8){
			if(!(jobj->flags & 0x800000) && (jobj->flags & 0x40))
				result = true;
			if(!result)
				result = HSD_JObjSetMtxDirtySub(jobj);
		}
		jobj->flags &= ~flags;
	}
	return result;
}

//80371F9C
bool HSD_JObjClearFlagsAll(HSD_JObj *jobj, u32 flags){
	bool result = false;
	if(jobj){
		if((jobj->flags ^ flags) & 8){
			if(!(jobj->flags & 0x800000) && (jobj->flags & 0x40))
				result = true;
			if(!result)
				result = HSD_JObjSetMtxDirtySub(jobj);
		}
		jobj->flags &= ~flags;
		
		if(!(jobj->flags & JOBJ_INSTANCE)){
			HSD_JObj *i;
			for(i = jobj->child; i; i = i->child){
				if(i){
					if((i->flags ^ flags) & 8){
						result = false;
						if(!(i->flags & 0x800000) && (i->flags & 0x40))
							result = true;
						if(!result)
							result = HSD_JObjSetMtxDirtySub(i);
					}
					i->flags &= ~flags;
					
					if(!(i->flags & JOBJ_INSTANCE)){
						for ( j = i->child; j; j = j->child )
							result = HSD_JObjClearFlagsAll(j, flags);
					}
				}
			}
		}
	}
	return result;
}

//8037210C
HSD_JObj* HSD_JObjAlloc()
{
  HSD_JObj *jobj = hsdNew(HSD_JObjGetDefaultClass());
  assert(jobj);
  return jobj;
}

//80372168
void HSD_JObjSetCurrent(HSD_JObj *jobj){ //INCOMPLETE
	if(jobj)
		current_jobj = jobj;
}

//80372314
HSD_JObj* HSD_JObjGetCurrent(){
	return current_jobj;
}