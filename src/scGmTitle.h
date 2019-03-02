#ifndef _scGmTitle_h_
#define _scGmTitle_h_

#include <gctypes.h>

#include "scene.h"

struct _GmTitle_File {
    void* dat_start;
    void* top_joint;
    void* top_animjoint;
    void* top_matanim_joint;
} GmTitle_File;

#endif