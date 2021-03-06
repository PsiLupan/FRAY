#include "hsd_fog.h"

static void FogInfoInit(void);
static void FogAdjInfoInit(void);

HSD_FogInfo hsdFog = { FogInfoInit }; //80407078
HSD_ObjAllocData fog_init_alloc; //8040708C

HSD_ObjAllocData fogadj_init_alloc; //804070A8
HSD_FogAdjInfo hsdFogAdj = { FogAdjInfoInit }; //804070B4

//8037D970
void HSD_FogSet(HSD_Fog* fog)
{
    GXFogAdjTbl table;

    if (fog == NULL) {
        GXColor color = { 0, 0, 0, 0 };
        GX_SetFog(0, 0.f, 0.f, 0.f, 0.f, color);
    } else {
        HSD_CObj* cobj = HSD_CObjGetCurrent();
        HSD_CheckAssert("You must specify a CObj for fog first", cobj != NULL);
        GXColor color = fog->color;
        f32 far = HSD_CObjGetFar(cobj);
        f32 near = HSD_CObjGetNear(cobj);
        GX_SetFog(fog->type, fog->start, fog->end, near, far, color);
        if (fog->fog_adj == NULL) {
            GX_SetFogRangeAdj(0, 0, NULL);
        } else {
            f32 v[6];
            GX_GetViewportv(v);
            HSD_FogAdj* fogadj = fog->fog_adj;
            s32 offset = (v[0] + (v[2] * (f32)fogadj->center));
            if (offset < 0) {
                offset = 0;
            } else if (offset > 640) {
                offset = 640;
            }

            if (fogadj->width == 0) {
                Mtx44 mtx;
                GX_LoadProjectionMtx(mtx, GX_PERSPECTIVE);
                GX_InitFogAdjTable(&table, (u16)v[2], mtx);
            }else{
                GX_InitFogAdjTable(&table, fogadj->width, fogadj->mtx);
            }
            GX_SetFogRangeAdj(GX_ENABLE, (u16)offset, &table);
        }
    }
}

//8037DC38
HSD_Fog* HSD_FogLoadDesc(HSD_FogDesc* fogdesc)
{
    HSD_Fog* fog = (HSD_Fog*)hsdNew((HSD_ClassInfo*)&hsdFog);
    assert(fog != NULL);
    HSD_FogInit(fog, fogdesc);
    if (fogdesc->fogadjdesc != 0) {
        fog->fog_adj = HSD_FogAdjLoadDesc(fogdesc->fogadjdesc);
    }
    return fog;
}

//8037DCCC
void HSD_FogInit(HSD_Fog* fog, HSD_FogDesc* desc)
{
    if (fog != NULL) {
        if (desc == NULL) {
            f32 v[6];
            GX_GetViewportv(v);
            fog->type = 2;
            fog->start = v[0];
            fog->end = v[1];
            fog->color.r = 0xff;
            fog->color.g = 0xff;
            fog->color.b = 0xff;
            fog->color.a = 0xff;
        } else {
            fog->type = desc->type;
            fog->start = desc->start;
            fog->end = desc->end;
            fog->color = desc->color;
        }
    }
}

//8037DD58
HSD_FogAdj* HSD_FogAdjLoadDesc(HSD_FogAdjDesc* fogadjdesc)
{
    HSD_FogAdj* fog_adj = (HSD_FogAdj*)hsdNew((HSD_ClassInfo*)&hsdFogAdj);
    assert(fog_adj != NULL);
    HSD_FogAdjInit(fog_adj, fogadjdesc);
    return fog_adj;
}

//8037DDD8
void HSD_FogAdjInit(HSD_FogAdj* fog_adj, HSD_FogAdjDesc* desc)
{
    if (fog_adj != NULL) {
        if (desc == NULL) {
            f32 v[6];
            GX_GetViewportv(v);
            fog_adj->width = v[1];
            fog_adj->center = fog_adj->width >> 1;
            guMtxIdentity(fog_adj->mtx);
            fog_adj->mtx[3][0] = 0.0f;
            fog_adj->mtx[3][1] = 0.0f;
            fog_adj->mtx[3][2] = 0.0f;
            fog_adj->mtx[3][3] = 1.0f;
        } else {
            fog_adj->width = desc->width;
            fog_adj->center = desc->center;
            guMtxCopy(desc->mtx, fog_adj->mtx);
        }
    }
}

//8037DE7C
void HSD_FogAddAnim(HSD_Fog* fog, HSD_AObjDesc* aodesc)
{
    if (fog != NULL) {
        if (fog->aobj != NULL) {
            HSD_AObjRemove(fog->aobj);
        }
        fog->aobj = HSD_AObjLoadDesc(aodesc);
    }
}

//8037DED0
void HSD_FogReqAnim(HSD_Fog* fog, f32 frame)
{
    HSD_FogReqAnimByFlags(fog, 0x7FF, frame);
}

//8037DEF4
void HSD_FogReqAnimByFlags(HSD_Fog* fog, u32 flags, f32 frame)
{
    if (fog != NULL && (flags & 0x200) != 0) {
        HSD_AObjReqAnim(fog->aobj, frame);
    }
}

//8037DF2C
void HSD_FogInterpretAnim(HSD_Fog* fog)
{
    if (fog != NULL) {
        HSD_AObjInterpretAnim(fog->aobj, fog, FogUpdateFunc);
    }
}

void HSD_FogAdjSetCenter(HSD_FogAdj* fog_adj, s32 center){
    if (fog_adj != NULL){
        if(center < -320){
            fog_adj->center = -320;
        }else{
            if(center < 320){
                fog_adj->center = (s16)center;
            }else{
                fog_adj->center = 320;
            }
        }
    }
}

void HSD_FogAdjSetWidth(HSD_FogAdj* fog_adj, s32 width){
    if (fog_adj != NULL){
        if(width < 1){
            fog_adj->width = 0;
        }else{
            if(width < 640){
                fog_adj->width = (s16)width;
            }else{
                fog_adj->width = 640;
            }
        }
    }
}

//8037DF60
void FogUpdateFunc(HSD_Fog* fog, u32 type, FObjData* val)
{
    if (fog != NULL) {
        switch (type) {
        case 1:
            fog->start = val->fv;
            break;

        case 2:
            fog->end = val->fv;
            break;

        case 5:
            fog->color.r = (u8)(255.f * HSD_ClampFloat(val->fv, 0, 1));
            break;

        case 6:
            fog->color.g = (u8)(255.f * HSD_ClampFloat(val->fv, 0, 1));
            break;

        case 7:
            fog->color.b = (u8)(255.f * HSD_ClampFloat(val->fv, 0, 1));
            break;

        case 8:
            fog->color.a = (u8)(255.f * HSD_ClampFloat(val->fv, 0, 1));
            break;

        case 20:
            if (fog->fog_adj != NULL) {
                HSD_FogAdjSetCenter(fog->fog_adj, (s32)val->fv);
            }
            break;
        
        case 21:
            if (fog->fog_adj != NULL){
                HSD_FogAdjSetWidth(fog->fog_adj, (s32)val->fv);
            }
            break;
        }
    }
}

//8037E04C
static void FogRelease(HSD_Class* o)
{
    HSD_Fog* fog = (HSD_Fog*)o;
    if (fog != NULL) {
        u16 ref_count = fog->parent.ref_count;
        u32 lz = __builtin_clz(0xFFFF - ref_count);
        lz = lz >> 5;
        if (lz == 0) {
            fog->parent.ref_count -= 1;
            lz = __builtin_clz(-ref_count);
            lz = lz >> 5;
        }
        if (lz != 0) {
            HSD_OBJECT_METHOD(fog)->release(o);
            HSD_OBJECT_METHOD(fog)->destroy(o);
        }
    }
    HSD_AObjRemove(fog->aobj);
    HSD_OBJECT_PARENT_INFO(&hsdFog)->release(o);
}

//8037E120
static void FogInfoInit(void)
{
    hsdInitClassInfo(HSD_CLASS_INFO(&hsdFog), HSD_CLASS_INFO(&hsdObj), HSD_BASE_CLASS_LIBRARY, "hsd_fog", sizeof(HSD_FogInfo), sizeof(HSD_Fog));
    HSD_OBJECT_INFO(&hsdFog)->release = FogRelease;
}

//8037E178
static void FogAdjInfoInit(void)
{
    hsdInitClassInfo(HSD_CLASS_INFO(&hsdFogAdj), HSD_CLASS_INFO(&hsdObj), HSD_BASE_CLASS_LIBRARY, "hsd_fogadj", sizeof(HSD_FogAdjInfo), sizeof(HSD_FogAdj));
}
