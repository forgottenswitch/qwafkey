CFLAGS = -Wall -O2
LDFLAGS = -static

WINDRES = windres

SYSLIBS = -lpowrprof

all: l5.exe

l5.exe: main.c layout.h resource.o
	$(LINK.c) -o $@ main.c resource.o $(SYSLIBS)

resource.o: resource.rc
	$(WINDRES) -o $@ $<

.PHONY: clean
clean:
	@rm *.exe *.o 2>/dev/null || true
