#include "hsd_state.h"

static u8   state_num_chans = -1; //This variable is set but unused?
static u8   state_num_tevstages;

/* HSD_STATE_PRIMITIVE Group */
static u8	state_line_width;
static u8	state_line_tex_offsets;
static u8	state_point_size;
static u8	state_point_tex_offsets;
static u8	state_cull_mode;
static u8	state_coplanar;

/* HSD_STATE_RENDER_MODE Group */
static u8	state_blend_type;
static u8	state_src_factor;
static u8	state_dst_factor;
static u8	state_logic_op;
static u8	state_z_enable;
static u8	state_z_func;
static u8   state_z_update;
static u8	state_alpha_comp0;
static u8	state_alpha_ref0;
static u8	state_alpha_op;
static u8	state_alpha_comp1;
static u8	state_alpha_ref1;
static u8	state_color_update;
static u8	state_alpha_update;
static u8	state_enable_dst_alpha;
static u8	state_dst_alpha;
static u8	state_before_tex;
static u8	state_dither;

static struct {
  GXColor ambient;
  GXColor diffuse;
  GXColor specular;
  u8  alpha;
  f32 shininess;
} matstate;

//803615D0
HSD_SetupChannelMode(u32 rendermode){
    
}

//80361778
HSD_SetupPEMode(u32 rendermode, HSD_PEDesc *pe){
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
void HSD_StateSetNumTevStages()
{
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