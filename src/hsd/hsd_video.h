#ifndef _hsd_video_h_
#define _hsd_video_h_

#include <gctypes.h>
#include <ogc/gx.h>

#include "hsd_init.h"

#define HSD_ANTIALIAS_OVERLAP 4
#define HSD_ANTIALIAS_GARBAGE_SIZE	(640 * HSD_ANTIALIAS_OVERLAP * VI_DISPLAY_PIX_SZ)
typedef void* HSD_VIGXDrawDoneCallback;
typedef void* VIRetraceCallback;

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
	GXRModeObj rmode;
	u8 black;
	u8 vf;
	u8 gamma;
	GXColor clear_clr;
	u32 clear_z;
	u8 update_clr;
	u8 update_alpha;
	u8 update_z;
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