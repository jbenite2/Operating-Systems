#Notes to self:
# $@ = all prerequisites
# $< = first dependency
# $^ = all the dependencies, including duplicates

CC= gcc
CFLAGS= -std=c99 -Wall
LIBS = -lX11 -lm

SOURCE = fractalthread.c
LIBRARY = gfx.c
EXECUTABLE = fractal

$(EXECUTABLE): $(SOURCE) $(LIBRARY)
	$(CC) $(SOURCE) $(LIBRARY) -o $(EXECUTABLE) $(LIBS)

run:
	./$(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE)


