CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -O2
SRCDIR = src
INCDIR = include
TARGET = main

SRCS = $(SRCDIR)/graph.cpp $(SRCDIR)/solver.cpp $(SRCDIR)/main.cpp
OBJS = $(SRCS:.cpp=.o)
OUTPUT_DIR = bin

.PHONY: all debug clean

all: $(OUTPUT_DIR)/$(TARGET)

$(OUTPUT_DIR):
	mkdir -p $(OUTPUT_DIR)

$(OUTPUT_DIR)/$(TARGET): $(OBJS)
	$(CXX) -I$(INCDIR) $(CXXFLAGS) $^ -o $@

$(SRCDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) -I$(INCDIR) $(CXXFLAGS) -c $< -o $@

debug:
	gdb $(OUTPUT_DIR)/$(TARGET)

clean:
	rm -rf $(OUTPUT_DIR) $(SRCDIR)/*.o