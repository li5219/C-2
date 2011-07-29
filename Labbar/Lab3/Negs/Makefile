SRC	= malloc.h malloc.c tstalgorithms.c  tstcrash_complex.c tstcrash_simple.c \
	  tstextreme.c tstmalloc.c  tstmemory.c tstrealloc.c tstmerge.c

OBJ	= malloc.o tstalgorithms.o  tstcrash_complex.o tstcrash_simple.o\
	  tstextreme.o tstmalloc.o  tstmemory.o tstrealloc.o tstmerge.o

BIN	= t0 t1 t2 t3 t4 t5 t6 t7

CFLAGS	= -O -Wall -DSTRATEGY=2

XFLAGS	=

#CC	= gcc -ansi -pedantic -Wall -g -pipe -O -pg
CC	= gcc -O


all: $(BIN)
	exec ./RUN_TESTS

t0: tstmerge.o malloc.o $(X)
	$(CC) $(CFLAGS) -o  $@ tstmerge.o malloc.o $(X)

t1: tstalgorithms.o malloc.o $(X)
	$(CC) $(CFLAGS) -o  $@ tstalgorithms.o malloc.o $(X)

t2: tstextreme.o malloc.o $(X)
	$(CC) $(CFLAGS) -o $@  tstextreme.o malloc.o $(X)

t3: tstmalloc.o  malloc.o $(X)
	$(CC) $(CFLAGS) -o $@ tstmalloc.o  malloc.o $(X) 

t4: tstmemory.o malloc.o $(X)
	$(CC) $(CFLAGS) -o $@ tstmemory.o malloc.o $(X)

t5: tstrealloc.o malloc.o $(X)
	$(CC) $(CFLAGS) -o $@ tstrealloc.o malloc.o $(X)

t6: tstcrash_simple.o malloc.o $(X)
	$(CC) $(CFLAGS) -o  $@ tstcrash_simple.o malloc.o $(X)

t7: tstcrash_complex.o malloc.o $(X)
	$(CC) $(CFLAGS) -o $@  tstcrash_complex.o malloc.o $(X) 

clean:
	\rm -f $(BIN) $(OBJ) core

cleanall: clean
	\rm -f *~

