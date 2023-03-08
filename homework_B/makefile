#Notes to self:
# $@ = all prerequisites
# $< = first dependency
# $^ = all the dependencies, including duplicates

CC= gcc

SOURCE = reader-writer.c
OBJECT = reader-writer.o 
EXECUTABLE = reader-writer.out


all: $(EXECUTABLE)

$(OBJECT): $(SOURCE)
	$(CC) -c $< -o $@


$(EXECUTABLE): $(OBJECT)
	$(CC) $^ -pthread -o $@

run:
	./$(EXECUTABLE)

clean:
	rm -f $(OBJECT) $(EXECUTABLE)
