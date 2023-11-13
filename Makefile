##################################################################################
## Paths                                                                        ##
##################################################################################

BAKDIR    = ../Backups
PUBDIR    = ../Publish

ZIPFILTER = -xr!.git -xr!.vscode -xr!backup -xr!publish -xr!.dep -xr!.obj -xr!.bin -xr!*.a -xr!*.o -xr!*.so -xr!*.efi -xr!*.elf -xr!*.sym -xr!*.map

##################################################################################
## General                                                                      ##
##################################################################################

.PHONY: prepare backup publish loc

prepare:
	@apt install build-essential nasm gdb mtools kpartx parted qemu-system-x86 cloc p7zip-full

backup:
	@mkdir -p $(BAKDIR)
	@bash -c "7z a $(BAKDIR)/StarOS_Backup_$$(date "+%Y-%m-%d_%H-%M-%S").zip * $(ZIPFILTER)"

publish:
	@mkdir -p $(PUBDIR)
	@rm -rf $(PUBDIR)/*
	@bash -c "7z a $(PUBDIR)/publish.zip * $(ZIPFILTER)"
	@cd $(PUBDIR) && 7z x publish.zip
	@rm -rf $(PUBDIR)/publish.zip

loc:
	@cloc --exclude-dir=resources .
