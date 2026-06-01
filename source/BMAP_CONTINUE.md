# BMAP port — continuation brief (read this first)

You are continuing a port of the **CP‑6 cross‑assembler suite** to Linux. The
microprocessor assemblers/disassemblers and **ASMDAL** are already done; **BMAP**
is the remaining work. This brief plus the files it points to are everything you
need — the conversation that produced them is gone. Read `BMAP_NOTES.md` (next to
this file) in full before writing code; it has the architecture, the
OP.TYPE→handler table, the object‑unit spec, and the 9‑phase plan.

## Where everything is

- Build/test from the repo root (`make`, `make test`); the C port lives in
  `source/`, tests in `tests/`, the original CP‑6 sources in `.original/`.
- Original CP‑6 PL/6/etc. sources: `*.XSI` in **`.original/`**. BMAP's are
  `BMAP_SI61.XSI` (5782‑line assembler), `BMAP_DA1.XSI` (common‑data / packet
  macros), `BMAP_DA2.XSI` (787‑line master opcode list), `BMAP_SI64.XSI`
  (LINKOP bootstrap), `BMAP_SIG/SI62/SI63/SIN1/SIH1`, `HELP_BMAP.XSI`.
- **Done foundation:** `source/bmap_opcodes.py` → committed
  `source/bmap_opcodes.h` (787 opcodes from DA2: `mnem,val,mask,ar,rpl,prfs,
  type`, sorted, + 46 `bmap_optype[]` names).
- **Full plan/map:** `source/BMAP_NOTES.md`.
- **Worked precedent to mirror** (the just‑finished ASMDAL port): `asmdal.c`,
  `asmdal_tables.{py,h}`, `ASMDAL_NOTES.md`, `tests/dal*.dal`,
  `tests/expected/dal*.obj`, and how they're wired into `tests/run_tests.sh`.
- Auto‑memory (persists across sessions): `cp6asm‑port‑state`, `cp6asm‑prefs`,
  `cp6‑listings`. The CP‑6 OS listings we reference are copied into `.original/`
  (object‑unit spec `B$OBJECT_C.txt`, the `object‑writer` library,
  `FORTRAN_HELP.txt`), so the repo is self‑contained.

## Guardrails

- `make test` (from the repo root) must stay green — currently **49/49**
  (`bmapscan` scanner trace, `bmapconv` self‑test, `bmapasm` octal listing,
  `bmapobj`/`bmaprel` absolute/relocatable objects, `bmapdef` entry/symbol
  definitions, `bmapref` external/segment references, `bmapsec` control sections
  + `,type`, `bmapdbg` debug schema via `‑g`, `bmapflt` floating point, `bmapio` IO/ASCNT, `bmapeis` EIS, `bmapclm` CLIMB, `bmapdsc` descriptors, `bmapptr` PTR, `bmapnsa`/`bmapvec` NSA, `bmapmcp` MICROP, `bmapedc` EDEC, `bmapdate` DATE, `bmapmac`/`bmapdup`/`bmapif`/`bmapidrp` macros, `bmaplit` literals, `bmapsyn` OPSYN, `bmapxref`/`bmaplst` xref+listing‑control, `bmapall`/`bmaperr`/`bmapsig` integration/error/real‑GMAP, `bmapcrsm` CRSM, `bmapnidrp` nested/literal IDRP, `bmapxlit` extended literals). Run it before and after each
  change. Keep everything **additive**: new files, new Makefile targets, new
  test cases only; do not alter the existing asmz80/asm6502/msa/dal paths.
- Build the `bmap` binary via a new `tools:`/Makefile target; add `/bmap` to
  `.gitignore` (binaries are listed explicitly there).

## The three non‑obvious facts about BMAP

1. BMAP assembles **GMAP** for **Honeywell GCOS / DPS‑8 (36‑bit)** — *not* PDP‑10.
   So **KLH10 does not apply to BMAP** (ASMDAL's deferred emulator is irrelevant
   here; a dps8m sim could run output someday, out of scope).
2. `BMAP_C` and `BMAP_SI62` are **generated** at build‑time (from `DA1` templates
   + `DA2` opcodes by the self‑hosting LINKOP tool, `BMAP_SI64`) and are **not in
   the repo**. The port **replaces that whole bootstrap** with `bmap_opcodes.h` +
   plain C structs for the `BMAP_COMMON` globals (defined in `DA1`).
3. SI61 does **not** write the object unit itself — it calls `XUO$*` library
   routines that aren't in the *BMAP* source. But their **full PL/6 source IS in
   the CP‑6 listings** (`XUO$BUILD/INIT/ENTRIES/OBJ_C/ROOT` at `.original/ XUO$BUILD/INIT/ENTRIES/OBJ_C/ROOT`;
   `B$OBJECT_C` record spec at `.original/B$OBJECT_C.txt`), so phase 6 **ports** that
   library rather than reimplement it. Likewise the routines SI61 declares
   `ENTRY` (`FIX`/`SCALE`/`CONVERTSTEP` DPS‑8 float, `ANSYM`, `BITINSERT`,
   `XLATEV`, `MRL`, `NEG`) are defined in **`BMAP_SIG.XSI` in the repo root**
   (GMAP assembly). The only genuinely‑external pieces are the `M$*` monitor
   services. (See the "External entries … where their source lives" section in
   `BMAP_NOTES.md`.)

## Working conventions (from `cp6asm‑prefs` + the ASMDAL session)

- **Faithful per‑module port**; modernize a routine only when its 9‑bit packing
  is too entangled to keep cleanly. New tools may be written in C.
- Model a 36‑bit word as `uint64_t` masked to 36 bits; pack fields MSB‑first.
  CP‑6 chars are 9‑bit but hold ASCII (≤127), so they fit.
- **Validation = bootstrap our own tests + hand‑verify bytes** against documented
  specs (there is no golden output and no emulator wired up). For BMAP: hand‑check
  DPS‑8 instruction encodings against the Honeywell‑6000 reference using
  `bmap_opcodes.h`; walk the emitted object bytes against `B$OBJECT_C`. Snapshot
  hand‑verified fixtures into `tests/expected/`, exactly as the `dal*` cases do.
- **Commit the table generators** (like `asmdal_tables.py`/`bmap_opcodes.py`) and
  keep their generated headers committed so the build needs no Python.
- Shell: prefer `rg`; run `env cp -f`/`env mv -f`; use **absolute paths** in Bash
  (the working dir persists between calls and a bare `cd` can prompt).
- End commit messages with the `Co‑Authored‑By: Claude …` trailer.

## Reusable patterns already proven in `asmdal.c`

- A Horowitz‑&‑Sahni **AVL** symbol table (BMAP uses the same idea; its names are
  up to 30‑char with `. $ @ _`, vs ASMDAL's 6). BMAP's `TREESRCH` (SI64 lines
  421‑524 / SI61 5094) is the same AVL — port it once, reuse for op/sym/lit trees.
- CP‑6 intrinsic semantics reverse‑engineered from call sites: `INDEX` = 0‑based
  offset or length‑if‑absent; number conversion stops at a blank. BMAP adds
  float + scaled `nBm` (see `CONVERT`, SI61 line 1005).
- Object emission as a self‑framed record stream + a byte‑walk verifier (the
  python snippet used for ASMDAL) — generalize it for BMAP's record types.

## Current state — all 9 phases complete (minor refinements deferred)

`source/bmap.c` is wired into `make` / `make test`. **Phases 2–7 are done, and
phase 8's literals (`=`‑constants) too —
`bmap` ASSEMBLES a GMAP program to a real octal listing AND a complete
relocatable object unit (.obj): relocation directives, def/ref + segment‑ref
records, multiple control sections, and (with `‑g`) the full debug schema. The
ENTIRE GMAP instruction set is ported and byte‑walk verified.** What's
live: the scanner; AVL symbol table + `SYMTAB`; `REL` packet; `CONVERT`
integers + float; `VARSCAN` (expression + modifier); `GEN`/`GENLOC`/`GENVAL`
(word pack + PC + octal listing + `char‑pack`); `INST` instruction types
**1–6**, data types **13/15/16/17/18** (EQU/BOOL/SET, ASCII/BCI/EBCDIC,
DEC/OCT‑int, ZERO, VFD), DEC/OCT **float**, and ALL specialized families —
**IO (23), ASCNT (32), EIS (27 via MFSCAN), CLIMB (26), the ADSC/VDSC/BDSC/NDSC
descriptors (28–30), the NSA family (PTR 33 / vectors 34 / descriptors 35–36 via
`flags_scan`), MICROP (31), and EDEC (42)**; mainline **BSS/ORG/EVEN/OUNAME**,
**DEF/REF (14:
ENTDEF/ENTREF/SYMDEF/SYMREF)**, and **USE/BLOCK (7, control sections — each with
its own location counter, ,type override)**; and the **object writer**
(`xuo_*`) emitting HEAD/DNAM/RNAM/SECT/EDEF/EREF/SDEF/SREF/SEGREF/PROG(+RELOC) +
(with `‑g`) LOGBLK/EXST/VREBL/DBGNAM + END records — TYPE codes per `B$OBJECT_C`
(see BMAP_NOTES phase‑6 §). `bmap prog.gmap` → `.scan` listing + `.obj` object
(when OU on); `--scan` = scanner trace; `‑g`/`--debug` adds the debug schema.
DEC/OCT **floating point** (single/double/scaled, via `convert`+`dps8_float`,
needs `‑lm`).  Verified by `bmapobj`/`bmaprel` (absolute/relocatable), `bmapdef`
(entry/symbol defs), `bmapref` (external/segment refs), `bmapsec` (control
sections + type override), `bmapdbg` (debug schema EXST+VREBL), `bmapflt`
(floats) — all byte‑walked — plus `bmapasm` listing, `bmapscan` (`--scan`),
`bmapconv` (`‑t`), and the per‑family object byte‑walks `bmapio`/`bmapeis`/
`bmapclm`/`bmapdsc`/`bmapptr`/`bmapnsa`/`bmapvec`/`bmapmcp`/`bmapedc`/`bmapdate`.
**Deferred (optional refinements only -- all 9 phases are otherwise complete):**
typed/instruction/float/multi‑word `=` literals + the `LITORG` flush (CASE 45)
-- only single‑word numeric `=expr`/`=Oexpr` are done; `forward refs`
(the two‑pass model already resolves ordinary forward references, so this is
largely moot); the aggregate VREBL type variants + B$LBNTRY0 debug header (BMAP
needs only the scalar form); SEGDEF (no directive emits it); the DATE `TTLDAT`/`,1`
path (the title‑date form; the `DATE` op itself is done); macro refinements (CRSM
auto‑generated unique symbols, literal‑list IDRP, per‑card macro‑body
listing); IF string ordering for IFG/IFL (string IFE/INE are done; ordering uses
`strcmp`); full listing pagination (EJECT/TTL are markers, not page headers).
Carry‑forward gotchas:
`SEARCH/INDEX1` substring‑base + `NEXTFLD` off‑by‑one; `6‑bit‑ASCII` == `strcmp`;
`sx36` 36‑bit signed; EQU/ORG/EVEN labels defined by their handlers; LDX1 is
type 1 but ADX/LDX type 2; ORG/EVEN operands decimal (ORG 100 → PC 0o144); DA1
INIT run‑length is VALUE*COUNT; the object stream is a deterministic self‑framed
serialization (TYPE codes match `B$OBJECT_C` but it is not the CP‑6 keyed‑file
container); **SYMTAB's reference path clears `REL.F`** but merges incoming XDEF
(EDEF/SDEF) flags onto the symbol; EREF/SREF numbers are 0‑based, assigned by a
name‑order sweep between the passes; control sections each keep their own PC in
`OSECT[].pc` (reset per pass), and the section table is built even when OU is
off (so USE/BLOCK PC tracking + the listing section column work); the PROG
record's payload is `[SUBTYP_PROG, section, words…]` (LEN = nwords+3), so a
naive object decoder that treats the payload as raw words sees two phantom
leading zeros (the subtype + section) — they are framing, not program words;
the delimiter codes were remapped in this port (`D_PLUS`=1…`D_COMMA`=9,
`D_BLANK`=10) but preserve the original ordering "operators < COMMA < BLANK", so
EDEC's `DEL<=%COMMA`/`DEL<%COMMA`/`DEL>=%COMMA` translate verbatim; EDEC/MICROP
run their field parse in **both** passes with no pass‑1 guard (GEN self‑guards
PC), so word counts stay pass‑stable as long as they depend only on source text.

## Immediate next step — none required; all 9 phases are complete

**All 9 phases are done.** `bmap` assembles a GMAP program to an octal listing
(with `XR` cross‑reference + listing‑control) and a complete relocatable object
unit, supports the full instruction set + macros + literals + OPSYN, and the
real BMAP subroutine library `BMAP_SIG.XSI` assembles to a hand‑walked,
byte‑verified object (`bmapsig`).

The high‑value refinements are now **DONE**: CRSM auto‑generated unique symbols
(`tests/bmapcrsm`), the extended `=` literal forms -- floating‑point, character
(=A/=U/=H/=Z/=R), multi‑word, and the `LITORG` flush (`tests/bmapxlit`) -- and
literal‑list IDRP (`tests/bmapnidrp`).  CRSM and the literal forms also
corrected latent bugs in `BMAP_SIG.XSI` (its CRSM‑generated ADJUST labels and
its `=1A`/`=35B25` literals), so that fixture was re‑verified and regenerated.

### Remaining deferred work (all optional, none blocking)

- **`=M` (instruction) and `=V` (VFD) literals.** Other `=` forms are done;
  these two need an instruction's / VFD's `GEN` output captured into the literal
  packet (a capture mode) rather than computed directly.  `literal()`
  currently `error(4)`s on them.  Unused by the GMAP sources in `.original/`.
- **IF string *ordering* (IFG/IFL on quoted strings).** Equality (IFE/INE) is
  done; ordering uses C `strcmp`, which may not match CP‑6 6‑bit collation for
  mixed‑length operands.
- **Full listing pagination.** `EJECT`/`TTL`/`TTLS` emit a form feed / a title‑marker
  line, not true paginated page headers (the listing is not paginated).
- **Object‑writer edge cases.** The `forward‑reference` fixup packet
  (the two‑pass model already resolves ordinary forward references, so this is
  largely moot); the aggregate `VREBL` debug‑record variants + the `B$LBNTRY0`
  header (only the scalar `VREBL` form is emitted); the `DATE` `TTLDAT` / `DATE,1`
  path (the title‑date form; the `DATE` op itself is done).
- **`SEGDEF`** record -- no directive emits it, so it is never needed.

Real CP‑6 listings in the source tree could calibrate the listing format (and
the IF collation) if byte‑exactness against the original is later wanted.

**Phases 2–7 are COMPLETE: the front end, the full GMAP instruction set, the
object writer, and the macro processor.** Every instruction family is ported and
byte‑walk verified (instructions 1–6, data 13/15/16/17/18, float, IO 23, ASCNT
32, EIS 27, CLIMB 26, descriptors 28–30, NSA 33–36, MICROP 31, EDEC 42, DATE 44;
IDCW 25 has no opcode).  The **macro processor** (phase 7) is a behavioral
C‑native engine: MACRO/ENDM definition (`case_macro_def`), `#N` positional
substitution + nested calls (`case_macro_call`, with `scanop` resolving macro
names), DUP (`case_dup`), the IFE/IFG/IFL/INE conditional skip (`case_if` +
`SKIPCT`), and IDRP list iteration -- bodies expand via an `EXPSTK` frame stack
drained by `next_phys_line`, re‑expanded each pass.  See the phase‑7 § of
`BMAP_NOTES.md` (and the macro refinements still deferred).

**Literals and OPSYN are DONE** -- numeric `=expr`/`=Oexpr` deduped into LITSECT
and emitted by `genlits()` at END (`tests/bmaplit`); `NEW OPSYN OLD` aliases an
op via a synonym table SCANOP consults (`tests/bmapsyn`).

**Phase 9 (tests) is DONE**: `bmapall` (integration -- macros + literals + EQU +
relocation together, exercising per‑feature fixtures isolate),
`bmaperr` (an invalid op -> exit 1 + HEAD severity 4), and `bmapsig` (the real
`BMAP_SIG.XSI` GMAP library assembles to a hand‑walked byte‑verified object).  The `bmapsig` exercise drove two
macro‑engine fixes: single‑pass substitution (so `#1#2` concatenation works) and
the string‑compare IF.  Commit at each green checkpoint.
