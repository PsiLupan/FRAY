#include "hsd_cobj.h"

#include <math.h>

#include "hsd_display.h"
#include "hsd_memory.h"

static void CObjInfoInit(void);

static HSD_CObjInfo hsdCObj = { CObjInfoInit };

static HSD_CObjInfo* default_class = NULL;

static HSD_CObj* current_cobj = NULL; //r13_4044

//803676F8
void HSD_CObjEraseScreen(HSD_CObj* cobj, s32 enable_color, s32 enable_alpha, s32 enable_depth)
{
    if (cobj != NULL && (enable_color != 0 || enable_alpha != 0 || enable_depth != 0)) {
        f32 far = HSD_CObjGetFar(cobj);
        f32 near = HSD_CObjGetNear(cobj);
        f64 constant = 0.5;
        f64 z_val = (constant * (near + far));

        f32 right_res = 0.0f; //f29
        f32 left_res = 0.0f; //f30
        f32 top_res = 0.0f; //f28
        f32 bottom_res = 0.0f; //f27

        u8 proj_type = HSD_CObjGetProjectionType(cobj);
        if (proj_type == PROJ_FRUSTRUM) {
            f32 val = z_val / near;
            right_res = val * cobj->projection_param.frustrum.right;
            left_res = val * cobj->projection_param.frustrum.left;
            top_res = val * cobj->projection_param.frustrum.top;
            bottom_res = val * cobj->projection_param.frustrum.bottom;
        } else if (proj_type == PROJ_PERSPECTIVE) {
            top_res = (z_val * tan(0.5f * DegToRad(cobj->projection_param.perspective.fov)));
            bottom_res = -top_res;
            right_res = cobj->projection_param.perspective.aspect * top_res;
            left_res = -right_res;
        } else if (proj_type == PROJ_ORTHO) {
            right_res = cobj->projection_param.ortho.right;
            left_res = cobj->projection_param.ortho.left;
            top_res = cobj->projection_param.ortho.top;
            bottom_res = cobj->projection_param.ortho.bottom;
        }

        HSD_EraseRect(top_res, bottom_res, left_res, right_res, -z_val, enable_color, enable_alpha, enable_depth);
    }
}

//80367874
void HSD_CObjRemoveAnim(HSD_CObj* cobj)
{
    if (cobj != NULL) {
        HSD_AObjRemove(cobj->aobj);
        cobj->aobj = NULL;
        HSD_WObjRemoveAnim(HSD_CObjGetEyePositionWObj(cobj));
        HSD_WObjRemoveAnim(HSD_CObjGetInterestWObj(cobj));
    }
}

//803678CC
void HSD_CObjAddAnim(HSD_CObj* cobj, HSD_CameraAnim* anim)
{
    if (cobj != NULL && anim != NULL) {
        if (cobj->aobj != NULL)
            HSD_AObjRemove(cobj->aobj);

        cobj->aobj = HSD_AObjLoadDesc(anim->aobjdesc);
        HSD_WObjAddAnim(HSD_CObjGetEyePositionWObj(cobj), anim->eye_anim);
        HSD_WObjAddAnim(HSD_CObjGetInterestWObj(cobj), anim->interest_anim);
    }
}

//80367948
void CObjUpdateFunc(HSD_CObj* cobj, u32 type, FObjData* val)
{
    if (cobj != NULL) {
        guVector vec;
        switch (type) {
        case 1:
            HSD_CObjGetEyePosition(cobj, &vec);
            vec.x = val->fv;
            HSD_CObjSetEyePosition(cobj, vec);
            break;

        case 2:
            HSD_CObjGetEyePosition(cobj, &vec);
            vec.y = val->fv;
            HSD_CObjSetEyePosition(cobj, vec);
            break;

        case 3:
            HSD_CObjGetEyePosition(cobj, &vec);
            vec.z = val->fv;
            HSD_CObjSetEyePosition(cobj, vec);
            break;

        case 5:
        case 6:
        case 7:
            HSD_CObjGetInterest(cobj, &vec);
            vec.x = val->fv;
            HSD_CObjSetInterest(cobj, vec);
            break;

        case 9:
            HSD_CObjSetRoll(cobj, val->fv);
            break;

        case 10:
            HSD_CObjSetFov(cobj, val->fv);
            break;

        case 11:
            HSD_CObjSetNear(cobj, val->fv);
            break;

        case 12:
            HSD_CObjSetFar(cobj, val->fv);
            break;

        default:
            break;
        }
    }
}

//80367AB8
void HSD_CObjAnim(HSD_CObj* cobj)
{
    if (cobj != NULL) {
        HSD_AObjInterpretAnim(cobj->aobj, cobj, CObjUpdateFunc);
        HSD_WObjInterpretAnim(cobj->eye_position);
        HSD_WObjInterpretAnim(cobj->interest);
    }
}

//80367B08
void HSD_CObjReqAnim(HSD_CObj* cobj, f32 frame)
{
    if (cobj != NULL) {
        HSD_AObjReqAnim(cobj->aobj, frame);
        HSD_WObjReqAnim(cobj->eye_position, frame);
        HSD_WObjReqAnim(cobj->interest, frame);
    }
}

//80367B68
u8 makeProjectionMtx(HSD_CObj* cobj, MtxP mtx)
{
    u8 isOrtho = 0;
    switch (cobj->projection_type) {
    case PROJ_PERSPECTIVE:
        guPerspective(mtx, cobj->projection_param.perspective.fov, cobj->projection_param.perspective.aspect,
            cobj->near, cobj->far);
        break;

    case PROJ_FRUSTRUM:
        guFrustum(mtx, cobj->projection_param.frustrum.top, cobj->projection_param.frustrum.bottom, cobj->projection_param.frustrum.left,
            cobj->projection_param.frustrum.right, cobj->near, cobj->far);
        break;

    case PROJ_ORTHO:
        isOrtho = 1;
        guOrtho(mtx, cobj->projection_param.ortho.top, cobj->projection_param.ortho.bottom, cobj->projection_param.ortho.left,
            cobj->projection_param.ortho.right, cobj->near, cobj->far);
        break;

    default:
        HSD_Halt("Bad projection type!");
    }
    return isOrtho;
}

//80367C28
static BOOL setupNormalCamera(HSD_CObj* cobj)
{
    f32 left = (f32)cobj->viewport_left;
    f32 right = (f32)cobj->viewport_right;
    f32 top = (f32)cobj->viewport_top;
    f32 bottom = (f32)cobj->viewport_bottom;
    //if (DAT_804c1d98 == 0) {
    GX_SetViewport_Wrapper(left, top, right, bottom, 0.f, 1.f);
    //}
    /*else{
        GX_SetViewportJitter(left, right, top, bottom, 0.f, 1.f, VIDEO_GetNextField());
    }
    */

    u32 s_top = cobj->scissor_top;
    u32 s_left = cobj->scissor_left;
    u32 s_right = cobj->scissor_right;
    u32 s_bottom = cobj->scissor_bottom;
    GX_SetScissor(s_right, s_bottom, s_left, s_top);

    Mtx44 mtx;
    BOOL ortho = FALSE;
    switch (cobj->projection_type) {
    case PROJ_PERSPECTIVE:
        guPerspective(mtx, cobj->projection_param.perspective.fov, cobj->projection_param.perspective.aspect, cobj->near, cobj->far);
        break;

    case PROJ_FRUSTRUM:
        guFrustum(mtx, cobj->projection_param.frustrum.top, cobj->projection_param.frustrum.bottom, cobj->projection_param.frustrum.left,
            cobj->projection_param.frustrum.right, cobj->near, cobj->far);
        break;

    case PROJ_ORTHO:
        ortho = TRUE;
        guOrtho(mtx, cobj->projection_param.ortho.top, cobj->projection_param.ortho.bottom, cobj->projection_param.ortho.left,
            cobj->projection_param.ortho.right, cobj->near, cobj->far);
        break;
    }
    GX_LoadProjectionMtx(mtx, ortho);
    return TRUE;
}

//80368458
BOOL HSD_CObjSetCurrent(HSD_CObj* cobj)
{
    if (cobj == NULL) {
        return FALSE;
    }
    HSD_RenderPass pass = HSD_GetCurrentRenderPass();
    current_cobj = cobj;
    _HSD_ZListClear();
    BOOL res = FALSE;
    switch (pass) {
    case HSD_RP_SCREEN:
        res = setupNormalCamera(cobj);
        break;

    case HSD_RP_TOPHALF:
        HSD_Report("setupTopHalfCamera not implemented");
        //res = setupTopHalfCamera(cobj);
        break;

    case HSD_RP_BOTTOMHALF:
        HSD_Report("setupBottomHalfCamera not implemented");
        //res = setupBottomHalfCamera(cobj);
        break;

    case HSD_RP_OFFSCREEN:
        GX_SetViewport_Wrapper(cobj->viewport_left, cobj->viewport_top,
            cobj->viewport_right - cobj->viewport_left,
            cobj->viewport_bottom - cobj->viewport_top,
            0.f, 1.f);
        GX_SetScissor(cobj->scissor_left, cobj->scissor_top,
            cobj->scissor_right - cobj->scissor_left,
            cobj->scissor_bottom - cobj->scissor_top);
        Mtx44 mtx = { 0 };
        u8 type = makeProjectionMtx(cobj, mtx);
        GX_LoadProjectionMtx(mtx, type);
        res = TRUE;
        break;

    default:
        HSD_Halt("Unknown renderpass");
    }

    if (res == FALSE) {
        return FALSE;
    } else {
        if ((cobj->flags & 2) == 0) {
            if (HSD_CObjMtxIsDirty(cobj)) {
                guVector eye_pos;
                guVector up;
                guVector interest;
                HSD_CObjGetEyePosition(cobj, &eye_pos);
                HSD_CObjGetUpVector(cobj, &up);
                HSD_CObjGetInterest(cobj, &interest);
                guLookAt(cobj->view_mtx, &eye_pos, &up, &interest);
                cobj->eye_position->flags = cobj->eye_position->flags & 0xFFFFFFFD;
                cobj->interest->flags = cobj->interest->flags & 0xFFFFFFFD;
                HSD_CObjClearFlags(cobj, 0x40000000);
                HSD_CObjSetFlags(cobj, 0x80000000);
            }
        }
    }
    return TRUE;
}

//80368608
void HSD_CObjEndCurrent(void)
{
    _HSD_ZListSort();
    _HSD_ZListDisp();
}

//8036862C
HSD_WObj* HSD_CObjGetInterestWObj(HSD_CObj* cobj)
{
    assert(cobj != NULL);
    return cobj->interest;
}

//8036866C
HSD_WObj* HSD_CObjGetEyePositionWObj(HSD_CObj* cobj)
{
    assert(cobj != NULL);
    return cobj->eye_position;
}

//803686AC
void HSD_CObjGetInterest(HSD_CObj* cobj, guVector* interest)
{
    assert(cobj != NULL);
    HSD_WObjGetPosition(cobj->interest, interest);
}

//80368718
void HSD_CObjSetInterest(HSD_CObj* cobj, guVector interest)
{
    assert(cobj != NULL);
    HSD_WObjSetPosition(cobj->interest, &interest);
}

//80368784
void HSD_CObjGetEyePosition(HSD_CObj* cobj, guVector* pos)
{
    assert(cobj != NULL);
    HSD_WObjGetPosition(cobj->eye_position, pos);
}

//803687F0
void HSD_CObjSetEyePosition(HSD_CObj* cobj, guVector pos)
{
    assert(cobj != NULL);
    HSD_WObjSetPosition(cobj->eye_position, &pos);
}

static s32 vec_normalize_check(guVector* src, guVector* dst)
{
    if (src == NULL || dst == NULL) {
        return -1;
    } else if (FLT_MIN < fabs(src->x)
        || FLT_MIN < fabs(src->y)
        || FLT_MIN < fabs(src->z)) {
        guVecNormalize(src);
        dst = src;
        return 0;
    }
    return -1;
}


static s32 roll2upvec(HSD_CObj* cobj, f32 roll, guVector* up)
{
    guVector eye, v0, v1;
    Mtx m;

    if (HSD_CObjGetEyeVector(cobj, &eye) != 0) {
        if (0.0001 <= 1.0 - fabsf(eye.y)) {
            f64 xz = sqrtf((eye.x * eye.x) + (eye.z * eye.z));
            v0.y = xz;
            v0.x = eye.x * (-eye.y / v0.y);
            v0.z = eye.z * (-eye.y / v0.y);
        } else {
            f64 yz = sqrtf((eye.y * eye.y) + (eye.z * eye.z));
            v0.x = yz;
            v0.y = eye.y * (-eye.x / v0.x);
            v0.z = eye.z / (-eye.x / v0.x);
        }
        guMtxRotAxisRad(m, &v0, -roll);
        guVecMultiplySR(m, &v0, &v1);
        up->x = v1.x;
        up->y = v1.y;
        up->z = v1.z;
        guVecNormalize(up);
        return 1;
    }
    return 0;
}


//8036885C
s32 HSD_CObjGetEyeVector(HSD_CObj* cobj, guVector* vec)
{
    if (cobj != NULL && cobj->eye_position != NULL && cobj->interest != NULL && vec != NULL) {
        guVector eye_pos;
        guVector interest_pos;
        HSD_CObjGetEyePosition(cobj, &eye_pos);
        HSD_CObjGetInterest(cobj, &interest_pos);

        guVecSub(&interest_pos, &eye_pos, vec);

        return __builtin_clz(vec_normalize_check(vec, vec)) >> 5;
    }
    return 0;
}

//80368A08
f32 HSD_CObjGetEyeDistance(HSD_CObj* cobj)
{
    f32 res = 0.0f;
    if (cobj != NULL) {
        assert(cobj->eye_position != NULL);
        assert(cobj->interest != NULL);

        guVector pos;
        guVector interest;
        HSD_WObjGetPosition(cobj->eye_position, &pos);
        HSD_WObjGetPosition(cobj->interest, &interest);
        guVector dist;
        guVecSub(&interest, &pos, &dist);
        res = sqrtf(dist.x * dist.x + dist.y * dist.y + dist.z * dist.z);
    }
    return res;
}

//80378E70
s32 HSD_CObjGetUpVector(HSD_CObj* cobj, guVector* vec)
{
    if (cobj != NULL && vec != NULL) {
        if ((cobj->flags & 1) == 0) {
            return roll2upvec(cobj, cobj->u.roll, vec);
        } else {
            vec->x = cobj->u.up.x;
            vec->y = cobj->u.up.y;
            vec->z = cobj->u.up.z;
            return 1;
        }
    }
    return 0;
}

//803690B4
void HSD_CObjSetUpVector(HSD_CObj* cobj, guVector* vec)
{
    if (cobj != NULL && vec != NULL) {
        if ((cobj->flags & 1) == 0) {
            guVector uvec;
            f32 val = 0.0f;
            s32 res = HSD_CObjGetEyeVector(cobj, &uvec);
            if (res == 0) {
                val = guVecDotProduct(vec, &uvec);
                if (FLT_MIN <= 1.0f - fabsf(val)) {
                    guVector zero = { 0.f, 0.f, 0.f };
                    guVector upvec = { 0.f, 1.f, 0.f };
                    Mtx mtx;
                    guLookAt(mtx, &zero, &upvec, &uvec);
                    guVector rvec = { 0.0f, 0.0f, 0.0f };
                    guVecMultiplySR(mtx, vec, &rvec);
                    if (0.0f == rvec.y) {
                        if (rvec.x < 0.0f) {
                            val = -1.94635f;
                        }
                    } else {
                        val = atan2f(rvec.y, rvec.x);
                    }
                } else {
                    val = 0.0f;
                }
            }
            HSD_CObjSetRoll(cobj, val);
        } else {
            guVector up;
            vec_normalize_check(vec, &up);

            if (cobj->u.up.x != up.x || cobj->u.up.y != up.y || cobj->u.up.z != up.z) {
                HSD_CObjSetMtxDirty(cobj);
                cobj->u.up.x = up.x;
                cobj->u.up.y = up.y;
                cobj->u.up.z = up.z;
            }
        }
    }
}

//80369564
void HSD_CObjSetMtxDirty(HSD_CObj* cobj)
{
    cobj->flags |= 0xC0000000;
}

MtxP HSD_CObjGetViewingMtxPtrDirect(HSD_CObj* cobj)
{
    return cobj->view_mtx;
}

//80369574
BOOL HSD_CObjMtxIsDirty(HSD_CObj* cobj)
{
    if ((cobj->flags & 0x40000000) == 0) {
        if (cobj->eye_position != NULL && (cobj->eye_position->flags & 2) != 0) {
            return TRUE;
        }
        if (cobj->interest != NULL && (cobj->interest->flags & 2) != 0) {
            return TRUE;
        }
        return FALSE;
    }
    return TRUE;
}

//80369624
MtxP HSD_CObjGetInvViewingMtxPtrDirect(HSD_CObj* cobj)
{
    if ((cobj->flags & 0x80000000) != 0) {
        if (cobj->proj_mtx == NULL) {
            cobj->proj_mtx = (MtxP)HSD_MemAlloc(sizeof(Mtx));
        }
        guMtxInverse(cobj->view_mtx, cobj->proj_mtx);
        HSD_CObjClearFlags(cobj, 0x80000000);
    }
    return cobj->proj_mtx;
}

//803699C0
void HSD_CObjSetRoll(HSD_CObj* cobj, f32 roll)
{
    guVector up;

    if (cobj != NULL) {
        if ((cobj->flags & 1) == 0) {
            if (cobj->u.roll != roll) {
                HSD_CObjSetMtxDirty(cobj);
            }
            cobj->u.roll = roll;
        } else {
            roll2upvec(cobj, roll, &up);
            HSD_CObjSetUpVector(cobj, &up);
        }
    }
}

//80369BC8
f32 HSD_CObjGetFov(HSD_CObj* cobj)
{
    if (cobj != NULL) {
        if (cobj->projection_type == PROJ_PERSPECTIVE)
            return cobj->projection_param.perspective.fov;
    }
    return 0.0f;
}

//80369BEC
void HSD_CObjSetFov(HSD_CObj* cobj, f32 fov)
{
    if (cobj != NULL) {
        if (cobj->projection_type == PROJ_PERSPECTIVE)
            cobj->projection_param.perspective.fov = fov;
    }
}

//80369C0C
f32 HSD_CObjGetAspect(HSD_CObj* cobj)
{
    if (cobj != NULL) {
        if (cobj->projection_type == PROJ_PERSPECTIVE)
            return cobj->projection_param.perspective.aspect;
    }
    return 0.0f;
}

//80369C30
void HSD_CObjSetAspect(HSD_CObj* cobj, f32 aspect)
{
    if (cobj != NULL) {
        if (cobj->projection_type == PROJ_PERSPECTIVE)
            cobj->projection_param.perspective.aspect = aspect;
    }
}

//80369C50
f32 HSD_CObjGetTop(HSD_CObj* cobj)
{
    if (cobj != NULL) {
        if (cobj->projection_type == PROJ_FRUSTRUM || cobj->projection_type == PROJ_ORTHO) {
            return cobj->projection_param.frustrum.top;
        }
        if (cobj->projection_type == PROJ_PERSPECTIVE) {
            return cobj->near * tan(0.5f * (cobj->projection_param.perspective.fov * 0.017453292f));
        }
    }
    return 0.0f;
}

//80369CE4
void HSD_CObjSetTop(HSD_CObj* cobj, f32 top)
{
    if (cobj != NULL) {
        if (cobj->projection_type == PROJ_FRUSTRUM || cobj->projection_type == PROJ_ORTHO)
            cobj->projection_param.frustrum.top = top;
    }
}

//80369D18
f32 HSD_CObjGetBottom(HSD_CObj* cobj)
{
    f32 res = 0.0f;
    if (cobj != NULL) {
        u8 proj_type = cobj->projection_type;
        switch (proj_type) {
        case PROJ_PERSPECTIVE:
            res = tanf(0.5f * 0.017453292f * cobj->projection_param.perspective.fov);
            res = cobj->near * res;
            break;
        case PROJ_FRUSTRUM:
        case PROJ_ORTHO:
            res = cobj->projection_param.frustrum.bottom;
            break;
        default:
            res = 0.0f;
        }
    }
    return res;
}

//80369DB0
void HSD_CObjSetBottom(HSD_CObj* cobj, f32 bottom)
{
    if (cobj != NULL) {
        if (cobj->projection_type == PROJ_FRUSTRUM || cobj->projection_type == PROJ_ORTHO) {
            cobj->projection_param.frustrum.bottom = bottom;
        }
    }
}

//80369DE4
f32 HSD_CObjGetLeft(HSD_CObj* cobj)
{
    f32 res = 0.0f;
    if (cobj != NULL) {
        u8 proj_type = cobj->projection_type;
        switch (proj_type) {
        case PROJ_PERSPECTIVE:
            res = tanf(0.5f * 0.017453292f * cobj->projection_param.perspective.fov);
            res = cobj->projection_param.perspective.aspect * cobj->near * res;
            break;
        case PROJ_FRUSTRUM:
        case PROJ_ORTHO:
            res = cobj->projection_param.frustrum.left;
            break;
        }
    }
    return res;
}

//80369E84
void HSD_CObjSetLeft(HSD_CObj* cobj, f32 left)
{
    if (cobj != NULL) {
        if (cobj->projection_type == PROJ_FRUSTRUM || cobj->projection_type == PROJ_ORTHO)
            cobj->projection_param.frustrum.left = left;
    }
}

//80369EB8
f32 HSD_CObjGetRight(HSD_CObj* cobj)
{
    f32 res = 0.0f;
    if (cobj != NULL) {
        u8 proj_type = cobj->projection_type;
        switch (proj_type) {
        case PROJ_PERSPECTIVE:
            res = tanf(0.5f * 0.017453292f * cobj->projection_param.perspective.fov);
            res = cobj->projection_param.perspective.aspect * cobj->near * res;
            break;
        case PROJ_FRUSTRUM:
        case PROJ_ORTHO:
            res = cobj->projection_param.frustrum.right;
            break;
        }
    }
    return res;
}

//80369F54
void HSD_CObjSetRight(HSD_CObj* cobj, f32 right)
{
    if (cobj != NULL) {
        if (cobj->projection_type == PROJ_FRUSTRUM || cobj->projection_type == PROJ_ORTHO)
            cobj->projection_param.frustrum.right = right;
    }
}

//80369F88
f32 HSD_CObjGetNear(HSD_CObj* cobj)
{
    if (cobj != NULL)
        return cobj->near;
    return 0.0f;
}

//80369FA0
void HSD_CObjSetNear(HSD_CObj* cobj, f32 near)
{
    if (cobj != NULL)
        cobj->near = near;
}

//80369FB0
f32 HSD_CObjGetFar(HSD_CObj* cobj)
{
    if (cobj != NULL)
        return cobj->far;
    return 0.0f;
}

//80369FC8
void HSD_CObjSetFar(HSD_CObj* cobj, f32 far)
{
    if (cobj != NULL)
        cobj->far = far;
}

//80369FD8
void HSD_CObjGetScissor(HSD_CObj* cobj, u16 scissors[4])
{
    if (cobj != NULL) {
        scissors[0] = cobj->scissor_left;
        scissors[1] = cobj->scissor_right;
        scissors[2] = cobj->scissor_top;
        scissors[3] = cobj->scissor_bottom;
    }
}

//80369FF4
void HSD_CObjSetScissor(HSD_CObj* cobj, u16 scissors[4])
{
    if (cobj != NULL) {
        cobj->scissor_left = scissors[0];
        cobj->scissor_right = scissors[1];
        cobj->scissor_top = scissors[2];
        cobj->scissor_bottom = scissors[3];
    }
}

//8036A010
void HSD_CObjSetScissorx4(HSD_CObj* cobj, u16 left, u16 right, u16 top, u16 bottom)
{
    if (cobj != NULL) {
        cobj->scissor_left = left;
        cobj->scissor_right = right;
        cobj->scissor_top = top;
        cobj->scissor_bottom = bottom;
    }
}

//8036A02C
void HSD_CObjGetViewportf(HSD_CObj* cobj, f32 viewports[4])
{
    if (cobj != NULL) {
        viewports[0] = cobj->viewport_left;
        viewports[1] = cobj->viewport_right;
        viewports[2] = cobj->viewport_top;
        viewports[3] = cobj->viewport_bottom;
    }
}

//8036A058
void HSD_CObjSetViewport(HSD_CObj* cobj, u16 viewports[4])
{
    if (cobj != NULL) {
        cobj->viewport_left = (f32)viewports[0];
        cobj->viewport_right = (f32)viewports[1];
        cobj->viewport_top = (f32)viewports[2];
        cobj->viewport_bottom = (f32)viewports[3];
    }
}

//8036A0E4
void HSD_CObjSetViewportf(HSD_CObj* cobj, f32 viewports[4])
{
    if (cobj != NULL) {
        cobj->viewport_left = viewports[0];
        cobj->viewport_right = viewports[1];
        cobj->viewport_top = viewports[2];
        cobj->viewport_bottom = viewports[3];
    }
}

//8036A110
void HSD_CObjSetViewportfx4(HSD_CObj* cobj, u16 left, u16 right, u16 top, u16 bottom)
{
    if (cobj != NULL) {
        cobj->viewport_left = (f32)left;
        cobj->viewport_right = (f32)right;
        cobj->viewport_top = (f32)top;
        cobj->viewport_bottom = (f32)bottom;
    }
}

//8036A12C
u8 HSD_CObjGetProjectionType(HSD_CObj* cobj)
{
    if (cobj != NULL)
        return cobj->projection_type;
    return PROJ_PERSPECTIVE;
}

//8036A144
void HSD_CObjSetProjectionType(HSD_CObj* cobj, u8 proj_type)
{
    if (cobj != NULL)
        cobj->projection_type = proj_type;
}

//8036A154
void HSD_CObjSetPerspective(HSD_CObj* cobj, f32 fov, f32 aspect)
{
    if (cobj != NULL) {
        cobj->projection_type = PROJ_PERSPECTIVE;
        cobj->projection_param.perspective.fov = fov;
        cobj->projection_param.perspective.aspect = aspect;
    }
}

//8036A170
void HSD_CObjSetFrustrum(HSD_CObj* cobj, f32 top, f32 bottom, f32 left, f32 right)
{
    if (cobj != NULL) {
        cobj->projection_type = PROJ_FRUSTRUM;
        cobj->projection_param.frustrum.top = top;
        cobj->projection_param.frustrum.bottom = bottom;
        cobj->projection_param.frustrum.left = left;
        cobj->projection_param.frustrum.right = right;
    }
}

//8036A194
void HSD_CObjSetOrtho(HSD_CObj* cobj, f32 top, f32 bottom, f32 left, f32 right)
{
    if (cobj != NULL) {
        cobj->projection_type = PROJ_ORTHO;
        cobj->projection_param.ortho.top = top;
        cobj->projection_param.ortho.bottom = bottom;
        cobj->projection_param.ortho.left = left;
        cobj->projection_param.ortho.right = right;
    }
}

//8036A1B8
void HSD_CObjGetPerspective(HSD_CObj* cobj, f32* top, f32* bottom)
{
    if (cobj != NULL) {
        if (cobj->projection_type == PROJ_PERSPECTIVE) {
            if (top != NULL)
                *top = cobj->projection_param.perspective.fov;
            if (bottom != NULL)
                *bottom = cobj->projection_param.perspective.aspect;
        }
    }
}

//8036A1F4
void HSD_CObjGetOrtho(HSD_CObj* cobj, f32* top, f32* bottom, f32* left, f32* right)
{
    if (cobj != NULL) {
        if (cobj->projection_type == PROJ_ORTHO) {
            if (top != NULL)
                *top = cobj->projection_param.ortho.top;
            if (bottom != NULL)
                *bottom = cobj->projection_param.ortho.bottom;
            if (left != NULL)
                *left = cobj->projection_param.ortho.left;
            if (right != NULL)
                *right = cobj->projection_param.ortho.right;
        }
    }
}

//8036A250
u32 HSD_CObjGetFlags(HSD_CObj* cobj)
{
    if (cobj != NULL) {
        return cobj->flags;
    }
    return 0;
}

//8036A258
void HSD_CObjSetFlags(HSD_CObj* cobj, u32 flags)
{
    if (cobj != NULL) {
        cobj->flags |= flags;
    }
}

//8036A270
void HSD_CObjClearFlags(HSD_CObj* cobj, u32 flags)
{
    if (cobj != NULL) {
        cobj->flags &= ~flags;
    }
}

//8036A288
HSD_CObj* HSD_CObjGetCurrent(void)
{
    return current_cobj;
}

//8036A290
HSD_CObj* HSD_CObjAlloc(void)
{
    HSD_ClassInfo* info = (HSD_ClassInfo*)(default_class ? default_class : &hsdCObj);
    HSD_CObj* cobj = (HSD_CObj*)hsdNew(info);
    assert(cobj != NULL);
    return cobj;
}

//8036A2EC
static int CObjLoad(HSD_CObj* cobj, HSD_CObjDesc* desc)
{
    cobj->flags = desc->flags;
    cobj->flags = (cobj->flags & 0xC0000000) | desc->flags;

    cobj->viewport_left = (f32)desc->viewport_left;
    cobj->viewport_right = (f32)desc->viewport_right;
    cobj->viewport_top = (f32)desc->viewport_top;
    cobj->viewport_bottom = (f32)desc->viewport_bottom;

    cobj->scissor_left = desc->scissor_lr;
    cobj->scissor_top = desc->scissor_tb;

    HSD_WObjInit(cobj->eye_position, desc->eye_desc);
    HSD_WObjInit(cobj->interest, desc->interest_desc);
    HSD_CObjSetNear(cobj, desc->near);
    HSD_CObjSetFar(cobj, desc->far);

    if ((desc->flags & 1) == 0) {
        HSD_CObjSetRoll(cobj, desc->roll);
    } else {
        if (desc->vector == NULL) {
            guVector vec = { 0.f, 1.f, 0.f };
            HSD_CObjSetUpVector(cobj, &vec);
        } else {
            HSD_CObjSetUpVector(cobj, desc->vector);
        }
    }
    
    u8 proj_type = desc->projection_type;
    switch (proj_type) {
    case PROJ_PERSPECTIVE:
        HSD_CObjSetPerspective(cobj, desc->projection_param.perspective.fov, desc->projection_param.perspective.aspect);
        break;
    case PROJ_FRUSTRUM:
        HSD_CObjSetFrustrum(cobj, desc->projection_param.frustrum.top, desc->projection_param.frustrum.bottom, desc->projection_param.frustrum.left, desc->projection_param.frustrum.right);
        break;
    case PROJ_ORTHO:
        HSD_CObjSetOrtho(cobj, desc->projection_param.ortho.top, desc->projection_param.ortho.bottom, desc->projection_param.ortho.left, desc->projection_param.ortho.right);
        break;
    default:
        HSD_Halt("Unexpected projection type");
    }
    return 0;
}

//8036A55C
void HSD_CObjInit(HSD_CObj* cobj, HSD_CObjDesc* desc)
{
    if (cobj != NULL && desc != NULL) {
        CObjLoad(cobj, desc);
    }
}

//8036A590
HSD_CObj* HSD_CObjLoadDesc(HSD_CObjDesc* desc)
{
    if (desc == NULL) {
        return NULL;
    }

    HSD_CObj* cobj = NULL;
    HSD_ClassInfo* info;
    if (!desc->class_name || !(info = hsdSearchClassInfo(desc->class_name))) {
        info = HSD_CLASS_INFO(&hsdCObj);
        if (info == NULL) {
            info = HSD_CLASS_INFO(default_class);
        }
    }
    cobj = (HSD_CObj*)hsdNew(info);
    assert(cobj != NULL);
    HSD_COBJ_METHOD(cobj)->load(cobj, desc);
    return cobj;
}

//8036A654
static void CObjInit(HSD_Class* o)
{
    HSD_OBJECT_PARENT_INFO(&hsdCObj)->init(o);

    if (o != NULL) {
        HSD_CObj* cobj = HSD_COBJ(o);
        cobj->flags |= 0xC0000000;

        cobj->eye_position = HSD_WObjAlloc();
        cobj->interest = HSD_WObjAlloc();
    }
}

//8036A6C8
static void CObjRelease(HSD_Class* o)
{
    HSD_CObj* cobj = HSD_COBJ(o);

    HSD_AObjRemove(cobj->aobj);

    HSD_WObj* wobj = cobj->eye_position;
    HSD_WObjUnref(wobj);

    wobj = cobj->interest;
    HSD_WObjUnref(wobj);

    if (cobj->proj_mtx != NULL) {
        HSD_Free(cobj->proj_mtx);
    }

    HSD_OBJECT_INFO(&hsdCObj)->release(o);
}

//8036A85C
static void CObjAmnesia(HSD_ClassInfo* info)
{
    if (info == HSD_CLASS_INFO(default_class)) {
        default_class = NULL;
    }
    if (info == HSD_CLASS_INFO(&hsdCObj)) {
        current_cobj = NULL;
    }
    HSD_OBJECT_INFO(&hsdCObj)->amnesia(info);
}

//8036A8BC
static void CObjInfoInit(void)
{
    hsdInitClassInfo(HSD_CLASS_INFO(&hsdCObj), HSD_CLASS_INFO(&hsdObj), HSD_BASE_CLASS_LIBRARY, "hsd_cobj", sizeof(HSD_CObjInfo), sizeof(HSD_CObj));

    HSD_OBJECT_INFO(&hsdCObj)->init = CObjInit;
    HSD_OBJECT_INFO(&hsdCObj)->release = CObjRelease;
    HSD_OBJECT_INFO(&hsdCObj)->amnesia = CObjAmnesia;
    HSD_COBJ_INFO(&hsdCObj)->load = CObjLoad;
}
