# 6CROSS: GE/Honeywell/Bull CP-6 cross-assembler suite

This is a Linux port of the Honeywell/Bull **CP-6** cross-assembler
toolchain. The original is FORTRAN + PL/6 + CP-6 BASIC; this port builds and
runs on an ordinary Linux box with **gfortran** + a C compiler. It comprises:

* **ASMZ80 / ASM6502** — the Z80 and 6502 cross-assemblers (FORTRAN port; they
  share most of their code via `asmz80_sif2` and the compat/I-O/startup layers).

* **MSA disassemblers** — `msaz80`, `msa6502`, `msa6800`, `msa8085`, `msa8748`
  (C; one shared load/trace/emit engine + a per-CPU `decode()`).

* **ASMDAL** — a two-pass assembler for a subset of DAL (PDP-10/DEC-20 MACRO);
  a faithful C port of `ASMDAL_SI61.XSI`. See `source/ASMDAL_NOTES.md`.

* **BMAP** — the CP-6 Macro Assembly Program for **GMAP** (Honeywell DPS-8,
  36-bit); a C port of `BMAP_SI61.XSI`. It assembles a GMAP program to an octal
  listing (with cross-reference) and a complete relocatable object unit, with
  the full instruction set, macros, and literals; the real BMAP subroutine
  library `BMAP_SIG.XSI` assembles to a byte-verified object. See
  `source/BMAP_NOTES.md` /
  `source/BMAP_CONTINUE.md`.

* **cp6link / ouconv / sim6502** — a C linker (reimplementing the BASIC
  `BAS_LINK`), an object/run-unit → raw-binary / Intel-HEX converter, and a
  vendored 6502 core for execution tests.

## Quick start

```sh
make                 # build everything
make asm             # build ./asmz80 (and ./asm6502)
make tools           # build the C tools: cp6link ouconv sim6502 msa* asmdal bmap
make test            # build everything and run the test suite (49 checks)
make clean

# microprocessor assemblers -> object unit (.obj) + listing (.lst)
./asmz80  prog.z80
./asm6502 prog.s

# disassemble an object unit back to source
./msaz80  prog.obj -o prog.z80

# link object unit(s) -> CP-6 run-unit (.ru); convert -> raw binary / Intel HEX
./cp6link prog.obj -o prog.ru
./ouconv  prog.obj -o prog.com         # raw binary (e.g. a CP/M .COM)
./ouconv  prog.obj -o prog.hex --ihex  # Intel HEX

# ASMDAL: assemble a DAL (PDP-10) program
./asmdal  prog.dal                     # -> prog.obj + prog.lst

# BMAP: assemble a GMAP (DPS-8) program -> octal listing + object unit (.obj)
./bmap    prog.gmap                    # -> prog.scan (octal listing)
./bmap    prog.gmap -S                 # -S/--scan: scanner trace only
./bmap    prog.gmap -g                 # -g/--debug: also emit the debug schema
```

Options (the CP-6 option list) are passed as extra arguments, e.g.
`./asmz80 prog.z80 "(LS,OU,XR)"`. By default `LS` (listing) and `OU` (object
output) are on.

For reproducible builds, BMAP's `DATE` pseudo-op honours the
[`SOURCE_DATE_EPOCH`](https://reproducible-builds.org/docs/source-date-epoch/)
environment variable (a decimal Unix time, interpreted as UTC); when it is set,
`DATE` emits a fixed date word instead of reading the live clock.

## Layout

| File | Role |
|---|---|
| `source/asmz80_sif0.f` | `BLOCK DATA` — opcode tables and COMMON initialisation |
| `source/asmz80_sif1.f` | driver (`MAIN`, `DOPASS`, `SCANOP`) |
| `source/asmz80_sif2.f` | support routines, **shared by both assemblers** (`EVE`, `PACK`, `NXTCHR`, `OBJOUT`, `FLUSH`, …) |
| `source/asm6502_sif0.f`, `source/asm6502_sif1.f` | 6502 `BLOCK DATA` + driver (reuse `asmz80_sif2`) |
| `source/cp6_compat.f` | CP-6 FORTRAN intrinsics (`ISL`/`ISA`/`ISC`, `ACPU`, `CLK$`/`DAT$`) + 9-bit char pack/unpack (`S2W`, `W2S`, `UCH`) |
| `source/cp6_init*.f`, `source/cp6_io.f`, `source/cp6_startup.f` | runtime opcode packing, portable file I/O, option parsing |
| `source/cp6link.c`, `source/ouconv.c`, `source/sim6502.c` | linker, object→binary/HEX converter, vendored 6502 core |
| `source/msa_engine.c` + `source/msa.h`, `source/msa*.c` | MSA disassembler engine + per-CPU decoders |
| `source/asmdal.c`, `source/asmdal_tables.{py,h}` | ASMDAL assembler + its opcode/JSYS tables (generated from the PL/6) |
| `source/bmap.c`, `source/bmap_opcodes.{py,h}` | BMAP assembler + its 787-entry GMAP op-code table (generated from `BMAP_DA2.XSI`) |
| `source/*_NOTES.md`, `source/BMAP_CONTINUE.md` | per-tool porting notes; BMAP's continuation brief |
| `source/hollerith.py` | build-time preprocessor for `1Hx` Hollerith constants in the FORTRAN |
| `tests/` | `*.z80`/`*.s`/`*.dal`/`*.gmap` sources, hand-verified `expected/*` fixtures, `run_tests.sh` |

## How the port works

The approach is **faithful per-module**: algorithms, control flow, COMMON
blocks / data layouts and fixed-form source are preserved; only the
host-specific boundaries are adapted. New tools (linker, disassemblers, ASMDAL,
BMAP) are written in C.

* **36-bit words.** The FORTRAN is built with `-fdefault-integer-8` so an
  `INTEGER` holds a CP-6 36-bit word; `cp6_compat.f` reproduces the CP-6
  shift/logical intrinsics with exact 36-bit semantics. The C tools model a word
  as a `uint64_t` masked to 36 bits, packing instruction fields MSB-first.

* **9-bit characters.** Characters live in 9-bit bytes (4 per word). They hold
  ASCII (≤127), so they fit; emitted output is plain ASCII and host-independent.

* **I/O.** CP-6 monitor file services are replaced by portable file I/O; source/
  listing/object files come from the command line.

Validation is by **bootstrap tests + hand-verified fixtures** (there is no
golden output and no emulator wired in): object-unit/listing bytes are checked
by hand against the documented formats and snapshotted into `tests/expected/`.

## Status

* **ASMZ80 / ASM6502** assemble to the documented object-unit format with
  correct opcodes, checksums and listings.

* **All five MSA disassemblers** work; Z80 and 6502 round-trip byte-identically
  against their assemblers (`asm→msa→asm`), the others on hand-built objects.

* **ASMDAL** is fully ported and verified (two-pass, AVL symbol table, three
  word formats, the object unit; every instruction word hand-checked vs the
  PDP-10 encodings). See `source/ASMDAL_NOTES.md`.

* **BMAP** is complete (all 9 phases — the front end, the full instruction
  set, the object writer, the macro processor, literals, OPSYN, the
  cross-reference and listing-control directives, and the test corpus):
  the scanner, AVL symbol table, `CONVERT`, the `VARSCAN` expression evaluator +
  relocation, the `INST`/`GEN` encoder for the common instruction and data
  types, and the `XUO$` object-unit writer (HEAD/SECT/PROG records, relocation
  directives, and the def/ref records — DNAM/RNAM/EDEF/EREF/SDEF/SREF for
  ENTDEF/ENTREF/SYMDEF/SYMREF) are done — `bmap prog.gmap` assembles to a real
  octal listing **and** a complete relocatable object unit (`.obj`) — relocation,
  def/ref + segment-ref records, control sections (USE/BLOCK), and (with `-g`)
  the full debug schema. DPS-8 binary floating-point DEC/OCT literals
  (single/double/scaled) assemble, as do the IO, ASCNT, and EIS (via MFSCAN)
  instruction families, as do CLIMB, the ADSC/VDSC/BDSC/NDSC descriptors, the
  NSA family (PTR/descriptors/vectors via FLAGS), MICROP, EDEC (the decimal-edit
  compiler), and DATE (the date word, made reproducible via `SOURCE_DATE_EPOCH`).
  The full GMAP instruction set assembles, and the **macro processor** —
  MACRO/ENDM definitions with `#N` parameter substitution, DUP, the IFE/IFG/IFL/
  INE conditionals, and IDRP — expands. **Literal `=`-constants** are interned
  (deduped) into the LITERALS section, and **OPSYN** aliases opcodes. Only the
  cross-reference (`XR`) annotates the listing with a statement-number column
  and a per-symbol reference table, and the listing-control directives
  (`LIST`/`DETAIL`/…, `EJECT`, `TTL`) work. The real BMAP subroutine library
  (`BMAP_SIG.XSI`, 253 code words) assembles to a hand-walked, byte-verified
  object. Macro CRSM auto-symbols,
  the extended `=` literal forms (float/character/multi-word + LITORG), and
  nested/literal-list IDRP are supported; `=M`/`=V` literals and listing
  pagination remain deferred. See `source/BMAP_NOTES.md`.

* `make test` runs **49 checks**: object-unit fixtures (Z80/6502/DAL), two
  execution tests (uses `tnylpo` for Z80 `hello`, `sim6502` for 6502 `run6502`),
  five `asm→msa→asm` round-trips, the ASMDAL error case, and thirty-four BMAP checks
  (`bmapscan` scanner trace, `bmapconv` self-test, `bmapasm` octal listing,
  `bmapobj`/`bmaprel` absolute/relocatable object units, `bmapdef` entry/symbol
  definitions, `bmapref` external/segment references, `bmapsec` control
  sections, `bmapdbg` debug schema, `bmapflt` floating-point, `bmapio` IO/ASCNT,
  `bmapeis` EIS, `bmapclm` CLIMB, `bmapdsc` descriptors, `bmapptr` PTR, `bmapnsa`
  NSA descriptors, `bmapvec` NSA vectors, `bmapmcp` MICROP, `bmapedc` EDEC,
  `bmapdate` DATE, `bmapmac`/`bmapdup`/`bmapif`/`bmapidrp` macros, `bmaplit` literals, `bmapsyn` OPSYN,
  `bmapxref` cross-reference, `bmaplst` listing control, `bmapall` integration,
  `bmaperr` error handling, `bmapsig` real-GMAP byte-walk, `bmapcrsm` CRSM
  auto-symbols, `bmapnidrp` nested/literal-list IDRP, `bmapxlit` extended literals).

## Reference

The original CP-6 sources this port is derived from live in **`.original/`** —
the `*.XSI` PL/6/FORTRAN modules plus the few CP-6 OS listings we reference
(`B$OBJECT_C.txt`, the `XUO$*` object-writer library, `FORTRAN_HELP.txt`). The
generators in `source/` read their inputs from there, so the repository is
self-contained.
