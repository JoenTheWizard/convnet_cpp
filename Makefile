#Compiler settings
CXX      = g++
CXXFLAGS = -static-libgcc -static-libstdc++ -O3 -Wall

#Directories
SRCDIR   = src
BUILDDIR = build

#Target executable (Windows)
TARGET = $(BUILDDIR)/conv.exe

#Source files
SOURCES = $(SRCDIR)/main.cpp $(SRCDIR)/matrix.cpp
#Add more source files here:
#SOURCES += $(SRCDIR)/.cpp

#Object files
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(BUILDDIR)/%.o)

#Default target
all: $(TARGET)

#Rule to build the executable
$(TARGET): $(OBJECTS) | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

#Rule to compile source files
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

#Rule to create the build directory
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

#Clean rule
clean:
	rm -rf $(BUILDDIR)

.PHONY: all clean