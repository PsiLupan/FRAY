#ifndef _subaction_h_
#define _subaction_h_

#include <gctypes.h>

#include "gobj.h"
#include "player.h"

typedef struct _SubactionInfo {
	u32 timer;
	f32 frame_count;
	u32* data_position;
	u32 loop_count;
	void (*EventReturn);
	u32 loop_count_dup;
	u32 unk;
} SubactionInfo;

#endif