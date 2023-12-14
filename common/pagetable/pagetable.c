/**
 * pagetable.c - A page table data structure
 * 
 * Author: Josh Davis and John Kotz
 * Created: October 10, 2018
 * Copyright © 2018 Barely Disguised Cyborgs. All rights reserved.
 */

#include "pagetable.h"

typedef struct pte pte_t;

typedef struct pagetable {
    pte_t *table;
    int numPages;
} pagetable_t;

pagetable_t *pagetableCreate(int numPages) {
    pte_t *table = calloc(numPages, sizeof(pte_t));
    if (table == NULL) return NULL;

    // The table should start empty, with nothing valid, no protection; nothing
    int i = 0;
    while (i < numPages) {
        pte_t entry;
        entry.valid = 0;
        entry.pfn = 0;
        entry.prot = PROT_NONE;
        table[i] = entry;
        i++;
    }
    
    // Allocate memory for the pagetable object.
    // Since this is stored somewhere else in memory than the table itself,
    //   we can expand the table without loosing track of it
    pagetable_t *pagetable = malloc(sizeof(pagetable_t));
    if (pagetable == NULL) {
        free(table);
        return NULL;
    }

    pagetable->numPages = numPages;
    pagetable->table = table;

    return pagetable;
}

int pagetableNumPages(pagetable_t *pagetable) {
    if (pagetable == NULL) return 0;
    return pagetable->numPages;
}

/**
 * Expands the pagetable in memory
 * 
 * Expands by a factor of 2 by default. If until is greater than that,
 *  will continue to double until it is no longer greater. Will cap the expansion at MAX_PT_LEN
 * 
 * - parameter pagetable: The pagetable to expand
 * - parameter until: The number of pages to expand up to (rounded by multiples of numPages)
 * - returns: 0 on success, -1 if exceded the maximum number of pages, -2 if failed to find new memory
 */
int pagetableExpand(pagetable_t *pagetable, u_long until) {
    // If the pagetable is already max size or
    //  you are requesting something larger than possible, return -1
    if (pagetable->numPages >= MAX_PT_LEN || until > MAX_PT_LEN) return -1;

    int numPages = pagetable->numPages * 2;

    // Increase number of pages until 'until' can be fit in the table
    while (until > numPages) {
        numPages = numPages * 2;
    }

    numPages = MIN(numPages, MAX_PT_LEN);

    // Reallocate the table
    size_t size = numPages * sizeof(pte_t);
    pte_t *newTable = realloc(pagetable->table, size);
    if (newTable == NULL) {
        // FAILED to realloc!
        return -2;
    }

    // Clear out the new part of the table
    int i = pagetable->numPages;
    while (i < numPages) {
        pte_t entry;
        entry.pfn = 0;
        entry.valid = 0;
        entry.prot = PROT_NONE;
        newTable[i] = entry;
        i++;
    }

    // Complete! Reassign and return 0
    pagetable->table = newTable;
    pagetable->numPages = numPages;
    return 0;
}

void getPagetableMemoryRange(pagetable_t *pagetable, void **base, void **limit) {
    if (pagetable == NULL || base == NULL || limit == NULL) return;

    *base = (void *)(pagetable->table);
    *limit = (void *)(pagetable->table) + pagetable->numPages * sizeof(pte_t);
}

u_long pagetableInsert(pagetable_t *pagetable, u_long pfn, u_long prot, enum PAGETABLE_STATUS *status) {
    if (pagetable == NULL) {
        if (status != NULL) *status = FAILED;
        return;
    }

    u_long page = 0;
    if (status != NULL) *status = NOMINAL;
    
    // Get the next page number which is invalid (or is the last page)
    while (!pagetable->table[page].valid && page < pagetable->numPages) page++;

    // If the chosen page is valid (aka none of the entries are invalid)
    //      then expand the pagetable and set the page number to the next one
    if (pagetable->table[page].valid) {
        if (status != NULL) *status = EXPANDED;
        page = pagetable->numPages;
        int rc = pagetableExpand(pagetable, 0);
        if (rc != 0) {
            if (status != NULL) *status = FAILED;
            return 0;
        }
    }

    pte_t entry;
    entry.pfn = pfn;
    entry.prot = prot;
    entry.valid = 1;

    pagetable->table[page] = entry;
    return page;
}

void pagetableWrite(pagetable_t *pagetable, u_long vpn, u_long pfn, u_long prot, enum PAGETABLE_STATUS *status) {
    if (pagetable == NULL) {
        if (status != NULL) *status = FAILED;
        return;
    }

    if (status != NULL) *status = NOMINAL;

    // If the chosen page is outside of the table, expand until it is inside
    if (vpn > pagetable->numPages) {
        if (status != NULL) *status = EXPANDED;
        int rc = pagetableExpand(pagetable, vpn);
        if (rc != 0) {
            if (status != NULL) *status = FAILED;
            return;
        }
    }

    pte_t entry;
    entry.pfn = pfn;
    entry.prot = prot;
    entry.valid = 1;

    pagetable->table[vpn] = entry; 
}

struct pte *pagetableGetEntry(pagetable_t *pagetable, u_long vpn) {
    if (pagetable == NULL || vpn > pagetable->numPages) return NULL;
    return &pagetable->table[vpn];
}

void pagetableRemove(pagetable_t *pagetable, u_long page) {
    pte_t entry;
    entry.pfn = 0;
    entry.prot = PROT_NONE;
    entry.valid = 0;

    pagetable->table[page] = entry;
}

u_long pfnForPage(pagetable_t *pagetable, u_long page) {
    if (pagetable == NULL || page > pagetable->numPages) return 0;
    return pagetable->table[page].pfn;
}

u_long pageNumberForVirtualAddr(void *addr) {
    return (u_long)addr >> PAGESHIFT;
}

u_long offsetForVirtualAddr(void *addr) {
    return (u_long)addr - pageNumberForVirtualAddr(addr) << PAGESHIFT;
}

void *pfnForVirtualAddr(pagetable_t *pagetable, void *addr) {
    if (pagetable == NULL) return NULL;

    u_long page = pageNumberForVirtualAddr(addr);
    u_long offset = offsetForVirtualAddr(addr);
    u_long pfn = pagetable->table[page].pfn;

    return (void *)((page << PAGESHIFT) + offset);
}

void pagetableDelete(pagetable_t *pagetable) {
    if (pagetable == NULL) return;

    free(pagetable->table);
    free(pagetable);
}

void pagetableAssignToRegisters(pagetable_t *pagetable, int REGB, int REGL) {
    if (pagetable == NULL) return;

    void *PTB;
    void *PTL;

    getPagetableMemoryRange(pagetable, &PTB, &PTL);

    WriteRegister(REGB, (unsigned int)PTB);
    WriteRegister(REGL, (unsigned int)PTL);
}

#ifdef UNIT_TEST
int main(int argc, char const *argv[])
{
    
    return 0;
}
#endif
