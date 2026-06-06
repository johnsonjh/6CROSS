#!/bin/sh
# Bootstrap test harness for the ASMZ80 / ASM6502 Linux/gfortran port.
#
# Each tests/*.z80 is assembled with asmz80 and each tests/*.s with
# asm6502; the produced object unit is compared against the hand-verified
# fixture in tests/expected/.  The Z80 "hello" program is additionally
# linked, converted to a CP/M .COM, and executed on the tnylpo emulator
# to exercise the whole assemble -> link -> run pipeline.

ROOT=$(cd "$(dirname "$0")/.." && pwd)
ASMZ80="$ROOT/asmz80"
ASM6502="$ROOT/asm6502"
ASMDAL="$ROOT/asmdal"
BMAP="$ROOT/bmap"
CONV="$ROOT/ouconv"
TDIR="$ROOT/tests"
WORK=$(mktemp -d)
fail=0

check_obj()
{ # $1 = test name
  exp="$TDIR/expected/$1.obj"
  if [ -f "$exp" ]; then
    if diff -q "$exp" "$WORK/$1.obj" > /dev/null 2>&1; then
      echo "PASS  $1        	object unit matches fixture"
    else
      echo "FAIL  $1        	object unit differs:"
      diff "$exp" "$WORK/$1.obj" 2>&1 | sed 's/^/        /'
      fail=1
    fi
  else
    echo "----  $1        	(no fixture)"
  fi
}

check_scan()
{ # $1 = test name (BMAP phase-2 scan trace)
  exp="$TDIR/expected/$1.scan"
  if [ -f "$exp" ]; then
    if diff -q "$exp" "$WORK/$1.scan" > /dev/null 2>&1; then
      echo "PASS  $1        	scan trace matches fixture"
    else
      echo "FAIL  $1        	scan trace differs:"
      diff "$exp" "$WORK/$1.scan" 2>&1 | sed 's/^/        /'
      fail=1
    fi
  else
    echo "----  $1        	(no fixture)"
  fi
}

for src in "$TDIR"/*.z80; do
  [ -e "$src" ] || continue
  name=$(basename "$src" .z80)
  cp "$src" "$WORK/"
  (cd "$WORK" && "$ASMZ80" "$name.z80" > /dev/null 2>&1)
  check_obj "$name"
done
for src in "$TDIR"/*.s; do
  [ -e "$src" ] || continue
  name=$(basename "$src" .s)
  cp "$src" "$WORK/"
  (cd "$WORK" && "$ASM6502" "$name.s" > /dev/null 2>&1)
  check_obj "$name"
done
# ASMDAL: each tests/*.dal is assembled and its object unit compared against
# the hand-verified fixture (every 36-bit instruction word was checked by
# hand against the PDP-10 opcode table).  dalerr.dal is the error case.
for src in "$TDIR"/*.dal; do
  [ -e "$src" ] || continue
  name=$(basename "$src" .dal)
  [ "$name" = "dalerr" ] && continue
  cp "$src" "$WORK/"
  (cd "$WORK" && "$ASMDAL" "$name.dal" > /dev/null 2>&1)
  check_obj "$name"
done
# BMAP: each tests/*.gmap is run and compared against its hand-verified fixture.
#   bmapscan : scanner trace (--scan: location/op fields, OP.TYPE/VAL, the
#              DELSCAN/NEXTFLD tokenization).
#   bmapobj  : the relocatable object unit (.obj) for an absolute program -- the
#              HEAD/SECT/PROG/END records (B$OBJECT_C type codes), every word
#              checked by byte-walk.
#   bmaprel  : a section-relative program -- adds the PROG/RELOC records (type
#              10 subtype 1, B$RELOC2 general form incl. the 3-word EVALOP=MULT
#              case), byte-walked.
#   bmapdef  : entry/symbol definitions -- the DNAM names record plus the EDEF
#              (entry, PRI=1) and SDEF (address + constant forms) tables.
#   bmapref  : external references -- the RNAM names record and the EREF/SREF/
#              SEGREF entry tables, with RELOC directives naming them (OPNDTYP
#              OPEREREF/OPERSREF/OPERSEGREF, OPERAND = the 0-based entry number).
#   bmapsec  : control sections -- BLOCK opens a second section (its type set by
#              the `,n` override); code/data land in their own sections (each
#              with its own location counter); an inter-section reference
#              relocates against the target section #.
#   bmapdbg  : the debug schema (-g) -- DBGNAM names, an EXST statement entry
#              (with source line) per code label, a VREBL variable entry per
#              data/absolute symbol, and the LOGBLK descriptor framing them.
#   bmapflt  : DPS-8 floating-point DEC literals -- single (1.5/1E3/-0.5),
#              double (1.0D0 -> two words), and binary-scaled fixed (3B17).
#   bmapio   : specialized instruction families -- IO (IOTD/IOTP/TDCW, IOFORM
#              18|6|12) and ASCNT (ASFORM 16|11|1|8, the ,N flag).
#   bmapeis  : EIS instructions (MLR/CMPC/AD2D/AD3D) via MFSCAN -- the K-format
#              EFORM packing, opcode + MF fields, and the (ar,rl,id,reg) MF form.
#   bmapclm  : CLIMB family (ENTER/EXIT/GOTO/PMME) -- two words (IFORM + CL2FORM),
#              the OP.AR operand path, the parenthesized register modifier.
#   bmapdsc  : EIS operand descriptors (ADSC4/ADSC9/BDSC/VDSC/NDSC9) following an
#              EIS instruction, packed per the *DSCF formats; ADSC9 scales by 2,
#              VDSC takes a register modifier, IDS indexes the EIS's MF state.
#   bmapptr  : PTR (NSA pointer) -- four operands packed via BDSCF 18|2|4|12.
#   bmapnsa  : NSA descriptors (ODSC/DDSC/ODSB, type 35) + entry descriptor
#              (EDSC16, type 36) -- the DSCF/EDSCF formats, K-juggling, FLAGS.
#   bmapvec  : NSA vectors (VEC/SVEC/CVEC, type 34) -- VECFORM, the OP.VAL/256
#              word count, the FLAGS flag-name list (default 0o777, and (M,X)).
#   bmapmcp  : MICROP edit micro-ops (type 31) packed 4/word (QFORM) -- octal,
#              H/A BCD/ASCII chars, a repeat count, and the (MOPNAME,value) form.
#   bmapedc  : EDEC (type 42) decimal-edit compiler -- <count><A|P>[L]<number>
#              fields; A=9-bit ASCII, P=4-bit packed (2 nibbles/9-bit group).
#              The count phase tallies positions + decimal scale; the emit phase
#              zero-fills, places the sign overpunch, the digits, and (floating
#              point) the trailing scale byte via EDINS bit-insertion.
#   bmapdate : DATE (type 44) emits the M$TIME date word (6 BCD chars).  Run with
#              SOURCE_DATE_EPOCH=1614816000 (2021-03-04 UTC) so the build is
#              deterministic: "03/04/21" -> INSERT/XLATEV -> 0o610004000301.
#   bmapmac  : phase-7 macros -- MACRO/ENDM definition, #N positional parameter
#              substitution, and a macro that expands another (nested calls).
#   bmapdup  : DUP ncards,nreps -- the captured card block is emitted nreps times.
#   bmapif   : IFE/IFG/IFL/INE -- the next card is skipped when the test fails.
#   bmapidrp : IDRP -- the body repeats once per comma element of a list-valued
#              parameter (#k bound to each element in turn).
#   bmapcrsm : CRSM macro mode -- a reference beyond the supplied args (#9) is a
#              generated unique label _NNNN_, so a label-defining macro can be
#              called repeatedly (GENJ x2 -> _0000_/_0001_) without colliding.
#   bmapnidrp: IDRP refinements -- nested IDRP (an outer #1 block containing an
#              inner #2 block, body emitted per (#1,#2) pair) and the literal-list
#              form (IDRP (5,6,7) bound to the block's dummy parameter).
#   bmaplit  : phase-8 literals -- "=expr"/"=Oexpr" numeric literals interned
#              (deduped) into the LITERALS section (1); each reference relocates
#              against it.  =5 is shared by two instructions (one pool word).
#   bmapxlit : extended literals -- character (=HABCDEF -> BCD, =4AWXYZ -> ASCII)
#              and the LITORG pool flush (the =5 after LIT is a separate word).
#   bmapsyn  : phase-8 OPSYN -- "NEW OPSYN OLD" aliases NEW to op OLD; the alias
#              (LOADA->LDA, STOREQ->STQ) assembles identically to the original.
#   bmapxref : phase-8 cross-reference (XR option) -- the listing gains a
#              statement-# column, and a cross-reference table lists each
#              symbol's value, definition line (marked *), and reference lines.
#   bmaplst  : phase-8 listing control -- TTL title, LIST OFF/ON (code between is
#              assembled but not listed -- the PC advances), EJECT form feed.
#   bmapall  : phase-9 integration -- macros + literals + EQU + relocation in one
#              program; checks feature interactions (e.g. =5 deduped across the
#              macro-adjacent code, a code-label reference relocated correctly).
#   bmaperr  : phase-9 error handling -- an invalid op makes bmap exit nonzero and
#              flag severity 4 in the HEAD record (checked behaviorally, below).
#   others   : the real octal listing (location + 36-bit words + source), every
#              word checked by hand against bmap_opcodes.h + IFORM/XFORM.
if [ -x "$BMAP" ]; then
  for src in "$TDIR"/*.gmap; do
    [ -e "$src" ] || continue
    name=$(basename "$src" .gmap)
    [ "$name" = "bmaperr" ] && continue # error case: checked behaviorally below
    cp "$src" "$WORK/"
    epoch= # SOURCE_DATE_EPOCH for deterministic DATE
    opt=   # trailing CP-6 option list, e.g. "(LS,XR)"
    case "$name" in
    bmapscan)
      flag=--scan
      chk=check_scan
      ;;
    bmapxref)
      flag=
      chk=check_scan
      opt="(LS,XR)"
      ;; # phase 8: cross-reference
    bmaplst)
      flag=
      chk=check_scan
      opt="(LS)"
      ;; # phase 8: listing control
    bmapdbg)
      flag=-g
      chk=check_obj
      ;;
    bmapobj | bmaprel | bmapdef | bmapref)
      flag=
      chk=check_obj
      ;;
    bmapsec | bmapflt | bmapio | bmapeis)
      flag=
      chk=check_obj
      ;;
    bmapclm | bmapdsc | bmapptr)
      flag=
      chk=check_obj
      ;;
    bmapnsa | bmapvec | bmapmcp)
      flag=
      chk=check_obj
      ;;
    bmapedc)
      flag=
      chk=check_obj
      ;;
    bmapdate)
      flag=
      chk=check_obj
      epoch=1614816000
      ;; # 2021-03-04 UTC
    bmapmac | bmapdup | bmapif | bmapidrp)
      flag=
      chk=check_obj
      ;; # phase 7: macros
    bmapcrsm)
      flag=
      chk=check_obj
      ;; # CRSM auto-symbols
    bmapnidrp)
      flag=
      chk=check_obj
      ;; # nested/literal-list IDRP
    bmaplit)
      flag=
      chk=check_obj
      ;; # phase 8: literals
    bmapxlit)
      flag=
      chk=check_obj
      ;; # extended literals
    bmapsyn)
      flag=
      chk=check_obj
      ;; # phase 8: OPSYN
    bmapall)
      flag=
      chk=check_obj
      ;; # phase 9: integration
    *)
      flag=
      chk=check_scan
      ;;
    esac
    (cd "$WORK" && SOURCE_DATE_EPOCH="$epoch" "$BMAP" $flag "$name.gmap" $opt > /dev/null 2>&1)
    $chk "$name"
  done
  # BMAP phase-3 self-test: CONVERT (integer conversion) and the symbol-table
  # AVL (insert/sort/lookup/multiply-defined) exercised directly, since no
  # source-level caller reaches them until VARSCAN (phase 4)/INST (phase 5).
  (cd "$WORK" && "$BMAP" -t -l bmapconv.scan > /dev/null 2>&1)
  check_scan "bmapconv"

  # BMAP error handling: an invalid operation is flagged -- bmap must exit
  # nonzero and record severity 4 in the object's HEAD record (the HEAD header
  # word's ARG field = the low byte of the .obj's 5th byte).  The object is
  # still written, per the CP-6 model.
  if [ -f "$TDIR/bmaperr.gmap" ]; then
    cp "$TDIR/bmaperr.gmap" "$WORK/"
    (cd "$WORK" && "$BMAP" bmaperr.gmap > /dev/null 2>&1)
    rc=$?
    sev=$(od -An -tu1 -j4 -N1 "$WORK/bmaperr.obj" 2> /dev/null | awk '{print $1+0}')
    if [ "$rc" -ne 0 ] && [ "$sev" = "4" ]; then
      echo "PASS  bmaperr        	invalid op flagged (exit $rc, HEAD severity 4)"
    else
      echo "FAIL  bmaperr        	expected nonzero exit + severity 4 (rc=$rc sev=$sev)"
      fail=1
    fi
  fi

  # BMAP real-world byte-walk: the BMAP subroutine library BMAP_SIG.XSI is real
  # GMAP -- a DEFREGS macro that uses IDRP over a register list with #1#2 label
  # concatenation, an ADJUST macro full of string-compare IFs (IFE/INE
  # '#3','=0'), USE control sections, the full instruction set (basic, INDEX,
  # EIS, descriptors), OCT data and an EDEF.  It assembles (exit 0) to exactly
  # the snapshot object, which was hand-walked: every basic-instruction opcode
  # checked against bmap_opcodes.h (173/173), the INDEX opcodes (base+register)
  # and OCT data words (25/25) verified, the macro-generated register EQUs
  # (P0-7/X0-6/@AUTO) confirmed, and the HEAD severity/section/EDEF/relocation
  # structure checked -- EIS/descriptor encodings ride on bmapeis/bmapdsc.
  if [ -f "$ROOT/.original/BMAP_SIG.XSI" ]; then
    cp "$ROOT/.original/BMAP_SIG.XSI" "$WORK/bmapsig.gmap"
    (cd "$WORK" && "$BMAP" bmapsig.gmap > /dev/null 2>&1)
    rc=$?
    if [ "$rc" -eq 0 ]; then
      check_obj bmapsig
    else
      echo "FAIL  bmapsig        	BMAP_SIG.XSI failed to assemble (rc=$rc)"
      fail=1
    fi
  fi
else
  echo "SKIP  bmap-scan     	(bmap not built)"
fi

# Execution test: the Z80 hello program prints its message under tnylpo.
if command -v tnylpo > /dev/null 2>&1 && [ -f "$WORK/hello.obj" ]; then
  (cd "$WORK" && "$CONV" hello.obj -o hello.com > /dev/null 2>&1)
  out=$(cd "$WORK" && tnylpo hello.com 2> /dev/null | tr -d '\r')
  if [ "$out" = "HELLO FROM CP-6 ASMZ80" ]; then
    echo "PASS  hello-run     	tnylpo printed: $out"
  else
    echo "FAIL  hello-run     	tnylpo printed: '$out'"
    fail=1
  fi
else
  echo "SKIP  hello-run     	(tnylpo not found)"
fi

# Execution test: the 6502 run6502 program computes 5*3=15 into $80.
if [ -x "$ROOT/sim6502" ] && [ -f "$WORK/run6502.obj" ]; then
  "$CONV" "$WORK/run6502.obj" -o "$WORK/run6502.bin" > /dev/null 2>&1
  out=$("$ROOT/sim6502" "$WORK/run6502.bin" -l 0x200 -d 0x80)
  if [ "$out" = "0F" ]; then
    echo "PASS  run6502-run   	sim6502: \$80 = 0F (5*3=15)"
  else
    echo "FAIL  run6502-run   	sim6502: \$80 = '$out' (expected 0F)"
    fail=1
  fi
else
  echo "SKIP  run6502-run    	(sim6502 not built)"
fi

# Round-trip: asmz80 -> msaz80 (disassemble) -> asmz80 must reproduce the obj.
if [ -x "$ROOT/msaz80" ]; then
  for name in smoke hello cover; do
    [ -f "$WORK/$name.obj" ] || continue
    "$ROOT/msaz80" "$WORK/$name.obj" -o "$WORK/${name}_d.z80" < /dev/null 2> /dev/null
    (cd "$WORK" && "$ASMZ80" "${name}_d.z80" > /dev/null 2>&1)
    if diff -q "$WORK/$name.obj" "$WORK/${name}_d.obj" > /dev/null 2>&1; then
      echo "PASS  $name-rt        	round-trip asmz80->msaz80->asmz80 identical"
    else
      echo "FAIL  $name-rt        	round-trip differs"
      fail=1
    fi
  done
else
  echo "SKIP  msaz80-rt     	(msaz80 not built)"
fi

# Round-trip: asm6502 -> msa6502 (disassemble) -> asm6502 must reproduce.
if [ -x "$ROOT/msa6502" ]; then
  for name in smoke6502 run6502; do
    [ -f "$WORK/$name.obj" ] || continue
    "$ROOT/msa6502" "$WORK/$name.obj" -o "$WORK/${name}_d.s" < /dev/null 2> /dev/null
    (cd "$WORK" && "$ASM6502" "${name}_d.s" > /dev/null 2>&1)
    if diff -q "$WORK/$name.obj" "$WORK/${name}_d.obj" > /dev/null 2>&1; then
      echo "PASS  $name-rt    	round-trip asm6502->msa6502->asm6502 identical"
    else
      echo "FAIL  $name-rt    	round-trip differs"
      fail=1
    fi
  done
else
  echo "SKIP  msa6502-rt    	(msa6502 not built)"
fi

# ASMDAL error handling: a source with errors must be rejected (nonzero exit)
# and must NOT leave an object file behind.
if [ -x "$ASMDAL" ] && [ -f "$TDIR/dalerr.dal" ]; then
  cp "$TDIR/dalerr.dal" "$WORK/"
  (cd "$WORK" && "$ASMDAL" dalerr.dal > /dev/null 2>&1)
  rc=$?
  if [ "$rc" -ne 0 ] && [ ! -f "$WORK/dalerr.obj" ]; then
    echo "PASS  dalerr        	errors rejected (exit $rc, no object written)"
  else
    echo "FAIL  dalerr        	expected nonzero exit and no object (rc=$rc)"
    fail=1
  fi
else
  echo "SKIP  dalerr        	(asmdal not built)"
fi

rm -rf "$WORK"
if [ "$fail" -eq 0 ]; then echo "ALL TESTS PASSED"; else
  echo "SOME TESTS FAILED"
  exit 1
fi
