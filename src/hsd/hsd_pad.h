#ifndef _hsd_pad_h_
#define _hsd_pad_h_

#include <gctypes.h>
#include <ogc/irq.h>
#include <ogc/pad.h>

typedef enum _HSD_FlushType {
    HSD_PAD_FLUSH_QUEUE_MERGE = 0,
    HSD_PAD_FLUSH_QUEUE_THROWAWAY = 1,
    HSD_PAD_FLUSH_QUEUE_LEAVE1 = 2,
    HSD_PAD_FLUSH_QUEUE_TERMINATE = 3
} HSD_FlushType;

typedef union _HSD_Rumble {
    u16 def;
    struct {
        u16 op;
        u16 frame;
    } command;
} HSD_Rumble;

typedef struct _HSD_PadRumbleListData {
    struct _HSD_PadRumbleListData* next;
    u32 id;
    u8 pause;
    u8 pri;
    u8 status;
    u16 loop_count;
    u16 wait;
    s32 frame;
    HSD_Rumble* stack;
    HSD_Rumble* listp;
    HSD_Rumble* headp;
} HSD_PadRumbleListData;

typedef struct _HSD_PadData {
    PADStatus stat[4];
    u32 rumble_mask;
} HSD_PadData;

typedef struct _HSD_PadStatus {
    u32 button;
    u32 last_button;
    u32 trigger;
    u32 repeat;
    u32 release;
    s32 repeat_count;
    s8 stickX;
    s8 stickY;
    s8 subStickX;
    s8 subStickY;
    u8 analogL;
    u8 analogR;
    u8 analogA;
    u8 analogB;
    f32 nml_stickX;
    f32 nml_stickY;
    f32 nml_subStickX;
    f32 nml_subStickY;
    f32 nml_analogL;
    f32 nml_analogR;
    f32 nml_analogA;
    f32 nml_analogB;
    u8 cross_dir;
    u8 support_rumble;
    s8 err;
} HSD_PadStatus;

u8 HSD_PadGetRawQueueCount();
s32 HSD_PadGetResetSwitch();
void HSD_PadRenewRawStatus();
void HSD_PadFlushQueue(HSD_FlushType);
void HSD_PadClampCheck1(u8*, u8, u8, u8);
void HSD_PadClampCheck3(s8*, s8*, u8, s8, s8);
void HSD_PadRenewMasterStatus();
void HSD_PadRenewCopyStatus();
void HSD_PadZeroQueue();
void HSD_PadRenewStatus();
void HSD_PadReset();
void HSD_PadInit(u8, HSD_PadData*, u16, HSD_PadRumbleListData*);
void HSD_PadRumbleOn(u8);
void HSD_PadRumbleOffH(u8);
void HSD_PadRumbleRemoveAll();

#endif
