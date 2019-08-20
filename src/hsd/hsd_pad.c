#include "hsd_pad.h"

HSD_PadLibData pad_data;
HSD_RumbleData rumble_data[4];

//8037699C
u8 HSD_PadGetRawQueueCount()
{
    HSD_PadLibData* p = &pad_data;
    u32 intr = IRQ_Disable();
    u8 queue_count = p->qcount;
    IRQ_Restore(intr);
    return queue_count;
}

//803769D8
s32 HSD_PadGetResetSwitch()
{
    HSD_PadLibData* p = &pad_data;
    return (p->reset_switch != 0) ? TRUE : FALSE;
}

void HSD_PadRawMerge(HSD_PadStatus* src1, HSD_PadStatus* src2, HSD_PadStatus* dst)
{
}

//803769FC
void HSD_PadRenewRawStatus()
{
    
}

//80376D04
void HSD_PadFlushQueue(HSD_FlushType ftype)
{
    HSD_PadLibData* p = &pad_data;
    u32 intr = IRQ_Disable();
    switch (ftype) {
    case HSD_PAD_FLUSH_QUEUE_MERGE:
        while (p->qcount > 1) {
            u8 t1 = p->qread;
            u8 t2 = t1 + 1;
            p->qread = t2 - (t2 / p->qnum) * p->qnum;
            HSD_PadStatus pad = p->queue[t1];
            HSD_PadStatus pad_2 = p->queue[p->qread];
            pad_2[0] = ptr[0] | ptr_2[0];
            pad_2[6] = ptr[6] | ptr_2[6];
            pad_2[0xc] = ptr[0xc] | ptr_2[0xc];
            pad_2[0x12] = ptr[0x12] | ptr_2[0x12];
            p->qcount -= 1;
        }
        break;

    case HSD_PAD_FLUSH_QUEUE_THROWAWAY:
        p->qread = p->qwrite;
        p->qcount = 0;
        break;

    case HSD_PAD_FLUSH_QUEUE_LEAVE1:
        if (p->qcount > 1) {
            u8 num = 0;
            if (p->qwrite > 0) {
                num = p->qwrite - 1;
            } else {
                num = p->qnum - 1;
            }
            p->qread = num;
            p->qcount = 1;
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
    HSD_PadLibData* p = &pad_data;
    u32 intr = IRQ_Disable();
    
    HSD_PadRumbleRemoveAll();
    
    for (u8 i = 0; i < 4; ++i) {
        HSD_PadRumbleOffH(i);
    }
    
    HSD_PadFlushQueue(HSD_PAD_FLUSH_QUEUE_THROWAWAY);
    PAD_Recalibrate(0xF0000000);
    p->reset_switch = 0;
    
    IRQ_Restore(intr);
}

//80377D98
void HSD_PadInit(u8 qnum, HSD_PadStatus* queue, u16 nb_list, HSD_PadRumbleListData* listdatap)
{
    
}

//80378090
void HSD_PadRumbleOn(u8 pad)
{
    u32 intr = IRQ_Disable();
    rumble_data[pad].rumble_on = 1;
    IRQ_Restore(intr);
}

void HSD_PadRumbleOffN(u8 pad)
{
}

//803780DC
void HSD_PadRumbleOffH(u8 pad)
{
    u32 intr = IRQ_Disable();
    rumble_data[pad].rumble_on = 0;
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
