PROJ = qwafkey

CFLAGS += -Wall -Wextra -Wno-unused-parameter -Wstrict-prototypes
CFLAGS32 += -m32
CFLAGS64 += -mx32

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
RES_RC = resource.rc
HEADERS = stdafx.h resource.h

CC32 ?=i686-w64-mingw32-gcc
CC64 ?=x86_64-w64-mingw32-gcc
RESCOMP ?= windres

.PHONY: all
# Do not build x64 by default, as gcc fails on dk.c
all: 32

.PHONY: 32
32: con32 cui32 gui32

.PHONY: 64
64: con64 cui64 gui64

.PHONY: clean
clean:
	@rm -r *.exe .obj/* 2>/dev/null || true

$(CON32_OBJDIR)/%.o: %.c
	$(CC32) $(CFLAGS32) $(CFLAGS) -c -DNOGUI -DDEBUG $< -o $@
$(CUI32_OBJDIR)/%.o: %.c
	$(CC32) $(CFLAGS32) $(CFLAGS) -c -DDEBUG $< -o $@
$(GUI32_OBJDIR)/%.o: %.c
	$(CC32) $(CFLAGS32) $(CFLAGS) -c $< -o $@
$(CON64_OBJDIR)/%.o: %.c
	$(CC64) $(CFLAGS64) $(CFLAGS) -c -DNOGUI -DDEBUG $< -o $@
$(CUI64_OBJDIR)/%.o: %.c
	$(CC64) $(CFLAGS64) $(CFLAGS) -c -DDEBUG $< -o $@
$(GUI64_OBJDIR)/%.o: %.c
	$(CC64) $(CFLAGS64) $(CFLAGS) -c $< -o $@

$(CON32_OBJDIR)/resource.o: $(RES_RC) $(ICO)
	$(RESCOMP) -i $(RES_RC) -o $@
$(CUI32_OBJDIR)/resource.o: $(RES_RC) $(ICO)
	$(RESCOMP) -i $(RES_RC) -o $@
$(GUI32_OBJDIR)/resource.o: $(RES_RC) $(ICO)
	$(RESCOMP) -i $(RES_RC) -o $@
$(CON64_OBJDIR)/resource.o: $(RES_RC) $(ICO)
	$(RESCOMP) -i $(RES_RC) -o $@
$(CUI64_OBJDIR)/resource.o: $(RES) $(ICO)
	$(RESCOMP) -i $(RES_RC) -o $@
$(GUI64_OBJDIR)/resource.o: $(RES) $(ICO)
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

.PHONY: mkdir_con32 mkdir_cui32 mkdir_gui32
mkdir_con32:
	@echo
	@echo "Building Console for x86..."
	@mkdir -p $(CON32_OBJDIR)
mkdir_cui32:
	@echo
	@echo "Building Console-with-GUI for x86..."
	@mkdir -p $(CUI32_OBJDIR)
mkdir_gui32:
	@echo
	@echo "Building GUI for x86..."
	@mkdir -p $(GUI32_OBJDIR)
.PHONY: mkdir_con64 mkdir_cui64 mkdir_gui64
mkdir_con64:
	@echo
	@echo "Building Console for x86_64..."
	@mkdir -p $(CON64_OBJDIR)
mkdir_cui64:
	@echo
	@echo "Building Console-with-GUI for x86_64..."
	@mkdir -p $(CUI64_OBJDIR)
mkdir_gui64:
	@echo
	@echo "Building GUI for x86_64..."
	@mkdir -p $(GUI64_OBJDIR)

.PHONY: con32 cui32 gui32
con32: $(CON32_EXE)
cui32: $(CUI32_EXE)
gui32: $(GUI32_EXE)
$(CON32_EXE): mkdir_con32 $(HEADERS) $(CON32_OBJS)
	@echo "Linking ConsoleUI for x86..."
	$(CC32) $(LDFLAGS) $(CON32_OBJS) -s -o $@
$(CUI32_EXE): mkdir_cui32 $(HEADERS) $(CUI32_OBJS)
	@echo "Linking Console-with-GUI for x86..."
	$(CC32) $(LDFLAGS) $(CUI32_OBJS) -s -o $@ $(LDFLAGS_UI)
$(GUI32_EXE): mkdir_gui32 $(HEADERS) $(GUI32_OBJS)
	@echo "Linking GUI for x86..."
	$(CC32) $(LDFLAGS) $(GUI32_OBJS) -s -o $@ $(LDFLAGS_UI) -mwindows

.PHONY: con64 cui64 gui64
con64: $(CON64_EXE)
cui64: $(CUI64_EXE)
gui64: $(GUI64_EXE)
$(CON64_EXE): mkdir_con64 $(HEADERS) $(CON64_OBJS)
	@echo "Linking Console for x86_64..."
	$(CC64) $(LDFLAGS) $(CON64_OBJS) -s -o $@
$(CUI64_EXE): mkdir_gui64 $(HEADERS) $(CUI64_OBJS)
	@echo "Linking Console-with-GUI for x86_64..."
	$(CC64) $(LDFLAGS) $(CUI64_OBJS) -s -o $@ $(LDFLAGS_UI)
$(GUI64_EXE): mkdir_gui64 $(HEADERS) $(GUI64_OBJS)
	@echo "Linking GUI for x86_64..."
	$(CC64) $(LDFLAGS) $(GUI64_OBJS) -s -o $@ $(LDFLAGS_UI) -mwindows

