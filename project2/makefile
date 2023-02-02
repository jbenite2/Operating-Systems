#Notes to self:
# $@ = all prerequisites
# $< = first dependency
# $^ = all the dependencies, including duplicates

CC= gcc
CFLAGS= -std=c99 -Wall

SOURCE = myshell.c
OBJECT = myshell.o 
EXECUTABLE = mysh


all: $(EXECUTABLE)

$(OBJECT): $(SOURCE)
	$(CC) $(CFLAGS) -c $< -o $@

$(EXECUTABLE): $(OBJECT)
	$(CC) $^ -o $@

run:
	./$(EXECUTABLE)

clean:
	rm -f $(OBJECT) $(EXECUTABLE)
