CC = gcc
CFLAGS = -Iinclude

all: root splitter builder

root: src/root.o src/common.o
	$(CC) $(CFLAGS) -o root src/root.o src/common.o

splitter: src/splitter.o src/common.o
	$(CC) $(CFLAGS) -o splitter src/splitter.o src/common.o

builder: src/builder.o src/common.o
	$(CC) $(CFLAGS) -o builder src/builder.o src/common.o

src/root.o: src/root.c
	$(CC) $(CFLAGS) -c src/root.c -o src/root.o

src/splitter.o: src/splitter.c
	$(CC) $(CFLAGS) -c src/splitter.c -o src/splitter.o

src/builder.o: src/builder.c
	$(CC) $(CFLAGS) -c src/builder.c -o src/builder.o

src/common.o: src/common.c
	$(CC) $(CFLAGS) -c src/common.c -o src/common.o

clean:
	rm -f root splitter builder src/*.o