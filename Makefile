EE_BIN = pops-smb-config.elf
EE_BIN_STRIPPED = pops-smb-config-stripped.elf
EE_BIN_PACKED = pops-smb-config-packed.elf
EE_OBJS = main.o pad.o modules.o display.o math.o fileio.o
EE_LIBS = -ldebug -lc -lpad

all: $(EE_BIN_PACKED)

clean:
	rm -rf *.o *.elf

$(EE_BIN_STRIPPED): $(EE_BIN)
	$(EE_STRIP) --strip-all -o $@ $<

$(EE_BIN_PACKED): $(EE_BIN_STRIPPED)
	ps2-packer -v $< $@

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
