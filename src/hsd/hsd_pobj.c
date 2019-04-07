#include "hsd_pobj.h"

#include "hsd_memory.h"

#include "hsd_aobj.h"
#include "hsd_jobj.h"
#include "hsd_memory.h"
#include "hsd_state.h"

static void PObjInfoInit(void);

HSD_PObjInfo hsdPObj = { PObjInfoInit };

static HSD_PObjInfo *default_class = NULL;

static f32 (*vertex_buffer)[3] = NULL;
static f32 (*normal_buffer)[3] = NULL;
static u32 vertex_buffer_size = 0;
static u32 normal_buffer_size = 0;

static HSD_VtxDescList *prev_vtxdesclist_array = NULL;
static HSD_VtxDescList *prev_vtxdesc = NULL;

static struct {
  void *obj;
  u32 mark;
} mtx_mark[2];

//8036B8D0
u16 HSD_PObjGetFlags(HSD_PObj* pobj){
    return (pobj != NULL) ? pobj->flags : 0;
}

//8036B8E8
void HSD_PObjRemoveAnimAllByFlags(HSD_PObj* pobj, u16 flags){   
    if (pobj != NULL){
        for (HSD_PObj *pp = pobj; pp != NULL; pp = pp->next) {
            if (pobj_type(pobj) == POBJ_SHAPEANIM && (pp->flags & POBJ_ANIM)) {
                HSD_AObjRemove(pobj->u.shape_set->aobj);
                pobj->u.shape_set->aobj = NULL;
            }
        }
    }
}

//8036B978
void HSD_PObjRemoveAnimAll(HSD_PObj* pobj, HSD_ShapeAnim* sh_anim){
    if (pobj != NULL && sh_anim != NULL){
        HSD_ShapeAnim* j = sh_anim;
        for (HSD_PObj *pp = pobj; pp; pp = pp->next) {
            assert(pobj_type(pobj) == POBJ_SHAPEANIM && pobj->u.shape_set != NULL);
            if(pp->u.shape_set->aobj != NULL){
                HSD_AObjRemove(pobj->u.shape_set->aobj);
            }
            pobj->u.shape_set->aobj = HSD_AObjLoadDesc(j->aobjdesc);
            if(j != NULL){
                j = j->next;
            }
        }
    }
}

//8036BA6C
void HSD_PObjReqAnimAllByFlags(HSD_PObj* pobj, float startframe, u32 flags){
    if(pobj != NULL){
        for (HSD_PObj* p = pobj; p != NULL && flags != 0; p = p->next){
            if(flags & POBJ_ANIM){
                HSD_AObjReqAnim(pobj->u.shape_set, startframe);
            }
        }
    }
}

//8036BAF4
static void PObjUpdateFunc(void *obj, u32 type, FObjData* val){
    HSD_PObj *pobj = HSD_POBJ(obj);
    if (pobj == NULL)
        return;
    
    if (pobj_type(pobj) == POBJ_SHAPEANIM) {
        HSD_ShapeSet* shape_set = pobj->u.shape_set;
        if (shape_set->flags & SHAPESET_ADDITIVE) {
            shape_set->blend.bp[type - HSD_A_S_W0] = val->fv;
        } else {
            shape_set->blend.bl = val->fv;
        }
    }
}

//8036BB40
void HSD_PObjAnimAll(HSD_PObj *pobj){ 
    if (pobj != NULL){
        for (HSD_PObj* pp = pobj; pp; pp = pp->next) {
            HSD_AObjInterpretAnim(pp->u.shape_set->aobj, pp, PObjUpdateFunc);
        }
    }
}

//8036BBBC
static HSD_SList* loadEnvelopeDesc(HSD_EnvelopeDesc** edesc_p){
    HSD_SList *list = NULL, **list_p = &list;
    
    if (!edesc_p) 
        return NULL;
    
    while (*edesc_p) {
        HSD_Envelope *envelope = NULL, **env_p = &envelope;
        HSD_EnvelopeDesc *edesc = *edesc_p;
        
        while (edesc->joint) {
            (*env_p) = hsdAllocMemPiece(sizeof(HSD_Envelope));
            assert(*env_p != NULL);
            memset(*env_p, 0, sizeof(HSD_Envelope));
            (*env_p)->weight = edesc->weight;
            env_p = &((*env_p)->next);
            edesc++;
        }
        
        (*list_p) = HSD_SListAlloc();
        (*list_p)->data = envelope;
        list_p = &((*list_p)->next);
        edesc_p++;
    }
    return list;
}

//8036BC9C
static HSD_ShapeSet* loadShapeSetDesc(HSD_ShapeSetDesc* sdesc){
    HSD_ShapeSet* shape_set = hsdAllocMemPiece(sizeof(HSD_ShapeSet));
    assert(shape_set);
    memset(shape_set, 0, sizeof(HSD_ShapeSet));
    shape_set->flags = sdesc->flags;
    shape_set->nb_shape = sdesc->nb_shape;
    shape_set->nb_vertex_index = sdesc->nb_vertex_index;
    shape_set->vertex_desc = sdesc->vertex_desc;
    shape_set->vertex_idx_list = sdesc->vertex_idx_list;
    shape_set->nb_normal_index = sdesc->nb_normal_index;
    shape_set->normal_desc = sdesc->normal_desc;
    shape_set->normal_idx_list = sdesc->normal_idx_list;
    if (shape_set->flags & SHAPESET_ADDITIVE) {
        shape_set->blend.bp = (f32 *)HSD_Alloc(shape_set->nb_shape * sizeof(f32));
        for (u32 i = 0; i < shape_set->nb_shape; i++){
            shape_set->blend.bp[i] = 0.0F;
        }
    }else {
        shape_set->blend.bl = 0.0F;
    }
    shape_set->aobj = NULL;
    return shape_set;
}

//8036BDA0
static s32 PObjLoad(HSD_PObj* pobj, HSD_PObjDesc *desc){
    pobj->next = HSD_PObjLoadDesc(desc->next);
    pobj->verts = desc->verts;
    pobj->flags = desc->flags;
    pobj->n_display = desc->n_display;
    pobj->display = desc->display;
    switch(pobj_type(pobj)){
        case POBJ_SHAPEANIM:
        pobj->u.shape_set = loadShapeSetDesc(desc->u.shape_set);
        break;
        
        case POBJ_ENVELOPE:
        pobj->u.envelope_list = loadEnvelopeDesc(desc->u.envelope_p);
        break;

        default:
        assert(TRUE);
    }
    return 0;
}

//8036C384
static void setupShapeAnimArrayDesc(HSD_VtxDescList* verts){
   HSD_VtxDescList* desc;
    for(desc = verts; desc->attr != GX_VA_NULL; desc++){
        if(desc->attr_type != GX_DIRECT){
            switch (desc->attr) {
                case GX_VA_NBT:
                case GX_VA_NRM:
                case GX_VA_POS:
                    break;
                default:
                    GX_SetArray(desc->attr, desc->vertex, desc->stride);
            }
        }
    }
    prev_vtxdesclist_array = NULL;
}

//8036C404
static void setupShapeAnimVtxDesc(HSD_PObj* pobj){
    GX_ClearVtxDesc();
    for (HSD_VtxDescList* desc = pobj->verts; desc->attr != GX_VA_NULL; desc++) {
        switch (desc->attr) {
            case GX_VA_NRM:
            case GX_VA_POS:
            case GX_VA_NBT:
                GX_SetVtxDesc(desc->attr, GX_DIRECT);
                GX_SetVtxAttrFmt(GX_VTXFMT0, desc->attr, desc->comp_cnt, GX_F32, 0);
            break;
            
            case GX_VA_PTNMTXIDX:
            case GX_VA_TEX0MTXIDX:
            case GX_VA_TEX1MTXIDX:
            case GX_VA_TEX2MTXIDX:
            case GX_VA_TEX3MTXIDX:
            case GX_VA_TEX4MTXIDX:
            case GX_VA_TEX5MTXIDX:
            case GX_VA_TEX6MTXIDX:
            case GX_VA_TEX7MTXIDX:
                GXSetVtxDesc(desc->attr, desc->attr_type);
                break;
            
            default:
                GX_SetVtxDesc(desc->attr, desc->attr_type);
                GXSetVtxAttrFmt(GX_VTXFMT0, desc->attr, desc->comp_cnt, desc->comp_type, desc->frac);
        }
    }
    prev_vtxdesc = NULL;
}

//8036E034
void HSD_PObjClearMtxMark(void *obj, u32 mark){
    for (u32 i = 0; i < 2; i++) {
        mtx_mark[i].obj  = obj;
        mtx_mark[i].mark = mark;
    }
}

//8036E04C
void HSD_PObjSetMtxMark(int idx,  void *obj, u32 mark){
    if (idx >= 2)
        return;
        
    if (0 <= idx && idx < 2) {
    } else {
        mtx_mark[idx].obj  = obj;
        mtx_mark[idx].mark = mark;
    }
}

//8036E080
void HSD_PObjGetMtxMark(int idx,  void **obj, u32 *mark){
    assert(obj);
    assert(mark);
    
    if (idx < 0 || 2 <= idx) {
        *obj  = NULL;
        *mark = 0;
    } else {
        *obj  = mtx_mark[idx].obj;
        *mark = mtx_mark[idx].mark;
    }
}

//8036E83C
static void PObjSetupMtx(HSD_PObj *pobj, Mtx vmtx, Mtx pmtx, u32 rendermode){
    switch (pobj_type(pobj)) {
        case POBJ_SKIN:
            if (pobj->u.jobj == NULL) {
                SetupRigidModelMtx(pobj, vmtx, pmtx, rendermode);
            } else {
                SetupSharedVtxModelMtx(pobj, vmtx, pmtx, rendermode);
            }
            break;
        case POBJ_SHAPEANIM:
            SetupRigidModelMtx(pobj, vmtx, pmtx, rendermode);
            break;
        case POBJ_ENVELOPE:
            SetupEnvelopeModelMtx(pobj, vmtx, pmtx, rendermode);
            break;
    }
}

//8036E8AC
void HSD_PObjDisp(HSD_PObj* pobj, Mtx vmtx, Mtx pmtx, u32 rendermode){
    switch(pobj->flags & (POBJ_CULLFRONT|POBJ_CULLBACK)) {
        case 0:
            HSD_StateSetCullMode(GX_CULL_NONE);
            break;
        case POBJ_CULLFRONT:
            HSD_StateSetCullMode(GX_CULL_FRONT);
            break;
        case POBJ_CULLBACK:
            HSD_StateSetCullMode(GX_CULL_BACK);
            break;
        case POBJ_CULLFRONT|POBJ_CULLBACK:
            return;
    }
    HSD_POBJ_INFO(pobj)->setup_mtx(pobj, vmtx, pmtx, rendermode);
    if(pobj_type(pobj) == POBJ_SHAPEANIM){
        setupShapeAnimArrayDesc(pobj->verts);
        setupShapeAnimVtxDesc(pobj);
        assert(pobj->u.shape_set != NULL);
        drawShapeAnim(pobj);
    }else{
        setupArrayDesc(pobj->verts);
        setupVtxDesc(pobj);
        GX_CallDispList(pobj->display, pobj->n_display << 5);
    }
}

//8036E9F0
static void PObjRelease(HSD_Class* o){
    HSD_PObj *pobj = HSD_POBJ(o);
    switch (pobj_type(pobj)) {
        case POBJ_SHAPEANIM:
            HSD_ShapeSet* shape_set = pobj->u.shape_set; 
            if (shape_set != NULL) {
                if (shape_set->flags & SHAPESET_ADDITIVE){
                    HSD_Free(shape_set->blend.bp);
                }
                HSD_AObjRemove(shape_set->aobj);
                hsdFreeMemPiece(shape_set, sizeof(HSD_ShapeSet));
            }
            break;
        case POBJ_ENVELOPE:
            HSD_EnvelopeListFree(pobj->u.envelope_list);
            break;
        case POBJ_SKIN:
            HSD_JObjUnrefThis(pobj->u.jobj);
            break;
        default:
            break;
    }
    HSD_PARENT_INFO(&hsdPObj)->release(o);
}

//8036EB14P
static void PObjAmnesia(HSD_ClassInfo* info){
    if (info == HSD_CLASS_INFO(default_class)) {
        default_class = NULL;
    }
    if (info == HSD_CLASS_INFO(&hsdPObj)) {
        if (_HSD_MemCheckOwn(vertex_buffer)) {
            vertex_buffer = NULL;
            vertex_buffer_size = 0;
            normal_buffer = NULL;
            normal_buffer_size = 0;
    }
    prev_vtxdesclist_array = NULL;
    prev_vtxdesc = NULL;
    }
    HSD_PARENT_INFO(&hsdPObj)->amnesia(info);
}

//8036EB88
static void PObjInfoInit(){
    hsdInitClassInfo(HSD_CLASS_INFO(&hsdPObj), HSD_CLASS_INFO(&hsdClass), HSD_BASE_CLASS_LIBRARY, "hsd_pobj", sizeof(HSD_PObjInfo), sizeof(HSD_PObj));
    HSD_CLASS_INFO(&hsdPObj)->release   = PObjRelease;
    HSD_CLASS_INFO(&hsdPObj)->amnesia   = PObjAmnesia;
    HSD_POBJ_INFO(&hsdPObj)->disp      = HSD_PObjDisp;
    HSD_POBJ_INFO(&hsdPObj)->setup_mtx = PObjSetupMtx;
    HSD_POBJ_INFO(&hsdPObj)->load      = PObjLoad;
    HSD_POBJ_INFO(&hsdPObj)->update    = PObjUpdateFunc;
}