#include "hsd_pobj.h"

#include "hsd_memory.h"

#include "hsd_display.h"
#include "hsd_jobj.h"
#include "hsd_state.h"

static void PObjInfoInit(void);

HSD_PObjInfo hsdPObj = { PObjInfoInit };

static HSD_PObjInfo* default_class = NULL;

static f32 (*vertex_buffer)[3] = NULL;
static f32 (*normal_buffer)[3] = NULL;
static u32 vertex_buffer_size = 0;
static u32 normal_buffer_size = 0;

static HSD_VtxDescList* prev_vtxdesclist_array = NULL;
static HSD_VtxDescList* prev_vtxdesc = NULL;

static struct {
    void* obj;
    u32 mark;
} mtx_mark[2];

//8036B8D0
u16 HSD_PObjGetFlags(HSD_PObj* pobj)
{
    return (pobj != NULL) ? pobj->flags : 0;
}

//8036B8E8
void HSD_PObjRemoveAnimAllByFlags(HSD_PObj* pobj, u32 flags)
{
    if (pobj != NULL) {
        for (HSD_PObj* pp = pobj; pp != NULL; pp = pp->next) {
            if (pobj_type(pobj) == POBJ_SHAPEANIM && (pp->flags & POBJ_ANIM)) {
                HSD_AObjRemove(pobj->u.shape_set->aobj);
                pobj->u.shape_set->aobj = NULL;
            }
        }
    }
}

//8036B978
void HSD_PObjRemoveAnimAll(HSD_PObj* pobj, HSD_SList* list)
{
    if (pobj != NULL && list != NULL) {
        HSD_SList* j = list;
        for (HSD_PObj* pp = pobj; pp != NULL; pp = pp->next) {
            assert(pobj_type(pobj) == POBJ_SHAPEANIM && pobj->u.shape_set != NULL);
            if (pp->u.shape_set->aobj != NULL) {
                HSD_AObjRemove(pobj->u.shape_set->aobj);
            }
            pobj->u.shape_set->aobj = HSD_AObjLoadDesc((HSD_AObjDesc*)j->data);
            if (j != NULL) {
                j = j->next;
            }
        }
    }
}

//8036BA6C
void HSD_PObjReqAnimAllByFlags(HSD_PObj* pobj, f32 startframe, u32 flags)
{
    if (pobj != NULL) {
        for (HSD_PObj* p = pobj; p != NULL && flags != 0; p = p->next) {
            if (flags & POBJ_ANIM) {
                HSD_AObjReqAnim(pobj->u.shape_set->aobj, startframe);
            }
        }
    }
}

//8036BAF4
static void PObjUpdateFunc(void* obj, u32 type, FObjData* val)
{
    HSD_PObj* pobj = HSD_POBJ(obj);
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
void HSD_PObjAnimAll(HSD_PObj* pobj)
{
    if (pobj != NULL) {
        for (HSD_PObj* pp = pobj; pp != NULL; pp = pp->next) {
            if (pobj_type(pobj) == POBJ_SHAPEANIM) {
                HSD_AObjInterpretAnim(pp->u.shape_set->aobj, pp, PObjUpdateFunc);
            }
        }
    }
}

//8036BBBC
static HSD_SList* loadEnvelopeDesc(HSD_EnvelopeDesc** edesc_p)
{
    HSD_SList *list = NULL, **list_p = &list;

    if (!edesc_p)
        return NULL;

    while (*edesc_p) {
        HSD_Envelope *envelope = NULL, **env_p = &envelope;
        HSD_EnvelopeDesc* edesc = *edesc_p;

        while (edesc->joint) {
            (*env_p) = hsdAllocMemPiece(sizeof(HSD_Envelope));
            assert(*env_p != NULL);
            memset(*env_p, 0, sizeof(HSD_Envelope));
            (*env_p)->weight = edesc->weight;
            env_p = &((*env_p)->next);
            edesc++;
        }

        (*list_p) = HSD_MemAlloc(sizeof(HSD_SList));
        (*list_p)->data = envelope;
        list_p = &((*list_p)->next);
        edesc_p++;
    }
    return list;
}

//8036BC9C
static HSD_ShapeSet* loadShapeSetDesc(HSD_ShapeSetDesc* sdesc)
{
    HSD_ShapeSet* shape_set = hsdAllocMemPiece(sizeof(HSD_ShapeSet));
    HSD_CheckAssert("loadShapeSetDesc unexpected null", shape_set != NULL);
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
        shape_set->blend.bp = (f32*)HSD_MemAlloc(shape_set->nb_shape * sizeof(f32));
        for (u32 i = 0; i < shape_set->nb_shape; i++) {
            shape_set->blend.bp[i] = 0.0F;
        }
    } else {
        shape_set->blend.bl = 0.0F;
    }
    return shape_set;
}

//8036BDA0
static s32 PObjLoad(HSD_PObj* pobj, HSD_PObjDesc* desc)
{
    pobj->next = HSD_PObjLoadDesc(desc->next);
    pobj->verts = desc->verts;
    pobj->flags = desc->flags;
    pobj->n_display = desc->n_display;
    pobj->display = desc->display;
    switch (pobj_type(pobj)) {
    case POBJ_SHAPEANIM:
        pobj->u.shape_set = loadShapeSetDesc(desc->u.shape_set);
        break;

    case POBJ_ENVELOPE:
        pobj->u.envelope_list = loadEnvelopeDesc(desc->u.envelope_p);
        break;

    case POBJ_SKIN:
        break;

    default:
        HSD_Halt("PObjLoad: Unexpected type");
    }
    return 0;
}

//8036BE64
HSD_PObj* HSD_PObjLoadDesc(HSD_PObjDesc* pobjdesc)
{
    if (pobjdesc != NULL) {
        HSD_PObj* pobj;
        HSD_ClassInfo* info;

        if (pobjdesc->class_name == NULL || !(info = hsdSearchClassInfo(pobjdesc->class_name))) {
            pobj = HSD_PObjAlloc();
        } else {
            pobj = (HSD_PObj*)hsdNew(info);
            HSD_CheckAssert("PObjLoadDesc could not alloc pobj", pobj != NULL);
        }
        HSD_POBJ_METHOD(pobj)->load(pobj, pobjdesc);
        return pobj;
    }
    return NULL;
}

//8036BEFC
void HSD_PObjRemoveAll(HSD_PObj* pobj)
{
    while (pobj != NULL) {
        HSD_PObj* next = pobj->next;
        if (next != NULL) {
            HSD_CLASS_METHOD(pobj)->release((HSD_Class*)pobj);
            HSD_CLASS_METHOD(pobj)->destroy((HSD_Class*)pobj);
        }
        pobj = next;
    }
}

//8036BF70
void HSD_PObjSetDefaultClass(HSD_PObjInfo* info)
{
    if (info != NULL) {
        assert(hsdIsDescendantOf(info, &hsdPObj));
    }
    default_class = info;
}

//8036BFCC
HSD_PObj* HSD_PObjAlloc()
{
    HSD_ClassInfo* info = (HSD_ClassInfo*)(default_class ? default_class : &hsdPObj);
    HSD_PObj* pobj = (HSD_PObj*)hsdNew(info);
    HSD_CheckAssert("PObjAlloc could not alloc pobj", pobj != NULL);
    return pobj;
}

//8036C028
static void resolveEnvelope(HSD_SList* list, HSD_EnvelopeDesc** edesc_p)
{
    if (list == NULL || edesc_p == NULL)
        return;

    for (; list && *edesc_p; list = list->next, edesc_p++) {
        HSD_Envelope* env = list->data;
        HSD_EnvelopeDesc* edesc = *edesc_p;

        while (env && edesc->joint) {
            HSD_JObjUnrefThis(env->jobj);
            env->jobj = HSD_IDGetDataFromTable(NULL, (HSD_ID)edesc->joint, NULL);
            HSD_CheckAssert("resolveEnvelope env->jobj is NULL", env->jobj != NULL);
            HSD_JObjRefThis(env->jobj);
            env = env->next;
            edesc++;
        }
    }
}

//8036C124
void HSD_PObjResolveRefsAll(HSD_PObj* pobj, HSD_PObjDesc* pdesc)
{
    while (pobj != NULL && pdesc != NULL) {
        switch (pobj_type(pobj)) {
        case POBJ_ENVELOPE:
            resolveEnvelope(pobj->u.envelope_list, pdesc->u.envelope_p);
            break;

        case POBJ_SKIN:
            HSD_JObjUnrefThis(pobj->u.jobj);
            pobj->u.jobj = NULL;
            if (pdesc->u.joint != NULL) {
                pobj->u.jobj = HSD_IDGetDataFromTable(NULL, (HSD_ID)pdesc->u.joint, NULL);
                assert(pobj->u.jobj);
                pobj->u.jobj->parent.ref_count_individual += 1;
                assert(pobj->u.jobj->parent.ref_count_individual != 0);
            }
            break;

        default:
            break;
        }
        pobj = pobj->next;
        pdesc = pdesc->next;
    }
}

//8036C244
void HSD_ClearVtxDesc()
{
    GX_ClearVtxDesc();
    prev_vtxdesclist_array = NULL;
    prev_vtxdesc = NULL;
}

//8036C270
static void setupArrayDesc(HSD_VtxDescList* desc_list)
{
    HSD_VtxDescList* desc;
    if (prev_vtxdesclist_array != desc_list) {
        for (desc = desc_list; desc->attr != GX_VA_NULL; desc++) {
            if (desc->attr_type != GX_DIRECT) {
                GX_SetArray(desc->attr, desc->vertex, desc->stride);
            }
        }
        prev_vtxdesclist_array = desc_list;
    }
}

//8036C2E8
static void setupVtxDesc(HSD_PObj* pobj)
{
    HSD_VtxDescList* desc;
    if (prev_vtxdesc != pobj->verts) {
        GX_ClearVtxDesc();
        for (desc = pobj->verts; desc->attr != GX_VA_NULL; desc++) {
            GX_SetVtxDesc(desc->attr, desc->attr_type);
            switch (desc->attr) {
            case GX_VA_PTNMTXIDX:
            case GX_VA_TEX0MTXIDX:
            case GX_VA_TEX1MTXIDX:
            case GX_VA_TEX2MTXIDX:
            case GX_VA_TEX3MTXIDX:
            case GX_VA_TEX4MTXIDX:
            case GX_VA_TEX5MTXIDX:
            case GX_VA_TEX6MTXIDX:
            case GX_VA_TEX7MTXIDX:
                break;

            default:
                GX_SetVtxAttrFmt(GX_VTXFMT0, desc->attr, desc->comp_cnt, desc->comp_type, desc->frac);
            }
        }
        prev_vtxdesc = pobj->verts;
    }
}

//8036C384
static void setupShapeAnimArrayDesc(HSD_VtxDescList* verts)
{
    HSD_VtxDescList* desc;
    for (desc = verts; desc->attr != GX_VA_NULL; desc++) {
        if (desc->attr_type != GX_DIRECT) {
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
static void setupShapeAnimVtxDesc(HSD_PObj* pobj)
{
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
            GX_SetVtxDesc(desc->attr, desc->attr_type);
            break;

        default:
            GX_SetVtxDesc(desc->attr, desc->attr_type);
            GX_SetVtxAttrFmt(GX_VTXFMT0, desc->attr, desc->comp_cnt, desc->comp_type, desc->frac);
        }
    }
    prev_vtxdesc = NULL;
}

//8036C4D4
static void get_shape_vertex_xyz(HSD_ShapeSet* shape_set, s32 shape_id, s32 arrayidx, f32 dst[3])
{
    u8* index_array = shape_set->vertex_idx_list[shape_id];
    s32 idx;
    void* src_base;

    if (shape_set->vertex_desc->attr_type == GX_INDEX16) {
        idx = index_array[arrayidx * 2];
        idx = (idx << 8) + index_array[arrayidx * 2 + 1];
    } else {
        idx = index_array[arrayidx];
    }

    assert(shape_set->vertex_desc->comp_cnt == GX_POS_XYZ);
    src_base = ((u8*)shape_set->vertex_desc->vertex) + idx * shape_set->vertex_desc->stride;

    if (shape_set->vertex_desc->comp_type == GX_F32) {
        memcpy(dst, src_base, sizeof(f32[3]));
    } else {
        s32 decimal_point = 1 << shape_set->vertex_desc->frac;
        switch (shape_set->vertex_desc->comp_type) {
        case GX_U8: {
            u8* src = src_base;
            dst[0] = (f32)src[0] / decimal_point;
            dst[1] = (f32)src[1] / decimal_point;
            dst[2] = (f32)src[2] / decimal_point;
        } break;

        case GX_S8: {
            s8* src = src_base;
            dst[0] = (f32)src[0] / decimal_point;
            dst[1] = (f32)src[1] / decimal_point;
            dst[2] = (f32)src[2] / decimal_point;
        } break;

        case GX_U16: {
            u16* src = src_base;
            dst[0] = (f32)src[0] / decimal_point;
            dst[1] = (f32)src[1] / decimal_point;
            dst[2] = (f32)src[2] / decimal_point;
        } break;

        case GX_S16: {
            s16* src = src_base;
            dst[0] = (f32)src[0] / decimal_point;
            dst[1] = (f32)src[1] / decimal_point;
            dst[2] = (f32)src[2] / decimal_point;
        } break;

        default:
            HSD_Halt("Unexpected vertex type.\n");
        }
    }
}

//8036C860
static void get_shape_normal_xyz(HSD_ShapeSet* shape_set, s32 shape_id, s32 arrayidx, f32 dst[3])
{
    u8* index_array = shape_set->normal_idx_list[shape_id];
    s32 idx;
    void* src_base;

    if (shape_set->normal_desc->attr_type == GX_INDEX16) {
        idx = index_array[arrayidx * 2];
        idx = (idx << 8) + index_array[arrayidx * 2 + 1];
    } else {
        idx = index_array[arrayidx];
    }

    assert(shape_set->normal_desc->comp_cnt == GX_NRM_XYZ);
    src_base = ((u8*)shape_set->normal_desc->vertex) + idx * shape_set->normal_desc->stride;

    if (shape_set->normal_desc->comp_type == GX_F32) {
        memcpy(dst, src_base, sizeof(f32[3]));
    } else {
        s32 decimal_point = 1 << shape_set->normal_desc->frac;
        switch (shape_set->normal_desc->comp_type) {
        case GX_U8: {
            u8* src = src_base;
            dst[0] = (f32)src[0] / decimal_point;
            dst[1] = (f32)src[1] / decimal_point;
            dst[2] = (f32)src[2] / decimal_point;
        } break;

        case GX_S8: {
            s8* src = src_base;
            dst[0] = (f32)src[0] / decimal_point;
            dst[1] = (f32)src[1] / decimal_point;
            dst[2] = (f32)src[2] / decimal_point;
        } break;

        case GX_U16: {
            u16* src = src_base;
            dst[0] = (f32)src[0] / decimal_point;
            dst[1] = (f32)src[1] / decimal_point;
            dst[2] = (f32)src[2] / decimal_point;
        } break;

        case GX_S16: {
            s16* src = src_base;
            dst[0] = (f32)src[0] / decimal_point;
            dst[1] = (f32)src[1] / decimal_point;
            dst[2] = (f32)src[2] / decimal_point;
        } break;

        default:
            HSD_Halt("Unexpected normal type.");
        }
    }
}

//8036CBEC
static void get_shape_nbt_xyz(HSD_ShapeSet* shape_set, s32 shape_id, s32 arrayidx, f32* dst)
{
    u8* index_array = shape_set->normal_idx_list[shape_id];
    s32 idx;
    void* src_base;

    if (shape_set->normal_desc->attr_type == GX_INDEX16) {
        idx = index_array[arrayidx * 2];
        idx = (idx << 8) + index_array[arrayidx * 2 + 1];
    } else {
        idx = index_array[arrayidx];
    }

    assert(shape_set->normal_desc->comp_cnt == GX_NRM_XYZ);
    src_base = ((u8*)shape_set->normal_desc->vertex) + idx * shape_set->normal_desc->stride;

    if (shape_set->normal_desc->comp_type == GX_F32) {
        memcpy(dst, src_base, sizeof(f32[3]));
    } else {
        s32 decimal_point = 1 << shape_set->normal_desc->frac;
        switch (shape_set->normal_desc->comp_type) {
        case GX_U8: {
            for (u32 i = 0; i < 9; i++) {
                dst[i] = (f32)((u8*)src_base)[i] / decimal_point;
            }
        } break;

        case GX_S8: {
            for (u32 i = 0; i < 9; i++) {
                dst[i] = (f32)((s8*)src_base)[i] / decimal_point;
            }
        } break;

        case GX_U16: {
            for (u32 i = 0; i < 9; i++) {
                dst[i] = (f32)((u16*)src_base)[i] / decimal_point;
            }
        } break;

        case GX_S16: {
            for (u32 i = 0; i < 9; i++) {
                dst[i] = (f32)((s16*)src_base)[i] / decimal_point;
            }
        } break;

        default:
            HSD_Halt("Unexpected normal type.");
        }
    }
}

//8036D4D0
static void interpretShapeAnimDisplayList(HSD_PObj* pobj, f32 (*vertex)[3], f32 (*normal)[3])
{
    u8* dl = pobj->display;
    s32 length = pobj->n_display << 5;

    for (u32 l = 0; l + 3 < length;) {
        s32 n = dl[1] << 8 | dl[2];
        s32 m = 3;

        if ((dl[0] & GX_OPCODE_MASK) == GX_NOP) {
            break;
        }
        GX_Begin((dl[0] & GX_OPCODE_MASK), (dl[0] & GX_VAT_MASK), n);
        for (u32 i = 0; i < n; i++) {
            for (u32 j = 0;; j++) {
                HSD_VtxDescList* desc = &pobj->verts[j];
                if (desc->attr == GX_VA_NULL) {
                    break;
                } else {
                    u16 idx = dl[m++];
                    switch (desc->attr) {
                    case GX_VA_PTNMTXIDX:
                    case GX_VA_TEX0MTXIDX:
                    case GX_VA_TEX1MTXIDX:
                    case GX_VA_TEX2MTXIDX:
                    case GX_VA_TEX3MTXIDX:
                    case GX_VA_TEX4MTXIDX:
                    case GX_VA_TEX5MTXIDX:
                    case GX_VA_TEX6MTXIDX:
                    case GX_VA_TEX7MTXIDX:
                        GX_MatrixIndex1x8(idx);
                        break;

                    case GX_VA_POS:
                        if (desc->attr_type == GX_INDEX16) {
                            idx = (idx << 8) | dl[m++];
                        }
                        GX_Position3f32(vertex[idx][0], vertex[idx][1], vertex[idx][2]);
                        break;

                    case GX_VA_NRM:
                        if (desc->attr_type == GX_INDEX16) {
                            idx = (idx << 8) | dl[m++];
                        }
                        GX_Normal3f32(normal[idx][0], normal[idx][1], normal[idx][2]);
                        break;

                    case GX_VA_NBT:
                        if (desc->attr_type == GX_INDEX16) {
                            idx = (idx << 8) | dl[m++];
                        }
                        idx *= 3;
                        GX_Normal3f32(normal[idx + 0][0], normal[idx + 0][1], normal[idx + 0][2]);
                        GX_Normal3f32(normal[idx + 1][0], normal[idx + 1][1], normal[idx + 1][2]);
                        GX_Normal3f32(normal[idx + 2][0], normal[idx + 2][1], normal[idx + 2][2]);
                        break;

                    case GX_VA_TEX0:
                    case GX_VA_TEX1:
                    case GX_VA_TEX2:
                    case GX_VA_TEX3:
                    case GX_VA_TEX4:
                    case GX_VA_TEX5:
                    case GX_VA_TEX6:
                    case GX_VA_TEX7:
                        if (desc->attr_type == GX_INDEX16) {
                            idx = (idx << 8) | dl[m++];
                            GX_TexCoord1x16(idx);
                        } else {
                            GX_TexCoord1x8(idx);
                        }
                        break;

                    case GX_VA_CLR0:
                    case GX_VA_CLR1:
                        if (desc->attr_type == GX_INDEX16) {
                            idx = (idx << 8) | dl[m++];
                            GX_Color1x16(idx);
                        } else if (desc->attr_type == GX_INDEX8) {
                            GX_Color1x8(idx);
                        } else {
                            switch (desc->comp_type) {
                            case GX_RGB565:
                            case GX_RGBA4:
                                GX_Color1u16((idx << 8) | dl[m++]);
                                break;

                            case GX_RGB8:
                            case GX_RGBA6:
                                GX_Color3u8(idx, dl[m], dl[m + 1]);
                                m += 2;
                                break;

                            case GX_RGBA8:
                            case GX_RGBX8:
                                GX_Color4u8(idx, dl[m], dl[m + 1], dl[m + 2]);
                                m += 3;
                                break;
                            }
                        }
                        break;

                    default:
                        if (desc->attr_type == GX_INDEX16) {
                            idx = (idx << 8) | dl[m++];
                        }
                        HSD_Report("attr is not supported by sysdolphin\n");
                        break;
                    }
                }
            }
        }
        GX_End();
        l += m;
        dl += m;
    }
}

//8036D7E4
static void drawShapeAnim(HSD_PObj* pobj)
{
    HSD_ShapeSet* shape_set = pobj->u.shape_set;
    f32 blend;
    s32 shape_id;
    s32 blend_nbt;

    if (vertex_buffer_size == 0) {
        vertex_buffer_size = HSD_DEFAULT_MAX_SHAPE_VERTICES;
        vertex_buffer = HSD_MemAlloc(vertex_buffer_size * sizeof(f32[3]));
    }
    assert(vertex_buffer_size >= shape_set->nb_vertex_index);

    if (shape_set->normal_desc && normal_buffer_size == 0) {
        normal_buffer_size = HSD_DEFAULT_MAX_SHAPE_NORMALS;
        normal_buffer = HSD_MemAlloc(normal_buffer_size * sizeof(f32[3]));
    }

    if (shape_set->normal_desc) {
        if (shape_set->normal_desc->attr == GX_VA_NRM) {
            assert(normal_buffer_size >= shape_set->nb_normal_index);
            blend_nbt = 0;
        } else { // GX_VA_NBT
            assert(normal_buffer_size >= shape_set->nb_normal_index * 3);
            blend_nbt = 1;
        }
    }

    if (shape_set->flags & SHAPESET_AVERAGE) {
        blend = shape_set->blend.bl;
        shape_id = min(max(0, (s32)blend), shape_set->nb_shape - 1);
        blend = min(max(0.0, blend - (f32)shape_id), 1.0);
        for (u32 i = 0; i < shape_set->nb_vertex_index; i++) {
            f32 s0[3], s1[3];
            get_shape_vertex_xyz(shape_set, shape_id, i, s0);
            get_shape_vertex_xyz(shape_set, min(shape_id + 1, shape_set->nb_shape - 1), i, s1);
            vertex_buffer[i][0] = (s1[0] - s0[0]) * blend + s0[0];
            vertex_buffer[i][1] = (s1[1] - s0[1]) * blend + s0[1];
            vertex_buffer[i][2] = (s1[2] - s0[2]) * blend + s0[2];
        }
        if (shape_set->nb_normal_index) {
            if (blend_nbt) {
                for (u32 i = 0; i < shape_set->nb_normal_index; i++) {
                    f32 s0[9], s1[9];
                    s32 idx = i * 3;
                    get_shape_nbt_xyz(shape_set, shape_id, i, s0);
                    get_shape_nbt_xyz(shape_set, min(shape_id + 1, shape_set->nb_shape - 1), i, s1);
                    for (u32 j = 0; j < 9; j++) {
                        normal_buffer[idx][j] = (s1[j] - s0[j]) * blend + s0[j];
                    }
                }
            } else {
                for (u32 i = 0; i < shape_set->nb_normal_index; i++) {
                    f32 s0[3], s1[3];
                    get_shape_normal_xyz(shape_set, shape_id, i, s0);
                    get_shape_normal_xyz(shape_set, min(shape_id + 1, shape_set->nb_shape - 1), i, s1);
                    normal_buffer[i][0] = (s1[0] - s0[0]) * blend + s0[0];
                    normal_buffer[i][1] = (s1[1] - s0[1]) * blend + s0[1];
                    normal_buffer[i][2] = (s1[2] - s0[2]) * blend + s0[2];
                }
            }
        }
    } else {
        f32* blend_bp;
        blend_bp = shape_set->blend.bp;
        for (u32 i = 0; i < shape_set->nb_vertex_index; i++) {
            get_shape_vertex_xyz(shape_set, 0, i, vertex_buffer[i]);
            for (u32 j = 0; j < shape_set->nb_shape; j++) {
                f32 b = max(0.0, blend_bp[j]);
                f32 s[3];
                get_shape_vertex_xyz(shape_set, j + 1, i, s);
                vertex_buffer[i][0] += s[0] * b;
                vertex_buffer[i][1] += s[1] * b;
                vertex_buffer[i][2] += s[2] * b;
            }
        }
        if (shape_set->nb_normal_index) {
            if (blend_nbt) {
                for (u32 i = 0; i < shape_set->nb_normal_index; i++) {
                    s32 idx = i * 3;
                    get_shape_nbt_xyz(shape_set, 0, i, normal_buffer[idx]);
                    for (u32 j = 0; j < shape_set->nb_shape; j++) {
                        f32 b = max(0.0, blend_bp[j]);
                        f32 s[9];
                        get_shape_nbt_xyz(shape_set, j + 1, i, s);
                        for (u32 k = 0; k < 9; k++) {
                            normal_buffer[idx][k] += s[k] * b;
                        }
                    }
                }
            } else {
                for (u32 i = 0; i < shape_set->nb_normal_index; i++) {
                    get_shape_normal_xyz(shape_set, 0, i, normal_buffer[i]);
                    for (u32 j = 0; j < shape_set->nb_shape; j++) {
                        f32 b = max(0.0, blend_bp[j]);
                        f32 s[3];
                        get_shape_normal_xyz(shape_set, j + 1, i, s);
                        normal_buffer[i][0] += s[0] * b;
                        normal_buffer[i][1] += s[1] * b;
                        normal_buffer[i][2] += s[2] * b;
                    }
                }
            }
        }
    }
    interpretShapeAnimDisplayList(pobj, vertex_buffer, normal_buffer);
}

//8036E034
void HSD_PObjClearMtxMark(void* obj, u32 mark)
{
    for (u32 i = 0; i < 2; i++) {
        mtx_mark[i].obj = obj;
        mtx_mark[i].mark = mark;
    }
}

//8036E04C
void HSD_PObjSetMtxMark(s32 idx, void* obj, u32 mark)
{
    if (idx >= 2)
        return;

    if (0 <= idx && idx < 2) {
    } else {
        mtx_mark[idx].obj = obj;
        mtx_mark[idx].mark = mark;
    }
}

//8036E080
void HSD_PObjGetMtxMark(s32 idx, void** obj, u32* mark)
{
    assert(obj != NULL);
    assert(mark != NULL);

    if (idx < 0 || 2 <= idx) {
        *obj = NULL;
        *mark = 0;
    } else {
        *obj = mtx_mark[idx].obj;
        *mark = mtx_mark[idx].mark;
    }
}

static u32 GetSetupFlags(HSD_JObj* jobj)
{
    u32 flags = SETUP_NONE;
    if (jobj->flags & LIGHTING) {
        flags |= SETUP_NORMAL;
    }
    if (_HSD_TObjGetCurrentByType(NULL, TEX_COORD_REFLECTION)) {
        flags |= SETUP_NORMAL | SETUP_REFLECTION;
    }
    if (_HSD_TObjGetCurrentByType(NULL, TEX_COORD_HILIGHT)) {
        flags |= SETUP_NORMAL | SETUP_HIGHLIGHT;
    }
    return flags;
}

//8036E12C
static void SetupRigidModelMtx(HSD_PObj* pobj, Mtx vmtx, Mtx pmtx, u32 rendermode)
{
    HSD_JObj* jobj;
    Mtx n;
    u32 flags;

    jobj = HSD_JObjGetCurrent();
    void* obj;
    u32 mark;

    HSD_PObjGetMtxMark(0, &obj, &mark);
    if (obj == jobj && mark == HSD_MTX_RIGID)
        return;
    HSD_PObjSetMtxMark(0, jobj, HSD_MTX_RIGID);

    GX_SetCurrentMtx(GX_PNMTX0);
    GX_LoadPosMtxImm(pmtx, GX_PNMTX0);

    flags = GetSetupFlags(jobj);

    if (flags & SETUP_NORMAL) {
        guMtxInvXpose(pmtx, n);
        if (jobj->flags & LIGHTING) {
            GX_LoadNrmMtxImm(n, GX_PNMTX0);
        }
        if (flags & SETUP_NORMAL_PROJECTION) {
            GX_LoadTexMtxImm(n, GX_TEXMTX0, GX_MTX3x4);
        }
    }
}

//8036E268
static void SetupSharedVtxModelMtx(HSD_PObj* pobj, Mtx vmtx, Mtx pmtx, u32 rendermode)
{
    HSD_JObj* jobj;
    Mtx n0, n1, m;
    u32 flags = SETUP_NONE;

    jobj = HSD_JObjGetCurrent();
    void* obj;
    u32 mark;

    HSD_PObjGetMtxMark(0, &obj, &mark);
    if (obj != jobj && mark != HSD_MTX_RIGID) {
        flags |= SETUP_JOINT0;
    }
    HSD_PObjSetMtxMark(0, jobj, HSD_MTX_RIGID);

    HSD_PObjGetMtxMark(1, &obj, &mark);
    if (obj != pobj->u.jobj && mark != HSD_MTX_RIGID) {
        flags |= SETUP_JOINT1;
    }
    HSD_PObjSetMtxMark(1, pobj->u.jobj, HSD_MTX_RIGID);
    if (flags == SETUP_NONE)
        return;

    flags |= GetSetupFlags(jobj);

    if (flags | SETUP_JOINT0) {
        GX_SetCurrentMtx(GX_PNMTX0);
        GX_LoadPosMtxImm(pmtx, GX_PNMTX0);

        if (flags & SETUP_NORMAL) {
            guMtxInvXpose(pmtx, n0);
            if (jobj->flags & LIGHTING) {
                GX_LoadNrmMtxImm(n0, GX_PNMTX0);
            }
            if (flags & SETUP_NORMAL_PROJECTION) {
                GX_LoadTexMtxImm(n0, GX_TEXMTX0, GX_MTX3x4);
            }
        }
    }

    if (flags | SETUP_JOINT1) {
        HSD_JObjSetupMatrix(pobj->u.jobj);
        guMtxConcat(vmtx, pobj->u.jobj->mtx, m);
        GX_LoadPosMtxImm(m, GX_PNMTX1);

        if (flags & SETUP_NORMAL) {
            guMtxInvXpose(m, n1);
            if (jobj->flags & LIGHTING) {
                GX_LoadNrmMtxImm(n1, GX_PNMTX1);
            }
            if (flags & SETUP_NORMAL_PROJECTION) {
                GX_LoadTexMtxImm(n1, GX_TEXMTX1, GX_MTX3x4);
            }
        }
    }
}

//8036E4C4
static void SetupEnvelopeModelMtx(HSD_PObj* pobj, Mtx vmtx, Mtx pmtx, u32 rendermode)
{
    HSD_JObj* jobj;
    HSD_SList* list;
    s32 MtxIdx = 0;
    MtxP right;
    Mtx mtx;

    jobj = HSD_JObjGetCurrent();
    HSD_PObjClearMtxMark(NULL, HSD_MTX_ENVELOPE);
    u32 flags = GetSetupFlags(jobj);
    right = _HSD_mkEnvelopeModelNodeMtx(jobj, mtx);

    for (MtxIdx = 0, list = pobj->u.envelope_list; MtxIdx < 10 && list; MtxIdx++, list = list->next) {
        Mtx mtx, tmp;
        MtxP mtxp;
        HSD_Envelope* envelope = list->data;
        s32 mtx_no = HSD_Index2PosNrmMtx(MtxIdx);

        assert(envelope);
        if (envelope->weight >= (1.0f - FLT_EPSILON)) {
            HSD_JObjSetupMatrix(envelope->jobj);
            if (right) {
                guMtxConcat(envelope->jobj->mtx, envelope->jobj->vmtx, mtx);
                mtxp = mtx;
            } else {
                mtxp = envelope->jobj->mtx;
            }
        } else {
            mtx[0][0] = mtx[0][1] = mtx[0][2] = mtx[0][3] = mtx[1][0] = mtx[1][1] = mtx[1][2] = mtx[1][3] = mtx[2][0] = mtx[2][1] = mtx[2][2] = mtx[2][3] = 0.0f;
            while (envelope) {
                HSD_JObj* jp;

                assert(envelope->jobj);
                jp = envelope->jobj;
                HSD_JObjSetupMatrix(jp);
                assert(jp->mtx);
                assert(jp->vmtx);

                guMtxConcat(jp->mtx, jp->vmtx, tmp);
                HSD_MtxScaledAdd(envelope->weight, tmp, mtx, mtx);
                envelope = envelope->next;
            }
            mtxp = mtx;
        }
        if (right) {
            guMtxConcat(mtxp, right, mtx);
        }
        guMtxConcat(vmtx, mtxp, tmp);
        GX_LoadPosMtxImm(tmp, mtx_no);

        if (flags & SETUP_NORMAL) {
            guMtxInvXpose(tmp, mtx);
            if (jobj->flags & LIGHTING) {
                GX_LoadNrmMtxImm(mtx, mtx_no);
            }
            if (flags & SETUP_NORMAL_PROJECTION) {
                GX_LoadTexMtxImm(mtx, HSD_Index2TexMtx(MtxIdx), GX_MTX3x4);
            }
        }
    }
}

//8036E83C
static void PObjSetupMtx(HSD_PObj* pobj, Mtx vmtx, Mtx pmtx, u32 rendermode)
{
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
void HSD_PObjDisp(HSD_PObj* pobj, Mtx vmtx, Mtx pmtx, u32 rendermode)
{
    switch (pobj->flags & (POBJ_CULLFRONT | POBJ_CULLBACK)) {
    case 0:
        HSD_StateSetCullMode(GX_CULL_NONE);
        break;
    case POBJ_CULLFRONT:
        HSD_StateSetCullMode(GX_CULL_FRONT);
        break;
    case POBJ_CULLBACK:
        HSD_StateSetCullMode(GX_CULL_BACK);
        break;
    case POBJ_CULLFRONT | POBJ_CULLBACK:
        return;
    }
    HSD_POBJ_METHOD(pobj)->setup_mtx(pobj, vmtx, pmtx, rendermode);
    if (pobj_type(pobj) == POBJ_SHAPEANIM) {
        setupShapeAnimArrayDesc(pobj->verts);
        setupShapeAnimVtxDesc(pobj);
        assert(pobj->u.shape_set != NULL);
        drawShapeAnim(pobj);
    } else {
        setupArrayDesc(pobj->verts);
        setupVtxDesc(pobj);
        GX_CallDispList(pobj->display, pobj->n_display << 5);
    }
}

//8036E9F0
static void PObjRelease(HSD_Class* o)
{
    HSD_PObj* pobj = HSD_POBJ(o);
    HSD_ShapeSet* shape_set = NULL;
    HSD_SList* list = NULL;
    switch (pobj_type(pobj)) {
    case POBJ_SHAPEANIM:
        shape_set = pobj->u.shape_set;
        if (shape_set != NULL) {
            if (shape_set->flags & SHAPESET_ADDITIVE) {
                HSD_Free(shape_set->blend.bp);
            }
            HSD_AObjRemove(shape_set->aobj);
            hsdFreeMemPiece(shape_set, sizeof(HSD_ShapeSet));
        }
        break;
    case POBJ_ENVELOPE:
        list = pobj->u.envelope_list;
        while (list) {
            HSD_Envelope* env = list->data;
            while (env) {
                HSD_Envelope* next = env->next;
                HSD_JObjUnrefThis(env->jobj);
                hsdFreeMemPiece(env, sizeof(HSD_Envelope));
                env = next;
            }
            list = HSD_SListRemove(list);
        }
        break;
    case POBJ_SKIN:
        HSD_JObjUnrefThis(pobj->u.jobj);
        break;
    default:
        break;
    }
    HSD_PARENT_INFO(&hsdPObj)->release(o);
}

//8036EB14
static void PObjAmnesia(HSD_ClassInfo* info)
{
    if (info == HSD_CLASS_INFO(default_class)) {
        default_class = NULL;
    }
    if (info == HSD_CLASS_INFO(&hsdPObj)) {
        vertex_buffer = NULL;
        vertex_buffer_size = 0;
        normal_buffer = NULL;
        normal_buffer_size = 0;
        prev_vtxdesclist_array = NULL;
        prev_vtxdesc = NULL;
    }
    HSD_PARENT_INFO(&hsdPObj)->amnesia(info);
}

//8036EB88
static void PObjInfoInit()
{
    hsdInitClassInfo(HSD_CLASS_INFO(&hsdPObj), HSD_CLASS_INFO(&hsdClass), HSD_BASE_CLASS_LIBRARY, "hsd_pobj", sizeof(HSD_PObjInfo), sizeof(HSD_PObj));
    HSD_CLASS_INFO(&hsdPObj)->release = PObjRelease;
    HSD_CLASS_INFO(&hsdPObj)->amnesia = PObjAmnesia;
    HSD_POBJ_INFO(&hsdPObj)->disp = HSD_PObjDisp;
    HSD_POBJ_INFO(&hsdPObj)->setup_mtx = PObjSetupMtx;
    HSD_POBJ_INFO(&hsdPObj)->load = PObjLoad;
    HSD_POBJ_INFO(&hsdPObj)->update = PObjUpdateFunc;
}
