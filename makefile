# generic project makefile
# Bill Ola Rasmussen

CC=g++ 
CC_FLAGS=-Wall

# file names
EXEC = solve
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

# main target
$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXEC)

# obtain object files
%.o: %.cpp %.h
	$(CC) -c $(CC_FLAGS) $< -o $@

# remove generated files
clean:
	rm -f $(EXEC) $(OBJECTS)
