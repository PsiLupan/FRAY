#include "hsd_pad.h"

s8 pad_queue[0x44 * 4]; //Length determined from 80377B54's loop
HSD_RumbleQueue rumble_queue[4];

//8037699C
s8 HSD_PadGetRawQueueCount()
{
    u32 intr = IRQ_Disable();
    s8 queue_count = pad_queue[3];
    IRQ_Restore(intr);
    return queue_count;
}

//803769D8
BOOL HSD_PadGetResetSwitch()
{
    return (pad_queue[0x2B] != 0) ? TRUE : FALSE;
}

//803769FC
void HSD_PadRenewRawStatus(u32 unk)
{
}

//80376D04
void HSD_PadFlushQueue(u32 state)
{
    u32 intr = IRQ_Disable();
    switch (state) {
    case 0:
        while (pad_queue[3] > 1) {
            s8 t1 = pad_queue[1];
            s8 t2 = t1 + 1;
            pad_queue[1] = t2 - (t2 / pad_queue[0]) * pad_queue[0];
            s16* ptr = (s16*)(&pad_queue[8] + t1 * 0x30);
            s16* ptr_2 = (s16*)(&pad_queue[8] + pad_queue[1] * 0x30);
            ptr_2[0] = ptr[0] | ptr_2[0];
            ptr_2[6] = ptr[6] | ptr_2[6];
            ptr_2[0xc] = ptr[0xc] | ptr_2[0xc];
            ptr_2[0x12] = ptr[0x12] | ptr_2[0x12];
            pad_queue[3] -= 1;
        }
        break;

    case 1:
        pad_queue[1] = pad_queue[2];
        pad_queue[3] = 0;
        break;

    case 2:
        if (pad_queue[3] > 1) {
            s8 num = 0;
            if (pad_queue[2] > 0) {
                num = pad_queue[2] - 1;
            } else {
                num = pad_queue[0] - 1;
            }
            pad_queue[1] = num;
            pad_queue[3] = 1;
        }
        break;
    }
    IRQ_Restore(intr);
}

//8037750C
void HSD_PadRenewMasterStatus()
{
}

//80377B54
void HSD_PadZeroQueue()
{
}

//80377CE8
void HSD_PadRenewStatus()
{
    HSD_PadRenewRawStatus(0);
    HSD_PadRenewMasterStatus();
    HSD_PadRenewCopyStatus();
    HSD_PadZeroQueue(); //Not present in later versions
}

//80377D18
void HSD_PadReset()
{
    u32 intr = IRQ_Disable();
    HSD_PadRumbleRemoveAll();
    s32 count = 0;
    do {
        HSD_PadRumbleOffH(count++);
    } while (count < 4);
    HSD_PadFlushQueue(1);
    PAD_Recalibrate(0xF0000000);
    pad_queue[43] = 0;
    IRQ_Restore(intr);
}

//80377D98
void HSD_PadInit(u32 size, u8 raw_queue[], u32 unk, void* unk_queue)
{ //raw_queue is normally HAD_PadRawQueue - Don't know specifics yet
}

//80378090
void HSD_PadRumbleOn(u32 pad)
{
    u32 intr = IRQ_Disable();
    rumble_queue[pad].rumble_on = 1;
    IRQ_Restore(intr);
}

//803780DC
void HSD_PadRumbleOffH(u32 pad)
{
    u32 intr = IRQ_Disable();
    rumble_queue[pad].rumble_on = 0;
    IRQ_Restore(intr);
}

//80378208
void HSD_PadRumbleRemoveAll()
{
    for (u32 i = 0; i < 4; i++) {
        u32 intr = IRQ_Disable();
        //TODO
        IRQ_Restore(intr);
    }
}