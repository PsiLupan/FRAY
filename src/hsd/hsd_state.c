#include "hsd_state.h"

static u8   state_num_chans = -1; //This variable is set but unused?
static u8   state_num_tevstages;

/* HSD_STATE_PRIMITIVE Group */
static s8	state_line_width;
static u8	state_line_tex_offsets;
static u8	state_point_size;
static u8	state_point_tex_offsets;
static s8	state_cull_mode;
static u8	state_coplanar;

/* HSD_STATE_RENDER_MODE Group */
static s8	state_blend_type;
static s8	state_src_factor;
static s8	state_dst_factor;
static s8	state_logic_op;
static s8	state_z_enable;
static s8	state_z_func;
static s8   state_z_update;
static s8	state_alpha_comp0;
static u8	state_alpha_ref0;
static s8	state_alpha_op;
static s8	state_alpha_comp1;
static u8	state_alpha_ref1;
static s8	state_color_update;
static s8	state_alpha_update;
static s8	state_enable_dst_alpha;
static u8	state_dst_alpha;
static s8	state_before_tex;
static s8	state_dither;

static struct {
  GXColor ambient;
  GXColor diffuse;
  GXColor specular;
  u8  alpha;
  f32 shininess;
} matstate;

//803615D0
void HSD_SetupChannelMode(u32 rendermode){
    
}

//80361778
void HSD_SetupPEMode(u32 rendermode, HSD_PEDesc *pe){
    if (pe) {
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
void HSD_SetupRenderModeWithCustomPE(u32 rendermode, HSD_PEDesc *pe){
    if(!HSD_StateGetNumTevStages()){
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
void HSD_SetupRenderMode(u32 rendermode){
    if(!HSD_StateGetNumTevStages()){
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
void HSD_SetMaterialColor(GXColor ambient, GXColor diffuse, GXColor specular, f32 alpha){
    matstate.ambient = ambient;
    matstate.diffuse = diffuse;
    matstate.specular = specular;
    matstate.alpha = (u8)(255.0F * HSD_ClampFloat(alpha, 0, 1));
}

//80361A64
void HSD_SetMaterialShininess(f32 shininess){
    matstate.shininess = shininess;
}

//80361A74
void HSD_StateSetLineWidth(u8 width, u8 tex_offsets){
    if (state_line_width != width || state_line_tex_offsets != tex_offsets){
        GX_SetLineWidth(width, tex_offsets);
        state_line_width = width;
        state_line_tex_offsets = tex_offsets;
    }
}

//80361BB8
void HSD_StateSetZMode(s32 enable, u8 func, s32 update){
    u8 _enable = (enable) ? GX_ENABLE : GX_DISABLE;
    u8 _update = (update) ? GX_ENABLE : GX_DISABLE;
    
    if (state_z_enable != _enable || state_z_func != func || state_z_update != _update){
        GX_SetZMode(_enable, func, _update);
        state_z_enable = _enable;
        state_z_func = func;
        state_z_update = _update;
    }
}

//80361D6C
void HSD_StateSetColorUpdate(s32 enable){
    u8 _enable = (enable) ? GX_ENABLE : GX_DISABLE;
    
    if (state_color_update != _enable) {
        GX_SetColorUpdate(_enable);
        state_color_update = _enable;
    }
}

//80361DC0
void HSD_StateSetAlphaUpdate(s32 enable){
    u8 _enable = (enable) ? GX_ENABLE : GX_DISABLE;
    
    if (state_alpha_update != _enable) {
        GX_SetAlphaUpdate(_enable);
        state_alpha_update = _enable;
    }
}

//80361F34
static void _HSD_StateInvalidatePrimitive(){
  state_point_size	= 0;
  state_cull_mode	= 0;
  state_line_width	= -1;
}

//80361F4C
static void _HSD_StateInvalidateVtxAttr(){
    HSD_ClearVtxDesc();
}

//80361F6C
static void _HSD_StateInvalidateRenderMode(){
    state_blend_type	 = -1;
    state_src_factor	 = -1;
    state_dst_factor	 = -1;
    state_logic_op	 = -1;
    state_z_enable	 = -1;
    state_z_func		 = -1;
    state_z_update	 = -1;
    state_alpha_comp0	 = -1;
    state_alpha_ref0	 = 0;
    state_alpha_op	 = -1;
    state_alpha_comp1	 = -1;
    state_alpha_ref1	 = 0;
    state_color_update	 = -1;
    state_alpha_update	 = -1;
    state_enable_dst_alpha = -1;
    state_dst_alpha	 = 0;
    state_before_tex	 = -1;
    state_dither		 = -1;
}

//80361FC4
void HSD_StateInvalidate(HSD_StateMask state){
    extern void _HSD_StateInvalidateColorChannel();
    extern void _HSD_StateInvalidateTevStage();
    extern void _HSD_StateInvalidateTevRegister();
    extern void _HSD_StateInvalidateTexCoordGen();
    
    static struct {
        HSD_StateMask mask;
        void (*func)(void);
    } invalidate_funcs[] = {
    { HSD_STATE_PRIMITIVE,	_HSD_StateInvalidatePrimitive },
    { HSD_STATE_VTX_ATTR,	_HSD_StateInvalidateVtxAttr },
    { HSD_STATE_COLOR_CHANNEL,	_HSD_StateInvalidateColorChannel },
    { HSD_STATE_TEV_STAGE,	_HSD_StateInvalidateTevStage },
    { HSD_STATE_TEV_REGISTER,	_HSD_StateInvalidateTevRegister },
    { HSD_STATE_TEX_COORD_GEN,	_HSD_StateInvalidateTexCoordGen },
    { HSD_STATE_RENDER_MODE,	_HSD_StateInvalidateRenderMode },
    { HSD_STATE_NONE, NULL }
    };
    
    for (u32 i = 0; invalidate_funcs[i].mask != HSD_STATE_NONE; i++) {
        if (state & invalidate_funcs[i].mask) {
            invalidate_funcs[i].func();
        }
    }
}

//803623D0
void HSD_StateSetNumChans(u8 num){
    if(num != state_num_chans){
        GX_SetNumChans(num);
    }
}

//803624E4
u32 HSD_StateGetNumTevStages(){
    return state_num_tevstages;
}

//803624EC
u8 HSD_StateAssignTev(){
    u8 tevstage = state_num_tevstages;
    state_num_tevstages += 1;
    return HSD_Index2TevStage(state_num_tevstages);
}

//80362518
void HSD_StateSetNumTevStages(){
    GX_SetNumTevStages(state_num_tevstages);
    state_num_tevstages = 0;
}

//80362768
u8 HSD_Index2TevStage(u8 idx){
    switch(idx){
        case 0: return GX_TEVSTAGE0;
        case 1: return GX_TEVSTAGE1;
        case 2: return GX_TEVSTAGE2;
        case 3: return GX_TEVSTAGE3;
        case 4: return GX_TEVSTAGE4;
        case 5: return GX_TEVSTAGE5;
        case 6: return GX_TEVSTAGE6;
        case 7: return GX_TEVSTAGE7;
        case 8: return GX_TEVSTAGE8;
        case 9: return GX_TEVSTAGE9;
        case 10: return GX_TEVSTAGE10;
        case 11: return GX_TEVSTAGE11;
        case 12: return GX_TEVSTAGE12;
        case 13: return GX_TEVSTAGE13;
        case 14: return GX_TEVSTAGE14;
        case 15: return GX_TEVSTAGE15;
        default: assert(TRUE); break;
    }
}

//80362838
u8 HSD_TevStage2Index(u8 idx){
    switch(idx){
        case GX_TEVSTAGE0: return 0;
        case GX_TEVSTAGE1: return 1;
        case GX_TEVSTAGE2: return 2;
        case GX_TEVSTAGE3: return 3;
        case GX_TEVSTAGE4: return 4;
        case GX_TEVSTAGE5: return 5;
        case GX_TEVSTAGE6: return 6;
        case GX_TEVSTAGE7: return 7;
        case GX_TEVSTAGE8: return 8;
        case GX_TEVSTAGE9: return 9;
        case GX_TEVSTAGE10: return 10;
        case GX_TEVSTAGE11: return 11;
        case GX_TEVSTAGE12: return 12;
        case GX_TEVSTAGE13: return 13;
        case GX_TEVSTAGE14: return 14;
        case GX_TEVSTAGE15: return 15;
        default: assert(TRUE); break;
    }
}

//80362CA0
void _HSD_StateInvalidateColorChannel(){

}

//80362CF8
void _HSD_StateInvalidateTevStage(){

}

//80362D04
void _HSD_StateInvalidateTevRegister(){

}

//80362D24
void _HSD_StateInvalidateTexCoordGen(){

}
