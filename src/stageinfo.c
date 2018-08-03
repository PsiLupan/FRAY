#include <sys/types.h>

#include "stageinfo.h"

//80224A54
f32 StageInfo_GetCamLimitLeft(){
	return StageInfo.cambounds_left + StageInfo.cam_xoff;
}

//80224A68
f32 StageInfo_GetCamLimitRight(){
	return StageInfo.cambounds_right + StageInfo.cam_xoff;
}

//80224A80
f32 StageInfo_GetCamLimitTop(){
	return StageInfo.cambounds_top + StageInfo.cam_yoff;
}

//80224A98
f32 StageInfo_GetCamLimitBottom(){
	return StageInfo.cambounds_bottom + StageInfo.cam_yoff;
}

//80224BF8
f32* StageInfo_GetCamOffsets(f32 *pos)
{
	pos[0] = StageInfo.cam_xoff;
	pos[1] = StageInfo.cam_yoff;
	pos[2] = 0; //-0x3D08(r2);
	return pos;
}

//80224AB0
f32 StageInfo_GetCamPanXMulti(){
	return StageInfo.campan_x * 0.017453;
}

//80224AE8
f32 StageInfo_GetCamTiltY(){
	return StageInfo.camtilt_y;
}

//80224AF8
f32 StageInfo_GetGroundTilt(){
	return StageInfo.camtilt_gnd;
}

//80224B18
f32 StageInfo_GetCamTrackRatio(){
	return StageInfo.camtrack_ratio;
}

//80224B08
f32 StageInfo_GetCamZoom(){
	return StageInfo.cam_zoom;
}

//80224B28
f32 StageInfo_GetCamSpeedSmooth(){
	return StageInfo.cam_smooth;
}

//80224AD8
f32 StageInfo_GetCamZoomRate(){
	return StageInfo.cam_zoomrate;
}

//80224AC8
f32 StageInfo_GetCamMaxDepth(){
	return StageInfo.cam_maxdepth;
}

//80224C1C
f32 StageInfo_GetPauseCamMinZ()
{
	return StageInfo.pausecam_zpos_min;
}

//80224C2C
f32 StageInfo_GetPauseCamInitZ()
{
	return StageInfo.pausecam_zpos_init;
}

//80224C3C
f32 StageInfo_GetPauseCamMaxZ()
{
	return StageInfo.pausecam_zpos_max;
}

//80224C4C
f32 StageInfo_NormCamXMulti(){
	return StageInfo.normcam_pos[0] * 0.017453;
}

//80224C64
f32 StageInfo_NormCamYMulti(){
	return StageInfo.normcam_pos[1] * 0.017453;
}

//80224C7C
f32 StageInfo_NormCamZMulti(){
	return StageInfo.normcam_pos[2] * 0.017453;
}

//80224C94
f32 StageInfo_NormCamZoomMulti(){
	return StageInfo.normcam_zoom * 0.017453;
}

//80224D94
f32* StageInfo_GetFixCamPos(f32 *pos){
	pos[0] = StageInfo.fixedcam_pos[0];
	pos[1] = StageInfo.fixedcam_pos[1];
	pos[2] = StageInfo.fixedcam_pos[2];
	return pos;
}

//80224DB8
f32 StageInfo_GetFixCamZoom(){
	return StageInfo.fixedcam_zoom;
}

//80224B38
f32 StageInfo_GetBlastRightXOffset(){
	return StageInfo.blastpos_right + StageInfo.cam_xoff;
}

//80224B50
f32 StageInfo_GetBlastLeftXOffset(){
	return StageInfo.blastpos_left + StageInfo.cam_xoff;
}

//80224B68
f32 StageInfo_GetBlastTopYOffset(){
	return StageInfo.blastpos_top + StageInfo.cam_yoff;
}

//80224B80
f32 StageInfo_GetBlastBottomYOffset(){
	return StageInfo.blastpos_bottom + StageInfo.cam_yoff;
}

//80224B98
f32 StageInfo_GetBlastBottomMulti(){
	return (StageInfo_GetCamLimitBottom() + StageInfo_GetBlastBottomYOffset()) * 0.5;
}

//80224BC4
f32 StageInfo_GetBlastBottomMulti2(){
	return (StageInfo_GetBlastBottomMulti() + StageInfo_GetCamLimitBottom()) * 0.5;
}