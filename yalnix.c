#include "include/ctype.h"
#include "include/hardware.h"
#include "include/filesystem.h"
#include "include/yalnix.h"
#include "include/load_info.h"

#include "memoryManagement/memoryManagement.h"
#include "interuptsAndTraps/interuptsAndTraps.h"

void initIdleProcess(UserContext *uctxt);

void KernalStart(char *cmd_args[], unsigned int pmem_size, UserContext *uctxt) {
    initInterupts();
    initPaging();
    initVirtualMemory();
    initIdleProcess(uctxt);
    // InitScheduling();
    // CreateInitProcess(uctxt)
}

void initIdleProcess(UserContext *uctxt) {
    
}

int main(int argc, char const *argv[])
{
    return 0;
}
