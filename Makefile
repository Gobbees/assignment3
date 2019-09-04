all: create_folders server.exe client.exe 

create_folders: clean
	mkdir bin; mkdir bin/server/; mkdir bin/client/; mkdir bin/common/

server.exe: bin/server/server.o bin/server/check_phase.o bin/server/parse_message.o bin/common/utilities.o
	gcc -o server.exe bin/server/server.o bin/server/check_phase.o bin/server/parse_message.o bin/common/utilities.o

client.exe: bin/client/client.o bin/common/utilities.o
	gcc -o client.exe bin/client/client.o bin/common/utilities.o

bin/common/utilities.o: src/common/utilities.c
	gcc -c src/common/utilities.c -o bin/common/utilities.o

bin/server/server.o: src/server/server.c
	gcc -c src/server/server.c -o bin/server/server.o

bin/server/check_phase.o: src/server/check_phase.c
	gcc -c src/server/check_phase.c -o bin/server/check_phase.o

bin/server/parse_message.o: src/server/parse_message.c
	gcc -c src/server/parse_message.c -o bin/server/parse_message.o

bin/client/client.o: src/client/client.c
	gcc -c src/client/client.c -o bin/client/client.o

.PHONY: clean

clean:
	if [[ -e bin/ ]]; then rm -rf bin/; rm *.exe; fi