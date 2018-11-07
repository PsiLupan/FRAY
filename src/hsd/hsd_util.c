#include "hsd_util.h"

f32 HSD_ClampFloat(f32 val, f32 min, f32 max){
    if(val > max){
        return max;
    }else if(val < min){
        return min;
    }
    return val;
}