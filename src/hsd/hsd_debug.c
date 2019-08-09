#include "hsd_debug.h"

void HSD_Halt(char error[]){
    printf("HALT: %s\n", error);
    assert(TRUE);
}

void HSD_CheckAssert(char error[], bool condition){
    if(condition == false){
        printf("HALT: %s\n", error);
        assert(TRUE);
    }
}

void HSD_Panic(char error[]){
    printf("PANIC: %s\n", error);
}

void HSD_Report(char error[]){
    printf(error);
}
