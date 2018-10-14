.PHONY: all
all: pl0d

pl0d: codegen.o compile.o getSource.o main.o table.o
	gcc -o pl0d codegen.o compile.o getSource.o main.o table.o
codegen.o: codegen.c
	gcc -c codegen.c
compile.o: compile.c
	gcc -c compile.c
getSource.o: getSource.c
	gcc -c getSource.c
main.o: main.c
	gcc -c main.c
table.o: table.c
	gcc -c table.c
clean:
	rm -f codegen.o compile.o getSource.o main.o table.o
