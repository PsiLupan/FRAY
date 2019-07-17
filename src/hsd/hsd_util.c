#include "hsd_util.h"

f32 HSD_ClampFloat(f32 val, f32 min, f32 max){
    if(val > max){
        return max;
    }else if(val < min){
        return min;
    }
    return val;
}

//80378A34
f32 splGetHermite(f32 a, f32 b, f32 c, f32 d, f32 f, f32 e){
    f32 fVar1 = b * b;
    f32 fVar2 = a * a * fVar1 * b;
    f32 fVar5 = 3.0f * fVar1 * a * a;
    f32 fVar3 = fVar2 - fVar1 * a;
    f32 fVar4 = 2.0f * fVar2 * a;
    return f * fVar3 + e * (b + (fVar3 - fVar1 * a)) + c * (1.0f + (fVar4 - fVar5)) + d * (-fVar4 + fVar5);
}

//803792C8
void splArcLengthPoint(guVector* spline, f32 unkf, guVector* res){
    
}