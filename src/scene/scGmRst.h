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
    HSD_JObjDesc* jobjdesc;
    HSD_AnimJoint* animjoint;
    HSD_MatAnimJoint* matanimjoint;
    HSD_ShapeAnimJoint* shapeanimjoint;
} MnSlChar_joints;

typedef struct _MnSlChar_file {
    HSD_CObjDesc* cobjdesc;
    HSD_LightDesc* lightdesc_1;
    HSD_LightDesc* lightdesc_2;
    HSD_FogDesc* fogdesc;
    struct _MnSlChar_joints joints;
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