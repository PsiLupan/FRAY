#ifndef _physics_c_
#define _physics_c_

#include <gctypes.h>
#include <ogc/gu.h>

typedef struct _Physics {
    void *unk;
    guVector x4_xC_TopN;
    guVector x10_x18_TopN_copy; //Assumed
    guVector x1C_x24_TopN_prev;
    guVector x28_x30_TopN_prev_copy; //Assumed
    
    s32 x3C_floor_id_actionstate;
    
    u32 x130_unk;
    s32 x14C_floor_id;
    u32 x150_ground_type;
} Physics;

u32 Physics_CollisionCheck_Ground(Physics*);

u32 Physics_FallOffLedge(Physics*);

#endif