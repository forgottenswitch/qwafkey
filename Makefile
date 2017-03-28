PROG = lw

CXXFLAGS = -Wall -O2 -fpermissive -Wno-write-strings
LDFLAGS = -static

CROSS32 = i686-w64-mingw32-
CROSS64 = x86_64-w64-mingw32-

CXX = $(CROSS)g++
WINDRES = $(CROSS)windres

# Do not print Entering directory..
GNUMAKEFLAGS = --no-print-directory

all: usage

.PHONY: usage
usage:
	@echo "Usage: $(MAKE) FLAVOUR"
	@echo "Available FLAVOURs:"
	@echo "       32   - All of con32, cui32, gui32"
	@echo "       64   - All of con64, cui64, gui64"
	@echo "    con32   - Event printing to console, no GUI, 32 bit"
	@echo "    cui32   - Event printing to console, has GUI, 32 bit"
	@echo "    gui32   - GUI, 32 bit"
	@echo "    con64   - Event printing to console, no GUI, 64 bit"
	@echo "    cui64   - Event printing to console, has GUI, 64 bit"
	@echo "    gui64   - GUI, 64 bit"
	@echo ""
	@echo "To disable cross-compilation:"
	@echo " make CROSS32= <32 bit flavour>"
	@echo " make CROSS64= <64 bit flavour>"

.PHONY: 32 64
# Use sequential build for errors and warnings not to be interspersed
32:
	for flav in con32 cui32 gui32 ; do \
		$(MAKE) "$$flav" ; \
	done
64:
	for flav in con64 cui64 gui64 ; do \
		$(MAKE) "$$flav" ; \
	done

.PHONY: con32 cui32 gui32
con32:
	$(MAKE) $(PROG)-con32.exe CROSS=$(CROSS32)
cui32:
	$(MAKE) $(PROG)-cui32.exe CROSS=$(CROSS32)
gui32:
	$(MAKE) $(PROG)-gui32.exe CROSS=$(CROSS32)
.PHONY: con64 cui64 gui64
con64:
	$(MAKE) $(PROG)-con64.exe CROSS=$(CROSS64)
cui64:
	$(MAKE) $(PROG)-cui64.exe CROSS=$(CROSS64)
gui64:
	$(MAKE) $(PROG)-gui64.exe CROSS=$(CROSS64)

OBJS = eh.o hk.o ka.o kl.o km.o kn.o kr.o lm.o main.o os.o parse.o resource.o ui.o

LINK.cc.all_deps = $(LINK.cc) -o $@ $+
LINK_FLAGS_gui = -mwindows

$(PROG)-con32.exe: $(addprefix obj/con32/, $(OBJS))
	$(LINK.cc.all_deps)
$(PROG)-cui32.exe: $(addprefix obj/cui32/, $(OBJS))
	$(LINK.cc.all_deps)
$(PROG)-gui32.exe: $(addprefix obj/gui32/, $(OBJS))
	$(LINK.cc.all_deps) $(LINK_FLAGS_gui)

$(PROG)-con64.exe: $(addprefix obj/con64/, $(OBJS))
	$(LINK.cc.all_deps)
$(PROG)-cui64.exe: $(addprefix obj/cui64/, $(OBJS))
	$(LINK.cc.all_deps)
$(PROG)-gui64.exe: $(addprefix obj/gui64/, $(OBJS))
	$(LINK.cc.all_deps) $(LINK_FLAGS_gui)

MKDIR.outfile = @mkdir -p $(@D)

DEFINES_con = -DDEBUG -DNOGUI
DEFINES_cui = -DDEBUG
DEFINES_gui =

obj/con32/%.o: %.cpp
	$(MKDIR.outfile)
	$(COMPILE.cc) -o $@ $< $(DEFINES_con)
obj/cui32/%.o: %.cpp
	$(MKDIR.outfile)
	$(COMPILE.cc) -o $@ $< $(DEFINES_cui)
obj/gui32/%.o: %.cpp
	$(MKDIR.outfile)
	$(COMPILE.cc) -o $@ $< $(DEFINES_gui)

obj/con64/%.o: %.cpp
	$(MKDIR.outfile)
	$(COMPILE.cc) -o $@ $< $(DEFINES_con)
obj/cui64/%.o: %.cpp
	$(MKDIR.outfile)
	$(COMPILE.cc) -o $@ $< $(DEFINES_cui)
obj/gui64/%.o: %.cpp
	$(MKDIR.outfile)
	$(COMPILE.cc) -o $@ $< $(DEFINES_gui)

COMPILE.rc = $(WINDRES) -o $@ $<

obj/con32/%.o: %.rc
	$(MKDIR.outfile)
	$(COMPILE.rc)
obj/cui32/%.o: %.rc
	$(MKDIR.outfile)
	$(COMPILE.rc)
obj/gui32/%.o: %.rc
	$(MKDIR.outfile)
	$(COMPILE.rc)

obj/con64/%.o: %.rc
	$(MKDIR.outfile)
	$(COMPILE.rc)
obj/cui64/%.o: %.rc
	$(MKDIR.outfile)
	$(COMPILE.rc)
obj/gui64/%.o: %.rc
	$(MKDIR.outfile)
	$(COMPILE.rc)

.PHONY: clean
clean:
	@rm *.exe *.o 2>/dev/null || true
	@rm -r obj 2>/dev/null || true
