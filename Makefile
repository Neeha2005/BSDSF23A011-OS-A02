# ==========================
#  Makefile for Custom LS
#  Versions: v1.1.0 & v1.2.0
# ==========================

# Compiler
CC = gcc

# Flags
CFLAGS = -Wall -g

# Directories
SRC_DIR = src
BIN_DIR = bin

# Ensure bin directory exists
$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

# ======== Build Targets ========

v1.1.0: $(BIN_DIR)
	@echo "🔨 Building version 1.1.0 (Long Listing)..."
	$(CC) $(CFLAGS) $(SRC_DIR)/lsv1.1.0.c -o $(BIN_DIR)/lsv1.1.0
	@echo "✅ Build complete: bin/lsv1.1.0"

v1.2.0: $(BIN_DIR)
	@echo "🔨 Building version 1.2.0 (Column Display)..."
	$(CC) $(CFLAGS) $(SRC_DIR)/lsv1.2.0.c -o $(BIN_DIR)/lsv1.2.0
	@echo "✅ Build complete: bin/lsv1.2.0"

# ======== Run Targets ========

run-v1.2.0: v1.2.0
	@echo "🚀 Running ls version 1.2.0..."
	./$(BIN_DIR)/lsv1.2.0 .

# ======== Clean ========

clean:
	@echo "🧹 Cleaning build files..."
	rm -f $(BIN_DIR)/lsv1.1.0 $(BIN_DIR)/lsv1.2.0
	@echo "✅ Clean complete."

# ======== Help ========

help:
	@echo ""
	@echo "Commands:"
	@echo "  make v1.1.0     → Build Long Listing version"
	@echo "  make v1.2.0     → Build Column Display version"
	@echo "  make run-v1.2.0 → Run Column Display version"
	@echo "  make clean      → Clean binaries"
	@echo ""
