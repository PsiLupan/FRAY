#include "hsd_gobj.h"

#include "hsd_cobj.h"

#include "ext/hsd_cobj_ext.h"
#include "ext/hsd_jobj_ext.h"

#define P_LINK_MAX 63
#define GX_LINK_MAX 63

static HSD_GObjProc** slinklow_procs; //r13_3E5C
static HSD_GObjProc** slinkhigh_procs; //r13_3E60
static u32 curr_proc_prio = 0; //r13_3E64
static HSD_GObjProc* curr_gobjproc; //r13_3E68
static u32 last_s_link = 0; //r13_3E6C
static HSD_GObjProc* next_gobjproc; //r13_3E70
static HSD_GObj** plinkhigh_gobjs; //r13_3E74
static HSD_GObj** plinklow_gobjs; //r13_3E78
static HSD_GObj** highestprio_gxlink_procs; //r13_3E7C
static HSD_GObj** gxlink_procs; //r13_3E80
static HSD_GObj* current_gobj = NULL; //r13_3E84 - Really just a guess
static HSD_GObj* active_gx_gobj = NULL; //r13_3E88
static HSD_GObj* prev_gobj = NULL; //r13_3E8C
static void (**destructors)(void*); //r13_3E90 - Length is currently made up, TODO: need to explictly assign the functions to this at some point

s32 flag_array[4] = { 1, 4, 2, 0 }; //804085F0

u32 gobj_prep[3] = { 0x3F3F0200, 0, 0 }; //80408620

struct _HSD_GObjLibInitData {
    u8 p_link_max; //804CE380
    u8 gx_link_max; //804CE381
    u8 gproc_pri_max; //804CE382
    u32 unk_1; //804CE384
    u32* unk_2; //804CE388
    HSD_ObjAllocData gobj_def; //804CE38C
    HSD_ObjAllocData gobj_proc_def; //804CE3B8
} HSD_GObjLibInitData;

struct _unk_gobj_struct {
    s8 flags[4]; //804CE3E4
    u32 type; //804CE3E8
    u8 p_link; //804CE3EC
    u8 p_prio; //804CE3ED
    HSD_GObj* gobj; //804CE3F0
} unk_gobj_struct;

//80086960
BOOL GObj_IsPlayer(HSD_GObj* gobj)
{
    if (gobj != NULL) {
        if (gobj->classifier == GOBJ_CLASS_PLAYER) {
            return TRUE;
        }
    }
    return FALSE;
}

//8001CEC0
//80176BCC
//80176D18
void GObj_AnimAll_Callback(HSD_GObj* gobj)
{
    HSD_JObjAnimAll(GOBJ_HSD_JOBJ(gobj));
}

//80272D1C
BOOL GObj_IsItem(HSD_GObj* gobj)
{
    if (gobj != NULL) {
        if (gobj->classifier == GOBJ_CLASS_ITEM) {
            return TRUE;
        }
    }
    return FALSE;
}

//8031CC68
//8031D168
//8031D18C
//8031D6E4
//8031E304
//8031E6EC
//8031EE60
//8031F548
//8031F990
//8031F9B4
//803204C0
//803204E4
void GObj_AnimateJObj(HSD_GObj* gobj)
{
    HSD_JObjAnimAll(gobj->hsd_obj);
}

//80322D30
u64 GObj_GetProcFlags(u32 i, u32 j, u32 p)
{
    u64 n = i << p | j >> (0x20 - p) | j << (p + -0x20);
    return n << p;
}

//8038FAA8
void GObj_LinkProc(HSD_GObjProc* proc)
{
    HSD_GObj* gobj = proc->gobj;
    u32 s_link = proc->s_link;
    u32 p_link = gobj->p_link;
    u32 offset = (p_link + s_link * (HSD_GObjLibInitData.p_link_max + 1));
    HSD_GObjProc* prev = NULL;

    if (slinklow_procs[offset] == NULL) {
        slinklow_procs[offset] = proc;
    } else {
        for (HSD_GObj* i = proc->gobj; i != NULL; i = i->prev) {
            for (prev = i->proc; prev != NULL; prev = prev->child) {
                if (prev->s_link == s_link) {
                    HSD_GObjProc* p = slinklow_procs[offset];
                    if (p->child == prev) {
                        p->child = proc;
                    }
                    goto JMPLABEL_1;
                }
            }
        }
    }

    offset = (p_link + s_link * (HSD_GObjLibInitData.p_link_max + 1));
    do {
        BOOL isZero = p_link == 0;
        offset -= 1;
        p_link -= 1;
        if (isZero) {
            proc->next = slinkhigh_procs[s_link];
            slinkhigh_procs[s_link] = proc;
            proc->prev = NULL;
            goto JMPLABEL_2;
        }
        prev = slinklow_procs[offset];
    } while (prev == NULL);

JMPLABEL_1:
    proc->next = prev->next;
    prev->next = proc;
    proc->prev = prev;
JMPLABEL_2:
    if (proc->next != NULL) {
        proc->next->prev = proc;
    }
    proc->child = gobj->proc;
    gobj->proc = proc;
    if (unk_gobj_struct.flags[0] >> 7 == 0) {
        return;
    }
    if (proc->prev != curr_gobjproc) {
        return;
    }
    if (proc->next == next_gobjproc) {
        if (s_link == last_s_link) {
            next_gobjproc = proc;
        }
    }
}

//8038FC18
void GObj_ProcRemove(HSD_GObjProc* proc)
{
    u8 s_link;
    HSD_GObjProc* pHVar2;
    HSD_GObjProc** ppHVar3;
    u32 p_link;

    s_link = proc->s_link;
    p_link = proc->gobj->p_link;
    if (unk_gobj_struct.flags[0] < 0 && proc == next_gobjproc) {
        next_gobjproc = proc->next;
    }
    ppHVar3 = &slinklow_procs[p_link + s_link * (HSD_GObjLibInitData.p_link_max + 1)];
    if (proc == *ppHVar3) {
        pHVar2 = proc->prev;
        if (pHVar2 == NULL || pHVar2->gobj->p_link != p_link) {
            *ppHVar3 = NULL;
        } else {
            *ppHVar3 = pHVar2;
        }
    }
    if (proc->prev == NULL) {
        slinkhigh_procs[s_link] = proc->next;
    } else {
        proc->prev->next = proc->next;
    }
    if (proc->next == NULL) {
        return;
    }
    proc->next->prev = proc->prev;
}

//8038FCE4
void GObj_ProcReparent(HSD_GObjProc* proc)
{
    HSD_GObjProc* gobj_proc;
    HSD_GObjProc* temp;
    HSD_GObj* gobj;

    gobj = proc->gobj;
    GObj_ProcRemove(proc);
    gobj_proc = gobj->proc;
    if (gobj_proc == proc) {
        gobj->proc = proc->child;
    } else {
        do {
            temp = gobj_proc;
            gobj_proc = temp->child;
        } while (gobj_proc != proc);
        temp->child = proc->child;
    }
}

//8038FD54
HSD_GObjProc* GObj_CreateProcWithCallback(HSD_GObj* gobj, void (*cb)(), u8 s_prio)
{
    HSD_GObjProc* proc;

    proc = (HSD_GObjProc*)HSD_MemAlloc(sizeof(HSD_GObjProc)); //HSD_ObjAlloc(&gobj_proc_def);
    HSD_CheckAssert("GObj_CreateProcWithCallback: proc == NULL", proc != NULL);
    HSD_CheckAssert("GObj_CreateProcWithCallback: s_prio > gproc_pri_max ", s_prio <= HSD_GObjLibInitData.gproc_pri_max);
    proc->s_link = s_prio;
    proc->flags = proc->flags & 0xBF;
    proc->flags = proc->flags & 0x7F;
    proc->flags = (proc->flags & 0xCF) | 0x30;
    proc->gobj = gobj;
    proc->callback = cb;
    GObj_LinkProc(proc);
    return proc;
}

//8038FE24
void GObj_FreeProc(HSD_GObjProc* proc)
{
    if (unk_gobj_struct.flags[0] < 0 || proc != curr_gobjproc) {
        HSD_ObjFree(&HSD_GObjLibInitData.gobj_proc_def, (HSD_ObjAllocLink*)proc);
    } else {
        unk_gobj_struct.flags[0] = (unk_gobj_struct.flags[0] & 0xDF) | 0x20;
    }
    return;
}

//8038FED4
void GObj_ProcRemoveAll(HSD_GObj* gobj)
{
    HSD_GObjProc* child;
    HSD_GObjProc* proc;

    proc = gobj->proc;
    while (proc != NULL) {
        child = proc->child;
        if (unk_gobj_struct.flags[0] < 0 || proc != curr_gobjproc) {
            GObj_ProcReparent(proc);
            HSD_ObjFree(&HSD_GObjLibInitData.gobj_proc_def, (HSD_ObjAllocLink*)proc);
            proc = child;
        } else {
            unk_gobj_struct.flags[0] = (unk_gobj_struct.flags[0] & 0xDF) | 0x20;
            proc = child;
        }
    }
    return;
}

//8038FF5C
void GObj_PReorder(HSD_GObj* gobj, HSD_GObj* hiprio_gobj)
{
    gobj->prev = hiprio_gobj;
    if (hiprio_gobj != NULL) {
        gobj->next = hiprio_gobj->next;
        hiprio_gobj->next = gobj;
    } else {
        gobj->next = plinkhigh_gobjs[gobj->p_link];
        plinkhigh_gobjs[gobj->p_link] = gobj;
    }
    if (gobj->next != NULL) {
        gobj->next->prev = gobj;
    } else {
        plinklow_gobjs[gobj->p_link] = gobj;
    }
}

//8038FFB8
static HSD_GObj* CreateGObj(u32 order, u32 class, u32 p_link, u32 p_prio, HSD_GObj* p_gobj)
{
    HSD_CheckAssert("CreateGObj: p_link > p_link_max", p_link <= HSD_GObjLibInitData.p_link_max);
    HSD_GObj* gobj = (HSD_GObj*)HSD_MemAlloc(sizeof(HSD_GObj)); //HSD_ObjAlloc(&gobj_def);
    if (gobj != NULL) {
        gobj->classifier = class;
        gobj->p_link = p_link;
        gobj->gx_link = GOBJ_NOREF;
        gobj->p_priority = p_prio;
        gobj->render_priority = 0;
        gobj->obj_kind = GOBJ_NOREF;
        gobj->data_kind = GOBJ_NOREF;
        gobj->prev_gx = NULL;
        gobj->next_gx = NULL;
        gobj->proc = NULL;
        gobj->render_cb = NULL;
        gobj->gxlink_prios = 0;
        gobj->hsd_obj = NULL;
        gobj->data = NULL;
        gobj->user_data_remove_func = NULL;

        HSD_GObj* g = NULL;
        HSD_GObj* prev = p_gobj;
        switch (order) {
        case 0:
            for (g = plinklow_gobjs[gobj->p_link]; g != NULL && g->p_priority > gobj->p_priority; g = g->prev) {
                //Works backwards from lowest to highest priority till it finds the highest priority to be it's parent obj
                //Returns null if nothing is a higher priority than the current object or if there is none of that type
            }
            GObj_PReorder(gobj, g);
            break;

        case 1:
            for (g = plinkhigh_gobjs[gobj->p_link]; g != NULL && g->p_priority < gobj->p_priority; g = g->next) {
            }
            if (g != NULL) {
                g = g->prev;
            } else {
                g = plinklow_gobjs[gobj->p_link];
            }
            GObj_PReorder(gobj, g);
            break;

        case 2:
            gobj->prev = prev;
            if (p_gobj != NULL) {
                gobj->next = p_gobj->next;
                p_gobj->next = gobj;
            } else {
                gobj->next = plinkhigh_gobjs[gobj->p_link];
                plinkhigh_gobjs[gobj->p_link] = gobj;
            }
            if (gobj->next != NULL) {
                gobj->next->prev = gobj;
            } else {
                plinklow_gobjs[gobj->p_link] = gobj;
            }
            break;

        case 3:
            prev = p_gobj->prev;
            gobj->prev = prev;
            if (prev != NULL) {
                gobj->next = prev->next;
                prev->next = gobj;
            } else {
                gobj->next = plinkhigh_gobjs[gobj->p_link];
                plinkhigh_gobjs[gobj->p_link] = gobj;
            }
            if (gobj->next != NULL) {
                gobj->next->prev = gobj;
            } else {
                plinklow_gobjs[gobj->p_link] = gobj;
            }
            break;
        }
    }
    return gobj;
}

//803901F0
HSD_GObj* GObj_Create(u32 class, u32 p_link, u32 p_prio)
{
    return CreateGObj(0, class, p_link, p_prio, 0);
}

//80390228
void GObj_Free(HSD_GObj* gobj)
{
    HSD_CheckAssert("GObj_Free: gobj == NULL", gobj != NULL);
    if (((unk_gobj_struct.flags[0] >> 7) & 1) != 0 || gobj != current_gobj) {
        GObj_CallDestructor(gobj);
        GObj_CallHSDDestructor(gobj);
        GObj_ProcRemoveAll(gobj);

        if (gobj->gx_link != GOBJ_NOREF) {
            GObj_GXLinkDestructor(gobj);
        }

        if (gobj->prev != NULL) {
            gobj->prev->next = gobj->next;
        } else {
            plinkhigh_gobjs[gobj->p_link] = gobj->next;
        }
        if (gobj->next != NULL) {
            gobj->next->prev = gobj->prev;
        } else {
            plinklow_gobjs[gobj->p_link] = gobj->prev;
        }
        HSD_ObjFree(&HSD_GObjLibInitData.gobj_def, (HSD_ObjAllocLink*)gobj);
    } else {
        unk_gobj_struct.flags[0] = (unk_gobj_struct.flags[0] & 0xBF) | 0x40;
    }
}

//8039032C
void GObj_ProcUnk(s32 type, HSD_GObj* gobj, u8 p_link, u8 p_prio, HSD_GObj* gobj_2)
{
    HSD_CheckAssert("p_link > HSD_GObjLibInitData.p_link_max", p_link <= HSD_GObjLibInitData.p_link_max);
    if(unk_gobj_struct.flags[0] < 0 || gobj != current_gobj){
        HSD_GObjProc* proc = gobj->proc;
        HSD_GObjProc* child = NULL;
        for(HSD_GObjProc* curr = proc; curr != NULL; curr = proc){
            GObj_ProcRemove(curr);
            proc = curr->child;
            curr->child = child;
            child = curr;
        }
        gobj->proc = NULL;
        if (gobj->prev == NULL) {
            plinkhigh_gobjs[gobj->p_link] = gobj->next;
        }else{
            gobj->prev->next = gobj->next;
        }

        if(gobj->next == NULL){
            plinklow_gobjs[gobj->p_link] = gobj->prev;
        } else {
            gobj->next->prev = gobj->prev;
        }

        gobj->p_link = p_link;
        gobj->p_priority = p_prio;

        //TODO
    }
}

//8039063C
void GObj_GXReorder(HSD_GObj* gobj, HSD_GObj* hiprio_gobj)
{
    gobj->prev_gx = hiprio_gobj;
    if (hiprio_gobj != NULL) { //If there is a higher priority GObj, make gobj the next in GX order
        gobj->next_gx = hiprio_gobj->next_gx;
        hiprio_gobj->next_gx = gobj;
    } else { //If there's not a higher priority GObj of this type, this gobj becomes the highest priority
        gobj->next_gx = highestprio_gxlink_procs[gobj->gx_link];
        highestprio_gxlink_procs[gobj->gx_link] = gobj;
    }
    if (gobj->next_gx != NULL) {
        gobj->next_gx->prev_gx = gobj;
    } else {
        gxlink_procs[gobj->gx_link] = gobj;
    }
}

//8039069C
void GObj_SetupGXLink(HSD_GObj* gobj, void (*render_cb)(HSD_GObj*, s32), u32 gx_link, u32 priority)
{
    HSD_CheckAssert("GObj_SetupGXLink: gx_link > gx_link_max", gx_link <= HSD_GObjLibInitData.gx_link_max);
    gobj->render_cb = render_cb;
    gobj->gx_link = gx_link;
    gobj->render_priority = priority;

    HSD_GObj* i = NULL;
    for (i = gxlink_procs[gx_link]; i != NULL && (gobj->render_priority < i->render_priority); i = i->prev_gx) {
        //Works backwards from lowest to highest priority till it finds the highest priority to be it's parent obj
        //Returns null if nothing is a higher priority than the current object or if there is none of that type
    }
    GObj_GXReorder(gobj, i);
}

//8039075C
void GObj_SetupCameraGXLink(HSD_GObj* gobj, void (*render_cb)(HSD_GObj*, s32), u32 priority)
{
    gobj->render_cb = render_cb;
    gobj->gx_link = HSD_GObjLibInitData.gx_link_max + 1;
    gobj->render_priority = priority;
    HSD_GObj* i = NULL;
    for (i = gxlink_procs[gobj->gx_link]; i != NULL && (gobj->render_priority < i->render_priority); i = i->prev_gx) {
        //Works backwards from lowest to highest priority till it finds the highest priority to be it's parent obj
        //Returns null if nothing is a higher priority than the current object or if there is none of that type
    }
    GObj_GXReorder(gobj, i);
}

//8039075C
void GObj_SetupGXLink_HighestPrio_Max(HSD_GObj* gobj, void (*render_cb)(HSD_GObj*, s32), u32 priority)
{
    gobj->render_cb = render_cb;
    gobj->gx_link = HSD_GObjLibInitData.gx_link_max + 1;
    gobj->render_priority = priority;
    HSD_GObj* i = NULL;
    for (i = highestprio_gxlink_procs[gobj->gx_link]; i != NULL && (i->render_priority < gobj->render_priority); i = i->next_gx) {
        //Works backwards from highest to lowest priority till it finds the lowest priority to be it's parent obj
        //Returns null if nothing is a lower priority than the current object or if there is none of that type
    }
    if (i == NULL) {
        i = gxlink_procs[gobj->gx_link];
    } else {
        i = i->prev_gx;
    }
    GObj_GXReorder(gobj, i);
}

//8039084C
void GObj_GXLinkDestructor(HSD_GObj* gobj)
{
    HSD_CheckAssert("GObj_GXLinkDestructor: gobj->gx_link == GOBJ_NOREF", gobj->gx_link != GOBJ_NOREF);
    if (gobj->prev_gx != NULL) {
        gobj->prev_gx->next_gx = gobj->next_gx;
    } else {
        highestprio_gxlink_procs[gobj->gx_link] = gobj->next_gx;
    }
    if (gobj->next_gx != NULL) {
        gobj->next_gx->prev_gx = gobj->prev_gx;
    } else {
        gxlink_procs[gobj->gx_link] = gobj->prev_gx;
    }
    gobj->gx_link = GOBJ_NOREF;
    gobj->render_priority = 0;
    gobj->prev_gx = NULL;
    gobj->next_gx = NULL;
}

//80390A70
void GObj_InitKindObj(HSD_GObj* gobj, s8 obj_kind, void* obj_ptr)
{
    HSD_CheckAssert("GObj_InitKindObj: gobj->obj_kind != GOBJ_NOREF", gobj->obj_kind == GOBJ_NOREF);
    HSD_CheckAssert("GObj_InitKindObj: obj_ptr == NULL", obj_ptr != NULL);
    gobj->obj_kind = obj_kind;
    gobj->hsd_obj = obj_ptr;
}

//80390ADC
void* GObj_NullObj_ReturnPtr(HSD_GObj* gobj)
{
    if (gobj->obj_kind == GOBJ_NOREF) {
        return NULL;
    }
    gobj->obj_kind = GOBJ_NOREF;
    void* obj = gobj->hsd_obj;
    gobj->hsd_obj = NULL;
    return obj;
}

//80390B0C
void GObj_CallHSDDestructor(HSD_GObj* gobj)
{
    if (gobj->obj_kind != GOBJ_NOREF) {
        void (*destructor)(void*) = destructors[gobj->obj_kind];
        (*destructor)(gobj->hsd_obj);

        gobj->obj_kind = GOBJ_NOREF;
        gobj->hsd_obj = NULL;
    }
}

//80390B68
void GObj_InitKindData(HSD_GObj* gobj, s8 data_kind, void (*destructor_func)(void*), void* data_ptr)
{
    HSD_CheckAssert("GObj_InitKindData: gobj->data_kind == NULL", gobj->data_kind == GOBJ_NOREF);
    gobj->data_kind = data_kind;
    gobj->data = data_ptr;
    gobj->user_data_remove_func = destructor_func;
}

//80390BE4
void GObj_CallDestructor(HSD_GObj* gobj)
{
    if (gobj->data_kind != GOBJ_NOREF) {
        HSD_CheckAssert("GObj_CallDestructor: gobj->user_data_remove_func == NULL", gobj->user_data_remove_func != NULL);
        (*gobj->user_data_remove_func)(gobj->data);

        gobj->data_kind = GOBJ_NOREF;
        gobj->data = NULL;
    }
}

//80390CFC
void GObj_RunProcs(void)
{
    u32 unk1;
    u32 unk2;

    if(HSD_GObjLibInitData.unk_2 == NULL){
		unk1 = 0;
		unk2 = 0;
	}else{
		unk1 = HSD_GObjLibInitData.unk_2[0];
		unk2 = HSD_GObjLibInitData.unk_2[1];
	}

    curr_proc_prio += 1;
    if (curr_proc_prio > 2) {
        curr_proc_prio = 0;
    }
    for (u32 i = 0; i <= HSD_GObjLibInitData.gproc_pri_max; ++i) {
        last_s_link = i;
        HSD_GObjProc* proc = slinkhigh_procs[i];
        while (proc != NULL) {
            next_gobjproc = proc->next;
            if (((proc->flags >> 4) & 3) != curr_proc_prio) {
                proc->flags = ((curr_proc_prio << 4) & 0x30) | (proc->flags & 0xCF);
                HSD_GObj* gobj = proc->gobj;
                u64 res = GObj_GetProcFlags(0, 1, gobj->p_link);
                if (((unk2 & res) | (unk1 & (res >> 32))) == 0) {
                    if (proc->flags > -1 && ((proc->flags >> 6 & 1) == 0)) {
                        current_gobj = gobj;
                        curr_gobjproc = proc;
                        (*proc->callback)(proc->gobj);
                        next_gobjproc = proc->next;

                        if (*((u32*)(&unk_gobj_struct.flags[0])) != 0) {
                            *((u32*)(&unk_gobj_struct.flags[0])) = (*((u32*)(&unk_gobj_struct.flags[0])) & 0x7FFFFFFF) | 0x80000000;
                            if ((unk_gobj_struct.flags[0] >> 6 & 1) == 0) {
                                if ((unk_gobj_struct.flags[0] >> 4 & 1) != 0) {
                                    //FUN_8039032c(unk_gobj_struct.type, proc->gobj, unk_gobj_struct.p_link, unk_gobj_struct.p_prio, unk_gobj_struct.gobj);
                                }
                                if ((unk_gobj_struct.flags[0] >> 5 & 1) != 0) {
                                    GObj_FreeProc(proc);
                                }
                            } else {
                                GObj_Free(proc->gobj);
                            }
                            *((u32*)(&unk_gobj_struct.flags[0])) = 0;
                        }

                        current_gobj = NULL;
                        curr_gobjproc = NULL;
                    }
                }
            }
            proc = next_gobjproc;
        }
    }
}

//80390EB8
s32 GObj_GetFlagFromArray(s32 offset)
{
    return flag_array[offset];
}

//80390ED0
void GObj_SetTextureCamera(HSD_GObj* gobj, u32 iters)
{
    HSD_GObj* curr;
    u64 max_proc;

    u32 i = 0;
    while (true) {
        if (iters == 0)
            break;
        if ((iters & 1) != 0) {
            u32 j = 0;
            max_proc = gobj->gxlink_prios;
            while (true) {
                if (max_proc == 0)
                    break;
                if ((max_proc & 1) != 0) {
                    curr = highestprio_gxlink_procs[j];
                    while (curr != NULL) {
                        if (curr->render_cb != NULL) {
                            HSD_GObj* temp = prev_gobj;
                            prev_gobj = curr;
                            curr->render_cb(curr, i);
                            prev_gobj = temp;
                        }
                        curr = curr->next_gx;
                    }
                }
                ++j;
                max_proc = max_proc >> 1;
            }
        }
        iters = iters >> 1;
        ++i;
    }
}

//80390FC0
void GObj_RunGXLinkMaxCallbacks(void)
{
    HSD_GObj* gobj = highestprio_gxlink_procs[GX_LINK_MAX + 1];
    for (HSD_GObj* i = gobj; i != NULL; i = i->next_gx) {
        if (i->render_cb != NULL) {
            HSD_GObj* last_active = active_gx_gobj;
            active_gx_gobj = i;
            i->render_cb(i, 0);
            active_gx_gobj = last_active;
        }
    }
}

//803910D8
void GObj_SetCamera(HSD_GObj* gobj)
{
    BOOL res = HSD_CObjSetCurrent(GOBJ_HSD_COBJ(gobj));
    if (res == FALSE) {
        GObj_SetTextureCamera(gobj, 7);
        HSD_CObjEndCurrent();
    }
}

//80391260
/*void FUN_80391260(int iParm1)
{
    int iVar1;
    char cVar2;
    int in_r13;
  
    iVar1 = GObj_803912A8(iParm1,(int *)&DAT_80408610);
    cVar2 = (char)iVar1;
    *(char *)(in_r13 + -0x3e55) = cVar2;
    *(char *)(in_r13 + -0x3e56) = cVar2 + '\x01';
    *(char *)(in_r13 + -0x3e57) = cVar2 + '\x02';
    *(char *)(in_r13 + -0x3e58) = cVar2 + '\x03';
    return;
}*/

//803912A8
u32 GObj_803912A8(u32 array[], u32* unk)
{
    return 0;
}

//803912E0
void GObj_InitializeLibInitData(u32* cb)
{
    cb[0] = gobj_prep[0];
    cb[1] = gobj_prep[1];
    cb[2] = gobj_prep[2];
}

typedef struct _List {
    struct _List* next;
    u8 data;
} List;

//80391304
void GObj_Init(u32* param_1)
{
    *((u32*)&HSD_GObjLibInitData.p_link_max) = param_1[0];

    plinkhigh_gobjs = HSD_MemAlloc(HSD_GObjLibInitData.gx_link_max * 4);
    plinklow_gobjs = HSD_MemAlloc(HSD_GObjLibInitData.gx_link_max * 4);
    for(u32 i = 0; i < HSD_GObjLibInitData.gx_link_max; i++){
        plinklow_gobjs[i] = NULL;
        plinkhigh_gobjs[i] = NULL;
    }

    highestprio_gxlink_procs = HSD_MemAlloc((HSD_GObjLibInitData.gx_link_max + 2) * 4);
    gxlink_procs = HSD_MemAlloc((HSD_GObjLibInitData.gx_link_max + 2) * 4);
    for(u32 i = 0; i < HSD_GObjLibInitData.gx_link_max + 2; i++){
        gxlink_procs[i] = NULL;
        highestprio_gxlink_procs[i] = NULL;
    }

    u32 n = HSD_GObjLibInitData.gproc_pri_max + 1;
    slinkhigh_procs = HSD_MemAlloc((n) * 4);
    for(u32 i = 0; i < n; i++){
        slinkhigh_procs[i] = NULL;
    }

    slinklow_procs = HSD_MemAlloc((HSD_GObjLibInitData.gx_link_max) * n * 4);
    for(u32 i = 0; i < (HSD_GObjLibInitData.gx_link_max * n); i++){
        slinklow_procs[i] = NULL;
    }

    HSD_ObjAllocInit((HSD_ObjAllocData *)&HSD_GObjLibInitData.gobj_def, sizeof(HSD_GObj), 4);
    HSD_ObjAllocInit((HSD_ObjAllocData *)&HSD_GObjLibInitData.gobj_proc_def, sizeof(HSD_GObjProc), 4);

    List* list = (List*)param_1[1];
    u32 j = 0;
    while (list != NULL){
        u8 data = list->data;
        list = list->next;
        j += data;
    }

    if(j == 0){
        destructors = NULL;
    }else{
        //TODO
    }

    curr_proc_prio = 0;
    current_gobj = NULL;
    curr_gobjproc = NULL;
    *((u32*)&unk_gobj_struct.flags[0]) = 0;
    active_gx_gobj = NULL;
    prev_gobj = NULL;
}
