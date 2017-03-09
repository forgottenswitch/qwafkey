CXXFLAGS = -Wall -O2 -fpermissive -Wno-write-strings
LDFLAGS = -static

all: lw.exe

OBJS = eh.o hk.o ka.o kl.o kn.o lm.o os.o parse.o

lw.exe: main.cpp $(OBJS)
	$(LINK.cc) -o $@ $< $(OBJS)

SUFFIXES = .cpp .o
.cpp.o:
	$(COMPILE.cc) -o $@ $<

.PHONY: clean
clean:
	@rm *.exe *.o 2>/dev/null || true
