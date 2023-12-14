/**
 * interuptsAndTraps.c - Handles all interupts and traps the kernel will be receiving
 * 
 * Author: Josh Davis and John Kotz
 * Created: October 11, 2018
 * Copyright © 2018 Barely Disguised Cyborgs. All rights reserved.
 */

#include "../include/hardware.h"

void trap_kernel(void *params) {
    //todo: trap kernel code here
    printf("trap_kernel reached");
}

void trap_clock(void *params) {
    //todo: trap clock code here
    printf("trap_clock reached");
}

void trap_illegal(void *params) {
    //todo: trap illegal code here
    printf("trap_illegal reached");
}

void trap_memory(void *params) {
    //todo: trap memory code here
    printf("trap_memory reached");
}

void trap_math(void *params) {
    //todo: trap math code here
    printf("trap_math reached");
}

void tty_recieve(void *params) {
    //todo: tty_recieve code here
    printf("tty_recieve reached");
}

void tty_transmit(void *params) {
    //todo: tty_transmit code here
    printf("tty_transmit reached");
}

void trap_disk(void *params) {
    //todo: trap disk code here
    printf("trap_disk reached");
}

void initInterupts() {
    
    //todo: implement function handlers for vector table
    void (*interrup_vector_table[])(void *) = {trap_kernel, trap_clock, trap_illegal, trap_memory, trap_math, tty_recieve, tty_transmit, trap_disk, NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
    WriteRegister(REG_VECTOR_BASE, &interrup_vector_table);
}