#ifndef _scGmTitle_h_
#define _scGmTitle_h_

#include <gctypes.h>

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

extern f32 title_frames[3];

void Menu_Title_SetupLogos();
void Menu_Title_Prep();
void Menu_Title_Decide(void* state);
void Menu_Title_LoadDemo();

#endif
