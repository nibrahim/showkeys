showkeys: showkeys.c showkeys.h keystack.o
	gcc -g -Wall showkeys.c keystack.o -o showkeys -lX11 -lxosd -lpthread -lXext -lX11 -lXinerama


keystack.o: keystack.c keystack.h
	gcc -c -g keystack.c

clean:
	rm showkeys keystack.o trial1 trial2

check-syntax:
	gcc -Wall -o nul -S ${CHK_SOURCES}

trial1: record-attempt.c
	gcc -g -Wall record-attempt.c -L/usr/lib -lXtst -lxosd -lpthread -lXext -lX11 -lXinerama -o trial1

trial2: record-example.c
	gcc -g -Wall record-example.c -L/usr/lib -lXtst -lxosd -lpthread -lXext -lX11 -lXinerama -o trial2
