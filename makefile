# path to mos6502 repo
MOS6502=mos6502

# get file extension for executables on this OS
ifeq ($(OS),Windows_NT)
	EXE_EXT=.exe
endif

CC=g++
CFLAGS=-c -Wall
LDFLAGS=-static -static-libgcc -static-libstdc++
SOURCES=trace.cpp $(MOS6502)/mos6502.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=trace$(EXE_EXT)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@ -I $(MOS6502)

clean:
	rm -f $(OBJECTS)
	rm -f $(EXECUTABLE)

