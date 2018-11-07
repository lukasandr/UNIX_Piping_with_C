CC=gcc
CFLAGS=-I.
DEPS=piping.h
OBJ=main.o piping.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

app: main.o piping.o
	$(CC) -o $@ $^ $(CFLAGS)