#include "hsd_pad.h"

#include <math.h>
#include <ogc/system.h>

PadLibData HSD_PadLibData; //804C1F78
HSD_PadStatus HSD_PadMasterStatus[4]; //804C1FAC
HSD_PadStatus HSD_PadCopyStatus[4]; //804C20BC
HSD_RumbleData HSD_PadRumbleData[4]; //804C22E0

//8037699C
u8 HSD_PadGetRawQueueCount(void)
{
    u32 intr = IRQ_Disable();
    u8 queue_count = HSD_PadLibData.qcount;
    IRQ_Restore(intr);
    return queue_count;
}

//803769D8
s32 HSD_PadGetResetSwitch(void)
{
    return (HSD_PadLibData.reset_switch != 0) ? TRUE : FALSE;
}

//803769FC
void HSD_PadRenewRawStatus(void)
{
    /* This is based on a newer version - Melee's actually has a BOOL param that's checked and will read how many controllers there are err'd
    This occurs after the PAD_Read by checking err != 0 on each status and incrementing the controller number by 1. If every controller is err'd, it returns.
    */
    PADStatus status[4];
    u32 mask = 0;

    HSD_PadRumbleInterpret();
    PAD_Read(status);

    PADStatus* queue = (PADStatus*)(HSD_PadLibData.queue->stat + HSD_PadLibData.qwrite * 4);
    if (HSD_PadLibData.qcount == HSD_PadLibData.qnum) {
        if (HSD_PadLibData.qtype == 1) {

        } else if (HSD_PadLibData.qtype == 0) {
            HSD_PadLibData.qread = (HSD_PadLibData.qread + 1) - (HSD_PadLibData.qread + 1) / HSD_PadLibData.qnum * HSD_PadLibData.qnum;
            PADStatus* rqueue = (PADStatus*)(HSD_PadLibData.queue->stat + HSD_PadLibData.qread * 4);
            if (HSD_PadLibData.qnum == 1) {
                status[0].button = status[0].button | rqueue[0].button;
                status[1].button = status[1].button | rqueue[1].button;
                status[2].button = status[2].button | rqueue[2].button;
                status[3].button = status[3].button | rqueue[3].button;
            } else {
                rqueue[0].button = queue[0].button | rqueue[0].button;
                rqueue[1].button = queue[1].button | rqueue[1].button;
                rqueue[2].button = queue[2].button | rqueue[2].button;
                rqueue[3].button = queue[3].button | rqueue[3].button;
            }
        } else if (HSD_PadLibData.qtype < 3) {
            goto JMP;
        }
    } else {
        HSD_PadLibData.qcount += 1;
    }
    for (u8 i = 0; i < 4; i++) {
        queue[i] = status[i];
    }
    HSD_PadLibData.qwrite = (HSD_PadLibData.qwrite + 1) - (HSD_PadLibData.qwrite + 1) / HSD_PadLibData.qnum * HSD_PadLibData.qnum;
JMP:
    if (status[0].err == PAD_ERR_NO_CONTROLLER) {
        mask |= PAD_CHAN0_BIT;
    }
    if (status[1].err == PAD_ERR_NO_CONTROLLER) {
        mask |= PAD_CHAN1_BIT;
    }
    if (status[2].err == PAD_ERR_NO_CONTROLLER) {
        mask |= PAD_CHAN2_BIT;
    }
    if (status[3].err == PAD_ERR_NO_CONTROLLER) {
        mask |= PAD_CHAN3_BIT;
    }
    if (mask != 0) {
        PAD_Reset(mask);
    }

    u32 reset = SYS_ResetButtonDown();
    if (reset == 0) {
        if (HSD_PadLibData.reset_switch_status != 0) {
            HSD_PadLibData.reset_switch = 1;
            HSD_PadLibData.reset_switch_status = 0;
        }
    } else {
        HSD_PadLibData.reset_switch_status = 1;
    }
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
    f64 total = (f64)*x * (f64)*x + (f64)*y * (f64)*y;
    if (total > 0.0) {
        f64 v = 1.0 / sqrt(total);
        f64 half = 0.5;
        f64 three = 3.0;
        f64 d = half * v * -(total * v * v - three);
        f64 d2 = half * d * -(total * d * d - three);
        total = (total * half * d2 * -(total * d2 * d2 - three));
    }

    if ((f64)min <= total) {
        if ((f64)max < total) {
            *x = (s8)(((f64)*x * (f64)max) / total);
            *y = (s8)(((f64)*y * (f64)max) / total);
            total = (f64)*x * (f64)*x + (f64)*y * (f64)*y;
            if (total > 0.0) {
                f64 v = 1.0 / sqrt(total);
                f64 half = 0.5;
                f64 three = 3.0;
                f64 d = half * v * -(total * v * v - three);
                f64 d2 = half * d * -(total * d * d - three);
                total = (total * half * d2 * -(total * d2 * d2 - three));
            }
        }
        if (shift == 1 && total > 1e-10f) {
            *x = (s8)((f64)*x - (((f64)*x * (f64)min) / total));
            *x = (s8)((f64)*y - (((f64)*y * (f64)min) / total));
        }
    } else {
        *x = 0;
        *y = 0;
    }
}

//803771D4
void HSD_PadADConvertCheck1(HSD_PadStatus* mp, s8 x, s8 y, u32 up, u32 down, u32 left, u32 right)
{
    f64 total = (f64)x * (f64)x + (f64)y * (f64)y;
    if (total > 0.0) {
        f64 v = 1.0 / sqrt(total);
        f64 half = 0.5;
        f64 three = 3.0;
        f64 d = half * v * -(total * v * v - three);
        f64 d2 = half * d * -(total * d * d - three);
        total = (total * half * d2 * -(total * d2 * d2 - three));
    }

    f64 n = 0.0;
    if ((f64)x == 0.0) {
        f64 v = y < 0 ? -1.5708 : 1.5078; //1.5708 = 90 degrees in radians
        n = (f32)v; //There's an frsp here
    } else {
        n = atan2((f64)y, (f64)x);
    }

    f64 angle = (f64)(0.5f * HSD_PadLibData.adc_angle);
    if ((f64)HSD_PadLibData.adc_th <= total) {
        if (n < (-2.35619 + angle)) { //2.35619 = 135 degrees in radians
            mp->button |= left;
        }
        if ((-2.35619 - angle) <= n && n <= (-0.785398 + angle)) { //0.785398 = 45 degrees in radians
            mp->button |= down;
        }
        if ((-0.785398 + angle) < n && n < (0.785398 + angle)) {
            mp->button |= right;
        }
        if ((0.785398 - angle) <= n && n <= (2.35619 + angle)) {
            mp->button |= up;
        }
        if ((2.35619 - angle) < n) {
            mp->button |= left;
        }
    }
}

static void HSD_PadClamp(HSD_PadStatus* mp)
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

static void HSD_PadADConvert(HSD_PadStatus* mp)
{
    if (HSD_PadLibData.adc_type == 0) {
        HSD_PadADConvertCheck1(mp, mp->stickX, mp->stickY, 0x10000, 0x20000, 0x40000, 0x80000);
        HSD_PadADConvertCheck1(mp, mp->subStickX, mp->subStickY, 0x100000, 0x200000, 0x400000, 0x800000);
    }
}

static void HSD_PadScale(HSD_PadStatus* mp)
{
    mp->nml_stickX = (f32)((f64)mp->stickX / (f64)HSD_PadLibData.scale_stick);
    mp->nml_stickY = (f32)((f64)mp->stickY / (f64)HSD_PadLibData.scale_stick);
    mp->nml_subStickX = (f32)((f64)mp->subStickX / (f64)HSD_PadLibData.scale_stick);
    mp->nml_subStickY = (f32)((f64)mp->subStickY / (f64)HSD_PadLibData.scale_stick);
    mp->nml_analogL = (f32)((f64)HSD_PadMasterStatus->analogL / HSD_PadLibData.scale_analogLR);
    mp->nml_analogR = (f32)((f64)HSD_PadMasterStatus->analogR / HSD_PadLibData.scale_analogLR);
    mp->nml_analogA = (f32)((f64)HSD_PadMasterStatus->analogA / HSD_PadLibData.scale_analogAB);
    mp->nml_analogB = (f32)((f64)HSD_PadMasterStatus->analogB / HSD_PadLibData.scale_analogAB);
}

//80377450
static void HSD_PadCrossDir(HSD_PadStatus* mp)
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
void HSD_PadRenewMasterStatus(void)
{
    u32 intr = IRQ_Disable();
    if (HSD_PadLibData.qcount != 0) {
        u8 qread = HSD_PadLibData.qread;
        u8 qwrite = qread + 1;
        HSD_PadLibData.qread = qwrite - (qwrite / HSD_PadLibData.qnum) * HSD_PadLibData.qnum;
        PADStatus* pad_status = (PADStatus*)(HSD_PadLibData.queue->stat + qread * 4);
        HSD_PadLibData.qcount -= 1;

        for (u8 i = 0; i < 4; ++i) {
            HSD_PadMasterStatus[i].last_button = HSD_PadMasterStatus[i].button;
            HSD_PadMasterStatus[i].err = pad_status->err;

            if (HSD_PadMasterStatus[i].err == 0) {
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

            } else if (HSD_PadMasterStatus[i].err == -3) {
                HSD_PadMasterStatus[i].err = 0;
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
            pad_status = (PADStatus*)(HSD_PadLibData.queue->stat + qread * 4);
        }
    }
    IRQ_Restore(intr);
}

//803779C0
void HSD_PadRenewCopyStatus(void)
{
    for (u8 i = 0; i < 4; i++) {
        HSD_PadCopyStatus[i].last_button = HSD_PadCopyStatus[i].button;
        HSD_PadCopyStatus[i].err = HSD_PadMasterStatus[i].err;
        if (HSD_PadCopyStatus[i].err == 0) {
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
void HSD_PadZeroQueue(void)
{
}

//80377CE8
void HSD_PadRenewStatus(void)
{
    HSD_PadRenewRawStatus();
    HSD_PadRenewMasterStatus();
    HSD_PadRenewCopyStatus();
    HSD_PadZeroQueue(); //Not present in later versions
}

//80377D18
void HSD_PadReset(void)
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
    HSD_PadLibData.qtype = 0;
    HSD_PadLibData.repeat_start = 45;
    HSD_PadLibData.repeat_interval = 8;
    HSD_PadLibData.adc_type = 0;
    HSD_PadLibData.adc_th = 30;
    HSD_PadLibData.adc_angle = 0.f;
    HSD_PadLibData.clamp_stickType = 0;
    HSD_PadLibData.clamp_stickShift = 0;
    HSD_PadLibData.clamp_stickMax = 127;
    HSD_PadLibData.clamp_stickMin = 0;
    HSD_PadLibData.clamp_analogLRShift = 0;
    HSD_PadLibData.clamp_analogLRMax = 255;
    HSD_PadLibData.clamp_analogLRMin = 0;
    HSD_PadLibData.clamp_analogABShift = 0;
    HSD_PadLibData.clamp_analogABMax = 255;
    HSD_PadLibData.clamp_analogABMin = 0;
    HSD_PadLibData.scale_stick = 127;
    HSD_PadLibData.scale_analogLR = 255;
    HSD_PadLibData.scale_analogAB = 255;
    HSD_PadLibData.cross_dir = 0;
    HSD_PadLibData.reset_switch_status = 0;
    HSD_PadLibData.reset_switch = 0;
    HSD_PadLibData.rumble_info.max_list = 0;
    HSD_PadLibData.rumble_info.listdatap = NULL;
    HSD_PadLibData.qnum = qnum;
    HSD_PadLibData.queue = queue;

    HSD_PadRumbleInit(nb_list, listdatap);

    //The stuff normally here does some default init of the PadCopyStatus, etc. but it uses 0 everywhere I saw.
    //As a result we can assume it's not filled with garbage or memset later

    PAD_Init();
}

//80378090
void HSD_PadRumbleOn(u8 pad)
{
    u32 intr = IRQ_Disable();
    HSD_PadRumbleData[pad].direct_status = 1;
    IRQ_Restore(intr);
}

//803780DC
void HSD_PadRumbleOffH(u8 pad)
{
    u32 intr = IRQ_Disable();
    HSD_PadRumbleData[pad].direct_status = 0;
    IRQ_Restore(intr);
}

//80378129
void HSD_PadRumbleFree(HSD_RumbleData* rdp, HSD_PadRumbleListData* p)
{
    HSD_PadRumbleListData* data = rdp->listdatap;
    HSD_PadRumbleListData* temp;
    do {
        temp = data;
        data = temp->next;
    } while (data != p);
    temp->next = p->next;
    rdp->nb_list -= 1;
    p->next = HSD_PadLibData.rumble_info.listdatap;
    HSD_PadLibData.rumble_info.listdatap = p;
}

//80378208
void HSD_PadRumbleRemoveAll(void)
{
    for (u32 i = 0; i < 4; i++) {
        u32 intr = IRQ_Disable();
        HSD_PadRumbleListData* data = HSD_PadRumbleData[i].listdatap;
        while (data != NULL) {
            HSD_PadRumbleListData* next = data->next;
            HSD_PadRumbleFree(&HSD_PadRumbleData[i], data);
            data = next;
        }
        IRQ_Restore(intr);
    }
}

//803786F0
void HSD_PadRumbleInterpret(void)
{
}

//80378828
void HSD_PadRumbleInit(u16 nb_list, HSD_PadRumbleListData* listdatap)
{
    HSD_PadLibData.rumble_info.max_list = nb_list;
    HSD_PadLibData.rumble_info.listdatap = listdatap;

    if (nb_list != 0) {
        u32 idx = nb_list - 1;
        u32 z_idx = 0;
        if (idx > 0) {
            u32 loop_count = nb_list - 2 >> 3;
            if (idx > 8 && (nb_list - 9) > 0) {
                do {
                    HSD_PadLibData.rumble_info.listdatap[0].next = &HSD_PadLibData.rumble_info.listdatap[z_idx + 1];
                    HSD_PadLibData.rumble_info.listdatap[z_idx + 1].next = &HSD_PadLibData.rumble_info.listdatap[z_idx + 2];
                    HSD_PadLibData.rumble_info.listdatap[z_idx + 2].next = &HSD_PadLibData.rumble_info.listdatap[z_idx + 3];
                    HSD_PadLibData.rumble_info.listdatap[z_idx + 3].next = &HSD_PadLibData.rumble_info.listdatap[z_idx + 4];
                    HSD_PadLibData.rumble_info.listdatap[z_idx + 4].next = &HSD_PadLibData.rumble_info.listdatap[z_idx + 5];
                    HSD_PadLibData.rumble_info.listdatap[z_idx + 5].next = &HSD_PadLibData.rumble_info.listdatap[z_idx + 6];
                    HSD_PadLibData.rumble_info.listdatap[z_idx + 6].next = &HSD_PadLibData.rumble_info.listdatap[z_idx + 7];
                    HSD_PadLibData.rumble_info.listdatap[z_idx + 7].next = &HSD_PadLibData.rumble_info.listdatap[z_idx + 8];

                    z_idx = z_idx + 8;
                    loop_count -= 1;
                } while (loop_count > 0);
            }
            if (z_idx < idx) {
                u32 offset = z_idx + 1;
                for (u32 i = idx - z_idx; i > 0; --i, ++z_idx, ++offset) {
                    HSD_PadLibData.rumble_info.listdatap[offset].next = &HSD_PadLibData.rumble_info.listdatap[z_idx + 1];
                }
            }
        }
        HSD_PadLibData.rumble_info.listdatap[z_idx].next = NULL;
    }

    for (u8 i = 0; i < 4; i++) {
        HSD_PadRumbleData[i].last_status = 0;
        HSD_PadRumbleData[i].status = 0;
        HSD_PadRumbleData[i].direct_status = 0;
        HSD_PadRumbleData[i].nb_list = 0;
        HSD_PadRumbleData[i].listdatap = NULL;
    }
}
