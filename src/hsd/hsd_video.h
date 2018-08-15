#ifndef _hsd_video_h_
#define _hsd_video_h_

#include <gctypes.h>

#include "hsd_init.h"

#define HSD_ANTIALIAS_OVERLAP 4
#define HSD_ANTIALIAS_GARBAGE_SIZE	(640 * HSD_ANTIALIAS_OVERLAP * VI_DISPLAY_PIX_SZ)
typedef HSD_VIGXDrawDoneCallback void*

enum HSD_VIXFBDrawDispStatus {
	HSD_VI_XFB_NONE, 
	HSD_VI_XFB_NOUSE,
	HSD_VI_XFB_FREE,
	HSD_VI_XFB_DONE,
	HSD_VI_XFB_WAITDONE,
	HSD_VI_XFB_DRAWING,
	HSD_VI_XFB_DRAWDONE,
	HSD_VI_XFB_NEXT,
	HSD_VI_XFB_DISPLAY
};

struct HSD_VIStatus {
	GXRenderModeObj rmode;
	GXBool black;
	GXBool vf;
	GXGamma gamma;
	GXColor clear_clr;
	u32 clear_z;
	GXBool update_clr;
	GXBool update_alpha;
	GXBool update_z;
};

struct HSD_VIInfo {
	VIRetraceCallback pre_cb;
	VIRetraceCallback post_cb;
	
	struct drawdone {
		bool waiting;
		int arg;
		HSD_VIGXDrawDoneCallback cb;
	} drawdone;
	
	struct current {
		bool chg_flag;
		struct HSD_VIStatus vi;
	} current;
};

#endif