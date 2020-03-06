server: helpers.o server.o
	gcc obj/helpers.o obj/server.o -lws2_32 -o bin/server

helpers.o: src/helpers.c headers/helpers.h
	gcc -c src/helpers.c -o obj/helpers.o

server.o: src/server.c headers/helpers.h
	gcc -c src/server.c -o obj/server.o

win-clean:
	del "obj\helpers.o" "obj\server.o" "bin\server.exe"

unix-clean:
	rm ./obj/helpers.o ./obj/server.o ./bin/server.bin 