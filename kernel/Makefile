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

RES       = ../resources

OVMFDIR   = OVMF
OVMFCODE  = OVMF_CODE-pure-efi.fd
OVMFVARS  = OVMF_VARS-pure-efi.fd

FONT      = font
SYSFONT   = bizcat.psf

BOOTLDR   = ../boot/osload.efi

SRC       = src
DEP       = .dep
OBJ       = .obj
BIN       = .bin

LNK       = link.ld
EXE       = kernel.elf
SYM       = kernel.sym
MAP       = kernel.map
IMG       = disk.img
IMGV      = disk.vdi

ImageDataDisk  = data.img
ImageDataPart1 = dataP1.img
ImageDataPart2 = dataP2.img

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

EM        = qemu-system-x86_64

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

ifeq ($(DEBUG),true)
#-g
	DEBUGCP   = -ggdb3
	DEBUGEM   = -s -S
else
	DEBUGCP   =
	DEBUGEM   =
endif

NOWARNS   = -Wno-narrowing -Wno-write-strings -Wno-pointer-arith -Wno-int-to-pointer-cast -Wno-return-type

ASFLAGS   = -i ./src/
CCFLAGS   = -I ./src/ -ffreestanding -fno-exceptions -fshort-wchar -fno-stack-protector -fno-stack-check -mno-red-zone $(NOWARNS) $(DEBUGCP)
CPFLAGS   = -I ./src/ -ffreestanding -fno-exceptions -fshort-wchar -fno-stack-protector -fno-stack-check -mno-red-zone $(NOWARNS) $(DEBUGCP)
LDFLAGS   = -T $(LNK) -static -Bsymbolic -nostdlib --cref -Map=$(BIN)/$(MAP)

DBFLAGS   =  -ex "set pagination off" -ex "target remote localhost:1234" -ex "symbol-file .bin/kernel.elf" -ex "break BspMain" -ex "continue"
# -ex "break SmpStartAps" -ex "break SmpApTrampoline" -ex "break SmpApMain"

ifeq ($(USEKVM),true)
	KVM = -accel kvm
else
	KVM =
endif

PANICEM   =
# Debug: -d cpu_reset -no-shutdown -D qemu.log
# Full: -d int,cpu_reset -no-reboot -no-shutdown -D qemu.log 

EMFLAGS   = -cpu qemu64 $(KVM) -M q35 -m 2G -smp 4 -net none -k de $(DEBUGEM) $(PANICEM) \
            -drive file=$(BIN)/$(IMG),format=raw \
            -drive file=$(BIN)/$(ImageDataDisk),format=raw \
            -drive if=pflash,format=raw,unit=0,file="$(RES)/$(OVMFDIR)/$(OVMFCODE)",readonly=on \
            -drive if=pflash,format=raw,unit=1,file="$(RES)/$(OVMFDIR)/$(OVMFVARS)"

# -enable-kvm -accel kvm -cpu core2duo

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
## Data                                                                         ##
##################################################################################

.PHONY: prepdata whichdata filldata showdata

prepdata:
	@truncate -s 1M $(BIN)/$(ImageDataDisk)

	@truncate -s 16M $(BIN)/$(ImageDataPart1)
	@mkfs.vfat -F 32 $(BIN)/$(ImageDataPart1)
	@cat $(BIN)/$(ImageDataPart1) >> $(BIN)/$(ImageDataDisk)
	@$(RM) $(RMFLAGS) $(BIN)/$(ImageDataPart1)

	@truncate -s 32M $(BIN)/$(ImageDataPart2)
	@mkfs.vfat -F 32 $(BIN)/$(ImageDataPart2)
	@cat $(BIN)/$(ImageDataPart2) >> $(BIN)/$(ImageDataDisk)
	@$(RM) $(RMFLAGS) $(BIN)/$(ImageDataPart2)

	@truncate -s 50M $(BIN)/$(ImageDataDisk)

	@parted -m -s -a optimal "$(BIN)/$(ImageDataDisk)" mklabel gpt
	@parted -m -s -a optimal "$(BIN)/$(ImageDataDisk)" mkpart ESP fat32 1MiB 17MiB
	@parted -m -s -a optimal "$(BIN)/$(ImageDataDisk)" set 1 esp on
	@parted -m -s -a optimal "$(BIN)/$(ImageDataDisk)" mkpart system fat32 17MiB 49MiB

whichdata:
	@kpartx -a $(BIN)/$(ImageDataDisk)

	@ls /dev/mapper

	@kpartx -d $(BIN)/$(ImageDataDisk)

filldata:
	@kpartx -a $(BIN)/$(ImageDataDisk)

	@mount /dev/mapper/$(LOOP)p1 /mnt

	@touch /mnt/file1.txt
	@dd if=/dev/random of=/mnt/file1.txt bs=1040 count=1
	@mkdir -p /mnt/dir1
	@touch /mnt/dir1/file1.txt
	@mkdir -p /mnt/dir1/LongDirsAreAlsoPossible
	@touch /mnt/dir1/LongDirsAreAlsoPossible/LongFilenamesLookLikeThisExampleHere.txt
	@mkdir -p /mnt/programs
	@cp testfile.txt /mnt/programs/
	@cp ../applications/test/.bin/test.elf /mnt/programs/
	@mkdir -p /mnt/fonts
	@cp $(RES)/$(FONT)/$(SYSFONT) /mnt/fonts/system.psf

	@umount /mnt

	@mount /dev/mapper/$(LOOP)p2 /mnt

	@touch /mnt/file2.txt
	@dd if=/dev/random of=/mnt/file2.txt bs=1040 count=1
	@mkdir -p /mnt/dir2
	@touch /mnt/dir2/file2.txt
	@mkdir -p /mnt/dir2/LongDirsAreAlsoPossible
	@touch /mnt/dir2/LongDirsAreAlsoPossible/LongFilenamesLookLikeThisExampleHere.txt

	@umount /mnt

	@kpartx -d $(BIN)/$(ImageDataDisk)

showdata:
	@kpartx -a $(BIN)/$(ImageDataDisk)

	@mount /dev/mapper/$(LOOP)p1 /mnt

	-@ls /mnt
	-@ls /mnt/dir1

	@umount /mnt

	@mount /dev/mapper/$(LOOP)p2 /mnt

	-@ls /mnt
	-@ls /mnt/dir2

	@umount /mnt

	@kpartx -d $(BIN)/$(ImageDataDisk)

##################################################################################
## Image                                                                        ##
##################################################################################

.PHONY: cleanall prepare vbox

cleanall:
	@-$(RM) $(RMFLAGS) $(DEP)
	@-$(RM) $(RMFLAGS) $(OBJ)
	@-$(RM) $(RMFLAGS) $(BIN)

prepare:
	@-$(MK) $(MKFLAGS) $(DEP)
	@-$(MK) $(MKFLAGS) $(OBJ)
	@-$(MK) $(MKFLAGS) $(BIN)

	@-$(RM) $(RMFLAGS) $(BIN)/$(IMG)
	@dd if=/dev/zero of=$(BIN)/$(IMG) bs=512 count=93750
	@mformat -i $(BIN)/$(IMG) ::
	@mmd -i $(BIN)/$(IMG) ::/EFI
	@mmd -i $(BIN)/$(IMG) ::/EFI/BOOT
	@mcopy -i $(BIN)/$(IMG) $(BOOTLDR) ::/EFI/BOOT/BOOTX64.efi

vbox:
	@VBoxManage convertfromraw --format VDI $(BIN)/$(IMG) $(BIN)/$(IMGV)

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
	@echo "[COPYING  ] $(BIN)/$(EXE)"
	@mcopy -Do -i $(BIN)/$(IMG) $(BIN)/$(EXE) ::

##################################################################################
## Debug + Run                                                                  ##
##################################################################################

.PHONY: run dbg brun

run:
	@$(EM) $(EMFLAGS)

dbg:
	@$(DB) @$(DBFLAGS)

brun: build run
