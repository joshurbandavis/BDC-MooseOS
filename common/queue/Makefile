CC = cc
CFLAGS = -Wall -pedantic -std=c11 -ggdb
PROG = queuetest
OBJS = queuetest.o queue.o linkedListQueue.o
KERNEL_LIBS = ../../lib/libkernel.a ../../lib/libhardware.so
LIBS =

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

queuetest.o: queue.h linkedListQueue.h
queue.o: queue.h $(KERNEL_LIBS)
linkedListQueue.o: linkedListQueue.h $(KERNEL_LIBS)

clean:
	rm -f *~
	rm -f *.o
	rm -f $(PROG)