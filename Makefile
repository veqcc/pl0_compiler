.PHONY: all

CC = gcc
CFLAGS := -Wall -Wextra

all: pl0d

pl0d: codegen.o compile.o getSource.o main.o table.o
	$(CC) $(CFLAGS) -o pl0d codegen.o compile.o getSource.o main.o table.o
codegen.o: codegen.c
	$(CC) $(CFLAGS) -c codegen.c
compile.o: compile.c
	$(CC) $(CFLAGS) -c compile.c
getSource.o: getSource.c
	$(CC) $(CFLAGS) -c getSource.c
main.o: main.c
	$(CC) $(CFLAGS) -c main.c
table.o: table.c
	$(CC) $(CFLAGS) -c table.c

clean:
	rm -f codegen.o compile.o getSource.o main.o table.o
