KERNEL_LIBS = ../lib/libkernel.a ../lib/libhardware.so
CPPFLAGS= -m32 -fno-builtin -I. -I$(INCDIR) -g -DLINUX

interuptsAndTraps.o: interuptsAndTraps.c $(KERNEL_LIBS) interuptsAndTraps.h

clean:
	rm -f *~
	rm -f *.o