CXXFLAGS = -Wall -O2 -fpermissive -Wno-write-strings
LDFLAGS = -static

CROSS = i686-w64-mingw32-
CXX = $(CROSS)g++
WINDRES = $(CROSS)windres

all: lw.exe

OBJS = eh.o hk.o ka.o kl.o km.o kn.o lm.o os.o parse.o resource.o ui.o

lw.exe: main.cpp $(OBJS)
	$(LINK.cc) -o $@ $< $(OBJS)

SUFFIXES = .cpp .o
.cpp.o:
	$(COMPILE.cc) -o $@ $<

%.o: %.rc
	$(WINDRES) -o $@ $<

.PHONY: clean
clean:
	@rm *.exe *.o 2>/dev/null || true
