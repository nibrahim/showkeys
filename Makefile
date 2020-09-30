LDFLAGS = -lX11 -lxosd -lpthread -lXext -lX11 -lXinerama -lXtst
SOURCES = keystack.c showkeys.c
HEADERS = config.h keystack.h showkeys.h die.h
CC = gcc
CFLAGS = -g -Wall -Wextra -fdiagnostics-color=auto
LD = gcc

all: showkeys

clean:
	rm -f showkeys *.o

.c.o: $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

showkeys: $(SOURCES:.c=.o)
	$(LD) -o $@ $^ $(LDFLAGS)
