CC = gcc
CFLAGS = -Iinclude
LDFLAGS = -pthread

all: root splitter builder

root: src/root.o src/common.o src/modules/ADTHash.o src/modules/ADTSet.o src/modules/ADTMap.o src/modules/ADTVector.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o root src/root.o src/common.o src/modules/ADTHash.o src/modules/ADTSet.o src/modules/ADTMap.o src/modules/ADTVector.o

splitter: src/splitter.o src/common.o src/modules/ADTSet.o src/modules/ADTHash.o src/modules/ADTMap.o src/modules/ADTVector.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o splitter src/splitter.o src/common.o src/modules/ADTSet.o src/modules/ADTHash.o src/modules/ADTMap.o src/modules/ADTVector.o

builder: src/builder.o src/common.o src/modules/ADTHash.o src/modules/ADTMap.o src/modules/ADTVector.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o builder src/builder.o src/common.o src/modules/ADTHash.o src/modules/ADTMap.o src/modules/ADTVector.o

src/root.o: src/root.c
	$(CC) $(CFLAGS) -c src/root.c -o src/root.o

src/splitter.o: src/splitter.c
	$(CC) $(CFLAGS) -c src/splitter.c -o src/splitter.o

src/builder.o: src/builder.c
	$(CC) $(CFLAGS) -c src/builder.c -o src/builder.o

src/common.o: src/common.c
	$(CC) $(CFLAGS) -c src/common.c -o src/common.o

src/modules/ADTHash.o: src/modules/ADTHash.c
	$(CC) $(CFLAGS) -c src/modules/ADTHash.c -o src/modules/ADTHash.o

src/modules/ADTSet.o: src/modules/ADTSet.c
	$(CC) $(CFLAGS) -c src/modules/ADTSet.c -o src/modules/ADTSet.o

src/modules/ADTMap.o: src/modules/ADTMap.c
	$(CC) $(CFLAGS) -c src/modules/ADTMap.c -o src/modules/ADTMap.o

src/modules/ADTVector.o: src/modules/ADTVector.c
	$(CC) $(CFLAGS) -c src/modules/ADTVector.c -o src/modules/ADTVector.o

clean:
	rm -f root splitter builder src/*.o src/modules/*.o