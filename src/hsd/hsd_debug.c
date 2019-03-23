#include "hsd_debug.h"

void HSD_Halt(char error[]){
    printf("HALT: %s", error);
    assert(TRUE);
}

void HSD_Panic(char error[]){
    printf("PANIC: %s", error);
}

void HSD_Report(char error[]){
    printf(error);
}