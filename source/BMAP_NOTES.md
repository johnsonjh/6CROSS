# BMAP — porting notes & plan

`BMAP` is **the CP‑6 Macro Assembly Program**: a full production macro assembler
that accepts **GMAP** (the Honeywell GCOS / Level‑66 / DPS‑8 36‑bit assembly
language). Copyright (c) Bull HN Information Systems Inc., 1989; author Tom
Martin, LADC. It is far larger and richer than ASMDAL: macros (DUP/IDRP/created
symbols), literals, complex relocation, control sections, floating point,
cross‑reference, and a debug schema, emitting a CP‑6/GCOS relocatable object
unit. Target is **Honeywell DPS‑8**.

This file is the map and plan for the C port (which is a multi‑phase effort).
The opcode table has already been extracted (see "Foundation done", below).

## Source modules (in the repo root)

| file | lines | role |
|------|------:|------|
| `BMAP_SI61.XSI` | 5782 | **the assembler** (PL/6): scanner, VARSCAN expr eval, INST 46‑way dispatch, macros, symbol table, literals, listing, drives the `XUO$` object writer |
| `BMAP_DA1.XSI` | 403 | common‑data definitions: the `OP`/`SYM`/`REL`/`LIT`/`MAP`/`FR` packet macros and the `BMAP_COMMON` global state; the `SPOOL` pool with `{OPS}`/`{OPROOT}` template holes |
| `BMAP_DA2.XSI` | 787 | **master opcode table**: one line per mnemonic — `MNEM  VAL(oct)  FLAGS(6 binary = MASK² AR RPL PRFS²)  TYPE(dec 0..45)` |
| `BMAP_SI64.XSI` | 525 | `BMAP_LINKOP`: the bootstrap generator. Reads DA2 (opcodes) into an AVL tree in SPOOL, then expands DA1's `{...}` templates to emit `BMAP_C` (the `%INCLUDE`) and `BMAP_SI62` |
| `BMAP_SIG.XSI` | 321 | a helper written in **BMAP's own assembly language**, assembled by BMAP, linked into LINKOP (self‑hosting) |
| `BMAP_SI62` | — | **generated** from DA1/DA2 (SYMDEF storage for the common block); not in repo |
| `BMAP_C` | — | **generated** `%INCLUDE` for SI61 (initialized tables); `!DELETE`d at end of build, so not in repo |
| `BMAP_SI63.XSI` | 17 | tiny option/severity helper (compiled with `CSYS`) |
| `BMAP_SIN1.XSI` | 26 | `PARTRGE`‑built helper |
| `BMAP_SIH1.XSI` | 446 | HELP text (copied to ME at install) |

External entries SI61 calls and **where their source actually lives** (this was
under‑appreciated early on — most are recoverable, not "lost"):

- **`BMAP_SIG.XSI` (in the repo root!)** is BMAP's own GMAP‑assembly helper
  library and *defines* the routines SI61 declares as `ENTRY`: **`FIX`,
  `SCALE`, `CONVERTSTEP`** (the DPS‑8 binary/hex floating‑point decimal→binary
  conversion — incl. the decimal‑scale constants `1E13/1E4/1E1` and their
  reciprocals, in both binary and HEXFP form), **`ANSYM`** (packed‑symbol →
  chars), **`BITINSERT`** (bit‑field insert), `MRL` (move chars R→L), `NEG`,
  and **`XLATEV`** (6/9‑bit translate). So the floating‑point port is grounded
  in real source — it just needs DPS‑8 FP arithmetic (`DFMP`/`FAD`/`FNO`/
  `LLS`/`LRS`…) modeled in C. (`ANSYM` is moot here — symbols are kept as C
  strings, not 6‑bit packed; `BITINSERT` is a trivial mask/shift in C.)

- The **`XUO$*` object‑unit writer is full PL/6 source**, copied into
  `.original/`: `XUO$BUILD.txt` (~26k‑line: `XUO$PRGM`/`RELOC`/`SECTBUILD`/
  `EDEF`/`SDEF`/`EREF`/`SREF`/`DNAME`/`RNAME`/`EXST`/`VREBL`…), `XUO$INIT.txt`
  (`XUO$OUINIT`/`LOGBLK*`), `XUO$ENTRIES.txt`, `XUO$OBJ_C.txt` (the `%OPER*`/
  `%RELOCOP*`/`%EVALOP*` `%EQU`s) and `XUO$ROOT.txt`; `B$OBJECT_C.txt` is the
  record spec. **Phase 6 can port this library rather than reimplement it.**

- `X$PARSE` (the option parser) is a CP‑6 library routine, not needed by the
  port (option parsing is handled in the C CLI), so it was not copied.

- Genuinely external: the `M$*` monitor services (file I/O, time, exit) — these
  the port maps to stdio/OS as the microprocessor‑assembler ports already did.

## The bootstrap, and how the port sidesteps it

Build chain (`BMAP_CRU`): `DA2` → LINKOP builds the opcode AVL tree in SPOOL →
DA1 `{OPS}`/`{OPROOT}` holes filled → `BMAP_C` + `BMAP_SI62`. So `BMAP_C` is a
*generated* artifact and isn't in the repo. The opcode table content is all in
`DA2`; the AVL‑in‑SPOOL packing is just the original's storage mechanism. **The
port replaces this entire bootstrap with a sorted opcode array + binary search.**

### Foundation done
`tools/bmap_opcodes.py` parses `BMAP_DA2.XSI` → committed `tools/bmap_opcodes.h`:
787‑entry sorted `bmap_ops[]` (`mnem,val,mask,ar,rpl,prfs,type`) plus the 46
`bmap_optype[]` names. This is the same proven pattern as `asmdal_tables`.

## SI61 architecture (map to port from)

**Main flow** (`BMAP: PROC MAIN`, line 9): init object unit + pools; for each
pass (PASS1 counts/defines, PASS2 emits) loop `READCARD → dispatch on OP.TYPE`.
Per source line: read card (or macro expansion) → scan fields (location /
operation / variable) → `SCANOP` looks the opcode up in the tree → dispatch.

**OP.TYPE dispatch** (0..45; names in `bmap_optype[]`). Some types are handled in
the mainline, most route to `INST` (line 2029, a big `DO CASE(OP.TYPE)`):

| type | meaning | handler |
|------|---------|---------|
| 0 | IGNORE | (skip) |
| 1 | non‑EIS instruction | INST |
| 2 | index instruction | INST |
| 3 | tally | INST |
| 4/5 | repeat / RPTX,RPDX | INST |
| 6 | inst without variable field | INST |
| 7 | USE, BLOCK | mainline 219 |
| 8 | EJECT, TTL, TTLS | mainline 291 |
| 9 | END | mainline 306 |
| 10 | INHIB | INST |
| 11 | EVEN/ODD/EIGHT/PAGE | mainline 570 (`BOUNDARY`) |
| 12 | ORG | mainline 577 |
| 13 | BOOL/EQU/SET | INST |
| 14 | ENTDEF/ENTREF/SYMDEF/SYMREF/SEG* | INST → SYMTAB |
| 15 | ASCII/BCI/EBCDIC | INST |
| 16 | DEC, OCT | INST |
| 17 | ZERO | INST |
| 18 | VFD, OPD | INST → VFD |
| 19 | DUP | INST |
| 20 | MACRO definition | mainline 588 |
| 21 | MACRO call | INST |
| 22 | IFE/IFG/IFL/INE | INST |
| 23 | I/O | INST |
| 24 | BSS | mainline 662 |
| 25 | IDCW | INST |
| 26 | CLIMB (ENTER/EXIT/GOTO/PASS…) | INST |
| 27 | EIS instructions | INST |
| 28/29/30 | BDSC / ADSC / NDSC descriptors | INST |
| 31 | MICROP | INST |
| 32 | ASCNT | INST |
| 33–36 | NSA pointer/vectors/descriptors/entry | INST |
| 37 | IDRP | INST |
| 38 | OPSYN | mainline 674 |
| 39 | listing controls (DETAIL/LIST/PMC/HEXFP/CRSM/FLOAT/PCC/REF/REFMA) | mainline 693 |
| 40 | OUNAME | mainline 726 |
| 41 | LODM (macro library) | mainline 749 |
| 42 | EDEC | INST |
| 43 | ORGCSM | mainline 772 |
| 44 | DATE, TTLDAT | INST |
| 45 | LIT | mainline 783 |

**Key procedures** (line — purpose): `READCARD` 4260 (card reader + macro/DUP
expansion), `SCANOP` 4580 (opcode field → tree search), `DELSCAN` 1259 /
`NEXTFLD` 3860 (delimiter/field tokenizer; `CURRCH`/`NEXTCH`/`DEL`), `VARSCAN`
5257 (**expression evaluator**, shunting‑yard over `+ - * / OR EOR AND NOT ( )`
producing value + `REL` relocation packet), `SYMTAB` 4606 (symbol tree
search/enter, EDEF/SDEF/EREF/SREF, forward refs), `UFR` 4921 (resolve forward
refs at definition), `LITERAL` 3654 (=O/=A/=H/=V/=M literal pool), `VFD` 5614,
`INST` 2029, `GEN` ~1326 (emit a word: PASS1 bumps PC, PASS2 calls
`XUO$PRGM`/`XUO$RELOC` or builds an FR), `GENLITS` 877 (emit literal pool at
END), `TREESRCH`/`TREESTEP` 5094/5216 (AVL), `CONVERT` 1005 (numbers incl.
float/scaled), `PRINT`/`LIST` 3974/3472 (listing), `CTLCRD`/`GETOPTIONS` 1150/1531
(options via `X$PARSE`). Packets `OP`/`SYM`/`REL`/`LIT`/`MAP`/`FR`/`MAC` are in
`BMAP_DA1.XSI`.

## Object unit — port the XUO$ library (source is available)

SI61 does NOT write the object itself; it calls the `XUO$*` entries which build
the CP‑6/GCOS relocatable object unit. Those are **not** lost: the full PL/6
source is copied into `.original/` (`XUO$BUILD.txt`, `XUO$INIT.txt`,
`XUO$ENTRIES.txt`, `XUO$OBJ_C.txt`, `XUO$ROOT.txt`), so phase 6 ports that
library rather than reimplementing it. The record layout below is from
`.original/B$OBJECT_C.txt`.
Record types: HEAD(0),
DNAM(1)/RNAM(2) names, SECT(3), EDEF(4)/EREF(5)/SDEF(6)/SREF(7),
SEGDEF(8)/SEGREF(9), PROG(10) with sub‑types program(0)/relocation(1),
and debug LOGBLK(11)/INTNTRY(12)/EXST(13)/VREBL(14)/DBGNAM(15).

Relocation word (`%REL` packet, `BMAP_DA1` 89‑126): F flags
(EQU/SET/EDEF/SDEF/DEFED/REFED), `OPNDTYP`(4) operand type (1=section, 2=ENTREF,
3=SYMREF, 4=const, 5=SEGDEF, 6=SEGREF, 9=undef, 15=fref), `EVALOP`(4)
(add/sub/mul/div/shift), `RELOCOP`(4) (add/sub/mul/div/store‑L/R), `OPERAND`(18),
and a 2nd word with `DISP`(18)/`STBIT`(9)/`ENDBIT`(9) plus an optional `VALUE`
word when `EVALOP≠0`. `%EQU UNDEF='004401'O`, `XDEF='14'O`. The `XUO$RELOC`
2‑arg vs 4‑arg forms select the one‑ vs multi‑word relocation directive.

The existing `source/cp6link.c` and `ASMDAL` model only the *simple* object
records; BMAP's are the full thing (sections + complex relocation + schema).

## Verification strategy

No emulator is wired up. Plan: (1) hand‑verify DPS‑8 instruction encodings for a
sampled GMAP program against the GE‑635/Honeywell‑6000 reference (opcode bits
from `bmap_opcodes.h`); (2) validate the object‑unit bytes against the
`B$OBJECT_C` record spec by walking records, exactly as done for ASMDAL; (3) a
GCOS/DPS‑8 simulator (e.g. dps8m) could later run output — out of scope here and
distinct from ASMDAL's KLH10 hand‑off.

## Phase 2 — done (skeleton + scanner)

> **Historical milestone note.** This and the "Phased port plan" section below
> are the development log; **the port is now complete** (all 9 phases, the test
> suite is 49/49 -- see the OP.TYPE handler sections above and `BMAP_CONTINUE.md`
> for the current state).  A "deferred to phase N" / "TODO" note here was done by
> that later phase; the counts and "output is later phases" remarks are as of
> the phase being described.

`source/bmap.c` (built by `make bmap`/`make tools`; `/bmap` git‑ignored).
Verified by `tests/bmapscan.gmap` → `tests/expected/bmapscan.scan`, wired into
`make test` (16/16 at the time; 49/49 now). What landed and the non‑obvious
decisions:

* **CLI / options / DCBs.** `bmap source [object [listing]] [-o/‑l/‑u/‑s] [-O
  opts | (opts)]`; option tokens `1P/2P/LU/LS/NLS/OU/NOU/SC/NSC/SO/UI/NUI/XR/
  SRCH(...)` (CTLCRD, SI61 1150). Always two‑pass (HELP star 24631); default
  `LS`+`OU` when none given. M$SI=source (read into memory once, replayed each
  pass), M$LO=listing (the scan trace), M$UI/M$OU/M$SO accepted but their
  output is later phases.

* **`BMAP_COMMON` (DA1) → C structs**, original names kept: `OPTIONS`,
  `LISTING`, scanner state (`XCARD`/`XCARDL`/`CURRCH`/`NEXTCH`/`DEL`/`KEY`),
  `LOC`/`LOCSZ`, `OP` (a `const struct bmap_op *` into `bmap_ops[]`, replacing
  `OP$`→SPOOL), `PASS2`, `PC`, and the counters. SPOOL/MPOOL pools + packet
  trees arrive with phases 3‑7.

* **The CP‑6 `SEARCH`/`INDEX1` intrinsics** (PL/6 built‑ins, not in the
  sources) were reverse‑engineered from call sites: `SEARCH(outidx,outval,
  table,str,start)` scans `str` from `start` for the first char with a nonzero
  `table` entry, returning a **substring‑base‑relative** index (ALTRET if none).
  That base‑relativity is *load‑bearing*: READCARD's variable‑field scan uses a
  base‑1 substring (SI61 4382) precisely so `CURRCH` lands one short, which
  `NEXTFLD`'s pre‑increment (`CURRCH=CURRCH+1`) then corrects. `bmap.c`
  reproduces that observable result directly (`CURRCH = var_start - 1`).

* **Scanner:** `scanop`/`delscan`/`nextfld`/`read_card` ported faithfully;
  `DELTBL`/`NONBLK` built from DA1. Op lookup is `bsearch`+`strcmp` over
  `bmap_ops[]` (table is strcmp‑sorted; mnemonics are `[0‑9A‑Z]` only). The op
  field is uppercased first because CONSYM's 6‑bit‑ASCII fold maps a‑z→A‑Z.
  Empty op field → `NONOP` (no error); unknown mnemonic → `NONOP`+`error(2)`.
  Comment cards (col‑1 `*`, or first 16 cols blank) are skipped; EOF synthesizes
  an `END`.

* **Pass loop + dispatch shell:** `PASS2` 0→1 driven by `END` (CASE 9). The
  46‑way `OP.TYPE` switch routes mainline types (7,8,11,12,20,24,38‑41,43,45)
  and `INST` (CASE ELSE) to counting stubs; `STMNTCT`/`RECORDCT` match the
  original's bump points. The pre‑dispatch label‑define (`OP.PRFS&'01'B`) and
  the 34‑36 boundary call are stubbed (SYMTAB/GENLOC/BOUNDARY are phases 3/5).

* **Verification artifact:** pass 2 emits a hand‑verifiable *scan trace* to
  M$LO — per card: `CARD#  L=label  O=mnem  tNN:typename  vOCTAL  |
  "field"/DELIM ...`. Each line was checked against `bmap_opcodes.h` (TYPE/VAL)
  and HELP_BMAP field rules before snapshotting. This scan trace is retained as
  the `--scan` mode (the default listing mode is the phase‑5 octal listing).

## Phased port plan

> **All phases are complete (49/49).** This is the development log: each entry's
> "deferred to phase N" / "TODO" items were implemented by that later phase
> (e.g. CONVERT's float, VARSCAN's modifier path, the object writer, macros,
> literals).  For the authoritative current state and the few remaining optional
> refinements, see the handler sections above and `BMAP_CONTINUE.md`.

1. **Tables (DONE):** `bmap_opcodes.{py,h}` from DA2.

2. **Skeleton + scanner (DONE):** see the section above — CLI/options/DCBs,
   `BMAP_COMMON`→structs, `READCARD`/`SCANOP`/`DELSCAN`/`NEXTFLD` with bsearch
   op lookup, two‑pass loop + `OP.TYPE` dispatch shell, scan‑trace fixture.

3. **Symbol table + `CONVERT` (DONE, integer; float deferred):** the AVL
   (`TREESRCH`/`TREESTEP` as a malloc'd pointer tree, Horowitz pattern, 30‑char
   strcmp keys) + the `SYMTAB` define/lookup core (multiply‑defined → error
   -10); the `REL` packet as a C struct of logical fields (36‑bit packing
   deferred to phase 6); `CONVERT` integer conversion exact (dec/oct), with the
   float (SP/DP, HEXFP) and scaled `nBm` paths flagged deferred. Wired to label
   definition (`OP.PRFS&'01'B`) + sorted symbol dump; `‑t` self‑test fixture
   (`bmapconv`). (XUO$ emission and the xref were later done in phases 6/8; only
   the `UFR` forward‑ref fixup is still deferred.  Float was later done in phase
   5 -- `dps8_float`, a C port of the DPS‑8 `FIX`/`SCALE` FP encoding.)

4. **VARSCAN expression evaluator (DONE, TYPE 0/1; modifier→phase 5):** the
   operator‑precedence parse over `+ - * / ( )` (and the same delimiters as
   `OR/EOR/AND/NOT` in an octal field, `type&1`), tracking the `REL` packet:
   R1±R2 (with same‑section cancellation to absolute, and a second relocation
   word `S` when both survive), R1*A / R1/A via `EVALOP`. Leaf = number
   (`CONVERT`, all‑digit test) / symbol (`SYMTAB`) / `*` (location counter).
   Values are 36‑bit sign‑extended (`sx36`). Verified by the `bmapconv` `‑t`
   battery (arithmetic, precedence, parens, unary minus, octal Boolean, the
   relocatable cases, and the R1*R2 error). **Deferred:** the modifier path
   `S60` (TYPE 2, the address‑mod tag field — `MODSYM`/`MODVAL`) goes with INST
   (phase 5); literals (`=`) with phase 8.

5. **INST dispatcher + live encoder — IN PROGRESS (assembles to an octal listing):**
   - `GEN`/`GENLOC`/`GENVAL` (SI61 1326): pack NF fields of given bit‑widths
     into a 36‑bit word MSB‑first (the original's two `BITINSERT`s = a
     left‑shift+OR), bump `PC`, format the octal listing (per‑field octal with
     relocation markers: `(n)` section, `F` forward, `X` external). Object
     emission (`XUO$PRGM`/`XUO$RELOC`) deferred to phase 6; the forward‑ref
     `FR` build deferred to phase 8 (undefined fields assemble with value 0 and
     show `F`). `IFORM`={3,15,12,6,18,12,6}, `XFORM`={3,15,6,3,3,6,18,6,3,3,6}.

   - `VARSCAN` **modifier path `S60`** (TYPE 2): the address‑mod tag field —
     `MODSYM`/`MODVAL` bsearch, indirect `*`, numeric/symbol tags. Needed
     because type‑1/2 read the tag via `VARSCAN(...,OP.MASK)` (MASK 2 = symbolic
     → modifier mode).

   - `INST` instruction types **1** (non‑EIS: addr + tag + optional AR 4‑field,
     IC‑relative fix‑up), **2** (index: absolute reg → 8·idx+OP.VAL via IFORM,
     relocatable reg → 6‑field XFORM), **3** (tally), **4** (REPEAT: count +
     increment + condition list TOV..TNZ, `RFORM`), **5** (RPTX), **6** (no
     variable field), **0** (ignore).

   - data/pseudo types **13** (EQU/BOOL/SET/SETB — set `REL.F` flags, define
     label=value, SET‑redefinition allowed), **15** (ASCII/BCI/EBCDIC/UASCI —
     `char‑pack` via the `bmap_asciitbl.h` conversion tables, M=4+2·AR
     chars/word), **16** (DEC/OCT — `DECTBL` classify, sign, `CONVERT` integer;
     **float words deferred**), **17** (ZERO), **18** (VFD — `[type]width/value`
     bit fields packed MSB‑first across words via the per‑word accumulator;
     OPD opcode‑definition deferred).

   - **Mainline handlers** (in `do_pass`, not INST): **24** BSS (PC += count),
     **12** ORG (`GENLOC(val)` sets PC, line shows the new origin), **11**
     EVEN/ODD/EIGHT/PAGE via `BOUNDARY` (SI61 814: align PC, emit a NOP/TRA pad
     word). USE/BLOCK (7, control‑section table) is deferred to phase 6.

   - **WIRED into the live dispatch** as the default mode: `bmap prog.gmap`
     now assembles to a real **octal listing** (location + 36‑bit word(s) +
     source) via `GEN`/`GENLOC`/`GENVAL`; two passes resolve forward label
     references (pass 1 defines, pass 2 emits — so no `FR` needed for ordinary
     labels). `--scan` keeps the phase‑2 scanner trace (that's how `bmapscan`
     still runs). Object emission stays deferred to phase 6; `FR` (genuinely
     undefined symbols) to phase 8. Verified by `tests/bmapasm.gmap` →
     `bmapasm.scan` (a full program: instructions w/ fwd refs, EQU, ORG,
     OCT/DEC/ZERO, BSS, EVEN pad — every word + label hand‑checked) and the
     `bmap ‑t` `inst:`/`data:` batteries.

   - **The named phase‑5 list is COMPLETE** (instructions 1–6 + EQU/BOOL/SET,
     ASCII/BCI/EBCDIC, DEC/OCT, ZERO, VFD + BSS/ORG/EVEN); **DEF/REF (14)** and
     **USE/BLOCK (7)** are now done (phase 6).  The remaining tail:

       * **DEC/OCT float (DONE):** `convert` now computes a host `double` for a
         `.`/E/D literal (mantissa × 10^(E − fraction‑digit count
         + any E exponent)) and `dps8_float` encodes it to a DPS‑8 binary float
         word — single (`.`/E) or double (D, two words) — with `nBm`
         binary‑scaled to a fixed integer.  A modernization of
         `FIX`/`SCALE`/`CONVERTSTEP` (host double instead of emulating DPS‑8
         FP); the words match the hardware format (verified
         `tests/bmapflt.gmap`: 1.5, 1E3, -0.5, 1.0D0, 3B17).  Needs `‑lm`.

       * the specialized instruction families.  **DONE:** **IO (23)** (`IOFORM`
         18|6|12, opcode in the middle field), **ASCNT (32)** (`ASFORM`
         16|11|1|8, the `,N` flag), and **EIS (27, 92 opcodes)** — `mfscan`
         ports MFSCAN (a plain octal MF byte or the `(ar,rl,id,reg)` form, AR
         into bit 29 etc.; reg is a modifier name) and `case 27` ports the
         CASE 27 logic: `K=OP.MASK*2+OP.AR` (1–6) selects one of six `EFORM`
         7‑field formats, two MFs are scanned then the K‑specific operands, and
         `GEN(7‑L, EFORM[7K‑7+L], val[L..6])` packs the word (MF1's AR rides in
         the opcode field, MF1 low 6 bits in the last field, MF2 in field 4).
         Verified `tests/bmapeis.gmap` (MLR/CMPC K=1, AD2D K=5, AD3D K=6, and
         the paren‑MF form).  **CLIMB (26, DONE):** `case 26` ports the 2‑word
         CLIMB family (ENTER/EXIT/GOTO/PASS/PMME) — word 1 via `IFORM`, word 2
         via `CL2FORM` 1|9|8|6|12; the `OP.AR` path, the operand flag (`040` in
         val[7]), and a parenthesized register modifier (`tests/bmapclm.gmap`).
         **Descriptors ADSC/VDSC/BDSC/NDSC (28–30, DONE):** `case 28–30` follow
         an EIS instruction and read its MF state — `MF_[]`/`NDS` are now global
         (EIS sets them, `IDS=‑1`), each descriptor does `IDS++` (capped at slot
         3 = the all‑zero default) and uses `MF[IDS]`'s RL/AR bits to pick the
         varscan type and the AR extra‑field; packed by `ADSCF`/`VDSCF`/`BDSCF`/
         `NDSCF` (K=0 has a 3+15 AR prefix, K=1 an 18‑bit address).  ADSC scales
         its offset by `OP.MASK`; VDSCX sets a bit in val[2].  Verified
         `tests/bmapdsc.gmap` (ADSC4/ADSC9/BDSC/VDSC/NDSC9 after an MLR).
         **The NSA family (33–36, DONE):** `flag‑name scanner` ports the `FLAGS`
         flag‑name scanner (M/N/P/E/X/B/S/W/R → bit 2^index; ALL=0o777, NONE=3,
         NOT inverts; absent keeps the caller default).  **PTR (33)** is four
         operands via `BDSCF`.  **NSA descriptors (35: ODSC/DDSC/IDSC/ODSB)** and
         **entry descriptors (36: EDSC16/24/64)** are 2‑word, with `K/K2`
         juggling, the char‑offset `*4`/`/4`/`mod 4` math, `FLAGS` into val[2],
         and the `DSCF`/`EDSCF` formats (ODSB's `OP.MASK=0`→`K=‑1` indexes
         `val[‑1]`, hence the guard slots in `inst()`).  **NSA vectors (34:
         VEC/FVEC/SVEC/CVEC)** are 2‑ or 4‑word (`PC += OP.VAL/256`), with
         `K/K2/K3` juggling and the `VECFORM` formats.  Verified field‑by‑field:
         `tests/bmapptr.gmap`, `tests/bmapnsa.gmap` (ODSC/DDSC/ODSB/EDSC16),
         `tests/bmapvec.gmap` (VEC/SVEC/CVEC + a parenthesized `(M,X)` flag
         list).  **MICROP (31, DONE):** `case 31` packs edit micro‑ops 4/word (`QFORM`)
         -- octal digits, H/A/U BCD/ASCII chars (via `bmap_asciit`), a leading
         repeat count, and the `(MOPNAME,value)` form (`MOP` name table); a group
         emits when 4 fields are filled (`tests/bmapmcp.gmap`).  **EDEC (42, DONE):**
         a decimal‑edit compiler -- each `<count><A|P>[L]<number>` field is parsed
         twice: a count phase (`DIGITPMPE` classifier) tallies the significant
         positions `CT` and the decimal scale `DP` (negated fraction‑digit count
         + any `E` exponent), then an emit phase re‑walks the number and inserts
         each position as an edit byte via `EDINS` -- leading zero‑fill (unless
         `L`), the sign overpunch (`+`→`014`/`ASCBIT('+')`, `-`→`ASCBIT('-')`),
         the digits, and (floating point) a trailing scale byte.  `A`→9‑bit ASCII
         (4/word), `P`→4‑bit packed (2 nibbles per 9‑bit group, bit 0 clear).
         `NONDGT` (DA1 293) is the field‑header SEARCH table (`A`→9, `P`→4).  Like
         MICROP it has no pass‑1 guard -- the parse runs both passes and `GEN`
         (one word per 4 ASCII / 8 packed chars) bumps `PC` (`tests/bmapedc.gmap`,
         byte‑walked).  **DATE (44, DONE):** emits the M$TIME date word -- the
         monitor's 8‑char "MM/DD/YY" string, transformed by `INSERT(DBUF,6,2,DBUF)`
         (a 1‑based overwrite, verified against MSAZ80's column‑9 `INSERT` usage)
         and packed as 6 BCD chars from `SUBSTR(DBUF,2)`.  Made **reproducible**
         via `$SOURCE_DATE_EPOCH` (reproducible‑builds.org convention: decimal
         Unix time as UTC) -- when set, `date_word()` uses `gmtime` of it instead
         of the live clock (`tests/bmapdate.gmap` pins a fixed epoch).  **The full
         GMAP instruction set now assembles.**

      * **macro processor (phase 7, DONE)** -- types MACRO (20), macro call
         (21), DUP (19), IF (22), IDRP (37).  SI61 keeps prototype text in the
         dynamic `MPOOL` word‑pool with a `BASED` `MAC` stack and expands lazily
         in `READCARD`; this port (by design decision -- see the commit) takes a
         take a **behavioral C‑native** route: each macro's body is kept as plain card
         strings (`struct macro`), and a call pushes the `#N`‑substituted body
         onto an **expansion stack** (`EXPSTK`) that the card reader
         (`next_phys_line`) drains ahead of the file.  Expansion is redone each
         pass -- exactly as SI61's MAC stack does -- so symbol‑valued DUP/IF
         operands stay correct, and the shared `src_lines[]` is never mutated.
         **MACRO** (`case_macro_def`): captures body cards up to `ENDM`/`ENDOP`,
         names it from the location field, errors (‑13) if the name collides
         with a built‑in op (SI61's `TREESRCH` insert).  **call** (`case_macro_call`,
         reached because `scanop` looks the name up in the macro table when it is
         not a built‑in op): parses comma‑separated arguments with `()`/`[]`
         grouping + paren‑strip (SI61 `MACROPARAM`), then `#N` positional
         substitution (`macro_subst`; `#0`/bare `#` literal).  Nested calls work
         (an expanded card that names a macro pushes a deeper frame).  **DUP**
         (`case_dup`): "DUP n,m" captures the next `n` cards and pushes them `m`
         times.  **IF** (`case_if`): IFE/IFG/IFL/INE (`OP.VAL` 0..3) compare two
         operands; a failed test sets `SKIPCT` so the main loop skips the next N
         cards (N from a trailing field, default 1).  **IDRP** (`case_macro_call`
         phase 1): the "IDRP #k <body> IDRP" block repeats once per comma element
         of list‑valued argument #k (bound via `subst_one` before the general #N
         pass).  Verified: `tests/bmapmac` (#N + nested), `bmapdup`, `bmapif`,
         `bmapidrp` (all byte‑walked).  Substitution is a single pass so a
         concatenation like `#1#2` expands atomically (`P`+`0` -> `P0`, not the
         mis‑parsed `#10`); the quoted‑string compare (`IFE '#3','=0'`) is
         supported.  **CRSM** (`CRSM ON`) generates a unique label `_NNNN_` for a
         #N beyond the supplied args (`crsm_sym; tests/bmapcrsm`), and **nested /
         literal‑list IDRP** work via the recursive depth‑matched `expand_range`
         (`tests/bmapidrp`).  Still deferred: per‑card listing of macro‑body
         lines.  (The real BMAP subroutine library `BMAP_SIG.XSI` --
         DEFREGS via IDRP + label concatenation, ADJUST via string‑compare IFs --
         assembles to a byte‑verified object; the suite runs it as `bmapsig`, whose fixture was hand‑walked — 173/173 basic‑instruction opcodes checked against the op table, INDEX opcodes + OCT data + the macro‑generated EQUs verified, structure confirmed.)

       * **literals (phase 8, DONE):** a `=expr` / `=Oexpr` operand
         (VARSCAN, when the field allows it -- `type & 4`) is interned by
         `literal()`/`lit_intern()` -- deduped by value -- into the LITERALS
         section (LITSECT, which `xuo_sectbuild` builds as object section 1),
         and the reference relocates against that section (OPERSECT, operand
         LITSECT).  `genlits()` emits the pool at END (SI61 `GENLITS`), in
         pass‑2, in pc order; the table is rebuilt each pass so an address
         assigned in pass 1 recurs in pass 2.  Verified `tests/bmaplit` (=5
         shared by two instructions -> one pool word; =O17, =‑1).  The **extended
         forms** are done (`tests/bmapxlit`): floating point (=1.5 / =1.0D0 -> one
         or two words via `dps8_float`, doubles even‑aligned in the multi‑word
         pool), character (=A/=U/=H/=Z/=R via `lit_char`), and the `LITORG` flush
         (`case_litorg`, which starts a fresh pool batch).  **Still deferred:**
         `=M` (instruction) and `=V` (VFD) literals -- they need GEN/VFD output
         captured into the literal packet.

       * **OPSYN (phase 8, DONE):** "NEW OPSYN OLD" (CASE 38) adds NEW to a
         synonym table (`opsyn_add`) aliasing the existing op OLD; SCANOP
         consults it after the built‑in ops and macros, so NEW assembles
         identically.  Pass‑1 only, persists to pass 2.  Verified `tests/bmapsyn`
         (LOADA->LDA, STOREQ->STQ).

       * **cross‑reference + listing control (phase 8, DONE; listing‑only):**
         with the `XR` option, `listing_line` prepends a statement‑number column
         and `symtab` records each symbol's reference statements (`struct
         sym.refs`, pass 2); `write_summary` then prints a per‑symbol
         cross‑reference table (`dump_one_xref`, the definition line marked `*`).
         **LISTCTL** (CASE 39 -- DETAIL/LIST/PMC/HEXFP/CRSM/FLOAT/PCC/REF/REFMA,
         `OP.VAL` = the flag index 0..8) sets a `LISTING` flag from
         ON/OFF/SAVE/RESTORE (`case_listctl`); `LIST OFF` suppresses the listing
         (the code still assembles -- the PC advances).  **EJECT/TTL/TTLS** (CASE
         8) emit a form feed / a title line (`case_eject`; the listing is not
         paginated, so these are markers).  Tests `bmapxref`, `bmaplst`
         (`check_scan`).  **All 9 phases are now complete.**  Deferred
         refinements only: CRSM auto‑symbols, typed/multi‑word literals + LITORG,
         nested/literal‑list IDRP, full listing pagination.

6. **Object‑unit writer (XUO$*) — IN PROGRESS (parts 1–2 done):** the real
   object is a keyed file (UTS‑timestamped keys) built by the 26k‑line
   `XUO$BUILD`; with no CP‑6 linker to consume it, the port emits the
   record contents (per `.original/B$OBJECT_C.txt`) in a deterministic
   self‑framed stream — 36‑bit words as 5 bytes big‑endian, each record a
   header word `TYPE(9)|LEN(9)|ARG(18)` + `LEN`‑1 payload words, walked by
   `LEN` to `END`.  **The TYPE codes match `B$OBJECT_C`'s `B$RECORDSUBS`**
   (`HEAD`=0, `DNAM`=1, `RNAM`=2, `SECT`=3, `EDEF`=4, `EREF`=5, `SDEF`=6,
   `SREF`=7, `PROG`=10); the keyed file's head‑record name (`B$HEADKEY.TEXT`)
   is folded into the HEAD content, relocation is `PROG` with the
   `SUBTYPREL`=1 subtype (B$RELOC2, not a separate type), and `END`=`0o777` is
   a synthetic sentinel (the keyed file has none — its keys delimit records).

   - **Part 1 (DONE):** `xuo_ouinit`/`sectbuild`/`prgm`/`headname`/
     `head_severity`/`outterm` → records **HEAD(0)** (id `GMAP`, version `B00`,
     severity, start, *and the object‑unit name* — set by `OUNAME`, default
     `NO‑NAME`), **SECT(3)** per control section (type/bound/size/name; section
     0 = CODE, 1 = LITERALS), **PROG(10)** subtype 0 of contiguous code words
     (grouped by section+offset; a gap starts a new record), **END**. Wired
     into `GEN` PASS2 (non‑forward‑ref words via `xuo_prgm`) and the init/END
     flow; written to the `.obj` (M$OU) when `OU` is on. Verified by
     `tests/bmapobj.gmap` → `tests/expected/bmapobj.obj` (byte‑walk; an absolute
     program so no relocation is needed yet).

   - **Part 2 (DONE):** relocation directives — `xuo_reloc` emits a **PROG(10)
     subtype `SUBTYPREL`=1** record per relocated field, walking `GEN`'s fields
     with a bit offset `FB` (`STBIT`/`ENDBIT` = `FB`..`FB+NB-1`), exactly as
     SI61 GEN 1399–1447 calls `XUO$RELOC` (7‑arg when `EVALOP=0, 9‑arg with the
     value otherwise). The record carries `B$OBJECT_C`'s **B$RELOC2 general
     form**: payload `[subtype, key‑section, word0 = OPNDTYP(4)|EVALOP(4)|
     RELOCOP(4)|OPERAND(18), word1 = DISP(18)|STBIT(9)|ENDBIT(9)`(DISP 0, keyed
     at offset)`, word2 = VALUE` (only when `EVALOP≠0`)]. A field's second
     additive/subtractive relocation (R1+R2, in its `S` word with `EVALOP=0`)
     emits a second directive over the same bits. RELOC records are buffered
     (`REL_`) and written after all PROG records, before END. The compact
     1‑word `B$RELOC1` form is an encoder optimization we skip (general form is
     lossless).

   - **Part 3a (DONE):** realigned the stream's record TYPE codes to
     `B$OBJECT_C`'s `B$RECORDSUBS` (folded the OU name into HEAD, RELOC is now
     PROG subtype 1, END is a synthetic sentinel) so the def/ref records get
     their spec numbers (DNAM=1/RNAM=2/EDEF=4/EREF=5/SDEF=6/SREF=7).

   - **Part 3b (DONE):** the definition records.  CASE 14 (`case_defref`)
     decodes the opcode's `OP.VAL` (the first 18 REL bits) and flags each named
     symbol: ENTDEF→`F.EDEF`, SYMDEF→`F.SDEF` (both via `symtab(def=false)`, the
     reference path now merging the XDEF flags onto the locally‑defined label,
     SI61 4675); the first ENTDEF (else first SYMDEF) names the object unit.  A
     symbol‑table sweep at `xuo_outterm` emits **DNAM(1)** (the def names,
     B$DNAME dense packing, NPOINTER = word displacement), **EDEF(4)** (2‑word
     B$EDEF: `LFLAGS|SECTNUM|OFFSET` + `NPOINTER|PRI|ALT|CHECK|CST=1|NPARAM`;
     PRI marks the OU‑name entry), and **SDEF(6)** (2‑word B$SDEF: address form
     for a section‑relative symbol, CONSTNT form for an absolute EQU).  Also
     fixed the SECT size to track the high‑water PC (a trailing BSS now grows
     the section) and `xuo_headname` to NUL‑terminate.  Verified by
     `tests/bmapdef.gmap` → `tests/expected/bmapdef.obj` (byte‑walk).

   - **Part 3c (DONE):** the reference records.  CASE 14 with OP.AR=1
     (`symtab(def=true)`) defines ENTREF→OPNDTYP=OPEREREF / SYMREF→OPNDTYP=
     OPERSREF.  Between the passes, `assign_ext_numbers` walks the symbols in
     name order and gives each external ref its 0‑based EREF/SREF number (stored
     in `REL.OPERAND`), so the part‑2 RELOC directives a pass‑2 use emits carry
     OPNDTYP=OPEREREF/OPERSREF and OPERAND=that number.  The `xuo_emit_defs`
     sweep (same name order, so an entry's position equals its number) emits
     **RNAM(2)** (ref names), **EREF(5)** (1‑word B$EREF: `NPOINTER|SREF|ALT|
     CHECK|CST=1|NPARAM`), and **SREF(7)** (1‑word B$SREF: `NPOINTER|SREF|
     READ_ONLY|MBZ`), each NPOINTER a word displacement into RNAM.  Verified by
     `tests/bmapref.gmap` → `tests/expected/bmapref.obj` (byte‑walk: RNAM
     DATA/SUBR, EREF→SUBR, SREF→DATA, and the LDA/ADA RELOCs naming SREF#0/
     EREF#0 while TRA stays an OPERSECT relocation).

   - **Control sections — USE/BLOCK (7, DONE):** `case_use` (SI61 CASE 7)
     switches the current control section.  Each section keeps its own location
     counter in `OSECT[].pc` (the original parks it in `B$SECTION.MBZ`); USE
     parks the current PC and resumes the target's, so interleaved sections each
     advance independently.  The name is the operand — `USE name`→CODESECTION
     (OP.VAL=1), `BLOCK name`→LCOMSECTION (OP.VAL=4); `USE` alone is the blank
     default section 0; `USE PREVIOUS` returns to the prior section.  A section
     is created (`xuo_sectbuild`) on first mention in pass 1 (a new one in pass 2
     is error 6).  The base sections are now built unconditionally (not only when
     OU is on) so PC tracking + the listing's section column work without an
     object.  A label inherits `PCREL` (OPNDTYP=OPERSECT, OPERAND=section#), so
     PROG/RELOC/EDEF/SDEF records carry non‑zero section numbers and an
     inter‑section reference relocates against the target section.  A trailing
     `,type` on USE/BLOCK overrides the section type (`varscan` of the operand
     after the comma).  Verified by `tests/bmapsec.gmap` →
     `tests/expected/bmapsec.obj` (byte‑walk: code in section 0, VAL's data word
     in section COMM=2 — its type set to 0 by `BLOCK COMM,0` — and `LDA VAL`
     relocates against section COMM, while TRA START stays a section‑0
     relocation).

   - **SEGREF (9, DONE):** the SEGREF directive (type‑14, OPNDTYP=OPERSEGREF)
     gets a 0‑based number like EREF/SREF (`assign_ext_numbers`), and the
     `xuo_emit_defs` sweep emits a **SEGREF(9)** record (1‑word B$SEGREF:
     `NPOINTER` into RNAM); using the symbol relocates with OPNDTYP=OPERSEGREF,
     OPERAND=the segref number.  Covered by `tests/bmapref.gmap` (`STA SEG`).

   - **Part 4 — debug schema (DONE):** the DELTA‑debugger schema, gated by a new
     `‑g`/`--debug` flag (`opt_debug`).  In CP‑6 it is on unless `OPTIONS.ND`
     ("no debug"); the port keeps it off‑by‑default so ordinary objects stay
     lean and the other fixtures byte‑stable.  A symbol‑table sweep
     (`xuo_emit_dbg`, mirroring SI61 SYMTAB 4736 + the END VREBL pass 420) emits,
     after the PROG/RELOC records, for every defined non‑SET symbol: a
     **DBGNAM(15)** name plus either an **EXST(13)** statement entry (3‑word
     B$EXST, for a label in a CODESECTION — `SECTNUM|OFFSET`, `NPOINTER|LBE`,
     `COS|LINENUM|STTYPE`; STTYPE=1, LBE=0, LINENUM = the symbol's source‑card
     line) or a **VREBL(14)** variable entry (5‑word B$VREBL, for a
     data/absolute symbol whose address type `ATYPE[OPNDTYP]`/section‑type is
     non‑zero — `ADR.W|ADDRTYP|DATATYP=1|REF`, `LOGSIZ=36|LEVEL=ALIGN=1|OPNDTYP`,
     `IMPTR=‑1|OPERAND`, 0, `NPOINTER`).  One **LOGBLK(11)** descriptor frames
     them (`STLINE|ENLINE` from the line range, `NPOINTER=‑1|LEXLVL=1`,
     `STOFFST 0|STSIZE=#EXST`, `VAOFFST 0|VASIZE=#VREBL`).  Verified by
     `tests/bmapdbg.gmap` → `tests/expected/bmapdbg.obj` (byte‑walk: three code
     labels → EXST with their lines, the absolute `KON` → VREBL with addrtyp=3,
     each entry's NPOINTER addressing its DBGNAM name).

   - **Deferred:** the aggregate B$VREBLC/CA/CET/CSET type variants (BMAP emits
     only the scalar form), the B$LBNTRY0 compiler‑id header, and INTNTRY(12).

7. **Macros:** define/expand, DUP/IDRP/ETC, created symbols, parameter subst.

8. **Literals, forward references, listing + cross‑reference, options** (`UFR`
   resolution, `REFLINK`/xref, option parsing in the C CLI).

9. **Tests:** hand‑verified GMAP fixtures wired into `make test`, like the
   `dal*` cases; object‑unit byte checks against the spec.
