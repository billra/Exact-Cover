# generic project makefile
# Bill Ola Rasmussen

CC=g++
CC_FLAGS=-Wall -std=gnu++0x -pedantic -Wextra

# file names
EXEC = solve
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

# main target
$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXEC)

# obtain object files
%.o: %.cpp
	$(CC) -c $(CC_FLAGS) $< -o $@

# header file dependencies
dlx.o main.o: dlx.h
dlx.o main.o: Solver.h # indirectly from dlx.h

# remove generated files
clean:
	rm -f $(EXEC) $(OBJECTS)
