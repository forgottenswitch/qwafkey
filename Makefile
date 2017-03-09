CXXFLAGS = -Wall -O2
LDFLAGS = -static

all: lw.exe

OBJS = hk.o kl.o os.o

lw.exe: main.cpp $(OBJS)
	$(LINK.cc) -o $@ $< $(OBJS)

SUFFIXES = .cpp .o
.cpp.o:
	$(COMPILE.cc) -o $@ $<

.PHONY: clean
clean:
	@rm *.exe *.o 2>/dev/null || true
