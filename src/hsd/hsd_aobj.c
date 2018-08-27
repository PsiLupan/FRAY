#include "hsd_aobj.h"

#include <math.h>

static u32 r13_4070 = 0; //frames elapsed
static u32 r13_4074 = 0; //r13_4074 - callback/conditional frames elapsed
static bool active_cb; //r13_4078

static HSD_AObj *init_aobj = NULL; //804C0880

//80363FC8
void HSD_AObjInitAllocData(){
	HSD_ObjAllocInit(init_aobj, sizeof(HSD_AObj), 4);
}

//80363FF8
HSD_AObj* HSD_AObjGetAllocData(){
	return init_aobj;
}

//80364004
u32 HSD_AObjGetFlags(HSD_AObj* aobj){
	return (aobj) ? aobj->flags : 0;
}

//8036401C
void HSD_AObjSetFlags(HSD_AObj* aobj, u32 flags){
	if(aobj)
		aobj->flags |= flags & 0x30000000;
}

//80364038
void HSD_AObjClearFlags(HSD_AObj* aobj, u32 flags){
	if(aobj)
		aobj->flags &= ~flags;
}

//80364054
void HSD_AObjSetFObj(HSD_AObj* aobj, HSD_FObj* fobj){
	if(aobj){
		if(aobj->fobj)
			HSD_FObjRemoveAll(aobj->fobj);
		aobj->fobj = fobj;
	}
}

//803640A0
void HSD_AObjInitEndCallBack(){
	r13_4070 = 0;
	r13_4074 = 0;
}

//803640B0
void HSD_AObjInvokeCallBacks(){
	if(r13_4074 && !r13_4070){
		if(active_cb)
			(void*)(r13_4074)();
	}
}

//8036410C
void HSD_AObjReqAnim(HSD_AObj *aobj, f32 frame){
	if(aobj){
		aobj->curr_frame = frame;
		aobj->flags = aobj->flags & 0xBFFFFFFF | 0x8000000;
		HSD_FObjReqAnimAll(aobj->fobj, frame);
	}
}

//8036414C
void HSD_AObjStopAnim(HSD_AObj *aobj){
	if(aobj){
		HSD_FObjStopAnimAll(aobj->fobj);
		aobj->flags |= 0x40000000u;
	}
}

//80364190
void HSD_AObjInterpretAnim(HSD_AObj *aobj, void* caller_obj, void* updatecb){
	f32 framerate = 0;
	f32 curr_frame = 0;
	
	if(aobj){
		if(!(aobj->flags & 0x40000000)){
			if(aobj->flags & 0x8000000){
				aobj->flags &= 0xF7FFFFFF;
				framerate = 300.0f;
			}else{
				framerate = aobj->framerate;
				curr_frame = aobj->curr_frame;
				curr_frame = curr_frame + framerate;
				aobj->curr_frame = curr_frame;
			}

			if((aobj->flags & 0x20000000) && (aobj->end_frame <= aobj->curr_frame)){
				if(aobj->rewind_frame >= aobj->end_frame){
					aobj->curr_frame = aobj->end_frame;
				}else{
					HSD_FObjStopAnimAll(aobj->fobj);
					f32 e_frame = aobj->end_frame - aobj->rewind_frame;
					f32 c_frame = aobj->curr_frame - aobj->rewind_frame;
					f32 r_frame = aobj->rewind_frame - (f32)fmod((f64)c_frame, (f64)e_frame);
					aobj->curr_frame = r_frame;
					HSD_FObjReqAnimAll(aobj->fobj, aobj->curr_frame);
				}
				framerate = 300.0f;
				aobj->flags |= 0x4000000u;
			}else{
				aobj->flags &= 0xFBFFFFFF;
			}
			
			if(aobj->flags & 0x10000000)
				HSD_FObjInterpretAnimAll(aobj->fobj, caller_obj, 0);
			else
				HSD_FObjInterpretAnimAll(aobj->fobj, caller_obj, updatecb);
			
			if(!(aobj->flags & 0x20000000) && (aobj->end_frame <= aobj->curr_frame)){
				HSD_FObjStopAnimAll(aobj->fobj, caller_obj, updatecb);
				aobj->flags |= 0x40000000u;
			}
			
			if(aobj->flags & 0x40000000){
				r13_4074 += 1;
			}else{
				r13_4070 += 1;
			}
		}
	}
}

//8036539C
HSD_AObj* HSD_AObjLoadDesc(HSD_AObjDesc* aobjdesc){
	if(aobjdesc){
		HSD_AObj* aobj = HSD_AObjAlloc();
		if(aobj){
			aobj->flags = aobjdesc->flags & 0x30000000;
			HSD_AObjSetRewindFrame(aobj, 0.0f);
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
HSD_AObj* HSD_AObjAlloc(){
	HSD_AObj *aobj = HSD_AOBJ(HSD_ObjAlloc(init_aobj));
	assert(aobj);
	memset(aobj, 0, sizeof(aobj));
	aobj->flags = 0x40000000;
	aobj->framerate = 1.0f;
	return aobj;
}

//803645A8
void HSD_AObjFree(HSD_AObj* aobj){
	if(aobj)
		HSD_ObjFree(init_aobj, aobj);
}

//8036530C
void HSD_AObjSetRate(HSD_AObj* aobj, f32 rate){
	if(aobj)
		aobj->framerate = rate;
}

//8036531C
void HSD_AObjSetRewindFrame(HSD_AObj* aobj, f32 frame){
	if(aobj)
		aobj->rewind_frame = frame;
}

//8036532C
void HSD_AObjSetEndFrame(HSD_AObj* aobj, f32 frame){
	if(aobj)
		aobj->end_frame = frame;
}

//8036533C
void HSD_AObjSetCurrentFrame(HSD_AObj* aobj, f32 frame){
	if(aobj && !(aobj->flags & 0x40000000){
		aobj->curr_frame = frame;
		aobj->flags = aobj->flags & 0xBFFFFFFF | 0x8000000;
		HSD_FObjReqAnimAll(aobj->fobj, frame);
	}
}

//80365390
void _HSD_AObjForgetMemory(){
	active_cb = false;
}