win-client: helpers.o client.o
	gcc obj/helpers.o obj/client.o -lws2_32 -o bin/client

unix-client: helpers.o client.o
	gcc obj/helpers.o obj/client.o -o bin/client

win-server: helpers.o server.o
	gcc obj/helpers.o obj/server.o -lws2_32 -o bin/server

unix-server: helpers.o server.o
	gcc obj/helpers.o obj/server.o -o bin/server

helpers.o: src/helpers.c headers/helpers.h
	gcc -c src/helpers.c -o obj/helpers.o

server.o: src/server.c headers/helpers.h
	gcc -c src/server.c -o obj/server.o

client.o: src/client.c headers/helpers.h
	gcc -c src/client.c -o obj/client.o

win-clean:
	DEL /F /Q "obj\helpers.o" "obj\server.o" "obj\client.o" "bin\server.exe" "bin\client.exe"

unix-clean:
	rm -f ./obj/helpers.o ./obj/server.o ./obj/client.o ./bin/server ./bin/client