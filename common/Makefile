MAKE = make
LIB = common.a

$(LIB): queue/queue.o pagetable/pagetable.o
	ar cr $@ $^

queue/queue.o:
	cd queue && $(MAKE);

pagetable/pagetable.o:
	cd pagetable && $(MAKE);

clean:
	rm -f *~
	rm -f *.o
	rm -f common.a
	cd queue && $(MAKE) clean;
	cd pagetable && $(MAKE) clean;