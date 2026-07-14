# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra

# Executable name
TARGET = steganography

# Source files
SRC = main.c encode.c decode.c

# Object files
OBJ = $(SRC:.c=.o)

# Default target
all: $(TARGET)

# Linking
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# Compiling source files
%.o: %.c
	$(CC) $(CFLAGS) -c $<

# Remove generated files
clean:
	rm -f $(OBJ) $(TARGET)

# Rebuild project
rebuild: clean all
