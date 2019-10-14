showkeys: showkeys.c showkeys.h keystack.o config.h
	gcc -g -Wall showkeys.c keystack.o -o showkeys -lX11 -lxosd -lpthread -lXext -lX11 -lXinerama -lXtst


keystack.o: keystack.c keystack.h config.h
	gcc -c -g keystack.c

clean:
	rm showkeys keystack.o record-attempt record-example

check-syntax:
	gcc -Wall -o nul -S ${CHK_SOURCES}

record-attempt: record-attempt.c
	gcc -g -Wall record-attempt.c -L/usr/lib -lXtst -lxosd -lpthread -lXext -lX11 -lXinerama -o record-attempt

record-example: record-example.c
	gcc -g -Wall record-example.c -L/usr/lib -lXtst -lxosd -lpthread -lXext -lX11 -lXinerama -o record-example
