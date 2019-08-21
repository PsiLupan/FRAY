#include "hsd_pad.h"

struct {
    u8 qnum;
    u8 qread;
    u8 qwrite;
    u8 qcount;
    u8 qtype;
    struct _HSD_PadData* queue;
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
} HSD_PadLibData; //804C1F78

HSD_PadStatus HSD_PadMasterStatus[4]; //804C1FAC
HSD_PadStatus HSD_PadCopyStatus[4]; //804C20BC

struct {
    u8 last_status;
    u8 status;
    u8 direct_status;
    u16 nb_list;
    struct _HSD_PadRumbleListData* listdatap;
} HSD_PadRumbleStatus[4]; //804C22E0

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

//803769FC
void HSD_PadRenewRawStatus() //This is based on a newer version - Melee's actually has a param that's checked
{
}

//80376D04
void HSD_PadFlushQueue(HSD_FlushType ftype)
{
    u32 intr = IRQ_Disable();
    switch (ftype) {
    case HSD_PAD_FLUSH_QUEUE_MERGE:
        while (HSD_PadLibData.qcount > 1) {
            u8 qread = HSD_PadLibData.qread;
            HSD_PadLibData.qread = (qread + 1) - ((qread + 1) / HSD_PadLibData.qnum) * HSD_PadLibData.qnum;
            PADStatus* qwrite = (PADStatus*)(HSD_PadLibData.queue->stat + qread * 4);
            PADStatus* qdst = (PADStatus*)(HSD_PadLibData.queue->stat + HSD_PadLibData.qread * 4);
            qdst[0].button = qdst[0].button | qwrite[0].button;
            qdst[1].button = qdst[1].button | qwrite[1].button;
            qdst[2].button = qdst[2].button | qwrite[2].button;
            qdst[3].button = qdst[3].button | qwrite[3].button;
            HSD_PadLibData.qcount -= 1;
        }
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

//80376E48
void HSD_PadClampCheck1(u8* val, u8 shift, u8 min, u8 max)
{
    if (*val < min) {
        *val = 0;
        return;
    }
    if (max < *val) {
        *val = max;
    }
    if (shift != 1) {
        return;
    }
    *val = *val - min;
}

//80376E90
void HSD_PadClampCheck3(s8* x, s8* y, u8 shift, s8 min, s8 max)
{
}

//803771D4
void HSD_PadADConvertCheck1(HSD_PadStatus* mp, s8 x, s8 y, u32 up, u32 down, u32 left, u32 right)
{
}

static HSD_PadClamp(HSD_PadStatus* mp)
{
    if (HSD_PadLibData.clamp_stickType == 0) {
        HSD_PadClampCheck3(&mp->stickX, &mp->stickY, HSD_PadLibData.clamp_stickShift, HSD_PadLibData.clamp_stickMin, HSD_PadLibData.clamp_stickMax);
        HSD_PadClampCheck3(&mp->subStickX, &mp->subStickY, HSD_PadLibData.clamp_stickShift, HSD_PadLibData.clamp_stickMin, HSD_PadLibData.clamp_stickMax);
    }

    HSD_PadClampCheck1(&mp->analogL, HSD_PadLibData.clamp_analogLRShift, HSD_PadLibData.clamp_analogLRMin, HSD_PadLibData.clamp_analogLRMax);
    HSD_PadClampCheck1(&mp->analogR, HSD_PadLibData.clamp_analogLRShift, HSD_PadLibData.clamp_analogLRMin, HSD_PadLibData.clamp_analogLRMax);
    HSD_PadClampCheck1(&mp->analogA, HSD_PadLibData.clamp_analogABShift, HSD_PadLibData.clamp_analogABMin, HSD_PadLibData.clamp_analogABMax);
    HSD_PadClampCheck1(&mp->analogB, HSD_PadLibData.clamp_analogABShift, HSD_PadLibData.clamp_analogABMin, HSD_PadLibData.clamp_analogABMax);
}

static HSD_PadADConvert(HSD_PadStatus* mp)
{
    if (HSD_PadLibData.adc_type == 0) {
        HSD_PadADConvertCheck1(mp, mp->stickX, mp->stickY, 0x10000, 0x20000, 0x40000, 0x80000);
        HSD_PadADConvertCheck1(mp, mp->subStickX, mp->subStickY, 0x100000, 0x200000, 0x400000, 0x800000);
    }
}

static HSD_PadScale(HSD_PadStatus* mp)
{
}

//80377450
static HSD_PadCrossDir(HSD_PadStatus* mp)
{
    switch (HSD_PadLibData.cross_dir) {
    case 0:
        return;

    case 1:
        if ((mp->button & 0xC) == 0) {
            return;
        }
        mp->button = mp->button & 0xFFFFFFFC;
        return;
        ;

    case 2:
        if ((mp->button & 0x3) == 0) {
            return;
        }
        mp->button = mp->button & 0xFFFFFFF3;
        return;

    case 3:
        if ((mp->button & 0xC) == 0) {
            if ((mp->button & 0x3) == 0) {
                return;
            }
            mp->cross_dir = 2;
            return;
        }
        if ((mp->button & 3) == 0) {
            mp->cross_dir = 1;
            return;
        }
        if (mp->cross_dir == 1) {
            mp->button = mp->button & 0xFFFFFFFC;
            return;
        }
        mp->button = mp->button & 0xFFFFFFF3;
        return;
    }
}

//8037750C
void HSD_PadRenewMasterStatus()
{
    u32 intr = IRQ_Disable();
    if (HSD_PadLibData.qcount != 0) {
        u8 qread = HSD_PadLibData.qread;
        u8 qwrite = qread + 1;
        HSD_PadLibData.qread = qwrite - (qwrite / HSD_PadLibData.qnum) * HSD_PadLibData.qnum;
        PADStatus* pad_status = (HSD_PadData*)(HSD_PadLibData.queue->stat + qread * 4);
        HSD_PadLibData.qcount -= 1;

        for (u8 i = 0; i < 4; ++i) {
            HSD_PadMasterStatus[i].last_button = HSD_PadMasterStatus[i].button;
            HSD_PadMasterStatus[i].support_rumble = pad_status->err;

            if (HSD_PadMasterStatus[i].support_rumble == 0) {
                HSD_PadMasterStatus[i].button = pad_status->button;
                HSD_PadMasterStatus[i].stickX = pad_status->stickX;
                HSD_PadMasterStatus[i].stickY = pad_status->stickY;
                HSD_PadMasterStatus[i].subStickX = pad_status->substickX;
                HSD_PadMasterStatus[i].subStickY = pad_status->substickY;
                HSD_PadMasterStatus[i].analogL = pad_status->triggerL;
                HSD_PadMasterStatus[i].analogR = pad_status->triggerR;
                HSD_PadMasterStatus[i].analogA = pad_status->analogA;
                HSD_PadMasterStatus[i].analogB = pad_status->analogB;

                HSD_PadClamp(&HSD_PadMasterStatus[i]);
                HSD_PadADConvert(&HSD_PadMasterStatus[i]);
                HSD_PadScale(&HSD_PadMasterStatus[i]);
                HSD_PadCrossDir(&HSD_PadMasterStatus[i]);

            } else if (HSD_PadMasterStatus[i].support_rumble == -3) {
                HSD_PadMasterStatus[i].support_rumble = 0;
            } else {
                HSD_PadMasterStatus[i].button = 0;
                HSD_PadMasterStatus[i].stickX = 0;
                HSD_PadMasterStatus[i].stickY = 0;
                HSD_PadMasterStatus[i].subStickX = 0;
                HSD_PadMasterStatus[i].subStickY = 0;
                HSD_PadMasterStatus[i].analogL = 0;
                HSD_PadMasterStatus[i].analogR = 0;
                HSD_PadMasterStatus[i].analogA = 0;
                HSD_PadMasterStatus[i].analogB = 0;
                HSD_PadMasterStatus[i].nml_stickX = 0.f;
                HSD_PadMasterStatus[i].nml_stickY = 0.f;
                HSD_PadMasterStatus[i].nml_subStickX = 0.f;
                HSD_PadMasterStatus[i].nml_subStickY = 0.f;
                HSD_PadMasterStatus[i].nml_analogL = 0.f;
                HSD_PadMasterStatus[i].nml_analogR = 0.f;
                HSD_PadMasterStatus[i].nml_analogA = 0.f;
                HSD_PadMasterStatus[i].nml_analogB = 0.f;
            }

            HSD_PadMasterStatus[i].trigger = HSD_PadMasterStatus[i].button & (HSD_PadMasterStatus[i].last_button ^ HSD_PadMasterStatus[i].button);
            HSD_PadMasterStatus[i].release = HSD_PadMasterStatus[i].last_button & (HSD_PadMasterStatus[i].last_button ^ HSD_PadMasterStatus[i].button);

            if (HSD_PadMasterStatus[i].last_button == HSD_PadMasterStatus[i].button) {
                HSD_PadMasterStatus[i].repeat_count -= 1;
                if (HSD_PadMasterStatus[i].repeat_count == 0) {
                    HSD_PadMasterStatus[i].repeat = HSD_PadMasterStatus[i].button;
                    HSD_PadMasterStatus[i].repeat_count = HSD_PadLibData.repeat_interval;
                } else {
                    HSD_PadMasterStatus[i].repeat = 0;
                }
            } else {
                HSD_PadMasterStatus[i].repeat = HSD_PadMasterStatus[i].trigger;
                HSD_PadMasterStatus[i].repeat_count = HSD_PadLibData.repeat_start;
            }

            qread += 1;
            pad_status = (HSD_PadData*)(HSD_PadLibData.queue->stat + qread * 4);
        }
    }
    IRQ_Restore(intr);
}

//803779C0
void HSD_PadRenewCopyStatus()
{
    for (u8 i = 0; i < 4; i++) {
        HSD_PadCopyStatus[i].last_button = HSD_PadCopyStatus[i].button;
        HSD_PadCopyStatus[i].support_rumble = HSD_PadMasterStatus[i].support_rumble;
        if (HSD_PadCopyStatus[i].support_rumble == 0) {
            HSD_PadCopyStatus[i].button = HSD_PadMasterStatus[i].button;
            HSD_PadCopyStatus[i].stickX = HSD_PadMasterStatus[i].stickX;
            HSD_PadCopyStatus[i].stickY = HSD_PadMasterStatus[i].stickY;
            HSD_PadCopyStatus[i].subStickX = HSD_PadMasterStatus[i].subStickX;
            HSD_PadCopyStatus[i].subStickY = HSD_PadMasterStatus[i].subStickY;
            HSD_PadCopyStatus[i].analogL = HSD_PadMasterStatus[i].analogL;
            HSD_PadCopyStatus[i].analogR = HSD_PadMasterStatus[i].analogR;
            HSD_PadCopyStatus[i].analogA = HSD_PadMasterStatus[i].analogA;
            HSD_PadCopyStatus[i].analogB = HSD_PadMasterStatus[i].analogB;
            HSD_PadCopyStatus[i].nml_stickX = HSD_PadMasterStatus[i].nml_stickX;
            HSD_PadCopyStatus[i].nml_stickY = HSD_PadMasterStatus[i].nml_stickY;
            HSD_PadCopyStatus[i].nml_subStickX = HSD_PadMasterStatus[i].nml_subStickX;
            HSD_PadCopyStatus[i].nml_subStickY = HSD_PadMasterStatus[i].nml_subStickY;
            HSD_PadCopyStatus[i].nml_analogL = HSD_PadMasterStatus[i].nml_analogL;
            HSD_PadCopyStatus[i].nml_analogR = HSD_PadMasterStatus[i].nml_analogR;
            HSD_PadCopyStatus[i].nml_analogA = HSD_PadMasterStatus[i].nml_analogA;
            HSD_PadCopyStatus[i].nml_analogB = HSD_PadMasterStatus[i].nml_analogB;
        } else {
            HSD_PadCopyStatus[i].button = 0;
            HSD_PadCopyStatus[i].stickX = 0;
            HSD_PadCopyStatus[i].stickY = 0;
            HSD_PadCopyStatus[i].subStickX = 0;
            HSD_PadCopyStatus[i].subStickY = 0;
            HSD_PadCopyStatus[i].analogL = 0;
            HSD_PadCopyStatus[i].analogR = 0;
            HSD_PadCopyStatus[i].analogA = 0;
            HSD_PadCopyStatus[i].analogB = 0;
            HSD_PadCopyStatus[i].nml_stickX = 0.f;
            HSD_PadCopyStatus[i].nml_stickY = 0.f;
            HSD_PadCopyStatus[i].nml_subStickX = 0.f;
            HSD_PadCopyStatus[i].nml_subStickY = 0.f;
            HSD_PadCopyStatus[i].nml_analogL = 0.f;
            HSD_PadCopyStatus[i].nml_analogR = 0.f;
            HSD_PadCopyStatus[i].nml_analogA = 0.f;
            HSD_PadCopyStatus[i].nml_analogB = 0.f;
        }

        HSD_PadCopyStatus[i].trigger = HSD_PadCopyStatus[i].button & (HSD_PadCopyStatus[i].last_button ^ HSD_PadCopyStatus[i].button);
        HSD_PadCopyStatus[i].release = HSD_PadCopyStatus[i].last_button & (HSD_PadCopyStatus[i].last_button ^ HSD_PadCopyStatus[i].button);

        if (HSD_PadCopyStatus[i].last_button == HSD_PadCopyStatus[i].button) {
            HSD_PadCopyStatus[i].repeat_count -= 1;
            if (HSD_PadCopyStatus[i].repeat_count == 0) {
                HSD_PadCopyStatus[i].repeat = HSD_PadCopyStatus[i].button;
                HSD_PadCopyStatus[i].repeat_count = HSD_PadLibData.repeat_interval;
            } else {
                HSD_PadCopyStatus[i].repeat = 0;
            }
        } else {
            HSD_PadCopyStatus[i].repeat = HSD_PadCopyStatus[i].trigger;
            HSD_PadCopyStatus[i].repeat_count = HSD_PadLibData.repeat_start;
        }
    }
}

//80377B54
void HSD_PadZeroQueue()
{
}

//80377CE8
void HSD_PadRenewStatus()
{
    HSD_PadRenewRawStatus();
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
void HSD_PadInit(u8 qnum, HSD_PadData* queue, u16 nb_list, HSD_PadRumbleListData* listdatap)
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
    HSD_PadRumbleStatus[pad].direct_status = 0;
    IRQ_Restore(intr);
}

//80378129
void HSD_PadRumbleFree()
{
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
