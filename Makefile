all: server.exe client.exe

server.exe: server.o check_phase.o parse_message.o
	gcc -o server.exe server.o check_phase.o parse_message.o

client.exe: client.o message_sizes.h client_executor.o
	gcc -o client.exe client.o client_executor.o

server.o: server.c check_phase.h parse_message.h myfunction.h message_sizes.h
	gcc -c server.c

check_phase.o: check_phase.c check_phase.h
	gcc -c check_phase.c

parse_message.o: parse_message.c parse_message.h
	gcc -c parse_message.c

client_executor.o: client_executor.c client_executor.h myfunction.h
	gcc -c client_executor.c

client.o: client.c myfunction.h client_executor.h
	gcc -c client.c

.PHONY: clean

clean:
	rm *.o *.exe