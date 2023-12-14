KERNEL_LIBS = ../../lib/libkernel.a ../../lib/libhardware.so
CPPFLAGS= -m32 -fno-builtin -I. -I$(INCDIR) -g -DLINUX 

pagetable.o: pagetable.c $(KERNEL_LIBS) pagetable.h

clean:
	rm -f *~
	rm -f *.o