
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

RES  = ../resources/gnu-efi

PROG = osload

##################################################################################
## Tools                                                                        ##
##################################################################################

CC        = gcc
CP        = g++
LD        = ld
OB        = objcopy

##################################################################################
## Options                                                                      ##
##################################################################################

CCFLAGS   = \
	-Wno-error=pragmas -mno-red-zone -mno-avx -fpic -g -O2 -Wall -Wextra -Werror -fshort-wchar -fno-strict-aliasing -ffreestanding -fno-stack-protector -fno-stack-check -fno-stack-check -fno-merge-all-constants -Wno-error=unused-parameter -Wno-error=unused-variable \
	-I. -I$(RES)/inc -I$(RES)/inc/x86_64 -I$(RES)/inc/protocol -I/usr/src/sys/build/include \
	-DCONFIG_x86_64 -DGNU_EFI_USE_MS_ABI -maccumulate-outgoing-args --std=c11 -D__KERNEL__
CPFLAGS   = \
	-Wno-error=pragmas -mno-red-zone -mno-avx -fpic -g -O2 -Wall -Wextra -Werror -fshort-wchar -fno-strict-aliasing -ffreestanding -fno-stack-protector -fno-stack-check -fno-stack-check -fno-merge-all-constants -Wno-error=unused-parameter -Wno-error=unused-variable \
	-I. -I$(RES)/inc -I$(RES)/inc/x86_64 -I$(RES)/inc/protocol -I/usr/src/sys/build/include \
	-DCONFIG_x86_64 -DGNU_EFI_USE_MS_ABI -maccumulate-outgoing-args -D__KERNEL__
LDFLAGS   = \
	-nostdlib --warn-common --no-undefined --fatal-warnings --build-id=sha1 -shared -Bsymbolic \
	-T $(RES)/gnuefi/elf_x86_64_efi.lds \
	-L$(RES)/x86_64/lib -L$(RES)/x86_64/gnuefi $(RES)/x86_64/gnuefi/crt0-efi-x86_64.o \
	-lefi -lgnuefi /usr/lib/gcc/x86_64-linux-gnu/11/libgcc.a
OBFLAGS   = \
	-j .text -j .sdata -j .data -j .dynamic -j .dynsym -j .rel -j .rela -j .rel.* -j .rela.* -j .rel* -j .rela* -j .reloc \
	--target efi-app-x86_64

##################################################################################
## Build                                                                        ##
##################################################################################

.PHONY: clean build

clean:

	@-rm $(PROG).so
	@-rm $(PROG).o
	@-rm $(PROG).efi

build:

	$(CC) $(CCFLAGS) -c $(PROG).c -o $(PROG).o
#	$(CP) $(CPFLAGS) -c $(PROG).cpp -o $(PROG).o
	$(LD) $(PROG).o -o $(PROG).so $(LDFLAGS)
	$(OB) $(OBFLAGS) $(PROG).so $(PROG).efi
