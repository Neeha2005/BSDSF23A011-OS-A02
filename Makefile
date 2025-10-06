# ---------------------------------------
# Makefile for ls-v1.0.0 project
#   Compiles the starter ls-v1.0.0 program into the bin directory.
# ---------------------------------------

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -g

# Directories
SRC_DIR = src
BIN_DIR = bin

# Target name and paths
TARGET = $(BIN_DIR)/my_ls
SRC = $(SRC_DIR)/lsv1.0.0.c

# Default build rule
$(TARGET): $(SRC)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)
	@echo "✅ Build successful! Executable created: $(TARGET)"

# Run the program
run: $(TARGET)
	@echo "🚀 Running program..."
	./$(TARGET)

# Clean build files
clean:
	rm -f $(BIN_DIR)/*
	@echo "🧹 Cleaned build files."

