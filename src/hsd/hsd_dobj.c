#include "hsd_dobj.h"

static void DObjInfoInit(void);

HSD_DObjInfo hsdDObj = { DObjInfoInit }; //80405450

static HSD_DObj* current_dobj = NULL; //-0x40FC(r13)

static HSD_DObjInfo* default_class = NULL; //r13_4100

//8035DD98
void HSD_DObjSetCurrent(HSD_DObj* dobj)
{
    current_dobj = dobj;
}

//8035DDA0
u32 HSD_DObjGetFlags(HSD_DObj* dobj)
{
    if (dobj != NULL) {
        return dobj->flags;
    }
    return 0;
}

//8035DDB8
void HSD_DObjSetFlags(HSD_DObj* dobj, u32 flags)
{
    if (dobj != NULL) {
        dobj->flags |= flags;
    }
}

//8035DDD0
void HSD_DObjClearFlags(HSD_DObj* dobj, u32 flags)
{
    if (dobj != NULL) {
        dobj->flags &= ~flags;
    }
}

//8035DDE8
void HSD_DObjModifyFlags(HSD_DObj* dobj, u32 flags_1, u32 flags_2)
{
    if (dobj != NULL) {
        dobj->flags = (dobj->flags & ~flags_2) | (flags_1 & flags_2);
    }
}

//8035DE08
void HSD_DObjRemoveAnimAllByFlags(HSD_DObj* dobj, u32 flags)
{
    if (dobj != NULL) {
        for (HSD_DObj* i = dobj; i != NULL; i = i->next) {
            if ((flags & 2) != 0) {
                HSD_AObjRemove(dobj->aobj);
                dobj->aobj = NULL;
            }
            HSD_PObjRemoveAnimAllByFlags(dobj->pobj, flags);
            HSD_MObjRemoveAnimByFlags(dobj->mobj, flags);
        }
    }
}

//8035DEA0
void HSD_DObjAddAnimAll(HSD_DObj* dobj, HSD_MatAnim* mat_anim, HSD_ShapeAnimDObj* sh_anim)
{
    HSD_DObj* i;
    HSD_MatAnim* m;
    HSD_ShapeAnimDObj* sh;

    if (dobj != NULL) {
        for (i = dobj, m = mat_anim, sh = sh_anim; i != NULL; i = i->next, m = m->next, sh = sh->next) {
            HSD_ShapeAnim* shapeanim = NULL;
            if (sh != NULL) {
                shapeanim = sh->shapeanim;
            }
            HSD_PObjAddAnimAll(i->pobj, shapeanim);
            HSD_MObjAddAnim(i->mobj, m);
        }
    }
}

//8035DF58
void HSD_DObjReqAnimAllByFlags(HSD_DObj* dobj, u32 flags, f32 frame)
{
    if (dobj != NULL) {
        for (HSD_DObj* i = dobj; i != NULL; i = i->next) {
            HSD_PObjReqAnimAllByFlags(i->pobj, flags, frame);
            HSD_MObjReqAnimByFlags(i->mobj, flags, frame);
        }
    }
}

//8035DFD8
void HSD_DObjReqAnimAll(HSD_DObj* dobj, f32 frame)
{
    if (dobj != NULL) {
        for (HSD_DObj* i = dobj; i != NULL; i = i->next) {
            HSD_PObjReqAnimAllByFlags(i->pobj, 0x7FF, frame);
            HSD_MObjReqAnimByFlags(i->mobj, 0x7FF, frame);
        }
    }
}

//8035E04C
void HSD_DObjAnimAll(HSD_DObj* dobj)
{
    if (dobj != NULL) {
        for (HSD_DObj* i = dobj; i != NULL; i = i->next) {
            HSD_PObjAnimAll(i->pobj);
            HSD_MObjAnim(i->mobj);
        }
    }
}

//8035E0A4
static int DObjLoad(HSD_DObj* dobj, HSD_DObjDesc* desc)
{
    dobj->next = HSD_DObjLoadDesc(desc->next);
    dobj->mobj = HSD_MObjLoadDesc(desc->mobjdesc);
    dobj->pobj = HSD_PObjLoadDesc(desc->pobjdesc);

    if (dobj->mobj != NULL) {
        u32 rendermode = dobj->mobj->rendermode & 0x60000000;
        switch (rendermode) {
        case 0:
            dobj->flags = (dobj->flags & 0xfffffff1) | 2;
            break;
        case 0x40000000:
            dobj->flags = (dobj->flags & 0xfffffff1) | 8;
            break;
        case 0x60000000:
            dobj->flags = (dobj->flags & 0xfffffff1) | 4;
            break;
        default:
            HSD_Halt("Unexpected blendflags in MObj");
        }
    }
    return 0;
}

//8035E1B4
HSD_DObj* HSD_DObjLoadDesc(HSD_DObjDesc* desc)
{
    if (desc != NULL) {
        HSD_DObj* dobj = HSD_DObjAlloc();
        HSD_DOBJ_METHOD(dobj)->load(dobj, desc);
        return dobj;
    }
    return NULL;
}

//8035E24C
void HSD_DObjRemoveAll(HSD_DObj* dobj)
{
    if (dobj != NULL) {
        for (HSD_DObj* i = dobj; i != NULL; i = dobj) {
            dobj = i->next;
            HSD_CLASS_METHOD(i)->release((HSD_Class*)i);
            HSD_CLASS_METHOD(i)->destroy((HSD_Class*)i);
        }
    }
}

//8035E2C0
HSD_DObj* HSD_DObjAlloc(void)
{
    HSD_ClassInfo* info = (HSD_ClassInfo*)(default_class ? default_class : &hsdDObj);
    HSD_DObj* dobj = hsdNew(info);
    HSD_CheckAssert("DObjAlloc could not alloc DObj", dobj != NULL);
    return dobj;
}

//8035E31C
void HSD_DObjResolveRefsAll(HSD_DObj* dobj, HSD_DObjDesc* desc)
{
    HSD_DObj* dj = dobj;
    HSD_DObjDesc* dd = desc;
    for (; dj != NULL && dd != NULL; dj = dj->next, dd = dd->next) {
        HSD_PObjResolveRefsAll(dj->pobj, dd->pobjdesc);
    }
}

//8035E388
void HSD_DObjDisp(HSD_DObj* dobj, Mtx vmtx, Mtx pmtx, u32 rendermode)
{
    HSD_MObjSetCurrent(dobj->mobj);
    if ((rendermode & 0x4000000) == 0) {
        HSD_MOBJ_METHOD(dobj->mobj)->setup(dobj->mobj, rendermode);
    }
    for (HSD_PObj* p = dobj->pobj; p != NULL; p = p->next) {
        HSD_POBJ_METHOD(p)->disp(p, vmtx, pmtx, rendermode);
    }
    if ((rendermode & 0x4000000) == 0) {
        HSD_MOBJ_METHOD(dobj->mobj)->unset(dobj->mobj, rendermode);
    }
    HSD_MObjSetCurrent(NULL);
}

//8035E440
static void DObjRelease(HSD_Class* o)
{
    HSD_DObj* dobj = (HSD_DObj*)o;

    HSD_MObjRemove(dobj->mobj);
    HSD_PObjRemoveAll(dobj->pobj);
    HSD_AObjRemove(dobj->aobj);

    HSD_PARENT_INFO(&hsdDObj)->release(o);
}

//8035E49C
static void DObjAmnesia(HSD_ClassInfo* info)
{
    if (info == HSD_CLASS_INFO(default_class)) {
        default_class = NULL;
    }
    HSD_PARENT_INFO(&hsdDObj)->amnesia(info);
}

//8035E4E4
static void DObjInfoInit(void)
{
    hsdInitClassInfo(HSD_CLASS_INFO(&hsdDObj), HSD_CLASS_INFO(&hsdClass), HSD_BASE_CLASS_LIBRARY, "hsd_dobj", sizeof(HSD_DObjInfo), sizeof(HSD_DObj));

    HSD_PARENT_INFO(&hsdDObj)->release = DObjRelease;
    HSD_PARENT_INFO(&hsdDObj)->amnesia = DObjAmnesia;
    HSD_DOBJ_INFO(&hsdDObj)->disp = HSD_DObjDisp;
    HSD_DOBJ_INFO(&hsdDObj)->load = DObjLoad;
}
