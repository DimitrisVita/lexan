CC = gcc
CFLAGS = -Iinclude
LDFLAGS = -pthread
OBJDIR = obj

all: $(OBJDIR) lexan $(OBJDIR)/splitter $(OBJDIR)/builder

$(OBJDIR):
	mkdir -p $(OBJDIR)

lexan: $(OBJDIR)/lexan.o $(OBJDIR)/common.o $(OBJDIR)/ADTHash.o $(OBJDIR)/ADTSet.o $(OBJDIR)/ADTMap.o $(OBJDIR)/ADTVector.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o lexan $(OBJDIR)/lexan.o $(OBJDIR)/common.o $(OBJDIR)/ADTHash.o $(OBJDIR)/ADTSet.o $(OBJDIR)/ADTMap.o $(OBJDIR)/ADTVector.o

$(OBJDIR)/splitter: $(OBJDIR)/splitter.o $(OBJDIR)/common.o $(OBJDIR)/ADTSet.o $(OBJDIR)/ADTHash.o $(OBJDIR)/ADTMap.o $(OBJDIR)/ADTVector.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(OBJDIR)/splitter $(OBJDIR)/splitter.o $(OBJDIR)/common.o $(OBJDIR)/ADTSet.o $(OBJDIR)/ADTHash.o $(OBJDIR)/ADTMap.o $(OBJDIR)/ADTVector.o

$(OBJDIR)/builder: $(OBJDIR)/builder.o $(OBJDIR)/common.o $(OBJDIR)/ADTHash.o $(OBJDIR)/ADTMap.o $(OBJDIR)/ADTVector.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(OBJDIR)/builder $(OBJDIR)/builder.o $(OBJDIR)/common.o $(OBJDIR)/ADTHash.o $(OBJDIR)/ADTMap.o $(OBJDIR)/ADTVector.o

$(OBJDIR)/lexan.o: src/lexan.c
	$(CC) $(CFLAGS) -c src/lexan.c -o $(OBJDIR)/lexan.o

$(OBJDIR)/splitter.o: src/splitter.c
	$(CC) $(CFLAGS) -c src/splitter.c -o $(OBJDIR)/splitter.o

$(OBJDIR)/builder.o: src/builder.c
	$(CC) $(CFLAGS) -c src/builder.c -o $(OBJDIR)/builder.o

$(OBJDIR)/common.o: src/common.c
	$(CC) $(CFLAGS) -c src/common.c -o $(OBJDIR)/common.o

$(OBJDIR)/ADTHash.o: src/modules/ADTHash.c
	$(CC) $(CFLAGS) -c src/modules/ADTHash.c -o $(OBJDIR)/ADTHash.o

$(OBJDIR)/ADTSet.o: src/modules/ADTSet.c
	$(CC) $(CFLAGS) -c src/modules/ADTSet.c -o $(OBJDIR)/ADTSet.o

$(OBJDIR)/ADTMap.o: src/modules/ADTMap.c
	$(CC) $(CFLAGS) -c src/modules/ADTMap.c -o $(OBJDIR)/ADTMap.o

$(OBJDIR)/ADTVector.o: src/modules/ADTVector.c
	$(CC) $(CFLAGS) -c src/modules/ADTVector.c -o $(OBJDIR)/ADTVector.o

clean:
	rm -rf lexan $(OBJDIR)