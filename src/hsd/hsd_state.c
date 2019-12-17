#include "hsd_state.h"

#include "hsd_lobj.h"

static u8 state_num_chans = -1; //r13_40A0
static u8 state_num_tevstages; //r13_40A8

/* HSD_STATE_PRIMITIVE Group */
static s8 state_line_width;
static u8 state_line_tex_offsets;
static u8 state_point_size;
static u8 state_point_tex_offsets;
static s8 state_cull_mode;
static u8 state_coplanar;

/* HSD_STATE_RENDER_MODE Group */
static s8 state_blend_type;
static s8 state_src_factor;
static s8 state_dst_factor;
static s8 state_logic_op;
static s8 state_z_enable;
static s8 state_z_func;
static s8 state_z_update;
static s8 state_alpha_comp0;
static u8 state_alpha_ref0;
static s8 state_alpha_op;
static s8 state_alpha_comp1;
static u8 state_alpha_ref1;
static s8 state_color_update;
static s8 state_alpha_update;
static s8 state_enable_dst_alpha;
static u8 state_dst_alpha;
static s8 state_before_tex;
static s8 state_dither;

static struct {
    GXColor ambient;
    GXColor diffuse;
    GXColor specular;
    u8 alpha;
    f32 shininess;
} matstate;

static HSD_Chan prev_ch[4];
static HSD_Chan invalid_prev_ch;
static u8 chan_disabled[4] = {1, 1, 1, 1}; //r13_409C - r13_4090

static void HSD_DisableChannelLighting(u32 channel)
{
    u32 chan = channel & 3;
    if (channel < GX_COLORZERO && channel > GX_ALPHA1) {
        if(prev_ch[chan].enable != 0 || prev_ch[chan + 2].enable != 0){
            prev_ch[chan + 2].enable = 0;
            prev_ch[chan].enable = 0;
            prev_ch[chan + 2].light_mask = 0;
            prev_ch[chan].light_mask = 0;
            prev_ch[chan + 2].amb_src = prev_ch[chan].amb_src;
            prev_ch[chan + 2].mat_src = prev_ch[chan].mat_src;
            prev_ch[chan + 2].diff_fn = prev_ch[chan].diff_fn;
            prev_ch[chan + 2].attn_fn = prev_ch[chan].attn_fn;
            GX_SetChanCtrl(channel, 0, prev_ch[chan].amb_src, prev_ch[chan].mat_src, 0, prev_ch[chan].diff_fn, prev_ch[chan].attn_fn);
        }
    }else if(prev_ch[chan].enable != 0){
        prev_ch[chan].enable = 0;
        prev_ch[chan].light_mask = 0;
        GX_SetChanCtrl(channel, 0, prev_ch[chan].amb_src, prev_ch[chan].mat_src, 0, prev_ch[chan].diff_fn, prev_ch[chan].attn_fn);
    }
}

//803615D0
void HSD_SetupChannelMode(u32 rendermode)
{
    u32 diffuse_mode;
    u32 alpha_mode;
    s32 use_color1 = 0;
    s32 use_alpha1 = 0;

    diffuse_mode = rendermode & RENDER_DIFFUSE_BITS;
    if (diffuse_mode == RENDER_DIFFUSE_MAT0) {
        diffuse_mode = RENDER_DIFFUSE_MAT;
    }

    alpha_mode = rendermode & RENDER_ALPHA_BITS;
    if (alpha_mode == RENDER_ALPHA_COMPAT) {
        alpha_mode = diffuse_mode << (RENDER_ALPHA_SHIFT - RENDER_DIFFUSE_SHIFT);
    }

    if (rendermode & RENDER_SPECULAR) {
        static HSD_Chan ch = {
            NULL,
            GX_COLOR1,
            0,
            { 0, 0, 0, 0 },
            { 255, 255, 255, 255 },
            GX_TRUE,
            GX_SRC_REG,
            GX_SRC_REG,
            0,
            GX_DF_NONE,
            GX_AF_SPEC,
        };
        s32 i, num;

        ch.light_mask = HSD_LObjGetLightMaskSpecular();
        //ch.enable = ch.light_mask ? GX_TRUE : GX_FALSE;
        HSD_SetupChannel(&ch);
        use_color1 = 1;

        num = HSD_LObjGetNbActive();
        for (i = 0; i < num; i++) {
            HSD_LObj* lobj = HSD_LObjGetActiveByIndex(i);

            if (lobj != NULL) {
                HSD_LObjSetup(lobj, lobj->color, matstate.shininess);
            }
        }
    }

    if (rendermode & RENDER_DIFFUSE) {
        HSD_LObj* amb;
        static HSD_Chan ch = {
            NULL,
            GX_COLOR0,
            0,
            { 0, 0, 0, 0 },
            { 255, 255, 255, 255 },
            GX_ENABLE,
            GX_SRC_REG,
            GX_SRC_REG,
            0,
            GX_DF_CLAMP,
            GX_AF_SPOT,
        };
        static HSD_Chan ch_a = {
            NULL,
            GX_ALPHA0,
            0,
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            GX_ENABLE,
            GX_SRC_REG,
            GX_SRC_REG,
            0,
            GX_DF_CLAMP,
            GX_AF_SPOT,
        };
        static HSD_Chan ch_vtx_a = {
            NULL, GX_ALPHA0, 0,
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            GX_DISABLE, GX_SRC_REG, GX_SRC_VTX,
            0, GX_DF_CLAMP, GX_AF_SPOT
        };
        int amb_alpha;

        static GXColor dark_matter = { 0, 0, 0, 255 };

        amb = HSD_LObjGetActiveByID(GX_MAXLIGHT);

        if (amb && (amb->flags & LOBJ_DIFFUSE)) {
            HSD_MulColor(&matstate.ambient, &amb->color, &ch.amb_color);
        } else {
            ch.amb_color = dark_matter;
        }

        ch.mat_src = (diffuse_mode & RENDER_DIFFUSE_VTX) ? GX_SRC_VTX : GX_SRC_REG;
        ch.light_mask = HSD_LObjGetLightMaskDiffuse();
        HSD_SetupChannel(&ch);

        if (alpha_mode & RENDER_ALPHA_VTX) {
            ch_a.chan = GX_ALPHA1;
            use_alpha1 = 1;
            HSD_SetupChannel(&ch_vtx_a);

        } else {
            ch_a.chan = GX_ALPHA0;
        }

        ch_a.light_mask = HSD_LObjGetLightMaskAlpha();

        if (amb && (amb->flags & LOBJ_ALPHA)) {
            amb_alpha = amb->color.a;
        } else {
            amb_alpha = 0;
        }

        if (ch_a.light_mask) {
            ch_a.enable = GX_TRUE;
            ch_a.mat_color.a = 255;
            ch_a.amb_color.a = amb_alpha;
        } else {
            ch_a.enable = GX_FALSE;
            ch_a.mat_color.a = amb_alpha;
        }

        HSD_SetupChannel(&ch_a);

    } else {
        static HSD_Chan ch = {
            NULL,
            GX_COLOR0,
            0,
            { 0, 0, 0, 0 },
            { 255, 255, 255, 255 },
            GX_FALSE,
            GX_SRC_REG,
            GX_SRC_VTX,
            0,
            GX_DF_NONE,
            GX_AF_NONE,
        };
        static HSD_Chan ch_a = {
            NULL,
            GX_ALPHA0,
            0,
            { 0, 0, 0, 0 },
            { 0, 0, 0, 255 },
            GX_FALSE,
            GX_SRC_REG,
            GX_SRC_VTX,
            0,
            GX_DF_NONE,
            GX_AF_NONE,
        };

        ch.mat_src = (diffuse_mode & RENDER_DIFFUSE_VTX) ? GX_SRC_VTX : GX_SRC_REG;
        HSD_SetupChannel(&ch);

        ch_a.mat_src = (alpha_mode & RENDER_ALPHA_VTX) ? GX_SRC_VTX : GX_SRC_REG;
        HSD_SetupChannel(&ch_a);
    }

    if (use_color1) {
        if (!use_alpha1) {
            HSD_DisableChannelLighting(GX_ALPHA1);
        }
        HSD_StateSetNumChans(2);
    } else { // !use_color1
        if (use_alpha1) {
            HSD_DisableChannelLighting(GX_COLOR1);
            HSD_StateSetNumChans(2);
        } else {
            HSD_DisableChannelLighting(GX_COLOR1A1);
            HSD_StateSetNumChans(1);
        }
    }
}

//80361778
void HSD_SetupPEMode(u32 rendermode, HSD_PEDesc* pe)
{
    if (pe != NULL) {
        HSD_StateSetColorUpdate(pe->flags & ENABLE_COLOR_UPDATE);
        HSD_StateSetAlphaUpdate(pe->flags & ENABLE_ALPHA_UPDATE);
        HSD_StateSetDstAlpha(pe->flags & ENABLE_DST_ALPHA, pe->dst_alpha);
        HSD_StateSetBlendMode(pe->type, pe->src_factor, pe->dst_factor, pe->logic_op);
        HSD_StateSetZMode(pe->flags & ENABLE_COMPARE, pe->z_comp, pe->flags & ENABLE_ZUPDATE);
        HSD_StateSetZCompLoc(pe->flags & BEFORE_TEX);
        HSD_StateSetAlphaCompare(pe->alpha_comp0, pe->ref0, pe->alpha_op, pe->alpha_comp1, pe->ref1);
        HSD_StateSetDither(pe->flags & ENABLE_DITHER);
    } else {
        HSD_StateSetColorUpdate(GX_ENABLE);
        HSD_StateSetAlphaUpdate(GX_DISABLE);
        HSD_StateSetDstAlpha(GX_DISABLE, 0);

        HSD_StateSetBlendMode((rendermode & RENDER_XLU) ? GX_BM_BLEND : GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_SET);
        HSD_StateSetZMode(GX_TRUE, (rendermode & RENDER_ZMODE_ALWAYS) ? GX_ALWAYS : GX_LEQUAL, (rendermode & RENDER_NO_ZUPDATE) ? GX_FALSE : GX_TRUE);
        if (!(rendermode & RENDER_NO_ZUPDATE) && (rendermode & RENDER_XLU)) {
            HSD_StateSetZCompLoc(GX_FALSE);
            HSD_StateSetAlphaCompare(GX_GREATER, 0, GX_AOP_AND, GX_GREATER, 0);
        } else {
            HSD_StateSetZCompLoc(GX_TRUE);
            HSD_StateSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
        }
        HSD_StateSetDither(GX_DISABLE);
    }
}

//8036190C
void HSD_SetupRenderModeWithCustomPE(u32 rendermode, HSD_PEDesc* pe)
{
    if (!HSD_StateGetNumTevStages()) {
        HSD_TevDesc tevdesc;
        tevdesc.flags = TEVOP_MODE;
        tevdesc.stage = HSD_StateAssignTev();
        tevdesc.coord = GX_TEXCOORDNULL;
        tevdesc.map = GX_TEXMAP_NULL;
        tevdesc.color = GX_COLOR0A0;
        tevdesc.u.tevop.tevmode = GX_PASSCLR;
        HSD_SetupTevStage(&tevdesc);
    }
    HSD_SetupPEMode(rendermode, pe);
    HSD_SetTevRegAll();
    HSD_StateSetNumTevStages();
    HSD_StateSetNumTexGens();
    HSD_SetupChannelMode(rendermode);
}

//8036199C
void HSD_SetupRenderMode(u32 rendermode)
{
    if (!HSD_StateGetNumTevStages()) {
        HSD_TevDesc tevdesc;
        tevdesc.flags = TEVOP_MODE;
        tevdesc.stage = HSD_StateAssignTev();
        tevdesc.coord = GX_TEXCOORDNULL;
        tevdesc.map = GX_TEXMAP_NULL;
        tevdesc.color = GX_COLOR0A0;
        tevdesc.u.tevop.tevmode = GX_PASSCLR;
        HSD_SetupTevStage(&tevdesc);
    }
    HSD_SetupPEMode(rendermode, NULL);
    HSD_SetTevRegAll();
    HSD_StateSetNumTevStages();
    HSD_StateSetNumTexGens();
    HSD_SetupChannelMode(rendermode);
}

//80361A20
void HSD_SetMaterialColor(GXColor ambient, GXColor diffuse, GXColor specular, f32 alpha)
{
    matstate.ambient = ambient;
    matstate.diffuse = diffuse;
    matstate.specular = specular;
    matstate.alpha = (u8)(255.0F * HSD_ClampFloat(alpha, 0.f, 1.f));
}

//80361A64
void HSD_SetMaterialShininess(f32 shininess)
{
    matstate.shininess = shininess;
}

//80361A74
void HSD_StateSetLineWidth(u8 width, u8 tex_offsets)
{
    if (state_line_width != width || state_line_tex_offsets != tex_offsets) {
        GX_SetLineWidth(width, tex_offsets);
        state_line_width = width;
        state_line_tex_offsets = tex_offsets;
    }
}

//80361AD8
void HSD_StateSetCullMode(u8 mode)
{
    if (state_cull_mode != mode) {
        GX_SetCullMode(mode);
        state_cull_mode = mode;
    }
}

//80361B18
void HSD_StateSetBlendMode(u8 type, u8 src_factor, u8 dst_factor, u8 op)
{
    if (state_blend_type != type || state_src_factor != src_factor || state_dst_factor != dst_factor || state_logic_op != op) {
        GX_SetBlendMode(type, src_factor, dst_factor, op);
        state_blend_type = type;
        state_src_factor = src_factor;
        state_dst_factor = dst_factor;
        state_logic_op = op;
    }
}

//80361BB8
void HSD_StateSetZMode(s32 enable, u8 func, s32 update)
{
    u8 _enable = (enable) ? GX_ENABLE : GX_DISABLE;
    u8 _update = (update) ? GX_ENABLE : GX_DISABLE;

    if (state_z_enable != _enable || state_z_func != func || state_z_update != _update) {
        GX_SetZMode(_enable, func, _update);
        state_z_enable = _enable;
        state_z_func = func;
        state_z_update = _update;
    }
}

//80361CC4
void HSD_StateSetAlphaCompare(u8 comp0, u8 ref0, u8 op, u8 comp1, u8 ref1)
{
    if (state_alpha_comp0 != comp0 || state_alpha_ref0 != ref0 || state_alpha_op != op || state_alpha_comp1 != comp1 || state_alpha_ref1 != ref1) {
        GX_SetAlphaCompare(comp0, ref0, op, comp1, ref1);
        state_alpha_comp0 = comp0;
        state_alpha_ref0 = ref0;
        state_alpha_op = op;
        state_alpha_comp1 = comp1;
        state_alpha_ref1 = ref1;
    }
}

//80361D6C
void HSD_StateSetColorUpdate(s32 enable)
{
    u8 _enable = (enable) ? GX_ENABLE : GX_DISABLE;

    if (state_color_update != _enable) {
        GX_SetColorUpdate(_enable);
        state_color_update = _enable;
    }
}

//80361DC0
void HSD_StateSetAlphaUpdate(s32 enable)
{
    u8 _enable = (enable) ? GX_ENABLE : GX_DISABLE;

    if (state_alpha_update != _enable) {
        GX_SetAlphaUpdate(_enable);
        state_alpha_update = _enable;
    }
}

//80361E14
void HSD_StateSetDstAlpha(s32 enable, u8 alpha)
{
    u8 _enable = (enable) ? GX_ENABLE : GX_DISABLE;

    if (state_enable_dst_alpha != _enable || state_dst_alpha != alpha) {
        GX_SetDstAlpha(_enable, alpha);
        state_enable_dst_alpha = _enable;
        state_dst_alpha = alpha;
    }
}

//80361E8C
void HSD_StateSetZCompLoc(s32 before_tex)
{
    u8 _before_tex = (before_tex) ? GX_TRUE : GX_FALSE;

    if (state_before_tex != _before_tex) {
        GX_SetZCompLoc(_before_tex);
        state_before_tex = _before_tex;
    }
}

//80361EE0
void HSD_StateSetDither(s32 dither)
{
    u8 _dither = (dither) ? GX_ENABLE : GX_DISABLE;

    if (state_dither != _dither) {
        GX_SetDither(_dither);
        state_dither = _dither;
    }
}

//80361F34
static void _HSD_StateInvalidatePrimitive(void)
{
    state_point_size = 0;
    state_cull_mode = 0;
    state_line_width = -1;
}

//80361F4C
static void _HSD_StateInvalidateVtxAttr(void)
{
    HSD_ClearVtxDesc();
}

//80361F6C
static void _HSD_StateInvalidateRenderMode(void)
{
    state_blend_type = -1;
    state_src_factor = -1;
    state_dst_factor = -1;
    state_logic_op = -1;
    state_z_enable = -1;
    state_z_func = -1;
    state_z_update = -1;
    state_alpha_comp0 = -1;
    state_alpha_ref0 = 0;
    state_alpha_op = -1;
    state_alpha_comp1 = -1;
    state_alpha_ref1 = 0;
    state_color_update = -1;
    state_alpha_update = -1;
    state_enable_dst_alpha = -1;
    state_dst_alpha = 0;
    state_before_tex = -1;
    state_dither = -1;
}

//80361FC4
void HSD_StateInvalidate(HSD_StateMask state)
{
    extern void _HSD_StateInvalidateColorChannel();
    extern void _HSD_StateInvalidateTevStage();
    extern void _HSD_StateInvalidateTevRegister();
    extern void _HSD_StateInvalidateTexCoordGen();

    static struct {
        HSD_StateMask mask;
        void (*func)(void);
    } invalidate_funcs[] = {
        { HSD_STATE_PRIMITIVE, _HSD_StateInvalidatePrimitive },
        { HSD_STATE_VTX_ATTR, _HSD_StateInvalidateVtxAttr },
        { HSD_STATE_COLOR_CHANNEL, _HSD_StateInvalidateColorChannel },
        { HSD_STATE_TEV_STAGE, _HSD_StateInvalidateTevStage },
        { HSD_STATE_TEV_REGISTER, _HSD_StateInvalidateTevRegister },
        { HSD_STATE_TEX_COORD_GEN, _HSD_StateInvalidateTexCoordGen },
        { HSD_STATE_RENDER_MODE, _HSD_StateInvalidateRenderMode },
        { HSD_STATE_NONE, NULL }
    };

    for (u32 i = 0; invalidate_funcs[i].mask != HSD_STATE_NONE; i++) {
        if (state & invalidate_funcs[i].mask) {
            invalidate_funcs[i].func();
        }
    }
}

//803620A4
void HSD_SetupChannel(HSD_Chan* ch)
{
    if (ch != NULL && ch->chan != 0xFF) {
        u32 idx = ch->chan & 3;
        u32 chan = ch->chan & 1;
        if (ch->enable != GX_DISABLE && ch->amb_src == GX_SRC_REG) {
            if (chan_disabled[chan] == 0) {
                if (ch->chan == 4 || ch->chan == 5) {
                    if (ch->amb_color.r != prev_ch[chan].amb_color.r || ch->amb_color.g != prev_ch[chan].amb_color.g || ch->amb_color.b != prev_ch[chan].amb_color.b || ch->amb_color.a != prev_ch[chan].amb_color.a) {
                        prev_ch[chan].amb_color = ch->amb_color;
                    }
                } else {
                    if (ch->chan == 0 || ch->chan == 1) {
                        if ((ch->amb_color.r ^ prev_ch[chan].amb_color.r) != 0 || (ch->amb_color.g ^ prev_ch[chan].amb_color.g) != 0 || (ch->amb_color.b ^ prev_ch[chan].amb_color.b) != 0) { 
                            //(chan->amb_color ^ prev_ch.amb_color) & 0xffffff00 != 0 - Alpha is being masked off
                            prev_ch[chan].amb_color.r = ch->amb_color.r;
                            prev_ch[chan].amb_color.g = ch->amb_color.g;
                            prev_ch[chan].amb_color.b = ch->amb_color.b;
                        }
                    } else {
                        if (ch->amb_color.a != prev_ch[chan].amb_color.a) {
                            prev_ch[chan].amb_color.a = ch->amb_color.a;
                        }
                    }
                }
                prev_ch[chan].amb_color.a = ch->amb_color.a;
                GX_SetChanAmbColor(ch->chan, ch->amb_color);
            } else {
                chan_disabled[chan] = 0;
                GX_SetChanAmbColor(chan + 4, ch->amb_color);
                prev_ch[chan].amb_color = ch->amb_color;
            }
        }

        if (ch->mat_src == GX_SRC_REG) {
            u8* offset = &chan_disabled[2];
            if (offset[chan] == 0) {
                if (ch->chan == 4 || ch->chan == 5) {
                    if (ch->mat_color.r != prev_ch[chan].mat_color.r || ch->mat_color.g != prev_ch[chan].mat_color.g || ch->mat_color.b != prev_ch[chan].mat_color.b || ch->mat_color.a != prev_ch[chan].mat_color.a) {
                        prev_ch[chan].mat_color = ch->mat_color;
                    }
                } else {
                    if (ch->chan == 0 || ch->chan == 1) {
                        if ((ch->mat_color.r ^ prev_ch[chan].mat_color.r) != 0 || (ch->mat_color.g ^ prev_ch[chan].mat_color.g) != 0 || (ch->mat_color.b ^ prev_ch[chan].mat_color.b) != 0) {
                            //(chan->mat_color ^ prev_ch.mat_color) & 0xffffff00 != 0 - Alpha is being masked off
                            prev_ch[chan].mat_color.r = ch->mat_color.r;
                            prev_ch[chan].mat_color.g = ch->mat_color.g;
                            prev_ch[chan].mat_color.b = ch->mat_color.b;
                        }
                    } else {
                        if (ch->mat_color.a != prev_ch[chan].mat_color.a) {
                            prev_ch[chan].mat_color.a = ch->mat_color.a;
                        }
                    }
                }
                GX_SetChanMatColor(ch->chan, ch->mat_color);
            } else {
                chan_disabled[chan] = 0;
                GX_SetChanMatColor(chan + 4, ch->mat_color);
                prev_ch[chan].mat_color = ch->mat_color;
            }
        }

        if ((ch->enable != prev_ch[idx].enable) || (ch->amb_src != prev_ch[idx].amb_src) || (ch->mat_src != prev_ch[idx].mat_src) || (ch->light_mask != prev_ch[idx].light_mask) || (ch->diff_fn != prev_ch[idx].diff_fn) || (ch->attn_fn != prev_ch[idx].attn_fn)) {
            GX_SetChanCtrl(ch->chan, ch->enable, ch->amb_src, ch->mat_src, ch->light_mask, ch->diff_fn, ch->attn_fn);
            prev_ch[idx].enable = ch->enable;
            prev_ch[idx].amb_src = ch->amb_src;
            prev_ch[idx].mat_src = ch->mat_src;
            prev_ch[idx].light_mask = ch->light_mask;
            prev_ch[idx].diff_fn = ch->diff_fn;
            prev_ch[idx].attn_fn = ch->attn_fn;
            if (ch->chan == 4 || ch->chan == 5) {
                prev_ch[idx + 2].enable = ch->enable;
                prev_ch[idx + 2].amb_src = ch->amb_src;
                prev_ch[idx + 2].mat_src = ch->mat_src;
                prev_ch[idx + 2].light_mask = ch->light_mask;
                prev_ch[idx + 2].diff_fn = ch->diff_fn;
                prev_ch[idx + 2].attn_fn = ch->attn_fn;
            }
        }
    }
}

//803623D0
void HSD_StateSetNumChans(u8 num)
{
    if (num != state_num_chans) {
        GX_SetNumChans(num);
    }
}

//803624D8
void HSD_StateInitTev(void)
{
    state_num_tevstages = 0;
}

//803624E4
u32 HSD_StateGetNumTevStages(void)
{
    return state_num_tevstages;
}

//803624EC
u8 HSD_StateAssignTev(void)
{
    u8 tevstage = state_num_tevstages;
    state_num_tevstages += 1;
    return HSD_Index2TevStage(tevstage);
}

//80362518
void HSD_StateSetNumTevStages(void)
{
    GX_SetNumTevStages(state_num_tevstages);
    state_num_tevstages = 0;
}

//80362548
void HSD_SetupTevStage(HSD_TevDesc* desc)
{
    GX_SetTevOrder(desc->stage, desc->coord, desc->map, desc->color);
    if (desc->flags == 0) {
        GX_SetTevOp(desc->stage, desc->u.tevconf.clr_op);
        GX_SetTevSwapMode(desc->stage, 0, 0);
    } else {
        GX_SetTevColorOp(desc->stage, desc->u.tevconf.clr_op, desc->u.tevconf.clr_bias, desc->u.tevconf.clr_scale, desc->u.tevconf.clr_clamp, desc->u.tevconf.clr_out_reg);
        GX_SetTevColorIn(desc->stage, desc->u.tevconf.clr_a, desc->u.tevconf.clr_b, desc->u.tevconf.clr_c, desc->u.tevconf.clr_d);
        GX_SetTevAlphaOp(desc->stage, desc->u.tevconf.alpha_op, desc->u.tevconf.alpha_bias, desc->u.tevconf.alpha_scale, desc->u.tevconf.alpha_clamp, desc->u.tevconf.alpha_out_reg);
        GX_SetTevAlphaIn(desc->stage, desc->u.tevconf.alpha_a, desc->u.tevconf.alpha_b, desc->u.tevconf.alpha_c, desc->u.tevconf.alpha_d);
        GX_SetTevSwapMode(desc->stage, desc->u.tevconf.ras_swap, desc->u.tevconf.tex_swap);
        GX_SetTevKColorSel(desc->stage, desc->u.tevconf.kcsel);
        GX_SetTevKAlphaSel(desc->stage, desc->u.tevconf.kasel);
    }
}

//80362768
u8 HSD_Index2TevStage(u8 idx)
{
    switch (idx) {
    case 0:
        return GX_TEVSTAGE0;
    case 1:
        return GX_TEVSTAGE1;
    case 2:
        return GX_TEVSTAGE2;
    case 3:
        return GX_TEVSTAGE3;
    case 4:
        return GX_TEVSTAGE4;
    case 5:
        return GX_TEVSTAGE5;
    case 6:
        return GX_TEVSTAGE6;
    case 7:
        return GX_TEVSTAGE7;
    case 8:
        return GX_TEVSTAGE8;
    case 9:
        return GX_TEVSTAGE9;
    case 10:
        return GX_TEVSTAGE10;
    case 11:
        return GX_TEVSTAGE11;
    case 12:
        return GX_TEVSTAGE12;
    case 13:
        return GX_TEVSTAGE13;
    case 14:
        return GX_TEVSTAGE14;
    case 15:
        return GX_TEVSTAGE15;
    default:
        assert(TRUE);
        break;
    }
    return 0;
}

//80362838
u8 HSD_TevStage2Index(u8 idx)
{
    switch (idx) {
    case GX_TEVSTAGE0:
        return 0;
    case GX_TEVSTAGE1:
        return 1;
    case GX_TEVSTAGE2:
        return 2;
    case GX_TEVSTAGE3:
        return 3;
    case GX_TEVSTAGE4:
        return 4;
    case GX_TEVSTAGE5:
        return 5;
    case GX_TEVSTAGE6:
        return 6;
    case GX_TEVSTAGE7:
        return 7;
    case GX_TEVSTAGE8:
        return 8;
    case GX_TEVSTAGE9:
        return 9;
    case GX_TEVSTAGE10:
        return 10;
    case GX_TEVSTAGE11:
        return 11;
    case GX_TEVSTAGE12:
        return 12;
    case GX_TEVSTAGE13:
        return 13;
    case GX_TEVSTAGE14:
        return 14;
    case GX_TEVSTAGE15:
        return 15;
    default:
        assert(TRUE);
        break;
    }
    return 0;
}

//80362CA0
void _HSD_StateInvalidateColorChannel(void)
{
    memcpy(&prev_ch, &invalid_prev_ch, sizeof(HSD_Chan));
    for(u32 i = 0; i < 4; i++){
        chan_disabled[i] = 1;
    }
    state_num_chans = -1;
}

//80362CF8
void _HSD_StateInvalidateTevStage(void)
{
    HSD_StateInitTev();
}