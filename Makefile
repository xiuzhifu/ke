ke: kiss_queue.o kiss_event.o kiss_man.o 
	cc kiss_queue.o kiss_event.o kiss_man.o -g -llua -o $@
kiss_queue.o: kiss_queue.c kiss_queue.h
	cc -c $< -o $@ -g
kiss_man.o: kiss_man.c
	cc -c $< -o $@ -g
kiss_event.o: kiss_event.c kiss_event.h
	cc -c $< -o $@ -g
kissocket.so: lkissocket.o
	cc -shared $< -llua -o $@
lkissocket.o: lkissocket.c
	cc -c $<
clean:
	-rm -f kissocket.so
	-rm -f *.o
	-rm -f ke

