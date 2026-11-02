CXX ?= g++
CXXFLAGS ?= -std=c++17 -O3 -Wall -Wextra -fopenmp -pthread

BUILD_DIR ?= build
TARGET = $(BUILD_DIR)/benchmark

THREADS ?= 8
OPS ?= 100000
INSERT ?= 10
REMOVE ?= 10
CONTAINS ?= 80
RANGE ?= 10000
IMPL ?= fine_pth

SRCS = main.cpp \
	   nonConcSet.cpp \
	   coarsePthreads.cpp \
	   finePThreads.cpp \
	   setController.cpp \
	   tests.cpp

OBJS = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SRCS))

.PHONY: all clean run sweep test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

# Compiles .cpp files into build/*.o
# '| $(BUILD_DIR)' is an order-only prerequisite ensuring the directory exists
$(BUILD_DIR)/%.o: %.cpp set.h tests.h | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Automatically create the build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

test: $(TARGET)
	./$(TARGET) --test


run: $(TARGET)
	./$(TARGET) --threads $(THREADS) --ops $(OPS) \
		--insert $(INSERT) --remove $(REMOVE) --contains $(CONTAINS) \
		--range $(RANGE) --impl $(IMPL)

# Runs the full assignment matrix (C.1, C.2, C.3) across implementations
sweep: $(TARGET)
	@for impl in pthread_coarse pthread_fine omp_coarse omp_fine; do \
		for range in 100 1000 10000 100000; do \
			for threads in 1 2 4 8 16 32; do \
				./$(TARGET) --threads $$threads --ops $(OPS) --insert 5 --remove 5 --contains 90 --range $$range --impl $$impl; \
				./$(TARGET) --threads $$threads --ops $(OPS) --insert 25 --remove 25 --contains 50 --range $$range --impl $$impl; \
				./$(TARGET) --threads $$threads --ops $(OPS) --insert 45 --remove 45 --contains 10 --range $$range --impl $$impl; \
			done; \
		done; \
	done


clean:
	rm -rf $(BUILD_DIR)
