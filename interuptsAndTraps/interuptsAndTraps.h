/**
 * interuptsAndTraps.h - Handles all interupts and traps the kernel will be receiving
 * 
 * Author: Josh Davis and John Kotz
 * Created: October 11, 2018
 * Copyright © 2018 Barely Disguised Cyborgs. All rights reserved.
 */

void initInterupts();
void trap_kernel(void);
void trap_clock(void);
void trap_illegal(void);
void trap_memory(void);
void trap_math(void);
void tty_recieve(void);
void tty_transmit(void);
void trap_disk(void);

