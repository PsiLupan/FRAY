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

//80362CA0
void _HSD_StateInvalidateColorChannel(){

}