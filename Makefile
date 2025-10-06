# ==========================
#  Makefile for Custom LS
#  Versions: v1.1.0, v1.2.0, v1.3.0, v1.4.0
# ==========================

CC = gcc
CFLAGS = -Wall -g
SRC_DIR = src
BIN_DIR = bin

# Ensure bin exists
$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

# Build v1.1.0 (Long listing)
v1.1.0: $(BIN_DIR)
	@echo "🔨 Building version v1.1.0 (Long Listing)..."
	$(CC) $(CFLAGS) $(SRC_DIR)/lsv1.1.0.c -o $(BIN_DIR)/lsv1.1.0
	@echo "✅ Build complete: $(BIN_DIR)/lsv1.1.0"

# Build v1.2.0 (Column display)
v1.2.0: $(BIN_DIR)
	@echo "🔨 Building version v1.2.0 (Column Display)..."
	$(CC) $(CFLAGS) $(SRC_DIR)/lsv1.2.0.c -o $(BIN_DIR)/lsv1.2.0
	@echo "✅ Build complete: $(BIN_DIR)/lsv1.2.0"

# Build v1.3.0 (Horizontal display / -x)
v1.3.0: $(BIN_DIR)
	@echo "🔨 Building version v1.3.0 (Horizontal Display / -x)..."
	$(CC) $(CFLAGS) $(SRC_DIR)/lsv1.3.0.c -o $(BIN_DIR)/lsv1.3.0
	@echo "✅ Build complete: $(BIN_DIR)/lsv1.3.0"

# Build v1.4.0 (Alphabetical Sort)
v1.4.0: $(BIN_DIR)
	@echo "🔨 Building version v1.4.0 (Alphabetical Sort)..."
	$(CC) $(CFLAGS) $(SRC_DIR)/lsv1.4.0.c -o $(BIN_DIR)/lsv1.4.0
	@echo "✅ Build complete: $(BIN_DIR)/lsv1.4.0"

# Run targets
run-v1.1.0: v1.1.0
	@echo "🚀 Running v1.1.0 (Long Listing)..."
	./$(BIN_DIR)/lsv1.1.0 .

run-v1.2.0: v1.2.0
	@echo "🚀 Running v1.2.0 (Column Display)..."
	./$(BIN_DIR)/lsv1.2.0 .

run-v1.3.0: v1.3.0
	@echo "🚀 Running v1.3.0 (Horizontal Display / -x)..."
	./$(BIN_DIR)/lsv1.3.0 .

run-v1.4.0: v1.4.0
	@echo "🚀 Running v1.4.0 (Alphabetical Sort)..."
	./$(BIN_DIR)/lsv1.4.0 .

# Build all
build-all: v1.1.0 v1.2.0 v1.3.0 v1.4.0
	@echo "🎯 All versions built"

# Clean
clean:
	@echo "🧹 Cleaning binaries..."
	rm -f $(BIN_DIR)/lsv1.1.0 $(BIN_DIR)/lsv1.2.0 $(BIN_DIR)/lsv1.3.0 $(BIN_DIR)/lsv1.4.0
	@echo "✅ Clean complete."

# Help
help:
	@echo ""
	@echo "Makefile commands:"
	@echo "  make v1.1.0     -> Build v1.1.0 (long listing)"
	@echo "  make v1.2.0     -> Build v1.2.0 (column display)"
	@echo "  make v1.3.0     -> Build v1.3.0 (horizontal display -x)"
	@echo "  make v1.4.0     -> Build v1.4.0 (alphabetical sort)"
	@echo "  make run-v1.4.0 -> Build+run v1.4.0"
	@echo "  make build-all  -> Build all versions"
	@echo "  make clean      -> Remove binaries"
	@echo ""
