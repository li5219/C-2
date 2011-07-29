SRC	= malloc.h malloc.c MyTest.c

OBJ	= malloc.o MyTest.o

BIN	= TimeTest MemTest tstalgorithms

CFLAGS	= -O -Wall

XFLAGS	=

#CC	= gcc -ansi -pedantic -Wall -g -pipe -O -pg
CC	= gcc -O2


all: $(BIN)
	@echo "All Done"


# Dummy
TimeTest: TimeTest512St1 TimeTest512St2 TimeTest512St3 TimeTest512St4 TimeTest512St5 TimeTest4096St1 TimeTest4096St2 TimeTest4096St3 TimeTest4096St4 TimeTest4096St5
	@echo "TimeTest Done."

# TimeTest 512 max block size
TimeTest512.o: TimeTest.c
	$(CC) -c $(CFLAGS) -DMAXBLOCKSIZE=512 -o $@ TimeTest.c

TimeTest512St1: TimeTest512.o malloc1.o
	$(CC) $(CFLAGS) -o $@ malloc1.o TimeTest512.o $(X)

TimeTest512St2: TimeTest512.o malloc2.o
	$(CC) $(CFLAGS) -o $@ malloc2.o TimeTest512.o $(X)

TimeTest512St3: TimeTest512.o malloc3.o
	$(CC) $(CFLAGS) -o $@ malloc3.o TimeTest512.o $(X)

TimeTest512St4: TimeTest512.o malloc4.o
	$(CC) $(CFLAGS) -o $@ malloc4.o TimeTest512.o $(X)

TimeTest512St5: TimeTest512.o
	$(CC) $(CFLAGS) -o $@ TimeTest512.o $(X)


# TimeTest 4096 max block size
TimeTest4096.o: TimeTest.c
	$(CC) -c $(CFLAGS) -DMAXBLOCKSIZE=4096 -o $@ TimeTest.c

TimeTest4096St1: TimeTest4096.o malloc1.o
	$(CC) $(CFLAGS) -o $@ malloc1.o TimeTest4096.o $(X)

TimeTest4096St2: TimeTest4096.o malloc2.o
	$(CC) $(CFLAGS) -o $@ malloc2.o TimeTest4096.o $(X)

TimeTest4096St3: TimeTest4096.o malloc3.o
	$(CC) $(CFLAGS) -o $@ malloc3.o TimeTest4096.o $(X)

TimeTest4096St4: TimeTest4096.o malloc4.o
	$(CC) $(CFLAGS) -o $@ malloc4.o TimeTest4096.o $(X)

TimeTest4096St5: TimeTest4096.o
	$(CC) $(CFLAGS) -o $@ TimeTest4096.o $(X)


# Dummy
MemTest: MemTest512St1 MemTest512St2 MemTest512St3 MemTest512St4 MemTest512St5 MemTest4096St1 MemTest4096St2 MemTest4096St3 MemTest4096St4 MemTest4096St5
	@echo "MemTest Done."

# MemTest 512 max block size
MemTest512.o: MemTest.c
	$(CC) -c $(CFLAGS) -DMAXBLOCKSIZE=512 -o $@ MemTest.c

MemTest512St1: MemTest512.o malloc1.o
	$(CC) $(CFLAGS) -o $@ malloc1.o MemTest512.o $(X)

MemTest512St2: MemTest512.o malloc2.o
	$(CC) $(CFLAGS) -o $@ malloc2.o MemTest512.o $(X)

MemTest512St3: MemTest512.o malloc3.o
	$(CC) $(CFLAGS) -o $@ malloc3.o MemTest512.o $(X)

MemTest512St4: MemTest512.o malloc4.o
	$(CC) $(CFLAGS) -o $@ malloc4.o MemTest512.o $(X)

MemTest512St5: MemTest512.o
	$(CC) $(CFLAGS) -o $@ MemTest512.o $(X)


# MemTest 4096 max block size
MemTest4096.o: MemTest.c
	$(CC) -c $(CFLAGS) -DMAXBLOCKSIZE=4096 -o $@ MemTest.c

MemTest4096St1: MemTest4096.o malloc1.o
	$(CC) $(CFLAGS) -o $@ malloc1.o MemTest4096.o $(X)

MemTest4096St2: MemTest4096.o malloc2.o
	$(CC) $(CFLAGS) -o $@ malloc2.o MemTest4096.o $(X)

MemTest4096St3: MemTest4096.o malloc3.o
	$(CC) $(CFLAGS) -o $@ malloc3.o MemTest4096.o $(X)

MemTest4096St4: MemTest4096.o malloc4.o
	$(CC) $(CFLAGS) -o $@ malloc4.o MemTest4096.o $(X)

MemTest4096St5: MemTest4096.o
	$(CC) $(CFLAGS) -o $@ MemTest4096.o $(X)

# Dummy
tstalgorithms: tstalgorithmsSt1 tstalgorithmsSt2 tstalgorithmsSt3 tstalgorithmsSt4 tstalgorithmsSt5
	@echo "tstalgorithms Done."

tstalgorithms.o: tstalgorithms.c
	$(CC) -c $(CFLAGS) -o $@ tstalgorithms.c

tstalgorithmsSt1: tstalgorithms.o malloc1.o
	$(CC) $(CFLAGS) -o $@ malloc1.o tstalgorithms.o $(X)

tstalgorithmsSt2: tstalgorithms.o malloc2.o
	$(CC) $(CFLAGS) -o $@ malloc2.o tstalgorithms.o $(X)

tstalgorithmsSt3: tstalgorithms.o malloc3.o
	$(CC) $(CFLAGS) -o $@ malloc3.o tstalgorithms.o $(X)

tstalgorithmsSt4: tstalgorithms.o malloc4.o
	$(CC) $(CFLAGS) -o $@ malloc4.o tstalgorithms.o $(X)

tstalgorithmsSt5: tstalgorithms.o
	$(CC) $(CFLAGS) -o $@ tstalgorithms.o $(X)


# Malloc compilation
malloc1.o: malloc.c
	$(CC) -c $(CFLAGS) -DSTRATEGY=1 -o $@ malloc.c

malloc2.o: malloc.c
	$(CC) -c $(CFLAGS) -DSTRATEGY=2 -o $@ malloc.c

malloc3.o: malloc.c
	$(CC) -c $(CFLAGS) -DSTRATEGY=3 -o $@ malloc.c

malloc4.o: malloc.c
	$(CC) -c $(CFLAGS) -DSTRATEGY=4 -o $@ malloc.c
