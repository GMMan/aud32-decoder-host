Sonix Audio32 Decoder Emulator Host
===================================

This program is designed to be inserted into SNC7320's core 1 ROM image to
act as a monitor for receiving in-memory requests to decode Audio32 frames.

Building
--------
First, source a copy of `audio32codec_decoder.h`. This is proprietary code
shipped as a part of SNC7320's SDK, so is not included in this repo.

The linker script has been set up for ROM with checksum `0x32ed0c2b`. Please
adjust `.text` start and function addresses if you have a different version
of the ROM. The `.text` start should be somewhere after the main data of the
ROM.

To build, use `make`. Supply your toolchain's path with the `BINPATH` env var
if necessary. The output will be `convert.bin`, which you can paste at the
address in the linker script.

A few additional modifications are necessary for the code to run properly. For
ROM with checksum `0x32ed0c2b`:

- Patch `main` pointer: at `0x130`, write the address of the `main` function
  of this code (it should be at the start of the `.text` section).
- Patch `aud32dec_initialise` to modify bitrate check (remove scaling by
  frequency):
  ```
          000019d0 00 bf           nop
          000019d2 00 bf           nop
          000019d4 30 46           mov        r0,r6
          000019d6 00 bf           nop
          000019d8 00 bf           nop
  ```

Execution
---------
The patched image can be run on QEMU using the `lm3s6965evb` Arm machine.
Suggested launch command:

```
qemu-system-arm -cpu cortex-m3 -machine lm3s6965evb -nographic -kernel core1_rom_decomp.bin -s
```

This will launch the machine with the ROM image headlessly and with `gdbserver`
active on default port `1234`.
