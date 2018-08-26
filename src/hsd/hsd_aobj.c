#include "hsd_aobj.h"

static void* r13_4078 = NULL; //AObjDesc *, maybe?

static f32 rewind = 0;
static f32 rate = 1.0; //Not aware of any evidence this is ever actually changed yet

ObjDef aobj_def = { 0, 0, 1, 0, 1, 0xFFFFFFFF}; //804C0880

//8036539C
HSD_AObj* HSD_AObjLoadDesc(HSD_AObjDesc* aobjdesc){
	if(aobjdesc){
		HSD_AObj* aobj = HSD_AObjAlloc();
		if(aobj){
			aobj->flags = aobjdesc->flags & 0x30000000;
			HSD_AObjSetRewindFrame(aobj, 0);
			HSD_AObjSetEndFrame(aobj, aobjdesc->end_frame);
			
			HSD_FObj* fobj = HSD_FObjLoadDesc(aobjdesc->fobjdesc);
			if(aobj->fobj != NULL)
				HSD_FObjRemoveAll(aobj->fobj);
			aobj->fobj = fobj;
			
			u32 id = aobjdesc->obj_id;
			if(id){
				HSD_Obj* hsdObj = HSD_IDGetDataFromTable(0, id, 0);
				void* obj = (void*)hsdObj;
				
				if(hsdObj){
					hsdObj->ref_count = hsdObj->ref_count + 1;
					assert(hsdObj->ref_count != HSD_OBJ_NOREF);
				}else{
					obj = (void*)HSD_JObjLoadJoint(aobjdesc->obj_id);
				}
				
				HSD_JObj* jobj = HSD_JOBJ(aobj->hsd_obj);
				if(jobj)
					HSD_JObjUnref(jobj);
				aobj->hsd_obj = obj;
				return aobj;
			}
		}
	}
	return NULL;
}

//803644CC
void HSD_AObjRemove(HSD_AObj* aobj){
	if(aobj){
		HSD_FObj* fobj = aobj->fobj;
		if(fobj)
			HSD_FObjRemoveAll(fobj);
		aobj->fobj = NULL;
		
		HSD_JObj* jobj = HSD_JOBJ(aobj->hsd_obj);
		if(jobj)
			HSD_JObjUnref(jobj);
		aobj->jobj = NULL;
		
		HSD_AObjFree(aobj);
	}
}

//8036453C
AObj* HSD_AObjAlloc(){
	HSD_AObj *aobj = HSD_AOBJ(HSD_ObjAlloc(aobj_def));
	assert(aobj);
	memset(aobj, 0, sizeof(aobj));
	aobj->flags = 0x40000000;
	aobj->framerate = rate;
	return aobj;
}

//803645A8
void HSD_AObjFree(HSD_AObj* aobj){
	if(aobj)
		HSD_ObjFree(aobj_def, aobj);
}

//8036530C
void HSD_AObjSetRate(HSD_AObj* aobj, float rate){
	if(aobj)
		aobj->framerate = rate;
}

//8036531C
void HSD_AObjSetRewindFrame(HSD_AObj* aobj, float frame){
	if(aobj)
		aobj->rewind_frame = frame;
}

//8036532C
void HSD_AObjSetEndFrame(HSD_AObj* aobj, float frame){
	if(aobj)
		aobj->end_frame = frame;
}

//8036533C
void HSD_AObjSetCurrentFrame(HSD_AObj* aobj, float frame){
	if(aobj && !(aobj->flags & 0x40000000){
		aobj->curr_frame = frame;
		aobj->flags = aobj->flags & 0xBFFFFFFF | 0x8000000;
		HSD_FObjReqAnimAll(aobj->fobj, frame);
	}
}

//80365390
void _HSD_AObjForgetMemory(){
	r13_4078 = NULL;
}