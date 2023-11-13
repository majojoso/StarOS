##################################################################################
## Project                                                                      ##
##################################################################################

NAME      = StarOS Operating System
VERSION   = 1.0.0.0000

##################################################################################
## Includes                                                                     ##
##################################################################################

include globals.mk

##################################################################################
## Paths                                                                        ##
##################################################################################

NAM       = test

SRC       = src
DEP       = .dep
OBJ       = .obj
BIN       = .bin

LNK       = link.ld
EXE       = ${NAM}.elf
SYM       = ${NAM}.sym
MAP       = ${NAM}.map

##################################################################################
## Tools                                                                        ##
##################################################################################

SU        = sudo
RM        = rm
MK        = mkdir
CP        = cp
MV        = mv
RS        = rsync
TA        = tar
SC        = scp
SS        = ssh
PY        = python3
SY        = sync

AS        = nasm
CC        = gcc
CP        = g++
LD        = ld

DB        = gdb

##################################################################################
## Options                                                                      ##
##################################################################################

SUFLAGS   =
RMFLAGS   = -rf
MKFLAGS   = --parents
CPFLAGS   = -r
MVFLAGS   =
RSFLAGS   = -av --exclude=".*"
CFFLAGS   = --exclude-vcs -Jcf
XFFLAGS   = --exclude-vcs --warning=no-timestamp -xf
SCFLAGS   =
SSFLAGS   =
PYFLAGS   =
SYFLAGS   =

ASFLAGS   = -i ../../kernel/
CCFLAGS   = -I ./src/ -ffreestanding -mno-red-zone -fshort-wchar -fno-exceptions
CPFLAGS   = -I ./src/ -ffreestanding -mno-red-zone -fshort-wchar -fno-exceptions
LDFLAGS   = -T $(LNK) -static -Bsymbolic -nostdlib --cref -Map=$(BIN)/$(MAP)

##################################################################################
## Files                                                                        ##
##################################################################################

rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

DIRS      = $(wildcard $(SRC)/*)

SRCASM    = $(call rwildcard,$(SRC),*.asm)
SRCCPP    = $(call rwildcard,$(SRC),*.cpp)

OBJASM    = $(patsubst $(SRC)/%.asm, $(OBJ)/%_asm.o, $(SRCASM))
OBJCPP    = $(patsubst $(SRC)/%.cpp, $(OBJ)/%.o,     $(SRCCPP))

OBJALL    = $(OBJASM) $(OBJCPP)

##################################################################################
## Targets                                                                      ##
##################################################################################

.DEFAULT_GOAL := empty

.PHONY: empty default all

empty:
	@echo "No target selected."

default: empty
all: empty

##################################################################################
## Image                                                                        ##
##################################################################################

.PHONY: cleanall prepare

cleanall:
	@-$(RM) $(RMFLAGS) $(DEP)
	@-$(RM) $(RMFLAGS) $(OBJ)
	@-$(RM) $(RMFLAGS) $(BIN)

prepare:
	@-$(MK) $(MKFLAGS) $(DEP)
	@-$(MK) $(MKFLAGS) $(OBJ)
	@-$(MK) $(MKFLAGS) $(BIN)

##################################################################################
## Build                                                                        ##
##################################################################################

.PHONY: clean link build

clean:
	@-$(RM) $(RMFLAGS) $(DEP)/*.d
	@-$(RM) $(RMFLAGS) $(OBJ)/*
	@-$(RM) $(RMFLAGS) $(BIN)/$(EXE)
	@-$(RM) $(RMFLAGS) $(BIN)/$(SYM)
	@-$(RM) $(RMFLAGS) $(BIN)/$(MAP)

-include $(DEP)/*.d

$(OBJ)/%.o: $(SRC)/%.cpp
	@echo "[COMPILING] $^"
	@$(MK) $(MKFLAGS) $(@D)
	@$(CC) $(CPFLAGS) -o $@ -c -MMD -MF $(DEP)/$(notdir $<).d $<

$(OBJ)/%_asm.o: $(SRC)/%.asm
	@echo "[COMPILING] $^"
	@$(MK) $(MKFLAGS) $(@D)
	@$(AS) $(ASFLAGS) $^ -f elf64 -o $@

link:
	@echo "[LINKING  ] $(BIN)/$(EXE)"
	@$(LD) $(LDFLAGS) -o $(BIN)/$(EXE) $(OBJALL)
#	objcopy --only-keep-debug $(BIN)/$(EXE) $(BIN)/$(SYM)

build: $(OBJALL) link

##################################################################################
## General                                                                      ##
##################################################################################

.PHONY: backup loc

backup:
	@7z a backup.zip src link.ld globals.mk Makefile

loc:
	@cloc src
