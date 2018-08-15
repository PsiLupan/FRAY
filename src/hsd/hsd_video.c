#include "hsd_video.h"

static u32 HSD_VINumXFB = 0; 
HSD_VIInfo HSD_VIData;

#define _p	((HSD_VIInfo *)&HSD_VIData)

static u8 garbage[HSD_ANTIALIAS_GARBAGE_SIZE] ATTRIBUTE_ALIGN(32);

//8037588C
static int HSD_VISearchXFBByStatus(HSD_VIXFBDrawDispStatus status)
{
  int i;

  for (i = 0; i < HSD_VI_XFB_MAX; i++){
	  if (_p->xfb[i].status == status) 
		  return i;
  }
  return -1;
}

//803758DC
VIRetraceCallback HSD_VISetUserPreRetraceCallback(VIRetraceCallback cb){
	bool intr;
	
	VIRetraceCallback old = _p->pre_cb;
	intr = OSDisableInterrupts();
	_p->pre_cb = cb;
	OSRestoreInterrupts(intr);
	
	return old;
}

//80375934
VIRetraceCallback HSD_VISetUserGXDrawDoneCallback(VIRetraceCallback cb){
	bool intr;
	
	VIRetraceCallback old = _p->post_cb;
	intr = OSDisableInterrupts();
	_p->post_cb = cb;
	OSRestoreInterrupts(intr);
	
	return old;
}

//8037598C
HSD_VIGXDrawDoneCallback HSD_VISetUserGXDrawDoneCallback(HSD_VIGXDrawDoneCallback cb){
	bool intr;
	
	HSD_VIGXDrawDoneCallback old = _p->drawdone.cb;
	intr = OSDisableInterrupts();
	_p->drawdone.cb = cb;
	OSRestoreInterrupts(intr);
	
	return old;
}

//80375E1C
static void HSD_VIGXDrawDoneCB(){
	_p->drawdone.waiting = 0;
	
	if (_p->drawdone.cb)
		_p->drawdone.cb(_p->drawdone.arg);
}

//80375E60
static int HSD_VIGetDrawDoneWaitingFlag(){
  return _p->drawdone.waiting;
}

//80375F74
void HSD_VICopyEFB2XFBPtr(HSD_VIStatus *vi, void *buffer, HSD_RenderPass rpass)
{
  GXRenderModeObj *rmode = &vi->rmode;
  int n_xfb_lines;
  u16 lines;
  u32 offset;

  GXSetCopyFilter(rmode->aa, rmode->sample_pattern, vi->vf, rmode->vfilter);
  GXSetDispCopyGamma(vi->gamma);

  HSD_StateSetColorUpdate(vi->update_clr);
  HSD_StateSetAlphaUpdate(vi->update_alpha);
  HSD_StateSetZMode(vi->update_z, GX_LEQUAL, GX_TRUE);

  GXSetCopyClear(vi->clear_clr, vi->clear_z);

  switch (rpass) {
  case HSD_RP_SCREEN:
    GXSetCopyClamp((GXFBClamp)(GX_CLAMP_TOP | GX_CLAMP_BOTTOM));
    GXSetDispCopySrc(0, 0, rmode->fbWidth, rmode->efbHeight);
    n_xfb_lines = GXSetDispCopyYScale((f32)(rmode->xfbHeight)
				      / (f32)(rmode->efbHeight)); 
    GXSetDispCopyDst(rmode->fbWidth, n_xfb_lines);
    GXCopyDisp(buffer, GX_TRUE);
    break;

  case HSD_RP_TOPHALF:
    HSD_VICopyEFB2XFBHiResoAA(rmode);
    GXSetCopyClamp(GX_CLAMP_TOP);
    lines = rmode->efbHeight - HSD_ANTIALIAS_OVERLAP;
    GXSetDispCopySrc(0, 0, rmode->fbWidth, lines);
    GXCopyDisp(buffer, GX_TRUE);
    GXPixModeSync();
    return;

  case HSD_RP_BOTTOMHALF:
    GXSetDispCopySrc(0, 0, rmode->fbWidth, rmode->efbHeight - HSD_ANTIALIAS_OVERLAP);
	GXSetDispCopyDst(rmode->fbWidth, GXSetDispCopyYScale(1.0));
    GXSetCopyClamp(GX_CLAMP_BOTTOM);
	lines = rmode->efbHeight - HSD_ANTIALIAS_OVERLAP;
	GXSetDispCopySrc(0, HSD_ANTIALIAS_OVERLAP, rmode->fbWidth, lines);
    offset = (VIPadFrameBufferWidth(rmode->fbWidth) * lines * (u32) VI_DISPLAY_PIX_SZ);
    GXCopyDisp((void *)((u32)buffer + offset), GX_TRUE);
    GXSetDispCopySrc(0, 0, rmode->fbWidth, HSD_ANTIALIAS_OVERLAP);
    GXSetCopyClamp((GXFBClamp)(GX_CLAMP_TOP | GX_CLAMP_BOTTOM));
    GXCopyDisp((void *)garbage, GX_TRUE);
    break;

  default:
    //HSD_PANIC("unexpected type of render pass.\n")
  }

  GXPixModeSync();
}

//803761C0
void HSD_VICopyXFBASync(HSD_RenderPass rpass){
	int idx = -1;
	
	if (HSD_VIGetNumXFB() >= 2){
		while((idx = HSD_VIGetXFBDrawEnable()) == -1)
			VIWaitForRetrace();

		HSD_VICopyEFB2XFBPtr(HSD_VIGetVIStatus(), HSD_VIGetXFBPtr(idx), rpass);
		HSD_VISetXFBWaitDone(idx);
	
		while (HSD_VIGetDrawDoneWaitingFlag())
			GXWaitDrawDone();
		_p->drawdone.waiting = 1;
		_p->drawdone.arg = arg;
		GXSetDrawDone();
	}
}

//803762C4
void HSD_VIDrawDoneXFB(int idx){
	bool intr = OSDisableInterrupts();
	
	assert(_p->xfb[idx].status == HSD_VI_XFB_WAITDONE);
	_p->xfb[idx].status = HSD_VISearchXFBByStatus(HSD_VI_XFB_NEXT) != -1 ? HSD_VI_XFB_DRAWDONE : HSD_VI_XFB_NEXT;
	
	OSRestoreInterrupts(intr);
}

//8037639C
void HSD_VISetXFBDrawDone(void){
	bool intr;
	int idx;
	if (HSD_VIGetNumXFB() >= 2){
		intr = OSDisableInterrupts();
		
		idx  = HSD_VISearchXFBByStatus(HSD_VI_XFB_DRAWING);
		assert(idx != -1);
		HSD_VISetXFBWaitDone(idx);
		HSD_VIDrawDoneXFB(idx);
		
		OSRestoreInterrupts(intr);
	}
}

//807364A8
int HSD_VIGetXFBLastDrawDone()
{
	int idx = -1;
	bool intr = OSDisableInterrupts();
	if ((idx = HSD_VISearchXFBByStatus(HSD_VI_XFB_WAITDONE)) == -1)
		if ((idx = HSD_VISearchXFBByStatus(HSD_VI_XFB_DRAWDONE)) == -1)
			if ((idx = HSD_VISearchXFBByStatus(HSD_VI_XFB_NEXT)) == -1)
				idx = HSD_VISearchXFBByStatus(HSD_VI_XFB_DISPLAY);
	OSRestoreInterrupts(intr);

  return idx;
}

//80375E70
int HSD_VIGetXFBDrawEnable(){
	bool intr;
	int idx = -1;
	
	if (HSD_VIGetNumXFB() >= 2){
		intr = OSDisableInterrupts();
		if ((idx = HSD_VISearchXFBByStatus(HSD_VI_XFB_DRAWING)) == -1)
			if ((idx = HSD_VISearchXFBByStatus(HSD_VI_XFB_FREE)) != -1)
				_p->xfb[idx].status = HSD_VI_XFB_DRAWING;
	
		OSRestoreInterrupts(intr);
	}
	return idx;
}

//80376718
void HSD_VISetConfigure(GXRenderModeObj *rmode)
{
  _p->current.vi.rmode = *rmode;
  _p->current.chg_flag = 1;
}

static u32 HSD_VIGetNumXFB(){
	return HSD_VINumXFB;
}