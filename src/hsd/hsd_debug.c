#include "hsd_debug.h"

void HSD_Halt(char error[])
{
    printf("HALT: %s\n", error);
    assert(FALSE);
}

void HSD_CheckAssert(char error[], bool condition)
{
    if (!condition) {
        printf("ASSERT: %s\n", error);
        assert(FALSE);
    }
}

void HSD_Panic(char error[])
{
    printf("PANIC: %s\n", error);
}

void HSD_Report(char error[])
{
    printf("%s", error);
}
