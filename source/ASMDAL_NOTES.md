# ASMDAL — notes on the C port

`asmdal.c` is a faithful C port of Dave Wagner's CP‑6 **ASMDAL** (`ASMDAL_SI61.XSI`,
© Bull HN Information Systems Inc., 1989), a two‑pass assembler for a subset of
**DAL** (DEC Assembly Language — i.e. PDP‑10 / DECSYSTEM‑20 MACRO‑style code).
ASMDAL was a CO430 *Systems Programming* course exercise; per its HELP file the
object it produces "CANNOT be used on a DEC system" — it emits CP‑6 object units
for an educational linker, not DEC executables. The 9‑bit octal opcodes it uses,
however, are the **real** PDP‑10 opcodes (ADD=270, MOVE=200, JRST=254, …), so
each assembled 36‑bit word is a valid PDP‑10 instruction.

## Building / running

    make asmdal                       # part of `make tools` and `make test`
    ./asmdal source.dal               # -> source.obj + source.lst
    ./asmdal src.dal -o a.obj -l a.lst
    ./asmdal src.dal obj.out lst.out  # positional: source [object [listing]]

No object file is written when the source has errors (faithful to the original);
the exit status is non‑zero in that case. The "ASMDAL A01 here at …" salutation
goes to stderr (the original's M$ME terminal write).

## Source format (column‑oriented, like classic MACRO)

A source line is fixed‑field, 0‑based columns:

    cols  0–5   LABEL      (6)   symbol, optional
    col   6     —          (1)   must be blank
    cols  7–12  MNEMONIC   (6)   opcode / JSYS alias / pseudo‑op
    cols 13–14  —          (2)   must be blank
    cols 15–30  OPERAND   (16)
    cols 31–    COMMENT          ';' may also start a comment in a field boundary

Operand grammar for a normal/I‑O instruction: `[AC,][@]VALUE[+/-BIAS][(XR)]`.
`AC`, `XR`, and the `VALUE`/device symbols are decimal numbers or symbols (a `.`
means "current location"); the `+/-BIAS` is decimal. Code must lie inside a
`TITLE … END` block.

### Pseudo‑ops
`TITLE sym` (opens the block, names it), `END [sym]` (closes; optional start
symbol), `EQU` (absolute equate), `ENTRY sym` / `EXTERN sym` (external symbols),
`OCT n` (one octal word), `DC n` / `DC hi:lo` / `DC sym` (one word; `:` splits
into two 18‑bit halves), `BLOCK n` (reserve n; one word emitted, location += n),
`Z` (one zero word).

## Instruction word formats (36 bits, MSB first)

    NORMAL:  OPCODE(9) ACC(4) IND(1) XR(4) VALUE(18)
    I/O:     7(3) DEV(7) OPCODE(3) IND(1) XR(4) VALUE(18)
    JSYS:    104(9) 0(9) NUMBER(18)

`op_type` in `asmdal_tables.h` selects the format (0 normal, 1 I/O, 2 JSYS).
The table is generated verbatim from the PL/6 source by `asmdal_tables.py`.

## Object‑unit format (what this port writes)

The original spooled M$OU records of 36‑bit words. This port serializes **each
36‑bit word as 5 bytes, big‑endian** (the top 4 bits are zero), and frames
records with a header word so the stream is self‑describing:

    WORD0 = TYPE(9) | LENGTH(9) | ADDRESS(18)

`LENGTH` is the record's **total** word count *including* WORD0. Record types:

| TYPE | meaning      | layout                                                        |
|------|--------------|---------------------------------------------------------------|
| 0    | code/text    | WORD0; word1 = 18× 2‑bit relocation codes; up to 18 code words |
| 1    | external defs| WORD0; word1 = 9× 4‑bit def/und flags; up to 9 × (name,value)   |
| 3    | local defs   | same shape as type 1                                            |
| 2    | trailer      | WORD0 only; ADDRESS = program start address                    |

* Relocation code (2 bits per code word): `00` absolute, `01` relocatable
  (local), `10` external reference, `11` BLOCK directive (value = count).

* A symbol entry is two words: 6 chars packed 9‑bit each (54 bits) then an
  18‑bit value. The def/und nibble in word1 is `0` = defined, `1` = undefined.

* For an external reference, the symbol's stored value is updated to the
  referencing location (the original builds a reference chain this way).

A reader walks the file by reading WORD0, taking `LENGTH` words, and repeating
until it sees TYPE 2. (See the decode loop used in the test notes below.)

## Verifying

Validation was by **byte‑exact object fixtures** plus hand‑checking every
instruction word's octal against the PDP‑10 opcode encodings
— `tests/dalsmoke.dal` and `tests/dalcover.dal` were verified this way and
snapshotted into `tests/expected/`; `tests/dalerr.dal` checks that an
erroneous source is rejected with no object. The listing prints each word
as two 6‑digit octal halves, which is the convenient artifact for hand‑verification.

To eventually exercise the encodings under **KLH10** (https://github.com/PDP-10/klh10):
the type‑0 code words are loadable PDP‑10 instructions. A hand‑off party can pull
the code record(s) out of the `.obj` (walk records by `LENGTH`, take the type‑0
text words from `ADDRESS` onward), deposit them into KLH10 memory at that address
and single‑step to confirm each instruction decodes/executes as the octal in the
listing predicts. JSYS calls (opcode 104) and the CP‑6‑style relocation/externals
are *not* meaningful to a bare DEC monitor, so verification is per‑instruction
(encoding), not whole‑program execution.

## Port notes (PL/6 → C)

* The pass‑1→pass‑2 **scratch token file** (M$TOKEN) becomes an in‑memory array
  of `struct token`. Symbol‑table memory (the original extended data segment 3
  one page at a time via M$GDS) becomes plain `malloc`.

* The symbol table is the original **Horowitz‑&‑Sahni AVL** insert, ported
  faithfully. (Output order is an in‑order walk, so it is sorted regardless.)

* CP‑6 intrinsics were reproduced from their call sites: `INDEX` returns the
  0‑based offset or the string length when not found; `CHAR_TO_SBIN` parses an
  optional sign then base‑N digits until a blank; `DECIMAL_TO_OCTAL` formats an
  18‑bit value as six octal digits.

* The 42 error codes keep the original ordering (their bit positions in the
  PL/6 `BIT(72)` error word == indices into the message table).
