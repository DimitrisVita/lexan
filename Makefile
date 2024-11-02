CC = gcc
CFLAGS = -Iinclude

all: root splitter builder

root: src/root.c src/common.c
    $(CC) $(CFLAGS) -o root src/root.c src/common.c

splitter: src/splitter.c src/common.c
    $(CC) $(CFLAGS) -o splitter src/splitter.c src/common.c

builder: src/builder.c src/common.c
    $(CC) $(CFLAGS) -o builder src/builder.c src/common.c

clean:
    rm -f root splitter builder