# Design Spec

## Major functionalities

### Booting

#### SetKernelBrk (void * addr)
```
(global) kernelBrk

if (virtual_memory_enabled == false)
    (global) kernelBrk = addr
else if (virtual_memory_enabled == true) 
{
    numPages = ceil(abs(addr - (global) kernelBrk) / PAGESIZE)
    for page in numPages {
        if addr is greater than (global) kernelBrk:
            frame = (global) FreeMemoryQueue.pop()
            Set pageTable at page to frame
            // Set page's privalaged value to true in pageTable (our PTEs don't have privalaged mode)
        else:
            Get entry of page in pageTable
            Delete entry in pageTable for page
            (global) FreeMemoryQueue.push(entry.frame)
    }
    
    page = VMEM_BASE / PAGESIZE
    while page < floor(addr / PAGESIZE)
    {
        Get pageTable's entry for page
        if entry == nil 
        {
            return -1
        }
        entry.valid = true
        page++
    }
    
    numPagesBelowStack = KERNAL_STACK_LIMIT / PAGESIZE;
    
    while (page < numPagesBelowStack)
    {
        Get pagetables's entry for page
        if entry != nil
        {
            entry.valid = false
        }
        page++
    }

    (global) kernelBrk = ceil(addr / PAGESIZE) * PAGESIZE
}
```

#### SetKernelData (void * _KernelDataStart, void *_KernelDataEnd)
```
// KernelDataEnd = highest address of data and instructions + 1
// KernelDataStart = lowest address of data
// PMEM_BASE = kernel begining at physical adress

SetKernelBrk(KernelDataEnd)
Store _KernelDataStart and _KernelDataEnd
```

#### KernelStart
```
InitInterupts()
InitPaging()
InitVirtualMemory()
idleProcessPCB = InitIdleProcess(uctxt)
InitScheduling(idleProcessPCB)
CreateInitProcess(uctxt)
return
```

#### InitInterupts
```
create interupt vector table
initialize entry in vector table (index = 0 to (TRAP_VECTOR_SIZE - 1)) 
    with pointer to handler function
write address of vector table to REG_VECTOR_BASE

```

#### InitPaging
```
create initial page tables

Initialize the registers PTBR0, PTLR0, PTBR1, and PTLR1 with references to these page tables
```

#### InitVirtualMemory
```
for each frame of physical memory (pfn) in use by kernel
    create page table entry for each pfn frame
    map pfn = vpn

Intialize pages in pageTable between PMEM_BASE to KernelDataStart with PROT_READ and PROT_EXEC and privaladged

Initialize pages in pageTable between KernelDataStart and KernelDataEnd with PROT_READ and PROT_WRITE and privaladged

// ---------   }
//  GLOBALS  <----------privelidge read and write these 
// ---------   }
//  TEXT     <----------privelidge read and execute these
// ---------   }

set REG_VM_ENABLE = true
```

#### InitIdleProcess() -> PCB
```
Create empty PCB
pcb.quantum = 0
Set up the PCB

Make the idle process execute the following:
    forever:
        Pause()

return pcb
```

NOTE: This process’s quantum is 0. The scheduler should switch to any other process if there exists one.

#### CreateInitProcess
```
Load file (like template.c)
Make PCB and set up with default quantum
Scheduler.addPCB(pcb)
```

### Scheduling
Generally will use a round-robin scheduler, running each process for the quantum defined in their PCB. The Scheduler will hold a reference to the currently running process PCB. All other processes will be in either the readyQueue or allQueue.

#### InitScheduling(idleProcessPCB)
Allocate memory for readyQueue and allQueue and set the currently running process to idleProcessPCB.

#### AddPCB(pcb)
```
If pcb.state == READY
	readyQueue.push(pcb)
else
    blockedQueue.push(pcb)
```

#### DidReceiveClockTick
```
(global) schedulerNumTicks += 1
if schedulerNumTicks >= currentProcessPCB.quantum
    if readyQueue is not empty
        ContextSwitchTo(readyQueue.pop())
```

#### ContextSwitchTo(processPCB)
```
KernelContextSwitch(ContextSwitchKCS, currentProcessPCB, processPCB)
```

#### ContextSwitchKCS(KernelContext *kc_in, ...)
```
Save kc_in in in current PCB
Update page table for next process
Flush TLB
return next process's KernelContext
```

### Traps
#### TRAP_TTY_RECEIVE
```
If the ttyReceiveBuffer is not allocated, do so.
TtyReceive(userContext.code, ttyReceiveBuffer, min(readRequest.len, TERMINAL MAX LINE))
copy ttyReceiveBuffer into readRequest.buffer
Unblock readRequest.pid
```

#### TRAP_TTY_TRANSMIT
```
Unblock writingProcess
```

#### TRAP_MATH
```
TracePrintf "Bad math (divide by zero, etc)"
Exit(Error)
```

#### TRAP_ILLEGAL
```
TracePrintf "Illegal operation"
Exit(Error)
```

#### TRAP_CLOCK
```
For each delayRequest in delayRequest list
	delayRequest.numClockTicks -= 1'
    
	If delayRequest.numClockTicks is 0:
		Unblock delayRequest.PID
        remove delayRequest from delayRequest list

Scheduler.didReceiveClockTick()
```

#### TRAP_MEMORY
```
if (UserContext.addr is in Region 1 && 
    UserContext.addr < stack memory allocation &&
    UserContext.addr > current break for executing process &&
    UserContext.code == YALNIX_MAPERR)
    {
        grow stack until > UserContext.addr
        leave at least one page unmapped for red zone
        set red zone valid bit to 0
        return 0
    }
else
    Exit(UserContext)
```

#### TRAP_KERNEL
```
callNumber = UserContext.code
UserContext.regs[0] = KernelCall(callNumber, regs[])
```

#### KernelCall (callNum, regs[])
```
//as defined in yalnix.h
//TODO: replace numbers with definition names in yalnix.h
if (callNum == 1)
    return fork()
if (callNum == 2)
    return exec()
if (callNum == 3)
    return exit()
if (callNum == 4)
    return wait()
if (callNum == 5)
    return getpid()
if (callNum == 6)
    return brk()
if (callNum == 7)
    return delay()
if (callNum == 21)
    return ttyread()
if (callNum == 22)
    return ttywrite()
if (callNum == 48)
    return pipeInit()
if (callNum == 49)
    return pipeRead()
if (callNum == 4A)
    return pipeWrite()
if (callNum == 63)
    return lockInit()
if (callNum == 64)
    return lockAcquire()
if (callNum == 65)
    return lockRelease()
if (callNum == 66)
    return cvarInit()
if (callNum == 67)
    return cvarSignal()
if (callNum == 68)
    return cvarBroadcast()
if (callNum == 69)
    return cvarWait()
if (callNum == 6A)
    return reclaim()
```

## Function pseudocode

### General methods

#### fork
Create a new process identical to the calling one, with copied memory

```
Create new PCB
Copy parent process user space (get 4 frames)
Create new page-table
Flush TLB
Save parent state to PCB
Set child return code to 0 and parent return code to child PID
```

#### exec
```
Replace user code text of process with text from filename contents
Free stack and heap
Load globals into data
argc = len(argv)
Return error if any
Run filename.main(argc, argv)
Return error if any
```

#### exit
```
If process is init, send Halt to CPU
If process is not orphan
	save exit status and PID in FIFO queue
	If parent process is blocked waiting:
		unblock parent

Terminate current process
Free stack, heap, data, text, libraries, and kernel stack
Delete PCB
```

#### wait
```
Get PID and exit status from FIFO queue
If PID and exit status:
	set status pointer to status
	return PID
If any child exists (not exited):
	block
	// ==================> Assuming we come back here when unblocked
	get PID and exit status from FIFO queue
	set status pointer to status
	return PID

Otherwise if there was an error:
	return error
```

#### getPID
```
Get current running PCB
return PCB’s PID
```

#### brk
```
incrementing = addr is greater than currentBRK

numPages = ceil(abs(addr - currentBRK) / PAGESIZE)
if arithmetic error:
    return error
	
for i in numPages {
    page = currentBRK / PAGESIZE + i
    if incrementing {
        Set frame for page in pageTable to FreeMemoryQueue.pop()
    } else {
        Get entry in pageTable for page
        FreeMemoryQueue.push(entry.framepointer)
        Remove entry in pageTable for page
    }
}
    
if memory error:
    return error
set currentBRK to ceil(addr / PAGESIZE) * PAGESIZE
if arithmetic error:
    return error
Return 0
```

#### delay
```
Add DelayRequest(PID, clock_ticks) to delay list
block current process
return 0 or error if any
```

Daemon will handle clock ticks and decrement numClockTicks until 0. When 0, DelayRequest is deleted and PID is unblocked
See section in Major Functionalities on delaying for more information.

### TTY

#### ttyRead
```
Aquire(readLock for terminal tty_id)
Global readRequest = ReadWriteRequest(buffer, len, current PID)
Block current process
Release(readLock for terminal tty_id)
Return 0 or error if any
```
Will unblock PID when the buffer has been filled.

#### ttyWrite
```
Aquire(writeLock for terminal tty_id)
TtyTransmit(tty_id, buffer, len)
Global writingProcess = PID
Block current process
Release(writeLock for terminal tty_id)
Return 0 or error if any
```

Daemon will unblock PID when the buffer has been written.

### Pipes

#### PipeInit
```
Create pipe id
Create Pipe and allocate buffer
Add Pipe to pipeTable
Assign pipe id to pipe_idp
Return 0 or error if any
```

#### PipeRead
```
Lookup pipe_id from pipe table to get Pipe
Lock lock

while data is not available in buffer:
	wait for broadcast from cvar

copy len bytes from pipe buffer to buf
remove item from pipe buffer
broadcast cvar
unlock lock 
return 0 or error if any
```

#### PipeWrite
```
Lookup pipe_id from pipe table to get Pipe
Lock lock

while buffer is full:
	wait for broadcast from cvar

copy len bytes from buf to pipe buffer
broadcast cvar
unlock lock 
return 0 or error if any
```

#### Internal functions
##### pipeIDForTableIndex
```
Return (index * 3) + 1
```

### Locks

#### LockInit
```
Create lockid
Create lock
Add lockid to lockTable
Assign lockid to lock_idp
Return 0 or error if any
```

#### Acquire
```
Lookup lock with lockid in lockTable
if lockId is invalid: return error
if lock is free:
	lock.currentPID = PID
Lock.free = false
	return 0 or error
else:
	lock.waitQueue.add(PID)
block PID
If error return ERROR;
Lock.free = false
Lock.currentPID = PID;
return 0 or error if any
```

#### Release
```
Lookup lock with lockid in lockTable
If PID != lock.currentPID
	Return ERROR
Lock.free = true
Unblock PID in waitQueue.pop()
Return 0 or error
```

#### Internal functions
##### LockID for LockTable
```
return (index*3) +2;
```

### Cvars

#### CvarInit
```
Create cvar id
Create cvar
Add cvar id to cvarTable
Assign cvar id to cvar idp
Return 0 or error
```

#### CvarSignal
```
Get cvar from cvarTable
Pop waiting PID from cvar.waitQueue
If waiting PID:
	Unblock waiting PID
Return 0 or error if any
```

#### CvarBroadcast
```
Get cvar from cvarTable
While cvar.waitQueue is not empty:
Pop waiting PID from cvar.waitQueue
If waiting PID:
		Unblock waiting PID
Return 0 or error if any
```

#### CvarWait
```
Lookup cvar from cvarTable
Release(lock_id)
Cvar.waitQueue.add(PID)
Block PID
Aquire(lock_id)
Return 0 or error if any
```

#### Reclaim
```
Type = (id -1) % 3
If (type == 0)
    //it’s a pipe
    Lookup id in pipe table
    Delete pipe at id
    Delete pipe id from pipe table
Else if (type == 1)
    // it’s a lock
    Lookup id in lock table
    Delete lock at id
    Delete lock id from lock table
Else if (type == 2)
    // it’s a cvar
        Lookup id in cvar table
    Delete cvar
    Remove entry in cvarTable 
```

#### Internal functions
##### cvarIDForTableIndex
```
Return (index * 3) + 3
```

## Data Structures

#### PageTable
A page table is a table of size NUM_PAGES filled with PTE objects.

#### Cvar
```c
int id;
LinkedListQueue *waitQueue;
```

#### Lock
```c
int id;
Bool free;
pid_t currentPID;
LinkedListQueue *waitQueue;
```

#### Pipe
```c
int id;
queue *buffer;
cvar cvar;
lock lock; 
```

#### DelayRequest
```c
pid_t pid;
int numClockTicks;
```

#### ReadRequest
```c
char *buffer;
int len;
pid_t pid;
```

#### PCB
```c
int pid;
int processState;
int programCounter;

void *uspace; // Pointer to Region 1
void *uctext; // User code text
void *kctext; // Pointer to region 0
void *kstack;  // The kernel stack
```

#### Queue
```c
int size;
void *list;
int front;
int back;
```

#### LinkedListQueue
```c
QueueItem *front;
QueueItem *back;
```

##### LinkedListQueueItem
```c
void *data;
QueueItem *next;
```