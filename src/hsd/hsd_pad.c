#include "hsd_pad.h"

struct {
    u8 qnum;
    u8 qread;
    u8 qwrite;
    u8 qcount;
    u8 qtype;
    struct _HSD_PadStatus* queue;
    s32 repeat_start;
    s32 repeat_interval;
    u8 adc_type;
    s8 adc_th;
    f32 adc_angle;
    u8 clamp_stickType;
    u8 clamp_stickShift;
    s8 clamp_stickMax;
    s8 clamp_stickMin;
    u8 clamp_analogLRShift;
    u8 clamp_analogLRMax;
    u8 clamp_analogLRMin;
    u8 clamp_analogABShift;
    u8 clamp_analogABMax;
    u8 clamp_analogABMin;
    s8 scale_stick;
    u8 scale_analogLR;
    u8 scale_analogAB;
    u8 cross_dir;
    u8 reset_switch_status;
    u8 reset_switch;
    struct {
        u16 max_list;
        struct _HSD_PadRumbleListData* listdatap
    } rumble_info;
} HSD_PadLibData;

struct {
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
} HSD_PadMasterStatus[4];

struct {
    u8 last_status;
    u8 status;
    u8 direct_status;
    u16 nb_list;
    struct _HSD_PadRumbleListData* listdatap;
} HSD_PadRumbleStatus[4];

//8037699C
u8 HSD_PadGetRawQueueCount()
{
    u32 intr = IRQ_Disable();
    u8 queue_count = HSD_PadLibData.qcount;
    IRQ_Restore(intr);
    return queue_count;
}

//803769D8
s32 HSD_PadGetResetSwitch()
{
    return (HSD_PadLibData.reset_switch != 0) ? TRUE : FALSE;
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
    u32 intr = IRQ_Disable();
    switch (ftype) {
    case HSD_PAD_FLUSH_QUEUE_MERGE: //Couldn't trigger during gameplay, so couldn't understand what it does with the qread/qdist part
        /*while (HSD_PadLibData.qcount > 1) {
            u8 t1 = HSD_PadLibData.qread;
            u8 t2 = t1 + 1;
            HSD_PadLibData.qread = t2 - (t2 / HSD_PadLibData.qnum) * HSD_PadLibData.qnum;
            HSD_PadStatus qdist = HSD_PadLibData.queue[t1];
            HSD_PadStatus qread = HSD_PadLibData.queue[HSD_PadLibData.qread];
            qread[0] = qdist[0] | qread[0];
            qread[6] = qdist[6] | qread[6];
            qread[0xc] = qdist[0xc] | qread[0xc];
            qread[0x12] = qdist[0x12] | qread[0x12];
            HSD_PadLibData.qcount -= 1;
        }*/
        break;

    case HSD_PAD_FLUSH_QUEUE_THROWAWAY:
        HSD_PadLibData.qread = HSD_PadLibData.qwrite;
        HSD_PadLibData.qcount = 0;
        break;

    case HSD_PAD_FLUSH_QUEUE_LEAVE1:
        if (HSD_PadLibData.qcount > 1) {
            HSD_PadLibData.qread = HSD_PadLibData.qnum;
            if (HSD_PadLibData.qwrite != 0) {
                HSD_PadLibData.qread = HSD_PadLibData.qwrite;
            }
            HSD_PadLibData.qread = HSD_PadLibData.qread - 1;
            HSD_PadLibData.qcount = 1;
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
    
    for (u8 i = 0; i < 4; ++i) {
        HSD_PadRumbleOffH(i);
    }
    
    HSD_PadFlushQueue(HSD_PAD_FLUSH_QUEUE_THROWAWAY);
    PAD_Recalibrate(0xF0000000);
    HSD_PadLibData.reset_switch = 0;
    
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
    HSD_PadRumbleStatus[pad].direct_status = 1;
    IRQ_Restore(intr);
}

void HSD_PadRumbleOffN(u8 pad)
{
}

//803780DC
void HSD_PadRumbleOffH(u8 pad)
{
    u32 intr = IRQ_Disable();
    HSD_PadRumbleStatus[pad].direct_status= 0;
    IRQ_Restore(intr);
}

//80378129
void HSD_PadRumbleFree(){

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
