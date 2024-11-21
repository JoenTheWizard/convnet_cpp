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

#stb_image.h file
STB_IMAGE = $(SRCDIR)/stb_image.h

#Remote URL for stb_image.h
STB_IMAGE_URL = https://raw.githubusercontent.com/nothings/stb/refs/heads/master/stb_image.h

#Default target
all: $(STB_IMAGE) $(TARGET)

#Rule to build the executable
$(TARGET): $(OBJECTS) | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

#Rule to download stb_image.h if it doesn't exist
$(STB_IMAGE):
	@echo "Downloading stb_image.h..."
	curl -L $(STB_IMAGE_URL) -o $@

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