#ifndef _hsd_init_h_
#define _hsd_init_h_

#include <stdarg.h>
#include <gctypes.h>
#include <dolphin/os.h>
#include <ogc/system.h>
#include <ogc/video_types.h>

#include "hsd_aobj.h"
#include "hsd_fobj.h"
#include "hsd_lobj.h"
#include "hsd_robj.h"
#include "hsd_video.h"

#define HSD_DEFAULT_FIFO_SIZE 0x40000
#define HSD_DEFAULT_XFB_MAX_NUM 2
#define HSD_DEFAULT_AUDIO_SIZE 0x80000

#define ROUNDDOWN32(v)              (((u32)(v)) & ~(32 - 1))
#define ROUNDUP32(v)                (((u32)(v) + 32 - 1) & ~(32 - 1))

typedef enum _HSD_InitParam {
    HSD_INIT_FIFO_SIZE,
    HSD_INIT_XFB_MAX_NUM,
    HSD_INIT_HEAP_MAX_NUM,
    HSD_INIT_AUDIO_HEAP_SIZE,
    HSD_INIT_RENDER_MODE_OBJ
} HSD_InitParam;

#endif