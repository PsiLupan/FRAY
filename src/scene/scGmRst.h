#ifndef _scGmRst_h_
#define _scGmRst_h_

#include <gctypes.h>

#include "hsd/hsd_archive.h"
#include "hsd/hsd_cobj.h"
#include "hsd/hsd_debug.h"
#include "hsd/hsd_gobj.h"
#include "hsd/hsd_lobj.h"

#include "hsd/ext/hsd_fog_ext.h"
#include "hsd/ext/hsd_lobj_ext.h"

#include "scene.h"

typedef struct _MnSlChar_joints {
    HSD_JObjDesc* x0_jobjdesc;
    HSD_AnimJoint* x4_animjoint;
    HSD_MatAnimJoint* x8_matanimjoint;
    HSD_ShapeAnimJoint* xC_shapeanimjoint;

    HSD_JObjDesc* x10_jobjdesc;
    HSD_AnimJoint* x14_animjoint;
    HSD_MatAnimJoint* x18_matanimjoint;
    HSD_ShapeAnimJoint* x1C_shapeanimjoint;

    HSD_JObjDesc* x20_jobjdesc;
    HSD_AnimJoint* x24_animjoint;
    HSD_MatAnimJoint* x28_matanimjoint;
    HSD_ShapeAnimJoint* x2C_shapeanimjoint;

    HSD_JObjDesc* x30_jobjdesc;
    HSD_AnimJoint* x34_animjoint;
    HSD_MatAnimJoint* x38_matanimjoint;
    HSD_ShapeAnimJoint* x3C_shapeanimjoint;

    HSD_JObjDesc* x40_jobjdesc;
    HSD_AnimJoint* x44_animjoint;
    HSD_MatAnimJoint* x48_matanimjoint;
    HSD_ShapeAnimJoint* x4C_shapeanimjoint;

    HSD_JObjDesc* x50_jobjdesc;
    HSD_AnimJoint* x54_animjoint;
    HSD_MatAnimJoint* x58_matanimjoint;
    HSD_ShapeAnimJoint* x5C_shapeanimjoint;

    HSD_JObjDesc* x60_jobjdesc;
    HSD_AnimJoint* x64_animjoint;
    HSD_MatAnimJoint* x68_matanimjoint;
    HSD_ShapeAnimJoint* x6C_shapeanimjoint;

    HSD_JObjDesc* x70_jobjdesc;
    HSD_AnimJoint* x74_animjoint;
    HSD_MatAnimJoint* x78_matanimjoint;
    HSD_ShapeAnimJoint* x7C_shapeanimjoint;
} MnSlChar_joints;

typedef struct _MnSlChar_file {
    HSD_CObjDesc* cobjdesc; //0x00
    HSD_LightDesc* lightdesc_1; //0x04
    HSD_LightDesc* lightdesc_2; //0x08
    HSD_FogDesc* fogdesc; //0x0C
    struct _MnSlChar_joints joints; //0x1C - ??
} MnSlChar_file;

void Menu_CSS_Init();
void Menu_CSS_Load();
void Menu_CSS_VSMode_Prep(GameState*);
void Menu_CSS_VSMode_Decide(GameState*);
void Menu_SSS_VSMode_Prep(GameState*);
void Menu_SSS_VSMode_Decide(GameState*);

void Menu_CSS_OnLoad(void* unk_struct);
void Menu_CSS_OnFrame();
void Menu_CSS_OnLeave();

#endif