/**
 * memoryManagement.c - A file containing all the memory management functions needed
 * 
 * Author: Josh Davis and John Kotz
 * Created: October 11, 2018
 * Copyright © 2018 Barely Disguised Cyborgs. All rights reserved.
 */

#include "../common/pagetable/pagetable.h"
#include "../common/queue/linkedListQueue.h"

void *kernelDataStart;
void *kernelDataEnd;
void *kernelBreak;

int virtualMemoryEnabled = 0;

pagetable_t *pagetableRegion0;
pagetable_t *pagetableRegion1;

LinkedListQueue_t *freeMemory;

int SetKernelBrk(void *addr) {
    if (!virtualMemoryEnabled) {
        kernelBreak = addr;
    } else {
        u_long numPagesBefore = (u_long)kernelBreak >> PAGESHIFT;
        u_long numPagesAfter = (u_long)addr >> PAGESHIFT;

        if (numPagesAfter == numPagesBefore) {
            kernelBreak = addr;
            return 0;
        }

        bool increasingSize = numPagesBefore < numPagesAfter;

        int prot = (PROT_READ | PROT_WRITE);
        int page = numPagesBefore;
        while (page != numPagesAfter)
        {
            if (increasingSize) {
                u_long frame = (u_long)linkedListQueuePop(freeMemory);
                enum PAGETABLE_STATUS status;
                page++;
                pagetableWrite(pagetableRegion0, page, frame, prot, &status);
            } else {
                u_long frame = pfnForPage(pagetableRegion0, page);
                pagetableRemove(pagetableRegion0, page);
                linkedListQueuePush(freeMemory, (void *)frame);
                page--;
            }
        }

        page = VMEM_0_BASE >> PAGESHIFT;
        while (page <= numPagesAfter) {
            struct pte *entry = pagetableGetEntry(pagetableRegion0, page);
            if (entry == NULL) {
                return -1;
            }
            entry->valid = 1;
            page++;
        }

        int numPagesBelowStack = KERNEL_STACK_LIMIT >> PAGESHIFT;
        while (page < numPagesBelowStack) {
            struct pte *entry = pagetableGetEntry(pagetableRegion0, page);
            if (entry != NULL) {
                entry->valid = 0;
            }
            page++;
        }

        kernelBreak = (void*)(((int)addr >> PAGESHIFT) << PAGESHIFT);
    }
    return 0;
}

void SetKernelData(void * KernelDataStart, void * KernelDataEnd) {
    kernelDataStart = KernelDataStart;
    kernelDataEnd = KernelDataEnd;

    SetKernelBrk(KernelDataEnd);
}

void initPaging(unsigned int pmem_size) {
    // Create a data structure to be used to keep track of free memory frames
    freeMemory = linkedListQueueCreate();
    if (freeMemory == NULL) {
        TracePrintf(0, "Failed to create a free memory database\n");
        Halt();
    }

    int frame = (int)kernelBreak / PAGESIZE;
    while (frame < pmem_size / PAGESIZE) {
        linkedListQueuePush(freeMemory, (void *)frame);
        frame += 1;
    }

    // Initialize the pagetables for region 0 and 1
    pagetableRegion0 = pagetableCreate(4);
    pagetableRegion1 = pagetableCreate(4);

    if (pagetableRegion0 == NULL || pagetableRegion1 == NULL) {
        TracePrintf(0, "Failed to create pagetables for either region 0 or 1 with 4 pages\n");
        Halt();
    }

    // Have the pagetables assign themselves to these registers
    pagetableAssignToRegisters(pagetableRegion0, REG_PTBR0, REG_PTLR0);
    pagetableAssignToRegisters(pagetableRegion1, REG_PTBR1, REG_PTLR1);
}

/// Initializes all pages between start (inclusive) and end (exclusive) with the given protection and with pfn = vpn
enum PAGETABLE_STATUS initializePagetableRange(pagetable_t *pagetable, u_long start, u_long end, int prot) {
    u_long frame = start;
    int didExpand = 0;

    while (frame < end) {
        enum PAGETABLE_STATUS status;
        pagetableWrite(pagetable, frame, frame, prot, &status);

        if (status == FAILED) {
            return FAILED;
        }
        frame++;
        didExpand = didExpand || status == EXPANDED;
    }

    return didExpand ? EXPANDED : NOMINAL;
}

void initVirtualMemory() {
    // Set up the pagetable with vpn = pfn for text block (protection is read exec)
    u_long textEnd = roundup((u_long)kernelDataStart / PAGESIZE, PAGESIZE);
    enum PAGETABLE_STATUS textStatus = initializePagetableRange(pagetableRegion0,
                                                                PMEM_BASE,
                                                                textEnd,
                                                                (PROT_EXEC | PROT_READ));
    if (textStatus == FAILED) {
        TracePrintf(0, "Failed to initialize text part of region 0 pagetable. Probably overran MAX_PT_LEN\n");
        Halt();
    }

    // Set up the pagetable with vpn = pfn for data block (protection is read write)
    u_long dataEnd = roundup((u_long)kernelBreak / PAGESIZE, PAGESIZE);
    enum PAGETABLE_STATUS dataStatus = initializePagetableRange(pagetableRegion0,
                                                                textEnd,
                                                                dataEnd,
                                                                (PROT_READ | PROT_WRITE));
    if (dataEnd == FAILED) {
        TracePrintf(0, "Failed to initialize text part of region 0 pagetable. Probably overran MAX_PT_LEN\n");
        Halt();
    }

    // If the pagetable needed to expand at any time during this process, it will have realloced
    // Thus we must reassign to the region 0 registers
    if (textStatus == EXPANDED || dataEnd == EXPANDED) {
        pagetableAssignToRegisters(pagetableRegion0, REG_PTBR0, REG_PTLR0);
    }

    WriteRegister(REG_VM_ENABLE, 1);
    virtualMemoryEnabled = 1;
}