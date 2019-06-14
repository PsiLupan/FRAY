#ifndef _stageinfo_h_
#define _stageinfo_h_

#include <gctypes.h>

#include "gobj.h"

#define STG_INTERNAL_CORNERIA 0xE
#define STG_INTERNAL_VENOM 0xF

//0x778 bytes
struct StageInfo
{
	f32 cambounds_left;
	f32 cambounds_right;
	f32 cambounds_top;
	f32 cambounds_bottom;
	f32 cam_xoff;
	f32 cam_yoff;
	f32 cam_vertilt;
	f32 campan_x;
	f32 camtilt_y;
	f32 camtilt_gnd;
	f32 camtrack_ratio;
	f32 cam_zoom;
	f32 cam_smooth;
	f32 cam_zoomrate;
	f32 cam_maxdepth;
	f32 pausecam_zpos_min;
	f32 pausecam_zpos_init;
	f32 pausecam_zpos_max;
	f32 normcam_pos[3];
	f32 normcam_zoom;
	f32 fixedcam_pos[3];
	f32 fixedcam_zoom;
	f32 fixedcam_xrot;
	f32 fixedcam_xfocus;
	f32 blastpos_left;
	f32 blastpos_right;
	f32 blastpos_top;
	f32 blastpos_bottom;
	u32 trophy_drops;
	u32 internal_id; //88
};

f32 StageInfo_GetCamLimitLeft();
f32 StageInfo_GetCamLimitRight();
f32 StageInfo_GetCamLimitTop();
f32 StageInfo_GetCamLimitBottom();
f32* StageInfo_GetCamOffsets(f32 *);

f32 StageInfo_GetCamPanXMulti();
f32 StageInfo_GetCamTiltY();
f32 StageInfo_GetGroundTilt();

f32 StageInfo_GetCamTrackRatio();
f32 StageInfo_GetCamZoom();
f32 StageInfo_GetCamSpeedSmooth();
f32 StageInfo_GetCamZoomRate();
f32 StageInfo_GetCamMaxDepth();

f32 StageInfo_GetPauseCamMinZ();
f32 StageInfo_GetPauseCamInitZ();
f32 StageInfo_GetPauseCamMaxZ();

f32 StageInfo_NormCamXMulti();
f32 StageInfo_NormCamYMulti();
f32 StageInfo_NormCamZMulti();
f32 StageInfo_NormCamZoomMulti();

f32* StageInfo_GetFixCamPos(f32 *);
f32 StageInfo_GetFixCamZoom();

f32 StageInfo_GetBlastRightXOffset();
f32 StageInfo_GetBlastLeftXOffset();
f32 StageInfo_GetBlastTopYOffset();
f32 StageInfo_GetBlastBottomYOffset();
f32 StageInfo_GetBlastBottomMulti();
f32 StageInfo_GetBlastBottomMulti2();

#endif
