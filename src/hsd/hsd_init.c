#include "hsd_init.h"

#include "hsd_lobj.h"
#include "hsd_state.h"

//static OSHeapHandle hsd_heap = -1; //r13 - 0x58A0
static void* hsd_heap_arena_lo = NULL;
static void* hsd_heap_arena_hi = NULL;
static void* hsd_heap_next_arena_lo = NULL;
static void* hsd_heap_next_arena_hi = NULL;

static void* FrameBuffer[HSD_VI_XFB_MAX];

static GXRModeObj* rmode = &TVNtsc480IntDf;

static HSD_RenderPass current_render_pass;
static GXFifoObj* DefaultFifoObj = NULL;
static u8 current_pix_fmt = GX_PF_RGB8_Z24;
static u8 current_z_fmt = GX_ZC_MID;

static BOOL shown; //r13 - 0x3DF0
static BOOL init_done = FALSE; //r13 - 0x3FD4
static u32 iparam_fifo_size = HSD_DEFAULT_FIFO_SIZE;
static u32 iparam_xfb_max_num = HSD_DEFAULT_XFB_MAX_NUM;
static u32 iparam_heap_max_num = 0;
static u32 iparam_audio_heap_size = HSD_DEFAULT_AUDIO_SIZE;

//80374F28
void HSD_InitComponent(void)
{
    //HSD_OSInit();
    {
        HSD_VIStatus vi_status;
        GXColor black = { 0, 0, 0, 0 };

        vi_status.rmode = *rmode;
        vi_status.black = GX_TRUE;
        vi_status.vf = GX_TRUE;
        vi_status.gamma = GX_GM_1_0;
        vi_status.clear_clr = black;
        vi_status.clear_z = GX_MAX_Z24;
        vi_status.update_clr = GX_ENABLE;
        vi_status.update_alpha = GX_ENABLE;
        vi_status.update_z = GX_ENABLE;

        HSD_VIInit(&vi_status, FrameBuffer[0], FrameBuffer[1], FrameBuffer[2]);
    }
    HSD_GXInit();
    HSD_DVDInit();
    HSD_IDSetup();
    VIDEO_WaitVSync();
    HSD_ObjInit();
    init_done = TRUE;
}

//80374F60
void HSD_GXSetFifoObj(GXFifoObj* fifo)
{
    //memReport.gxfifo = iparam_fifo_size;
    DefaultFifoObj = fifo;
}

//80374F78
void HSD_DVDInit(void)
{
}

//80374F7C
void** HSD_AllocateXFB(u32 nbBuffer, GXRModeObj* rm)
{
    //u32 fbSize;

    if (!rm)
        return NULL;

    //fbSize = (VIDEO_PadFramebufferWidth(rm->fbWidth) * rm->xfbHeight * (u32)VI_DISPLAY_PIX_SZ);

    for (u32 i = 0; i < nbBuffer; i++) {
        /*if ((FrameBuffer[i] = (void *) SYS_AllocArena1MemLo(fbSize, 32)) == NULL) {
            assert(TRUE);
        }*/
        FrameBuffer[i] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rm));
        if (FrameBuffer[i] == NULL) {
            HSD_Halt("Failed to allocate framebuffer\n");
        }
    }

    for (u32 i = nbBuffer; i < HSD_VI_XFB_MAX; i++) {
        FrameBuffer[i] = NULL;
    }

    return FrameBuffer;
}

//80375194
void* HSD_AllocateFIFO(u32 size)
{
    void* fifo;
    fifo = HSD_MemAlloc(size);
    if (!fifo) {
        HSD_Halt("Failed to allocate GFX FIFO\n");
    }

    return fifo;
}

//80375258
void HSD_GXInit(void)
{
    {
        GXLightObj lightobj;
        static GXColor black = { 0, 0, 0, 0 };
        int i;

        GX_InitLightPos(&lightobj, 1.0, 0.0, 0.0);
        GX_InitLightDir(&lightobj, 1.0, 0.0, 0.0);
        GX_InitLightAttn(&lightobj, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0);
        GX_InitLightColor(&lightobj, black);
        for (i = 0; i < MAX_GXLIGHT - 1; i++){
            GX_LoadLightObj(&lightobj, HSD_Index2LightID(i));
        }
    }
    HSD_StateInvalidate(HSD_STATE_ALL);
}

//80375304 - TODO: Rewrite custom memory management with lwp_heap
void HSD_OSInit(void)
{
    hsd_heap_arena_lo = (void*)ROUNDUP32(SYS_GetArenaLo());
    hsd_heap_arena_hi = (void*)ROUNDDOWN32(SYS_GetArenaHi());

    /*void* arena_start = OSInitAlloc(hsd_heap_arena_lo, hsd_heap_arena_hi, iparam_heap_max_num);
	hsd_heap = OSCreateHeap(hsd_heap_arena_lo, hsd_heap_arena_hi);
	OSSetCurrentHeap(hsd_heap);

	HSD_ObjSetHeap((u32)hsd_heap_arena_hi - (u32)hsd_heap_arena_lo, NULL);
	SYS_SetArenaLo(hsd_heap_arena_hi);*/
}

//80375404
/*OSHeapHandle HSD_GetHeap(){
  return hsd_heap;
}*/

//8037540C
/*void HSD_SetHeap(OSHeapHandle heap){
  hsd_heap = heap;
}*/

//80375414
void HSD_SetNextArena(void* start, void* end)
{
    if ((!start && end) || (start && !end)) {
        assert(TRUE);
        return;
    }
    hsd_heap_next_arena_lo = start;
    hsd_heap_next_arena_hi = end;
}

//80375530
HSD_RenderPass HSD_GetCurrentRenderPass(void)
{
  return current_render_pass;
}

//80375538
void HSD_StartRender(HSD_RenderPass pass)
{
    current_render_pass = pass;

    //GXRModeObj *rmode = HSD_VIGetConfigure(); - This function isn't present in the final version of Melee
    if (rmode->aa) {
        GX_SetPixelFmt(GX_PF_RGB565_Z16, current_z_fmt);
    } else {
        GX_SetPixelFmt(current_pix_fmt, GX_ZC_LINEAR);
    }
    GX_SetFieldMode(rmode->field_rendering, rmode->xfbHeight < rmode->viHeight);
}

//803755B4
void HSD_ObjInit(void)
{
    //HSD_ListInitAllocData();
    HSD_AObjInitAllocData();
    HSD_FObjInitAllocData();
    HSD_IDInitAllocData();
    //HSD_VecInitAllocData();
    //HSD_MtxInitAllocData();
    HSD_RObjInitAllocData();
    //HSD_RenderInitAllocData();
    //HSD_ShadowInitAllocData();
    HSD_ZListInitAllocData();
}

//803755F8
void HSD_ObjDumpStat(void)
{
}

//803756F8
BOOL HSD_SetInitParameter(HSD_InitParam param, ...)
{
    va_list ap;
    BOOL result = TRUE;
    if (init_done) {
        shown = FALSE;
        if (!shown) {
            HSD_Report("init parameter should be set before invoking HSD_Init().\n");
            shown = TRUE;
        }
        return result;
    }

    va_start(ap, param);
    switch (param) {
    case HSD_INIT_FIFO_SIZE: {
        u32 fifo_size = va_arg(ap, u32);
        if (fifo_size > 0) {
            iparam_fifo_size = fifo_size;
            result = TRUE;
        }
    } break;

    case HSD_INIT_XFB_MAX_NUM: {
        u32 xfb_max_num = va_arg(ap, u32);
        if (xfb_max_num > 0) {
            iparam_xfb_max_num = xfb_max_num;
            result = TRUE;
        }
    } break;

    case HSD_INIT_HEAP_MAX_NUM: {
        u32 heap_size = va_arg(ap, u32);
        if (heap_size > 0) {
            iparam_heap_max_num = heap_size;
            result = TRUE;
        }
    } break;

    case HSD_INIT_AUDIO_HEAP_SIZE: {
        u32 heap_size = va_arg(ap, u32);
        if (heap_size > 0) {
            iparam_audio_heap_size = heap_size;
            result = TRUE;
        }
    } break;

    case HSD_INIT_RENDER_MODE_OBJ: {
        GXRModeObj* tmp = va_arg(ap, GXRModeObj*);
        if (tmp) {
            rmode = tmp;
            result = TRUE;
        }
    } break;

    default:
        break;
    }
    va_end(ap);

    return result;
}
