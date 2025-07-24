WINDOWS_CC = x86_64-w64-mingw32-gcc
UNIX_CC = gcc

CFLAGS = -Wall

WINDOWS_TARGET = crencrypt.exe
UNIX_TARGET = crencrypt

all: unix windows

unix: main.c crencrypt.c
	$(UNIX_CC) $(CFLAGS) -o $(UNIX_TARGET) main.c crencrypt.c

windows: main.c crencrypt.c
	$(WINDOWS_CC) $(CFLAGS) -o $(WINDOWS_TARGET) main.c crencrypt.c

clean:
	rm -f crencrypt crencrypt.exe main.o crencrypt.o