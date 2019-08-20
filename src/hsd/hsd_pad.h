#ifndef _hsd_pad_h_
#define _hsd_pad_h_

#include <gctypes.h>
#include <ogc/irq.h>
#include <ogc/pad.h>

typedef struct _HSD_PadStatus {
    PADStatus stat[4];
    u32 rumble_mask;
} HSD_PadStatus;

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

extern s8 pad_queue[];

s8 HSD_PadGetRawQueueCount();
BOOL HSD_PadGetResetSwitch();

void HSD_PadRenewMasterStatus();
void HSD_PadZeroQueue();
void HSD_PadRenewStatus();
void HSD_PadReset();
void HSD_PadInit(u8, HSD_PadStatus*, u16, HSD_PadRumbleListData*);
void HSD_PadRumbleOn(u32);
void HSD_PadRumbleOffH(u32);
void HSD_PadRumbleRemoveAll();

#endif
