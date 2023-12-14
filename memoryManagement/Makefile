KERNEL_LIBS = ../lib/libkernel.a ../lib/libhardware.so
CPPFLAGS= -m32 -fno-builtin -I. -I$(INCDIR) -g -DLINUX

memoryManagement.o: memoryManagement.c $(KERNEL_LIBS) memoryManagement.h

clean:
	rm -f *~
	rm -f *.o