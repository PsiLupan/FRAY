#include "hsd_fobj.h"

#include "hsd_util.h"

HSD_ObjAllocData fobj_alloc_data;

//8036A938
HSD_ObjAllocData* HSD_FObjGetAllocData(void)
{
    return &fobj_alloc_data;
}

//8036A944
void HSD_FObjInitAllocData(void)
{
    HSD_ObjAllocInit(&fobj_alloc_data, sizeof(HSD_FObj), 4);
}

//8036A974
void HSD_FObjRemove(HSD_FObj* fobj)
{
    if (fobj) {
        HSD_FObjFree(fobj);
    }
}

//8036A99C
void HSD_FObjRemoveAll(HSD_FObj* fobj)
{
    if (fobj) {
        HSD_FObjRemoveAll(fobj->next);
        HSD_FObjRemove(fobj);
    }
}

//8036AA44
u8 HSD_FObjSetState(HSD_FObj* fobj, u8 state)
{
    if (fobj) {
        fobj->flags = (state & 0xF) | (fobj->flags & 0xF0);
    }
    return state;
}

//8036AA64
u32 HSD_FObjGetState(HSD_FObj* fobj)
{
    if (fobj) {
        return fobj->flags & 0xF;
    }
    return 0;
}

//8036AA80
void HSD_FObjReqAnimAll(HSD_FObj* fobj, f32 frame)
{
    if (fobj) {
        for (HSD_FObj* curr = fobj; curr != NULL; curr = curr->next) {
            curr->ad = curr->ad_head;
            curr->time = (f32)curr->startframe + frame;
            curr->op = 0;
            curr->op_intrp = 0;
            curr->flags &= 0xBF;
            curr->nb_pack = 0;
            curr->fterm = 0;
            curr->p0 = 0.f;
            curr->p1 = 0.f;
            curr->d0 = 0.f;
            curr->d1 = 0.f;
            curr->flags = (curr->flags & 0xF0) | 1;
        }
    }
}

//8036AB24
void HSD_FObjStopAnim(HSD_FObj* fobj, void* obj, void (*obj_update)(), f32 frame)
{
    if (fobj) {
        if (fobj->op_intrp == HSD_A_OP_KEY) {
            HSD_FObjInterpretAnim(fobj, obj, obj_update, frame);
        }
        fobj->flags &= 0xF0;
    }
}

//8036AB78
void HSD_FObjStopAnimAll(HSD_FObj* fobj, void* obj, void (*obj_update)(), f32 frame)
{
    for (HSD_FObj* curr = fobj; curr != NULL; curr = curr->next) {
        if (fobj->op_intrp == HSD_A_OP_KEY) {
            HSD_FObjInterpretAnim(curr, obj, obj_update, frame);
        }
        curr->flags &= 0xF0u;
    }
}

//8036AC10
static f32 FObjLoadData(u8** curr_parse, u8 frac)
{
    f32 result;
    u32 data;
    u8* parse_pos;

    if ((frac & 0xFF) == 0) {
        parse_pos = *curr_parse;
        data = *parse_pos;
        *curr_parse += 1;

        parse_pos = *curr_parse;
        data |= (*parse_pos) << 8;
        *curr_parse += 1;

        parse_pos = *curr_parse;
        data |= (*parse_pos) << 16;
        *curr_parse += 1;

        parse_pos = *curr_parse;
        data |= (*parse_pos) << 24;
        *curr_parse += 1;

        return (f32)(data);
    }

    u8 flag = frac & 0xE0;
    if (flag == 96) {
        u8 val = (**curr_parse);
        *curr_parse += 1;
        result = val;
    } else if (flag < 96) {
        if (flag == 64) {
            parse_pos = *curr_parse;
            data = (*parse_pos);
            *curr_parse += 1;

            parse_pos = *curr_parse;
            data |= (*parse_pos) << 8;
            *curr_parse += 1;

            result = (f32)(data);
        } else {
            if (63 < flag || flag != 32) {
                return 0.0f;
            } else {
                parse_pos = *curr_parse;
                data = (*parse_pos);
                *curr_parse += 1;

                parse_pos = *curr_parse;
                data |= (*parse_pos) << 8;
                *curr_parse += 1;

                result = (f32)(data);
            }
        }
    } else {
        if (flag != 128) {
            return 0.0f;
        }
        u8 val = (**curr_parse);
        *curr_parse += 1;
        result = (f32)val;
    }
    return result / (1 << (frac & 0x1F));
}

//8036ADDC
static u8 parseOpCode(u8** curr_parse)
{
    u8* temp = *curr_parse;
    *curr_parse = temp + 1;
    u32 lshift = 3;

    u8 result = ((*temp >> 4) & 7) + 1;
    if ((*temp & 0x80) == 0) {
        return result;
    }

    u8* parse;
    do {
        parse = *curr_parse;
        *curr_parse = parse + 1;
        result += (*parse & 0x7F) << lshift;
        lshift += 7;
    } while ((*parse & 0x80) != 0);
    return result;
}

//8036AE38
static void FObjLaunchKeyData(HSD_FObj* fobj)
{
    if (fobj->flags & 0x40) {
        fobj->op_intrp = fobj->op;
        fobj->flags &= 0xBF;
        fobj->flags |= 0x80;
        fobj->p0 = fobj->p1;
    }
}

//8036AE70
void FObjUpdateAnim(HSD_FObj* fobj, void* obj, void (*obj_update)(void*, u32, FObjData))
{
    FObjData fobjdata;
    if (obj_update) {
        u8 state = fobj->op_intrp;
        switch (state) {
        case HSD_A_OP_CON:
            if (fobj->time < (f32)fobj->fterm) {
                fobjdata.fv = fobj->p0;
            } else {
                fobjdata.fv = fobj->p1;
            }
            break;

        case HSD_A_OP_LIN:
            if (fobj->flags & 0x20) {
                fobj->flags &= 0xDF;
                if (fobj->fterm != 0) {
                    fobj->d0 = (fobj->p1 - fobj->p0) / (f32)fobj->fterm;
                } else {
                    fobj->d0 = 0;
                    fobj->p0 = fobj->p1;
                }
            }
            fobjdata.fv = fobj->d0 * fobj->time + fobj->p0;
            break;

        case HSD_A_OP_SPL0:
        case HSD_A_OP_SPL:
        case HSD_A_OP_SLP:
            if (fobj->fterm != 0) {
                fobjdata.fv = splGetHermite(1.0 / fobj->fterm, fobj->time, fobj->p0, fobj->p1, fobj->d0, fobj->d1);
            } else {
                fobjdata.fv = fobj->p1;
            }
            break;

        case HSD_A_OP_KEY:
            if (!(fobj->flags & 0x80)) {
                return;
            }
            fobjdata.fv = fobj->p0;
            fobj->flags &= 0x7F;
            break;
        }
        (*obj_update)(obj, fobj->obj_type, fobjdata);
    }
}

//8036B030
void HSD_FObjInterpretAnim(HSD_FObj* fobj, void* obj, void (*obj_update)(), f32 frame)
{
#if 0
    u8 result = 0;
    f32 v22 = 0;

    if (fobj != NULL)
        result = HSD_FObjGetState(fobj);
    if (result == 0)
        return;

    fobj->time = fobj->time + frame;
    if (fobj->time < 0)
        return;

    while (true) {
        while (result != 4) {
            switch (result) {

            case HSD_A_OP_NONE:
                return;

            case HSD_A_OP_CON:
            case HSD_A_OP_LIN:
                if ((fobj->ad - fobj->ad_head) < fobj->length) {
                    fobj->op_intrp = fobj->op;
                    if (fobj->nb_pack == 0) {
                        fobj->op = *fobj->ad & 0xF;
                        fobj->nb_pack = parseOpCode(&fobj->ad);
                    }
                    fobj->nb_pack -= 1;
                    u8 state = fobj->op;
                    u8 flags = fobj->flags & 0xF;
                    assert((flags - 1) > 1);

                    switch (state) {
                    case HSD_A_OP_CON:
                    case HSD_A_OP_LIN:
                        fobj->p0 = fobj->p1;
                        fobj->d1 = FObjLoadData(&fobj->ad, fobj->frac_value);
                        if (fobj->op_intrp != 5) {
                            fobj->d0 = fobj->d1;
                            fobj->d1 = 0;
                        }
                        if (flags == 1) {
                            result = 3;
                        } else {
                            result = 4;
                        }
                        fobj->flags = (result & 0xF) | (fobj->flags & 0xF0);
                        break;

                    case HSD_A_OP_SPL0:
                        fobj->p0 = fobj->p1;
                        fobj->d0 = fobj->d1;
                        fobj->p1 = FObjLoadData(&fobj->ad, fobj->frac_value);
                        fobj->d1 = 0;
                        if (flags == 1) {
                            result = HSD_FObjSetState(fobj, 3);
                        } else {
                            result = HSD_FObjSetState(fobj, 4);
                        }
                        break;

                    case HSD_A_OP_SPL:
                        fobj->p0 = fobj->p1;
                        fobj->p1 = FObjLoadData(&fobj->ad, fobj->frac_value);
                        fobj->d0 = fobj->d1;
                        fobj->d1 = FObjLoadData(&fobj->ad, fobj->frac_slope);
                        if (flags == 1) {
                            result = HSD_FObjSetState(fobj, 3);
                        } else {
                            result = HSD_FObjSetState(fobj, 4);
                        }
                        break;

                    case HSD_A_OP_SLP:
                        fobj->p0 = fobj->p1;
                        fobj->p1 = FObjLoadData(&fobj->ad, fobj->frac_value);
                        result = HSD_FObjGetState(fobj);
                        break;

                    case HSD_A_OP_KEY:
                        FObjLaunchKeyData(fobj);
                        fobj->p1 = FObjLoadData(&fobj->ad, fobj->frac_value);
                        fobj->flags |= 0x40u;

                        if (flags == 1) {
                            result = HSD_FObjSetState(fobj, 3);
                        } else {
                            result = HSD_FObjSetState(fobj, 4);
                        }
                        break;

                    default:
                        result = 0;
                        break;
                    }
                } else {
                    result = 6;
                }
                break;

            case HSD_A_OP_SPL0:
                if (fobj->flags & 0x80){
                    FObjUpdateAnim(fobj, obj, obj_update);
                }
                assert((fobj->flags & 0xF) != 3);

                if ((fobj->ad - fobj->ad_head) < fobj->length) {
                    u32 fterm = 0;
                    u32 lshift = 0;
                    u8 parse;
                    do {
                        u8* pos = fobj->ad;
                        fobj->ad += 1;
                        parse = *pos;
                        fterm |= (parse & 0x7F) << lshift;
                        lshift += 7;
                    } while ((parse & 0x80) != 0);
                    fobj->fterm = fterm;
                    fobj->flags |= 0x20u;
                    result = 2;
                    fobj->flags = (fobj->flags & 0xF0) | result;
                } else {
                    result = 6;
                }
                break;

            case HSD_A_OP_SLP:
                result = 4;
                fobj->flags = (fobj->flags & 0xF0) | result;
                break;

            case HSD_A_OP_KEY:
                fobj->time = fobj->time + v22;
                FObjLaunchKeyData(fobj);
                FObjUpdateAnim(fobj, obj, obj_update);
                return;

            default:
                break;
            }
        }
        if ((f32)fobj->fterm > fobj->time)
            break;

        fobj->time = fobj->time - fobj->fterm;
        v22 = fobj->time;
        result = 3;
        fobj->flags = (fobj->flags & 0xF0) | result;
    }
    FObjUpdateAnim(fobj, obj, obj_update);
    result = 5;
    fobj->flags = (fobj->flags & 0xF0) | result;
#else
    u32 fterm;
    u32 uVar2;
    s32 iVar3;
    u8 state;
    u8* read_ptr;
    f64 dVar5;
    f32 fVar6;

    if (fobj == NULL) {
        uVar2 = 0;
    } else {
        uVar2 = HSD_FObjGetState(fobj);
    }
    if (uVar2 == 0) {
        fobj->time = (fobj->time + frame);
        if (fobj->time < 0.0) {
            return;
        }
    }

    fVar6 = 0.0F;
    do {
        while (true) {
            while (true) {
                while (true) {
                    while (true) {
                        while (true) {
                            do {
                                while (true) {
                                    while (uVar2 == 4) {
                                        fterm = fobj->fterm;
                                        if (fobj->time < (f32)fterm) {
                                            FObjUpdateAnim(fobj, obj, obj_update);
                                            if (fobj == NULL) {
                                                return;
                                            }
                                            fobj->flags = (fobj->flags & 0xf0) | 5;
                                            return;
                                        }
                                        uVar2 = 3;
                                        fVar6 = (f32)fterm;
                                        fobj->time = fobj->time - (f32)fterm;
                                        if (fobj != NULL) {
                                            fobj->flags = (fobj->flags & 0xF0) | 3;
                                        }
                                    }
                                    if (uVar2 < 4)
                                        break;
                                    if (uVar2 == 6) {
                                        fobj->time = (fobj->time + fVar6);
                                        FObjLaunchKeyData(fobj);
                                        FObjUpdateAnim(fobj, obj, obj_update);
                                        return;
                                    }
                                    if ((uVar2 < 6) && (uVar2 = 4, fobj != NULL)) {
                                        fobj->flags = (fobj->flags & 0xF0) | 4;
                                    }
                                }
                                if (uVar2 == 0) {
                                    return;
                                }
                            } while (uVar2 < 0);
                            if (uVar2 < 3)
                                break;
                            if ((fobj->flags & 0x80) != 0) {
                                FObjUpdateAnim(fobj, obj, obj_update);
                            }
                            if (fobj == NULL) {
                                state = 0;
                            } else {
                                state = fobj->flags & 0xF;
                            }

                            if ((fobj->ad - fobj->ad_head) < fobj->length) {
                                uVar2 = 0;
                                iVar3 = 0;
                                do {
                                    read_ptr = fobj->ad;
                                    fobj->ad = read_ptr + 1;
                                    state = *read_ptr;
                                    uVar2 |= (state & 0x7f) << iVar3;
                                    iVar3 = iVar3 + 7;
                                } while ((state & 0x80) != 0);
                                fobj->fterm = (u16)uVar2;
                                fobj->flags |= 0x20;
                                if (fobj != NULL) {
                                    fobj->flags = (fobj->flags & 0xF0) | 2;
                                }
                                uVar2 = 2;
                            } else {
                                uVar2 = 6;
                            }
                        }
                        if ((fobj->ad - fobj->ad_head) < fobj->length)
                            break;
                        uVar2 = 6;
                    }
                    fobj->op_intrp = fobj->op;
                    if (fobj->nb_pack == 0) {
                        fobj->op = *fobj->ad & 0xF;
                        fobj->nb_pack = (u16)parseOpCode(&fobj->ad);
                    }
                    fobj->nb_pack = fobj->nb_pack - 1;
                    state = fobj->op;
                    if (state != 4)
                        break;
                    if (fobj == NULL) {
                        uVar2 = 0;
                    } else {
                        uVar2 = fobj->flags & 0xF;
                    }

                    fobj->p0 = fobj->p1;
                    dVar5 = FObjLoadData(&fobj->ad, fobj->frac_value);
                    fobj->p1 = dVar5;
                    fobj->d0 = fobj->d1;
                    dVar5 = FObjLoadData(&fobj->ad, fobj->frac_slope);
                    fobj->d1 = dVar5;
                    if (uVar2 == 1) {
                        state = 3;
                    } else {
                        state = 4;
                    }
                    state = HSD_FObjSetState(fobj, state);
                    uVar2 = state;
                }
                if (state < 4)
                    break;
                if (state == 6) {
                    if (fobj == NULL) {
                        uVar2 = 0;
                    } else {
                        uVar2 = fobj->flags & 0xF;
                    }

                    FObjLaunchKeyData(fobj);
                    dVar5 = FObjLoadData(&fobj->ad, fobj->frac_value);
                    fobj->p1 = dVar5;
                    fobj->flags |= 0x40;
                    if (uVar2 == 1) {
                        state = 3;
                    } else {
                        state = 4;
                    }
                    state = HSD_FObjSetState(fobj, state);
                    uVar2 = state;
                } else {
                    if (5 < state)
                        goto LAB_8036b51c;
                    if (fobj == NULL) {
                        uVar2 = 0;
                    } else {
                        uVar2 = fobj->flags & 0xf;
                    }

                    fobj->d0 = fobj->d1;
                    dVar5 = FObjLoadData(&fobj->ad, fobj->frac_slope);
                    fobj->d1 = dVar5;
                    uVar2 = HSD_FObjGetState(fobj);
                }
            }
            if (state == 2)
                break;
            if (state < 2) {
                if (state == 0) {
                LAB_8036b51c:
                    uVar2 = 0;
                } else {
                    if (fobj == NULL) {
                        uVar2 = 0;
                    } else {
                        uVar2 = fobj->flags & 0xf;
                    }

                    fobj->p0 = fobj->p1;
                    dVar5 = FObjLoadData(&fobj->ad, fobj->frac_value);
                    fobj->p1 = dVar5;
                    if (fobj->op_intrp != 5) {
                        fobj->d0 = fobj->d1;
                        fobj->d1 = 0.0F;
                    }
                    if (uVar2 == 1) {
                        uVar2 = 3;
                    } else {
                        uVar2 = 4;
                    }
                    if (fobj != NULL) {
                        fobj->flags = (u8)uVar2 | (fobj->flags & 0xf0);
                    }
                }
            } else {
                if (fobj == NULL) {
                    uVar2 = 0;
                } else {
                    uVar2 = fobj->flags & 0xf;
                }

                fobj->p0 = fobj->p1;
                fobj->d0 = fobj->d1;
                dVar5 = FObjLoadData(&fobj->ad, fobj->frac_value);
                fobj->p1 = dVar5;
                fobj->d1 = 0.0F;
                if (uVar2 == 1) {
                    state = 3;
                } else {
                    state = 4;
                }
                state = HSD_FObjSetState(fobj, state);
                uVar2 = state;
            }
        }
        if (fobj == NULL) {
            uVar2 = 0;
        } else {
            uVar2 = fobj->flags & 0xf;
        }

        fobj->p0 = fobj->p1;
        dVar5 = FObjLoadData(&fobj->ad, fobj->frac_value);
        fobj->p1 = dVar5;
        if (fobj->op_intrp != 5) {
            fobj->d0 = fobj->d1;
            fobj->d1 = 0.0F;
        }
        if (uVar2 == 1) {
            uVar2 = 3;
        } else {
            uVar2 = 4;
        }
        if (fobj != NULL) {
            fobj->flags = (u8)uVar2 | (fobj->flags & 0xF0);
        }
    } while (true);
#endif
}

//8036B6CC
void HSD_FObjInterpretAnimAll(HSD_FObj* fobj, void* caller_obj, void (*callback)(), f32 frame)
{
    for (HSD_FObj* curr = fobj; curr != NULL; curr = curr->next) {
        HSD_FObjInterpretAnim(curr, caller_obj, callback, frame);
    }
}

//8036B73C
HSD_FObj* HSD_FObjLoadDesc(HSD_FObjDesc* desc)
{
    if (desc) {
        HSD_FObj* fobj = HSD_FObjAlloc();
        fobj->next = HSD_FObjLoadDesc(desc->next);
        fobj->startframe = (s16)desc->startframe;
        fobj->obj_type = desc->type;
        fobj->frac_value = desc->frac_value;
        fobj->frac_slope = desc->frac_slope;
        fobj->ad_head = desc->ad;
        fobj->length = desc->length;
        fobj->flags = 0;
        return fobj;
    }
    return NULL;
}

//8036B848
HSD_FObj* HSD_FObjAlloc(void)
{
    HSD_FObj* fobj = (HSD_FObj*)HSD_MemAlloc(sizeof(HSD_FObj)); //HSD_ObjAlloc(&fobj_alloc_data);
    HSD_CheckAssert("FObjAlloc could not alloc", fobj != NULL);
    memset(fobj, 0, sizeof(HSD_FObj));
    return fobj;
}

//8036B8A4
void HSD_FObjFree(HSD_FObj* fobj)
{
    HSD_ObjFree(&fobj_alloc_data, (HSD_ObjAllocLink*)fobj);
}
