showkeys: showkeys.c
	gcc -Wall showkeys.c -o showkeys -lX11

clean:
	rm showkeys

check-syntax:
	gcc -Wall -o nul -S ${CHK_SOURCES}
