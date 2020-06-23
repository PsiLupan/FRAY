#ifndef _scGmTitle_h_
#define _scGmTitle_h_

#include <gctypes.h>

#include "hsd/hsd_gobj.h"

#include "scene.h"

typedef struct _GmTitle_File {
    void* top_joint;
    void* top_animjoint;
    void* top_matanim_joint;
    void* top_shapeanim_joint;
    void* bg_top_joint;
    void* bg_top_animjoint;
    void* bg_top_matanim_joint;
    void* bg_top_shapeanim_joint;
    void* debug_text;
} GmTitle_File;

extern GmTitle_File title_ptrs;
extern u32 title_inputs;
extern f32 title_frames[3];

void Menu_Title_ReqAnimAll_Callback(HSD_GObj*);
void Menu_Title_SetupLogos();

void Menu_Title_OnFrame(u32, u32);
void Menu_Title_OnLoad(void*);

void Menu_Title_Prep(GameState*);
void Menu_Title_Decide(GameState*);
void Menu_Title_LoadDemo();

#endif
