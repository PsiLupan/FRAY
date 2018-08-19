#include "hsd_jobj.h"

static HSD_JObj *current_jobj = NULL;

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
		
		if(!(jobj->flags & 0x1000)){
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
					
					if(!(i->flags & 0x1000)){
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
		
		if(!(jobj->flags & 0x1000)){
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
					
					if(!(i->flags & 0x1000)){
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