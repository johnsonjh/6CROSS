# MSAZ80 (Z80 reverse‑assembler) — PL/6 → C port notes

Reverse‑engineered from `MSAZ80_SI6.XSI` (Z80 decoder) + the shared engine
`MSA_C1..C5.XSI`. This documents the design for the C port (`msaz80.c`).

## What it is
A flow‑tracing Z80 disassembler. Reads the **ASMZ80 object‑unit format**
(`:aabbbbcc[dd]…ee` records — exactly what our `asmz80` emits), loads it into
a simulated 64K memory, traces execution from entry points to separate code
from data, and emits **re‑assemblable ASMZ80 source**. So `asmz80` → `.obj`
→ `msaz80` → source → `asmz80` is a round‑trip.

## Memory model (MSA_C1 CELL, indexed `MEM$ + addr*2` — 2 bytes/cell)
Per byte: `VALUE` (9‑bit: 0–255 = data, **256 = reserved/BSS**), and flags
`SOME` (loaded), `RD`, `WR`, `JMP`, `JSR`, `EXEC`, `VISIT`, and `BNO` (2‑bit:
offset of this byte within its instruction). In C: `unsigned short val[65536]`
+ a `unsigned char flag[65536]` bitset + `unsigned char bno[65536]`.

## Phase 1 — load (MSA_C3 27–134)
Parse each input record. For `:`‑records: verify checksum (Σ bytes mod 256 = 0).
Fields: `aa`=count (pos 1), `bbbb`=addr (pos 3), `cc`=type (pos 7), data at 9.
- `cc` high bit set (80/81/82): symbol. `TYPE=cc‑128` (0=DEF/80, 1=ENT/81,
  2=REF/82). value=`bbbb` (0 if 82). 8 name bytes follow.
- low nibble of `cc`: 0=data (store bytes, set SOME), 1=entry (`BEGIN=addr`),
  5=reserve (`ADRS`=count; mark cells VALUE=256, SOME, VISIT).
After EOF: sort symbols by value (REF/type‑2 last). If `BEGIN>=0`, auto‑trace
from it (`HOW=1`); else prompt (Phase 2).

## Trace — Phase 2 (MSA_C3 226 + MSA_C4 6–98)
`INSTR(addr,HOW)`: set JMP (HOW=1) or JSR (HOW=2) on the cell. If `!SOME` or
already `VISIT` → FINDIT. `I=TABLE1(val)`; `TYPE=I/8; LEN=I%8`. For Z80, **TABLE1
has negative entries** for prefixes CB/DD/ED/FD → resolved via the snippet in
MSAZ80_SI6 125–160 using TABLE3/TABLE4 (CASE -1/-2/-3). Mark each of LEN bytes
VISIT, EXEC, BNO=offset (if a byte isn't SOME, the instruction runs into the
weeds → undo EXEC, FINDIT). Then `CASE(TYPE)` for flow:
```
 0 fall‑through        1 cond rel‑branch→tag JMP    2 uncond rel→follow
 3 cond jp→tag JMP     4 uncond jp→follow           5 cond call→tag JSR
 6 uncond call→follow  7 RST→tag JSR(addr=op&070)   8 end‑of‑chain→FINDIT
 9 RD long ref        10 WR long ref               11 RD pg0  12 WR pg0
13 RD/WR long         14 RD/WR pg0                 15 uncond rel‑call→tag JSR
16/17/20 8748 3/8     18 uncond same‑page→follow   19 cond same‑page→tag JMP
```
After non‑following cases: `LOCTR += LEN; INSTR`. FINDIT (MSA_C4 101): scan
memory; resume at any SOME, !VISIT cell tagged JMP/JSR; when none, print
"`nnn byte(s) out of mmmm accounted for (zz%)`" and prompt.

Target fetchers (MSA_C5): `GETADRS(L)=mem[L]+mem[L+1]*256`; rel = signed
`mem`; RST addr=`op&070`.

## Z80 decode tables (MSAZ80_SI6)
- `TABLE0[256]` CHAR(4): shorthand opcode names (for the M/D dump only).
- `TABLE1[256]`: `type*8+len`; **negative** = prefix (‑1 CB‑like, ‑2 ED, ‑3 DD/FD).
- `TABLE3[256]`, `TABLE4[256]`: type*8+len for the second byte after a prefix.
- `REG1[8]`=B,C,D,E,H,L,(HL),A; `REG2[4]`=BC,DE,HL,SP; `REG3[4]`=BC,DE,HL,AF;
  `COND[8]`=NZ,Z,NC,C,PO,PE,P,M; `MRG[8]`=ADD,ADC,SUB,SBC,AND,XOR,OR,CP;
  `SP1`,`SP2`,`SH` (shift/rotate, block ops).

## Format decode (MSAZ80_SI6 162–552) — produces the mnemonic line
A big `CASE(I/64)` → `CASE(I%16 / I%8)` building `OUBUF` (label col 1,
mnemonic col 9, operand col 18). For the C port we build `mnem` + `oper`
strings (content‑faithful; re‑assemblable). Operand helpers (MSA_C5):
- `CVS(addr)` → a **label**: symbol name if one is defined at that address,
  else `L%04X`. (This is what makes output re‑assemblable.)
- `CVX(val)` → an **immediate/hex**: `$%02X` (≤255) or `$%04X` (16‑bit), or a
  symbol name if one equals the value. (`$` = hex in ASMZ80.)
- `MEMREF` → CVS of a NEXTTWO 16‑bit operand (handles mid‑instruction `+n`).
- `SDISP(d)` → signed **decimal** displacement for `(IX+d)`.
- `NEXTONE`/`NEXTTWO` advance LOCTR and fetch 1/2 (LE) operand bytes.

## Phase 3 — emit source (MSA_C3 224 + MSA_C5 6–33)
Header `NAME "file"`, then `DEF/REF` lines for symbols. Walk memory:
- referenced cell (RD/WR/JMP/JSR) → emit a `Lxxxx` label (+ `; ‑‑‑‑RWJC`
  comment); if `!SOME` → `EQU $addr` (external).
- new origin → `SKIP 1` / `ORG $addr`.
- `!EXEC` or `BNO>0` (data / mid‑instruction): `VALUE<256` → `DATA $xx…`
  (up to 4 bytes, `C'…'` ASCII); `VALUE=256` → `DEFS count`.
- instruction (EXEC, BNO=0) → the format decode.
Finally `EQU` lines for symbols, then `END [entry]`.

## CP‑6 services to replace
`M$READ/M$WRITE` (keyed‑file I/O) → stdin/stdout/files; `M$GDS` (get storage)
→ static arrays; prompts → stdin command loop (C/J/P/M/D/END/QUIT); `BREAK`
→ ignore. Input records read from a file; output source written to a file.

## C port plan
1. loader (Phase 1) + memory/flags + symbol table.  ✅ first
2. trace‑decode (TABLE1/3/4 + flow) → trace engine.
3. format‑decode (the big CASE) → re‑assemblable text.
4. Phase 3 emit + driver (entry from 01 record / `‑e` / stdin commands).
5. round‑trip test: `asmz80 p.z80` → `msaz80 p.obj` → `asmz80` → compare bytes.
