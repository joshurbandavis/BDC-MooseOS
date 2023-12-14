/**
 * memoryManagement.h - All the memory management functions needed
 * 
 * Author: Josh Davis and John Kotz
 * Created: October 11, 2018
 * Copyright © 2018 Barely Disguised Cyborgs. All rights reserved.
 */

/**
 * Set the kernel break to the given address
 */
int SetKernelBrk(void *addr);

/**
 * Set information about the start and end of the kernel data memory
 */
void SetKernelData(void * KernelDataStart, void * KernelDataEnd);

/**
 * Set up paging
 */
void initPaging();

/**
 * Set up virtual memory
 */
void initVirtualMemory();
