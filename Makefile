CFLAGS = -Wall
LDFLAGS = -static

all: l5.exe

l5.exe: main.c layout.h
	$(LINK.c) -o $@ main.c

.PHONY: clean
clean:
	@rm *.exe 2>/dev/null || true
