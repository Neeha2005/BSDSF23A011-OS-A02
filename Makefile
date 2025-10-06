# ---------------------------------------
# Makefile for ls-v1.0.0 project
# Author: <ZAneeha Afzal/Bsdsf23a011>
# Description:
#   Simple Makefile to compile and run the ls-v1.0.0 program.
# ---------------------------------------

# Compiler
CC = gcc

# Compiler flags
# -Wall : show all warnings
# -g    : include debugging info
CFLAGS = -Wall -g

# Target executable name
TARGET = my_ls

# Source file
SRC = src/lsv1.0.0.c

# Build the program
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# Run the program
run: $(TARGET)
	./$(TARGET)

# Clean build files
clean:
	rm -f $(TARGET)
