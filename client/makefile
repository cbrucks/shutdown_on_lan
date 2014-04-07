all: sol

sol: shutdown_on_lan.o udp_comm.o
	gcc *.o -o sol

shutdown_on_lan.o: shutdown_on_lan.c
	gcc -c shutdown_on_lan.c -o shutdown_on_lan.o

udp_comm.o: udp_comm.c
	gcc -c udp_comm.c -o udp_comm.o
	
clean:
	rm -rf *.o sol