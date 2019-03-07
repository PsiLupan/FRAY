#ifndef _scGmTitle_h_
#define _scGmTitle_h_

#include <gctypes.h>

#include "scene.h"

typedef struct _GmTitle_File {
    void* dat_start;
    void* top_joint;
    void* top_animjoint;
    void* top_matanim_joint;
} GmTitle_File;

extern GmTitle_File title_ptrs;
extern MinorScene GmTitle_Minors[2];


void Menu_Title_Prep();
void Menu_Title_Decide(void*);

#endif