/**
 * pagetable.h - A page table data structure
 * 
 * Author: Josh Davis and John Kotz
 * Created: October 10, 2018
 * Copyright © 2018 Barely Disguised Cyborgs. All rights reserved.
 */

#include "../../include/hardware.h"
#include <stdlib.h>

/**
 * A simple data structure used to keep track of the page table.
 */
typedef struct pagetable pagetable_t;

enum PAGETABLE_STATUS { NOMINAL = 0, EXPANDED = 1, FAILED = 2 };

/**
 * Create a new Page table.
 * 
 * - parameter numPages: The number of pages to include in this table
 * - returns: A new page table data structure reference
 */
pagetable_t *pagetableCreate(int numPages);

int pagetableNumPages(pagetable_t *pagetable);

/**
 * Get the actual PTE table within the page table
 * 
 * - parameter pagetable: The pagetable to get the PTE table of
 * - returns: The pagetable's PTE table
 */
void getPagetableMemoryRange(pagetable_t *pagetable, void **base, void **limit);

/**
 * Insert a phisical frame number into the pagetable with some protection
 * 
 * - parameter pagetable: The pagetable to insert into
 * - parameter pfn: The phisical frame number
 * - parameter prot: The protection to put on this entry (eg. PROT_READ | PROT_WRITE)
 * - parameter status: A pointer to a status. By the end will contain the status of the insert.
 *                          If it worked normally *status will be NOMINAL. If the table expanded, *status will be EXPANDED.
 *                          If something went wrong *status will be FAILED.
 * - returns: The page number inserted into (will be 0)
 */
u_long pagetableInsert(pagetable_t *pagetable, u_long pfn, u_long prot, enum PAGETABLE_STATUS *status);

/**
 * Write to the entry at the given virtual page number with protection and phisical frame number
 * 
 * - parameter pagetable: The pagetable to operate on
 * - parameter vpn: The virtual page number to assign to
 * - parameter pfn: The page frame number to assign
 * - parameter prot: The protection that should be put on this entry
 * - parameter status: A pointer to a status. By the end will contain the status of the write.
 *                          If it worked normally *status will be NOMINAL. If the table expanded, *status will be EXPANDED.
 *                          If something went wrong *status will be FAILED.
 */
void pagetableWrite(pagetable_t *pagetable, u_long vpn, u_long pfn, u_long prot, enum PAGETABLE_STATUS *status);

void pagetableRemove(pagetable_t *pagetable, u_long page);
struct pte *pagetableGetEntry(pagetable_t *pagetable, u_long vpn);

/**
 * The phisical frame number for a given page number
 * 
 * - parameter pagetable: The pagetable to use for the lookup
 * - parameter page: The page number to lookup
 * - returns: The phisical frame number
 */
u_long pfnForPage(pagetable_t *pagetable, u_long page);

/**
 * Get the page number for a virtual address
 */
u_long pageNumberForVirtualAddr(void *addr);

/**
 * Get the offset for a virtual address
 */
u_long offsetForVirtualAddr(void *addr);

/**
 * Translate a virtual address to a physical address
 */
void *pfnForVirtualAddr(pagetable_t *pagetable, void *addr);

/**
 * Delete and free a pagetable
 */
void pagetableDelete(pagetable_t *table);

/**
 * Assign the start and end address of the table to the given registers
 */
void pagetableAssignToRegisters(pagetable_t *pagetable, int REGB, int REGL);
