ALL = $(KERNAL_ALL) $(USER_APPS)
KERNAL_ALL = yalnix

#List all kernel source files here.  
KERNEL_SRCS = yalnix.c memoryManagement/memoryManagement.c interuptsAndTraps/interuptsAndTraps.c
#List the objects to be formed form the kernel source files here.  Should be the same as the prvious list, replacing ".c" with ".o"
KERNEL_OBJS = common/common.a  memoryManagement/memoryManagement.o interuptsAndTraps/interuptsAndTraps.o
#List all of the header files necessary for your kernel
KERNEL_INCS = common/pagetable/pagetable.h common/queue/queue.h common/queue/linkedListQueue.h memoryManagement/memoryManagement.h interuptsAndTraps/interuptsAndTraps.h

#List all user programs here.
USER_APPS = 
#List all user program source files here.  SHould be the same as the previous list, with ".c" added to each file
USER_SRCS = 
#List the objects to be formed form the user  source files here.  Should be the same as the prvious list, replacing ".c" with ".o"
USER_OBJS = 
#List all of the header files necessary for your user programs
USER_INCS = 

#write to output program yalnix
YALNIX_OUTPUT = yalnix

DDIR58 = ./
LIBDIR = $(DDIR58)/lib
INCDIR = $(DDIR58)/include
ETCDIR = $(DDIR58)/etc

# any extra loading flags...
LD_EXTRA = 

KERNEL_LIBS = $(LIBDIR)/libkernel.a $(LIBDIR)/libhardware.so
# the "kernel.x" argument tells the loader to use the memory layout
# in the kernel.x file..
KERNEL_LDFLAGS = $(LD_EXTRA) -L$(LIBDIR) -lkernel -lelf -Wl,-T,$(ETCDIR)/kernel.x -Wl,-R$(LIBDIR) -lhardware
LINK_KERNEL = $(LINK.c)

#
#	These definitions affect how your Yalnix user programs are
#	compiled and linked.  Use these flags *only* when linking a
#	Yalnix user program.
#

USER_LIBS = $(LIBDIR)/libuser.a
ASFLAGS = -D__ASM__
CPPFLAGS= -m32 -fno-builtin -I. -I$(INCDIR) -g -DLINUX 

MAKE = make
CC = gcc

all: $(ALL)	

kill:
	killall yalnixtty yalnixnet yalnix

$(KERNEL_ALL): $(KERNEL_OBJS) $(KERNEL_LIBS) $(KERNEL_INCS)
	$(LINK_KERNEL) -o $@ $(KERNEL_OBJS) $(KERNEL_LDFLAGS)

sources: $(KERNEL_OBJS)

common/common.a:
	cd common && $(MAKE)
memoryManagement/memoryManagement.o:
	cd memoryManagement && $(MAKE)
interuptsAndTraps/interuptsAndTraps.o:
	cd interuptsAndTraps && $(MAKE)

$(USER_APPS): $(USER_OBJS) $(USER_INCS)
	$(ETCDIR)/yuserbuild.sh $@ $(DDIR58) $@.o

no-core:
	rm -f core.*

clean:
	rm -f *~
	rm -f *.o
	rm -f $(PROG)
	rm -f TTYLOG*
	rm -f TRACE
	rm -f $(YALNIX_OUTPUT)
	rm -f $(USER_APPS)
	rm -f core.*
	cd common && $(MAKE) clean
	cd memoryManagement && $(MAKE) clean
	cd interuptsAndTraps && $(MAKE) clean
