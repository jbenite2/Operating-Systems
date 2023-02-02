# Specify execution instructions
CC = gcc            #C Compiler 
CFLAGS = --std=c99 -Wall   # C Flags

all: file_exec tree_exec

#Create the object file using the program 
treecopy.o: treecopy.c
	$(CC) $(CFLAGS) -c $< -o $@
filecopy.o: filecopy.c
	$(CC) $(CFLAGS) -c $< -o $@

#Create executables using object files
file_exec: filecopy.o
	$(CC) $^ -o $@
tree_exec: treecopy.o
	$(CC) $^ -o $@

clean:
	rm -f treecopy.o tree_exec filecopy.o file_exec


