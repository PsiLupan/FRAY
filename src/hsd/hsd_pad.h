#ifndef _hsd_pad_h_
#define _hsd_pad_h_

#include <gctypes.h>
#include <ogc/irq.h>
#include <ogc/pad.h>

typedef struct _HSD_RumbleQueue {
    u8 unk00;
    u8 unk01;
    u8 rumble_on;
    u8 unk03;
    u32 unk04;
    u32 unk08;
} HSD_RumbleQueue;

extern s8 pad_queue[];

s8 HSD_PadGetRawQueueCount();
BOOL HSD_PadGetResetSwitch();

void HSD_PadZeroQueue();
void HSD_PadRenewStatus();
void HSD_PadReset();

void HSD_PadRumbleOn(u32);
void HSD_PadRumbleOffH(u32);
void HSD_PadRumbleRemoveAll();

#endif
