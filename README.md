# 6CROSS: Honeywell/Bull CP‑6 cross‑assembler suite

This is a Linux port of the Honeywell/Bull **CP‑6** cross‑assembler
toolchain.  The original CP‑6 software was written in a mix of
FORTRAN, PL/6, and CP‑6 BASIC.

This port builds and runs on any Linux system with `gfortran` and `gcc`.

It includes the full set of CP‑6 assemblers, disassemblers, and tools:

* **ASMZ80**: CP‑6 `ASMZ80` Z80/8080 cross‑assembler.
[]()

[]()
* **ASM6502**: CP‑6 `ASM6502` 6502 cross‑assembler.
[]()

[]()
* **MSA disassemblers**: `MSAZ80`, `MSA8085`, `MSA6502`, `MSA6800`,
  and `MSA8748`.
[]()

[]()
* **ASMDAL**: a two‑pass assembler for "DEC Assembly Language", an
  educational subset of PDP‑10 MACRO‑10.
  * A port of the PL/6 [`ASMDAL_SI61.XSI`](.original/ASMDAL_SI61.XSI).
[]()

[]()
* **BMAP**: CP‑6 Macro Assembly Program for `GMAP` (for the 36‑bit
  Honeywell/Bull DPS‑8).
  * A complete port of the PL/6 [`BMAP_SI61.XSI`](.original/BMAP_SI61.XSI).
  * It assembles `GMAP` programs to an octal listing (with cross‑reference)
    and a complete relocatable object unit, with support for the full
    instruction set, macros, and literals.
[]()

[]()
* **cp6link**: a linker, reimplementing CP‑6 `BAS_LINK`.
[]()

[]()
* **ouconv**: an object/run‑unit to binary or Intel HEX converter.
[]()

[]()
* **sim6502**: a small 6502 emulator for running tests (not from CP‑6).

## Quick start

```
make                 # build everything
make test            # run the test suite (49 checks)
make clean           # cleanup build artifacts
```

```sh
# microprocessor assemblers -> generate object (.obj) + listing (.lst)
./asmz80  prog.z80
./asm6502 prog.s
```

```sh
# disassemble an object back to source
./msaz80  prog.obj -o prog.z80
```

```sh
# link object(s) -> CP‑6 run‑unit (.ru)
./cp6link prog.obj -o prog.ru
```

```sh
# convert object(s) -> raw binary / Intel HEX
./ouconv  prog.obj -o prog.com         # raw binary (e.g. a CP/M .COM)
./ouconv  prog.obj -o prog.hex --ihex  # Intel HEX
```

```sh
# ASMDAL: assemble a DAL (PDP‑10) program
./asmdal  prog.dal                     # -> prog.obj + prog.lst
```

```sh
# BMAP: assemble a GMAP (DPS‑8) program -> octal listing + object unit (.obj)
./bmap    prog.gmap                    # -> prog.scan (octal listing)
./bmap    prog.gmap -S                 # -S/--scan: scanner trace only
./bmap    prog.gmap -g                 # -g/--debug: also emit the debug schema
```

* Options (the CP‑6 option list) are passed as extra arguments, e.g.
  `./asmz80 prog.z80 "(LS,OU,XR)"`. By default `LS` (listing) and `OU`
  (object output) are on.

* For reproducible builds using `BMAP`, the `DATE` pseudo‑op honours the
  standard [`SOURCE_DATE_EPOCH`](https://reproducible-builds.org/docs/source-date-epoch/)
  environment variable.  When set, `DATE` emits a fixed date word instead
  of reading the live clock.

## How the port works

* AI *was* used (Gemini, Claude, Copilot, and ChatGPT), *especially* with
  the PL/6 to C conversions, the I/O routines, the bulk of the **ASMDAL** and
  **BMAP** ports, and for producing the automated test suites based on many
  hand‑written test cases.

  * This project was essentially an experiment in automated test‑driven
    porting of legacy production software written in a "dead" language (PL/6),
    for which no compilers are known to exist, which ran only on an operating
    system that is now lost to the ages (Honeywell/Bull CP‑6), which itself
    ran only on a very exotic platform, the GE/Honeywell/Bull DPS‑8/C
    36‑bit large systems mainframe with the NSA/VU (New System Architecture)
    ISA extensions.

* The approach was intended to be **faithful per‑module** where practical.
  Algorithms, control flow, FORTRAN `COMMON` blocks, data layouts, and
  style (fixed‑form source) are preserved where possible.

  * **36‑bit words**: The FORTRAN code is built with `-fdefault-integer-8` so
    an `INTEGER` holds a CP‑6 36‑bit word; `cp6_compat.f` reproduces the CP‑6
    shift/logical intrinsics with exact 36‑bit semantics.  The C‑language tools
    model a "word" as a `uint64_t` masked to 36 bits, and packing instruction
    fields big‑endian (MSB‑first).

  * **9‑bit characters**: Characters live in 9‑bit bytes (4 per word); emitted
    output is plain ASCII and host‑independent.

  * **CP‑6 I/O**: CP‑6 monitor and file services are replaced by portable file
    I/O; source/listing/object files come from the command line.

## Status

* **ASMZ80 / ASM6502** assemble to the documented object‑unit format with
  correct opcodes, checksums and listings.  A simple CP/M‑80 "Hello World"
  program is built with the assembler and executed using the `tnylpo` emulator
  as a test.

* **MSA disassemblers** work; Z80 and 6502 round‑trip byte‑identically
  against their assemblers (`ASM -> MSA -> ASM`), the others on hand‑built
  objects.

* **ASMDAL** is fully ported and verified (two‑pass, `AVL` symbol table, three
  word formats, the object unit.  All instruction words hand‑checked vs. the
  PDP‑10 encodings). See [`source/ASMDAL_NOTES.md`](source/ASMDAL_NOTES.md).

* **BMAP** is complete: `bmap prog.gmap` assembles to a real octal listing
  **and** a complete relocatable object unit (`.obj`) - relocation, def/ref +
  segment‑ref records, control sections (`USE`/`BLOCK`), and (with `-g`) full
  debug symbols.  The real CP‑6 `BMAP` subroutine library (`BMAP_SIG.XSI`, 253
  code words) assembles to a hand‑walked, byte‑verified object.
  See [`source/BMAP_NOTES.md`](source/BMAP_NOTES.md).

* `make test` runs **49 checks**.

## Reference

* The original CP‑6 sources this port is derived from are archived in
  [`.original/`](.original).  This includes BASIC, PL/6, and FORTRAN code
  and a few CP‑6 OS listings we reference (`B$OBJECT_C.txt`, the `XUO$*`
  object‑writer library, `FORTRAN_HELP.txt`, etc).
