PROG = lw

CXXFLAGS = -Wall -O2 -fpermissive -Wno-write-strings
LDFLAGS = -static

CROSS = i686-w64-mingw32-
CXX = $(CROSS)g++
WINDRES = $(CROSS)windres

all: cui

.PHONY: con cui gui
con: $(PROG)-con.exe
cui: $(PROG)-cui.exe
gui: $(PROG)-gui.exe

OBJS = eh.o hk.o ka.o kl.o km.o kn.o lm.o main.o os.o parse.o resource.o ui.o

OBJS_con = $(addprefix obj/con/, $(OBJS))
OBJS_cui = $(addprefix obj/cui/, $(OBJS))
OBJS_gui = $(addprefix obj/gui/, $(OBJS))

$(PROG)-con.exe: $(OBJS_con)
	$(LINK.cc) -o $@ $+
$(PROG)-cui.exe: $(OBJS_cui)
	$(LINK.cc) -o $@ $+
$(PROG)-gui.exe: $(OBJS_gui)
	$(LINK.cc) -o $@ $+ -Wl,--subsystem,windows

MKDIR.outfile = @mkdir -p $(@D)

DEFINES_con = -DDEBUG -DNOGUI
DEFINES_cui = -DDEBUG
DEFINES_gui =

obj/con/%.o: %.cpp
	$(MKDIR.outfile)
	$(COMPILE.cc) -o $@ $< $(DEFINES_con)
obj/cui/%.o: %.cpp
	$(MKDIR.outfile)
	$(COMPILE.cc) -o $@ $< $(DEFINES_cui)
obj/gui/%.o: %.cpp
	$(MKDIR.outfile)
	$(COMPILE.cc) -o $@ $< $(DEFINES_gui)

COMPILE.rc = $(WINDRES) -o $@ $<

obj/con/%.o: %.rc
	$(MKDIR.outfile)
	$(COMPILE.rc)
obj/cui/%.o: %.rc
	$(MKDIR.outfile)
	$(COMPILE.rc)
obj/gui/%.o: %.rc
	$(MKDIR.outfile)
	$(COMPILE.rc)

.PHONY: clean
clean:
	@rm *.exe *.o 2>/dev/null || true
	@rm -r obj 2>/dev/null || true
