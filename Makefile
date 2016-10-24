PROJ = qwafkey

WGET ?= wget
RESCOMP ?= windres
CC32 ?= i686-w64-mingw32-gcc
CC64 ?= x86_64-w64-mingw32-gcc

CFLAGS += -Wall -Wextra -Wno-unused-parameter -Wstrict-prototypes
CFLAGS32 += -m32
CFLAGS64 += -mx32

LDFLAGS += -s
LDFLAGS_UI += -lcomctl32

CON32_EXE=$(PROJ)32-con.exe
CUI32_EXE=$(PROJ)32-cui.exe
GUI32_EXE=$(PROJ)32.exe
CON64_EXE=$(PROJ)64-con.exe
CUI64_EXE=$(PROJ)64-cui.exe
GUI64_EXE=$(PROJ)64.exe

CON32_OBJDIR=.obj/con32
CUI32_OBJDIR=.obj/cui32
GUI32_OBJDIR=.obj/gui32
CON64_OBJDIR=.obj/con64
CUI64_OBJDIR=.obj/cui64
GUI64_OBJDIR=.obj/gui64

ICO = $(addprefix icons/, off.ico on.ico)
RES_H = resource.h
RES_RC = resource.rc

HEADERS = \
	dk.h eh.h freadline.h hk.h ka.h ka_dk.h \
	keysymdef.h kl.h km.h kn.h kr.h lm.h parse.h \
	resource.h scancodes.h stdafx.h ui.h

.PHONY: all
# Do not build x64 by default, as gcc fails on dk.c
all: 32

.PHONY: 32
32:
	$(MAKE) con32
	$(MAKE) cui32
	$(MAKE) gui32

.PHONY: 64
64:
	$(MAKE) con64
	$(MAKE) cui64
	$(MAKE) gui64

.PHONY: clean
clean:
	@rm -r *.exe .obj/* 2>/dev/null || true

.PHONY: fetch
fetch:
	$(WGET) -O config/Compose https://cgit.freedesktop.org/xorg/lib/libX11/plain/nls/en_US.UTF-8/Compose.pre
	$(WGET) -O config/keysymdef.h https://cgit.freedesktop.org/xorg/proto/x11proto/plain/keysymdef.h

$(CON32_OBJDIR)/%.o: %.c $(HEADERS)
	@mkdir -p $(@D)
	$(CC32) $(CFLAGS32) $(CFLAGS) -c -DNOGUI -DDEBUG $< -o $@
$(CUI32_OBJDIR)/%.o: %.c $(HEADERS)
	@mkdir -p $(@D)
	$(CC32) $(CFLAGS32) $(CFLAGS) -c -DDEBUG $< -o $@
$(GUI32_OBJDIR)/%.o: %.c $(HEADERS)
	@mkdir -p $(@D)
	$(CC32) $(CFLAGS32) $(CFLAGS) -c $< -o $@
$(CON64_OBJDIR)/%.o: %.c $(HEADERS)
	@mkdir -p $(@D)
	$(CC64) $(CFLAGS64) $(CFLAGS) -c -DNOGUI -DDEBUG $< -o $@
$(CUI64_OBJDIR)/%.o: %.c $(HEADERS)
	@mkdir -p $(@D)
	$(CC64) $(CFLAGS64) $(CFLAGS) -c -DDEBUG $< -o $@
$(GUI64_OBJDIR)/%.o: %.c $(HEADERS)
	@mkdir -p $(@D)
	$(CC64) $(CFLAGS64) $(CFLAGS) -c $< -o $@

$(CON32_OBJDIR)/resource.o: $(ICO) $(RES_RC) $(RES_H)
	$(RESCOMP) -i $(RES_RC) -o $@
$(CUI32_OBJDIR)/resource.o: $(ICO) $(RES_RC) $(RES_H)
	$(RESCOMP) -i $(RES_RC) -o $@
$(GUI32_OBJDIR)/resource.o: $(ICO) $(RES_RC) $(RES_H)
	$(RESCOMP) -i $(RES_RC) -o $@
$(CON64_OBJDIR)/resource.o: $(ICO) $(RES_RC) $(RES_H)
	$(RESCOMP) -i $(RES_RC) -o $@
$(CUI64_OBJDIR)/resource.o: $(ICO) $(RES_RC) $(RES_H)
	$(RESCOMP) -i $(RES_RC) -o $@
$(GUI64_OBJDIR)/resource.o: $(ICO) $(RES_RC) $(RES_H)
	$(RESCOMP) -i $(RES_RC) -o $@

COMMON_OBJS := \
	dk.o eh.o freadline.o hk.o ka.o kl.o km.o \
	kn.o kr.o lm.o os.o parse.o str.o main.o

CON_OBJS := $(COMMON_OBJS)
GUI_OBJS := $(COMMON_OBJS) ui.o resource.o

CON32_OBJS := $(addprefix $(CON32_OBJDIR)/, $(CON_OBJS))
CUI32_OBJS := $(addprefix $(CUI32_OBJDIR)/, $(GUI_OBJS))
GUI32_OBJS := $(addprefix $(GUI32_OBJDIR)/, $(GUI_OBJS))
CON64_OBJS := $(addprefix $(CON64_OBJDIR)/, $(CON_OBJS))
CUI64_OBJS := $(addprefix $(CUI64_OBJDIR)/, $(GUI_OBJS))
GUI64_OBJS := $(addprefix $(GUI64_OBJDIR)/, $(GUI_OBJS))

.PHONY: con32 cui32 gui32
con32: $(CON32_EXE)
cui32: $(CUI32_EXE)
gui32: $(GUI32_EXE)
.PHONY: con32_objs cui32_objs gui32_objs
con32_objs: $(CON32_OBJS)
cui32_objs: $(CUI32_OBJS)
gui32_objs: $(GUI32_OBJS)
$(CON32_EXE): $(HEADERS) $(CON32_OBJS)
	@echo "Building Console for x86..."
	@$(MAKE) con32_objs
	@echo "Linking ConsoleUI for x86..."
	$(CC32) $(LDFLAGS) $(CON32_OBJS) -o $@
$(CUI32_EXE): $(HEADERS) $(CUI32_OBJS)
	@echo "Building Console-with-GUI for x86..."
	@$(MAKE) cui32_objs
	@echo "Linking Console-with-GUI for x86..."
	$(CC32) $(LDFLAGS) $(CUI32_OBJS) -o $@ $(LDFLAGS_UI)
$(GUI32_EXE): $(HEADERS) $(GUI32_OBJS)
	@echo "Building GUI for x86..."
	@$(MAKE) gui32_objs
	@echo "Linking GUI for x86..."
	$(CC32) $(LDFLAGS) $(GUI32_OBJS) -o $@ $(LDFLAGS_UI) -mwindows

.PHONY: con64 cui64 gui64
con64: $(CON64_EXE)
cui64: $(CUI64_EXE)
gui64: $(GUI64_EXE)
.PHONY: con64_objs cui64_objs gui64_objs
con64_objs: $(CON64_OBJS)
cui64_objs: $(CUI64_OBJS)
gui64_objs: $(GUI64_OBJS)
$(CON64_EXE): $(HEADERS) $(CON64_OBJS)
	@echo "Building Console for x86_64..."
	@$(MAKE) con64_objs
	@echo "Linking Console for x86_64..."
	$(CC64) $(LDFLAGS) $(CON64_OBJS) -o $@
$(CUI64_EXE): $(HEADERS) $(CUI64_OBJS)
	@echo "Building Console-with-GUI for x86_64..."
	@$(MAKE) cui64_objs
	@echo "Linking Console-with-GUI for x86_64..."
	$(CC64) $(LDFLAGS) $(CUI64_OBJS) -o $@ $(LDFLAGS_UI)
$(GUI64_EXE): $(HEADERS) $(GUI64_OBJS)
	@echo "Building GUI for x86_64..."
	@$(MAKE) gui64_objs
	@echo "Linking GUI for x86_64..."
	$(CC64) $(LDFLAGS) $(GUI64_OBJS) -o $@ $(LDFLAGS_UI) -mwindows

