#include "hsd_aobj.h"

#include <math.h>

#include "hsd_jobj.h"

static u32 r13_4070 = 0; //frames elapsed
static u32 r13_4074 = 0; //conditional frames elapsed

HSD_ObjAllocData aobj_alloc_data; //804C0880

typedef struct _callback {
    struct _callback* next;
    void (*func_ptr)(void);
} callback;

static callback r13_4078;

//80363FC8
void HSD_AObjInitAllocData(void)
{
    HSD_ObjAllocInit(&aobj_alloc_data, sizeof(HSD_AObj), 4);
}

//80363FF8
HSD_ObjAllocData* HSD_AObjGetAllocData(void)
{
    return &aobj_alloc_data;
}

//80364004
u32 HSD_AObjGetFlags(HSD_AObj* aobj)
{
    return (aobj != NULL) ? aobj->flags : 0;
}

//8036401C
void HSD_AObjSetFlags(HSD_AObj* aobj, u32 flags)
{
    if (aobj != NULL)
        aobj->flags |= (flags & (AOBJ_LOOP | AOBJ_NO_UPDATE));
}

//80364038
void HSD_AObjClearFlags(HSD_AObj* aobj, u32 flags)
{
    if (aobj != NULL)
        aobj->flags &= ~flags;
}

//80364054
void HSD_AObjSetFObj(HSD_AObj* aobj, HSD_FObj* fobj)
{
    if (aobj != NULL) {
        if (aobj->fobj != NULL)
            HSD_FObjRemoveAll(aobj->fobj);
        aobj->fobj = fobj;
    }
}

//803640A0
void HSD_AObjInitEndCallBack(void)
{
    r13_4070 = 0;
    r13_4074 = 0;
}

//803640B0
void HSD_AObjInvokeCallBacks(void)
{
    if (r13_4074 != 0 && r13_4070 == 0) {
        callback* cb = &r13_4078;
        while (cb != NULL) {
            (r13_4078.func_ptr)();
            cb = cb->next;
        }
    }
}

//8036410C
void HSD_AObjReqAnim(HSD_AObj* aobj, f32 frame)
{
    if (aobj != NULL) {
        aobj->curr_frame = frame;
        aobj->flags = (aobj->flags & 0xBFFFFFFF) | AOBJ_FIRST_PLAY;
        HSD_FObjReqAnimAll(aobj->fobj, frame);
    }
}

//8036414C
void HSD_AObjStopAnim(HSD_AObj* aobj, void* obj, void (*callback)())
{
    if (aobj != NULL) {
        HSD_FObjStopAnimAll(aobj->fobj, obj, callback, aobj->framerate);
        aobj->flags |= AOBJ_NO_ANIM;
    }
}

//80364190
void HSD_AObjInterpretAnim(HSD_AObj* aobj, void* caller_obj, void (*updatecb)())
{
    f32 framerate = 0;

    if (aobj != NULL) {
        if (!(aobj->flags & AOBJ_NO_ANIM)) {
            if (aobj->flags & AOBJ_FIRST_PLAY) {
                aobj->flags &= 0xF7FFFFFF;
                framerate = 300.0f;
            } else {
                aobj->curr_frame += aobj->framerate;
            }

            if ((aobj->flags & AOBJ_LOOP) && (aobj->end_frame <= aobj->curr_frame)) {
                if (aobj->end_frame <= aobj->rewind_frame) {
                    aobj->curr_frame = aobj->end_frame;
                } else {
                    HSD_FObjStopAnimAll(aobj->fobj, caller_obj, updatecb, framerate);
                    f32 c_frame = aobj->curr_frame - aobj->rewind_frame;
                    f32 e_frame = aobj->end_frame - aobj->rewind_frame;
                    aobj->curr_frame = aobj->rewind_frame + fmodf(c_frame, e_frame);
                    HSD_FObjReqAnimAll(aobj->fobj, aobj->curr_frame);
                }
                framerate = 300.0f;
                aobj->flags |= AOBJ_REWINDED;
            } else {
                aobj->flags &= 0xFBFFFFFF;
            }

            if (aobj->flags & AOBJ_NO_UPDATE) {
                HSD_FObjInterpretAnimAll(aobj->fobj, caller_obj, updatecb, 0);
            } else {
                HSD_FObjInterpretAnimAll(aobj->fobj, caller_obj, updatecb, framerate);
            }

            if (!(aobj->flags & AOBJ_LOOP) && (aobj->end_frame <= aobj->curr_frame)) {
                HSD_FObjStopAnimAll(aobj->fobj, caller_obj, updatecb, aobj->framerate);
                aobj->flags |= AOBJ_NO_ANIM;
            }

            if (aobj->flags & AOBJ_NO_ANIM) {
                r13_4074 += 1;
            } else {
                r13_4070 += 1;
            }
        }
    }
}

//8036439C
HSD_AObj* HSD_AObjLoadDesc(HSD_AObjDesc* aobjdesc)
{
    if (aobjdesc == NULL) {
        return NULL;
    }
    HSD_AObj* aobj = HSD_AObjAlloc();
    if (aobj != NULL) {
        aobj->flags |= aobjdesc->flags & (AOBJ_LOOP | AOBJ_NO_UPDATE);
        HSD_AObjSetRewindFrame(aobj, 0.0f);
        HSD_AObjSetEndFrame(aobj, aobjdesc->end_frame);

        HSD_FObj* fobj = (HSD_FObj*)HSD_FObjLoadDesc(aobjdesc->fobjdesc);
        if (aobj->fobj != NULL){
            HSD_FObjRemoveAll(aobj->fobj);
        }
        aobj->fobj = fobj;

        u32 id = aobjdesc->obj_id;
        if (id != 0) {
            HSD_Obj* hsd_obj = (HSD_Obj*)HSD_IDGetDataFromTable(0, id, 0);
            void* obj = (void*)hsd_obj;

            if (hsd_obj != NULL) {
                hsd_obj->ref_count = hsd_obj->ref_count + 1;
                assert(hsd_obj->ref_count != HSD_OBJ_NOREF);
            } else {
                obj = (HSD_Obj*)HSD_JObjLoadJoint((HSD_JObjDesc*)aobjdesc->obj_id);
            }

            HSD_JObj* jobj = HSD_JOBJ(aobj->hsd_obj);
            if (jobj)
                HSD_JObjUnref(jobj);
            aobj->hsd_obj = obj;
        }
    }
    return aobj;
}

//803644CC
void HSD_AObjRemove(HSD_AObj* aobj)
{
    if (aobj != NULL) {
        HSD_FObj* fobj = aobj->fobj;
        if (fobj != NULL)
            HSD_FObjRemoveAll(fobj);
        aobj->fobj = NULL;

        HSD_JObj* jobj = HSD_JOBJ(aobj->hsd_obj);
        if (jobj != NULL)
            HSD_JObjUnref(jobj);
        aobj->hsd_obj = NULL;

        HSD_AObjFree(aobj);
    }
}

//8036453C
HSD_AObj* HSD_AObjAlloc(void)
{
    HSD_AObj* aobj = (HSD_AObj*)HSD_MemAlloc(sizeof(HSD_AObj)); //(HSD_ObjAlloc(&aobj_alloc_data));
    HSD_CheckAssert("AObjAlloc could not alloc", aobj != NULL);
    memset(aobj, 0, sizeof(HSD_AObj));
    aobj->flags = AOBJ_NO_ANIM;
    aobj->framerate = 1.0f;
    return aobj;
}

//803645A8
void HSD_AObjFree(HSD_AObj* aobj)
{
    if (aobj != NULL)
        HSD_ObjFree(&aobj_alloc_data, (HSD_ObjAllocLink*)aobj);
}

//8036530C
void HSD_AObjSetRate(HSD_AObj* aobj, f32 rate)
{
    if (aobj != NULL)
        aobj->framerate = rate;
}

//8036531C
void HSD_AObjSetRewindFrame(HSD_AObj* aobj, f32 frame)
{
    if (aobj != NULL)
        aobj->rewind_frame = frame;
}

//8036532C
void HSD_AObjSetEndFrame(HSD_AObj* aobj, f32 frame)
{
    if (aobj != NULL)
        aobj->end_frame = frame;
}

//8036533C
void HSD_AObjSetCurrentFrame(HSD_AObj* aobj, f32 frame)
{
    if (aobj != NULL && !(aobj->flags & AOBJ_NO_ANIM)) {
        aobj->curr_frame = frame;
        aobj->flags = (aobj->flags & 0xBFFFFFFF) | AOBJ_FIRST_PLAY;
        HSD_FObjReqAnimAll(aobj->fobj, frame);
    }
}

//80365390
void _HSD_AObjForgetMemory(void)
{
    r13_4078.next = NULL;
}
