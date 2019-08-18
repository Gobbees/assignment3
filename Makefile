all: server.exe client.exe

server.exe: server.o check_phase.o parse_message.o utilities.o
	gcc -o server.exe server.o check_phase.o parse_message.o utilities.o

client.exe: client.o utilities.o
	gcc -o client.exe client.o utilities.o

utilities.o: utilities.c
	gcc -c utilities.c

server.o: server.c
	gcc -c server.c

check_phase.o: check_phase.c check_phase.h
	gcc -c check_phase.c

parse_message.o: parse_message.c parse_message.h
	gcc -c parse_message.c

client.o: client.c
	gcc -c client.c

.PHONY: clean

clean:
	rm *.o *.exe