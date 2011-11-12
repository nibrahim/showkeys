showkeys: showkeys.c showkeys.h keystack.o
	gcc -g -Wall showkeys.c keystack.o -o showkeys -lX11 -lxosd -lpthread -lXext -lX11 -lXinerama


keystack.o: keystack.c keystack.h
	gcc -c -g keystack.c

clean:
	rm showkeys keystack.o

check-syntax:
	gcc -Wall -o nul -S ${CHK_SOURCES}
