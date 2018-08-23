#include "hsd_init.h"

static OSHeapHandle hsd_heap = -1; //r13 - 0x58A0
static void *hsd_heap_arena_lo = NULL;
static void *hsd_heap_arena_hi = NULL;
static void *hsd_heap_next_arena_lo = NULL;
static void *hsd_heap_next_arena_hi = NULL;

static void *FrameBuffer[HSD_VI_XFB_MAX];

static bool shown; //r13 - 0x3DF0
static bool init_done = false; //r13 - 0x3FD4

//80374F28
void HSD_InitComponent(){
	HSD_OSInit();
	{
		HSD_VIStatus vi_status;
		GXColor black = { 0, 0, 0, 0};
		
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
	VIWaitForRetrace();
	HSD_ObjInit();
	init_done = 1;
}

//80375304
void HSD_OSInit(){
	u8*    arenaLo;
	u8*    arenaHi;
    arenaLo = OSGetArenaLo();
    arenaHi = OSGetArenaHi();
	
	if (!iparam_memory_callbacks) {
		HSD_MemCallbacks cb;
		cb.alloc_func = _HSD_MemAllocDefaultCB;
		cb.free_func = _HSD_MemFreeDefaultCB;
		cb.remain_func = _HSD_MemGetRemainDefaultCB;
		cb.clear_func = _HSD_MemClearDefaultCB;
		cb.own_func = _HSD_MemCheckOwnDefaultCB;
		_HSD_MemSetCallbacks(&cb, sizeof(cb));
		
		if (hsd_heap_next_arena_lo && hsd_heap_next_arena_hi) {
			hsd_heap_arena_lo = hsd_heap_next_arena_lo;
			hsd_heap_arena_hi = hsd_heap_next_arena_hi;
			hsd_heap_next_arena_lo = NULL;
			hsd_heap_next_arena_hi = NULL;
			
			hsd_heap = OSCreateHeap(hsd_heap_arena_lo, hsd_heap_arena_hi);
			OSSetCurrentHeap(hsd_heap);
			HSD_ObjSetHeap((u32)hsd_heap_arena_hi - (u32)hsd_heap_arena_lo, NULL);
		} else {
			arenaLo = (void*)OSRoundUp32B(OSGetArenaLo());
			arenaHi = OSGetArenaHi();
		
			hsd_heap_arena_lo = arenaLo;
			if (iparam_heap_size > 0) {
				hsd_heap_arena_hi = arenaLo + iparam_heap_size;
				if (hsd_heap_arena_hi > arenaHi)
					hsd_heap_arena_hi = arenaHi;
				hsd_heap_arena_hi = (void*)OSRoundDown32B(hsd_heap_arena_hi);
			} else {
				hsd_heap_arena_hi = (void*)OSRoundDown32B(arenaHi);
			}
			hsd_heap = OSCreateHeap(hsd_heap_arena_lo, hsd_heap_arena_hi);
			OSSetCurrentHeap(hsd_heap);
			HSD_ObjSetHeap((u32)hsd_heap_arena_hi - (u32)hsd_heap_arena_lo, NULL);
			OSSetArenaLo(hsd_heap_arena_hi);
		}
	}
}

static void* _HSD_MemAllocDefaultCB(u32 size, u32 align, u32 flags)
{
#pragma unused(align, flags)
  void *addr;

  if (size == 0)
    return NULL;

  addr = OSAllocFromHeap(HSD_GetHeap(), size);
  assert (addr);
  return addr;
} 

static void _HSD_MemFreeDefaultCB(void *addr){
	OSFreeToHeap(HSD_GetHeap(), addr);
}

static u32 _HSD_MemGetRemainDefaultCB(void){
	return OSCheckHeap(HSD_GetHeap());
}

static void _HSD_MemClearDefaultCB(void)
{
  OSDestroyHeap(hsd_heap);
  if (hsd_heap_next_arena_lo && hsd_heap_next_arena_hi) {
    hsd_heap_arena_lo = hsd_heap_next_arena_lo;
    hsd_heap_arena_hi = hsd_heap_next_arena_hi;
    hsd_heap_next_arena_lo = NULL;
    hsd_heap_next_arena_hi = NULL;
  }
  hsd_heap = OSCreateHeap(hsd_heap_arena_lo, hsd_heap_arena_hi);
  OSSetCurrentHeap(hsd_heap);
}

static int _HSD_MemCheckOwnDefaultCB(void *addr){
	return hsd_heap_arena_lo <= addr && addr < hsd_heap_arena_hi;
}

//803755B4
void HSD_ObjInit(){
  HSD_ListInitAllocData();
  HSD_AObjInitAllocData();
  HSD_FObjInitAllocData();
  HSD_IDInitAllocData();
  HSD_VecInitAllocData();
  HSD_MtxInitAllocData();
  HSD_RObjInitAllocData();
  HSD_RenderInitAllocData();
  HSD_ShadowInitAllocData();
  HSD_ZListInitAllocData();
}

//803756F8
bool HSD_SetInitParameter(HSD_InitParam param, ...){
	va_list ap;
	bool result = false;
	if (init_done) {
		shown = false;
		if (!shown) {
			//HSD_Report("init parameter should be set before invoking HSD_Init().\n");
			shown = true;
		}
		return result;
	}

  va_start(ap, param);
  switch (param) {
  case HSD_INIT_FIFO_SIZE:
    {
      u32 fifo_size = va_arg(ap, u32);
      if (fifo_size > 0) {
		iparam_fifo_size = fifo_size;
		result = 1;
      }
    }
    break;

  case HSD_INIT_XFB_MAX_NUM:
    {
      u32 xfb_max_num = va_arg(ap, u32);
      if (xfb_max_num > 0) {
		iparam_xfb_max_num = xfb_max_num;
		result = 1;
      }
    }
    break;

  case HSD_INIT_HEAP_SIZE:
    {
      u32 heap_size = va_arg(ap, u32);
      if (heap_size < OSGetPhysicalMemSize()) {
		  iparam_heap_size = heap_size;
		  result = 1;
      }
    }
    break;
  
  case HSD_INIT_HEAP_ARENA:
    {
      void *arena_lo = va_arg(ap, void *);
      void *arena_hi = va_arg(ap, void *);

      hsd_heap_next_arena_lo = arena_lo;
      hsd_heap_next_arena_hi = arena_hi;
      result = true;
    }
    break;

  case HSD_INIT_MEMORY_CALLBACKS:
    {
      HSD_MemCallbacks cb;
      void *terminate;

      cb.alloc_func = va_arg(ap, HSD_MemAllocFunc);
      cb.free_func = va_arg(ap, HSD_MemFreeFunc);
      cb.clear_func = va_arg(ap, HSD_MemClearFunc);
      cb.remain_func = va_arg(ap, HSD_MemGetRemainFunc);
      cb.own_func = va_arg(ap, HSD_MemCheckOwnFunc);
      terminate = va_arg(ap, void *);
      if (terminate != NULL) {
		  //ERROR STATE
      } else {
		  _HSD_MemSetCallbacks(&cb, sizeof(cb));
		  iparam_memory_callbacks = 1;
		  result = true;
      }
    }
    break;

  case HSD_INIT_RENDER_MODE_OBJ:
    {
      GXRenderModeObj *tmp = va_arg(ap, GXRenderModeObj *);
      if (tmp) {
		rmode = tmp;
		result = 1;
      }
    }
    break;

  case HSD_INIT_HEAP_MAX_NUM:
    {
      int num = va_arg(ap, int);
      if (num == 0) {
		  result = true;
      }
    }
    break;
  
  case HSD_INIT_AUDIO_HEAP_SIZE:
    break;

  default:
    break;
  }
  va_end(ap);

  return result;
}

//80375404
OSHeapHandle HSD_GetHeap(){
	assert(iparam_memory_callbacks);
	return hsd_heap;
}

//8037540C
static void HSD_SetHeap(OSHeapHandle heap){
	hsd_heap = heap;
}