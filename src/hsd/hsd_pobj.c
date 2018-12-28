#include "hsd_pobj.h"

static void PObjInfoInit(void);

HSD_PObjInfo hsdPObj = { PObjInfoInit };

static HSD_PObjInfo *default_class = NULL;

//8036B8D0
u16 HSD_PObjGetFlags(HSD_PObj* pobj){
    return (pobj != NULL) ? pobj->flags : 0;
}

//8036B8E8
void HSD_PObjRemoveAnimAllByFlags(HSD_PObj* pobj, u16 flags){   
    if (pobj != NULL){
        for (HSD_PObj *pp = pobj; pp; pp = pp->next) {
            if (pp->flags & POBJ_ANIM) {
                HSD_AObjRemove(pp->aobj);
                pp->aobj = NULL;
            }
        }
    }
}

//8036B978
void HSD_PObjRemoveAnimAll(HSD_PObj* pobj){ //TODO: Function is a bit longer than later versions.
    if (pobj != NULL){
        for (HSD_PObj *pp = pobj; pp; pp = pp->next) {
            if (pp->flags & POBJ_ANIM) {
                HSD_AObjRemove(pp->aobj);
                pp->aobj = NULL;
            }
        }
    }
}

//8036BA6C
void HSD_PObjReqAnimAllByFlags(HSD_PObj* pobj){
    
}

//8036BAF4
static void PObjUpdateFunc(void *obj, u32 type, FObjData* val){
    HSD_PObj *pobj = obj;
    if (pobj == NULL)
        return;
    
    if (pobj_type(pobj) == POBJ_SHAPEANIM) {
        HSD_ShapeSet *shape_set = pobj->u.shape_set;
        ShapeSetSetAnimResult(shape_set, type, val);
    }
}

//8036BB40
void HSD_PObjAnimAll(HSD_PObj *pobj){ 
    if (pobj != NULL){
        for (HSD_PObj* pp = pobj; pp; pp = pp->next) {
            HSD_AObjInterpretAnim(pp->aobj, pp, PObjUpdateFunc);
        }
    }
}

//8036BBBC
static HSD_SList* loadEnvelopeDesc(HSD_EnvelopeDesc** edesc_p){
    HSD_SList *list = NULL, **list_p = &list;
    
    if (!edesc_p) return NULL;
    
    while (*edesc_p) {
        HSD_Envelope *envelope = NULL, **env_p = &envelope;
        HSD_EnvelopeDesc *edesc = *edesc_p;
        
        while (edesc->joint) {
            (*env_p) = HSD_EnvelopeAlloc();
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
        int i;
        shape_set->blend.bp = (float *)HSD_Alloc(shape_set->nb_shape * sizeof(float));
        for (i = 0; i < shape_set->nb_shape; i++){
            shape_set->blend.bp[i] = 0.0F;
        }
    }else {
        shape_set->blend.bl = 0.0F;
    }
    return shape_set;
}