/*
 * bmap.c - CP-6 BMAP: the GMAP (Honeywell GCOS / DPS-8, 36-bit) Macro
 * Assembly Program.  C port of Tom Martin's PL/6 BMAP_SI61.XSI
 * (Copyright (c) Bull HN Information Systems Inc., 1990), part of the CP-6
 * cross-assembler suite Linux port.
 *
 * ============================ COMPLETE ============================
 * This is the full assembler (all 9 phases of the plan in BMAP_NOTES.md): the
 * GMAP option list + M$SI/M$UI/M$OU/M$LO/M$SO file mapping; the BMAP_COMMON
 * globals (BMAP_DA1) as C structs; the column card scanner (READCARD/SCANOP/
 * DELSCAN/NEXTFLD) with op lookup by bsearch over the generated bmap_ops[];
 * the AVL symbol table + SYMTAB; CONVERT (integer/float/scaled); the VARSCAN
 * expression evaluator with REL relocation; GEN/GENLOC/GENVAL; the entire INST
 * instruction set (basic 1-6, data 13/15/16/17/18, float, IO/ASCNT/EIS/CLIMB/
 * descriptors/NSA/MICROP/EDEC/DATE); the mainline pseudo-ops (BSS/ORG/EVEN/
 * OUNAME/DEF/REF/USE/BLOCK); the XUO$ object-unit writer (HEAD/DNAM/RNAM/SECT/
 * EDEF/EREF/SDEF/SREF/SEGREF/PROG+RELOC, and with -g
 * LOGBLK/EXST/VREBL/DBGNAM); the macro processor (MACRO/ENDM, #N substitution,
 * CRSM auto-symbols, DUP, the IFE/IFG/IFL/INE conditionals,
 * nested/literal-list IDRP); literals (=expr, float, character, LITORG)
 * interned into the LITERALS section; OPSYN; and the XR cross-reference +
 * listing-control directives (LIST/DETAIL/.../EJECT/TTL).
 *
 * `bmap prog.gmap` writes an octal listing (M$LO) and, when OU is on, a
 * relocatable object unit (.obj); `--scan` writes a scanner trace; `-g` adds
 * the debug schema.  The object is a deterministic self-framed serialization
 * of B$OBJECT_C's record contents (see BMAP_NOTES phase-6 §) -- not the CP-6
 * keyed-file container, as there is no CP-6 linker here to consume it.
 *
 * Validation is by hand-verified byte-walk fixtures (in tests/expected) plus
 * the real BMAP subroutine library .original/BMAP_SIG.XSI, which assembles to
 * a byte-verified object (tests/run_tests.sh `bmapsig`).  Deferred
 * refinements, all optional, are listed in BMAP_CONTINUE.md (=M/=V literals,
 * IF string ordering, listing pagination, FR/UFR fixup, aggregate VREBL,
 * SEGDEF).
 *
 * Target is Honeywell DPS-8, NOT PDP-10, so KLH10 does not apply; a dps8m
 * simulator could run the output someday, out of scope here.  The opcode table
 * lives in bmap_opcodes.h, generated verbatim from BMAP_DA2.XSI by
 * bmap_opcodes.py (787 entries, sorted for bsearch), so the instruction
 * encodings match the original bit-for-bit.
 */

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "bmap_asciitbl.h"
#include "bmap_opcodes.h"

typedef uint64_t W; /* a 36-bit CP-6/GCOS word */
#define M36 0xFFFFFFFFFULL
#define M18 0777777ULL

/* ----------------------------------------------------- delimiter codes ----
 * The %EQU delimiter values from BMAP_DA1.XSI (lines 9-34).  DELSCAN's
 * DELTBL maps a source character to one of these (0 = not a delimiter). */
enum
{
  D_PLUS = 1,
  D_MINUS = 2,
  D_TIMES = 3,
  D_DIV = 4,
  D_LPAR = 5,
  D_RPAR = 6,
  D_LB = 7,
  D_RB = 8,
  D_COMMA = 9,
  D_BLANK = 10
};
static const char *const del_name[]
    = { "0",    "PLUS", "MINUS", "TIMES", "DIV",  "LPAR",
        "RPAR", "LB",   "RB",    "COMMA", "BLANK" };

/* ------------------------------------------------ relocation / object ----
 * Operand, evaluation and relocation operator codes (B$OBJECT_C, CP-6
 * listings .original/B$OBJECT_C.txt) + the operand types from BMAP_DA1.  Used
 * by the REL packet below; the section/bound codes belong to the object writer
 * (phase 6) but are listed here for reference. */
enum
{ /* REL.OPNDTYP */
  OPERSECT = 1,
  OPEREREF = 2,
  OPERSREF = 3,
  OPERCONST = 4,
  OPERSEGDEF = 5,
  OPERSEGREF = 6,
  OPERUNDEF = 9,
  OPERREL = 15
};
enum
{ /* control-section types (B$OBJECT_C) */
  DATASECTION = 0,
  CODESECTION = 1,
  UCOMSECTION = 2,
  RLCOMSECTION = 3,
  LCOMSECTION = 4,
  DCBSECTION = 5,
  ROSECTION = 6
};
enum
{ /* REL.EVALOP */
  EVALOPIGNORE = 0,
  EVALOPADD = 1,
  EVALOPSUB = 2,
  EVALOPMULT = 3,
  EVALOPDIV = 4,
  EVALOPSHFTR = 5,
  EVALOPSHFTL = 6,
  EVALOPRPT = 7,
  EVALOPFREF = 8
};
enum
{ /* REL.RELOCOP */
  RELOCOPADD = 1,
  RELOCOPSUB = 2,
  RELOCOPMULT = 3,
  RELOCOPDIV = 4,
  RELOCOPSTORER = 14,
  RELOCOPSTOREL = 15
};

/* The relocation packet (BMAP_DA1.XSI %REL, lines 89-126).  Kept here as a
 * struct of logical fields; the 36-bit object-word packing (and the
 * UNDEF='004401'O / XDEF='14'O bit constants) is deferred to the XUO$ writer
 * (phase 6), where it is verified against B$OBJECT_C.  Two relocation words
 * are modelled: the primary (opndtyp/evalop/relocop/operand + value) and the
 * second word `s` (s_opndtyp/s_relocop/s_operand) used when evalop != 0. */
struct rel
{
  /* F flags */
  unsigned f_equ : 1, f_set : 1, f_edef : 1, f_sdef : 1, f_defed : 1,
      f_refed : 1;
  int opndtyp, evalop, relocop, operand; /* primary word */
  int disp, stbit, endbit;               /* W2 (field placement) */
  int value;                             /* value for EVALOP */
  int s_opndtyp, s_relocop, s_operand;   /* second relocation word */
};
/* %UNDEF: the relocation of a not-yet-defined symbol (OPNDTYP = OPERUNDEF). */
static const struct rel REL_UNDEF
    = { 0, 0, 0, 0, 0, 0, OPERUNDEF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

/* XCARD is CHAR(256); LINE.CARD is CHAR(140) (BMAP_DA1 232/399). */
#define XCSIZE 256
#define CARDSIZE 140
#define MAXSYM 30 /* symbols up to 30 chars (HELP) */

/* ------------------------------------------------- BMAP_COMMON globals ----
 * Ported from BMAP_DA1.XSI lines 148-403.  Only the state phase 2 needs is
 * modelled; the SPOOL/MPOOL dynamic pools, packet trees and relocation words
 * arrive with the later phases (they will use malloc, not the CP-6 page
 * allocator).  Original names are kept so the later ports read 1:1. */

/* Assembly options (DA1 OPTIONS, lines 362-371). */
static struct
{
  int nacs; /* # accounts to search (SRCH) */
  int ls;   /* listing output */
  int nd;   /* no debug schema */
  int ou;   /* object output */
  int p2;   /* 2-pass assembly */
  int so;   /* source output */
  int ui;   /* update input */
  int xr;   /* cross-reference listing */
  int lu;   /* list updates */
} OPTIONS;

/* Listing on/off flags (DA1 LISTING, lines 238-240 + the %SUB names 42-50). */
static struct
{
  int detail, list, pmc, hexfp, crsm, floatf, pcc, ref, refma;
} LISTING;

/* Scanner state (DA1 194-291,399-401). */
static char XCARD[XCSIZE + 1]; /* current card image (blank padded) */
static int XCARDL;             /* length (chars) of meaningful card */
static int CURRCH;             /* index of 1st char of current field */
static int NEXTCH;             /* index of next delimiter */
static int DEL;                /* delimiter code at NEXTCH */
static char KEY[16]; /* key (line number) of current card; CHAR(10) */

/* Location (label) field.  The original packs LOC(0:5) as 6-bit ASCII via
 * CONSYM; here it is kept as text (uppercased to match the 6-bit case fold,
 * which maps a-z onto A-Z) plus its length. */
static char LOC[MAXSYM + 1];
static int LOCSZ; /* size of location field, 0 = none */

/* The current op-code packet OP (DA1 295-353).  In the original OP$ points
 * into the SPOOL tree; here it points into the generated bmap_ops[] table (or
 * at the NONOP sentinel below). */
static const struct bmap_op *OP;

static int PASS2;        /* -1 = 1-pass, 0 = pass 1, 1 = pass 2 */
static int PC;           /* program counter (bumped by GEN; phase 5) */
static struct rel PCREL; /* program-counter relocation (DA1 378) */

/* Counters (DA1 various). */
static int CARD_COUNT, CRD_COUNT = -1, STMNTCT, RECORDCT, TERRCT, ERRSEV;
static int ERRCT, ERRNUM[5]; /* per-line errors (DA1 196-197) */
/* TTLDAT (SI61 137): the title date word, BCD'NO TTL' until a TTL directive
 * sets it (TTL is a phase-8 listing op, still stubbed -- so DATE,1 would read
 * this constant; the DATE mnemonic itself is OP.VAL=0 and uses the clock). */
static W TTLDAT = 0454620636343ULL;

/* Sentinel ops the mainline references by SPOOL index in the original
 * (%EQU NONOP/END/... in DA1).  Resolved from bmap_ops[] at startup. */
static const struct bmap_op *OP_NONOP; /* blank / unknown operation */
static const struct bmap_op *OP_END;   /* END pseudo-op (type 9) */
/* synthetic op SCANOP points OP at for a macro call (SI61 registers macros in
 * the op tree with OP.TYPE=21, OP.PRFS='01'B): prfs=1 so a label on the call
 * line defines PC, type 21 dispatches to the macro-call handler. */
static const struct bmap_op MACRO_OP = { "", 0, 0, 0, 0, 1, 21 };

/* ----------------------------------------------------- symbol table -------
 * The SYM packet (BMAP_DA1.XSI 130-146) lived in the SPOOL as a balanced
 * tree; here it is a malloc'd AVL of `struct sym` keyed by the symbol name.
 * The 6-bit-ASCII packed ordering the original's TEST compares is a monotonic
 * remap of ASCII over the symbol charset (. $ @ _ 0-9 A-Z), so strcmp on the
 * uppercased name gives the identical order.  `defined` tracks DEF vs the
 * undefined-reference state (REL_UNDEF). FRROOT/UREF/REFLINK (forward-ref and
 * cross-reference chains) are phases 6/8 and are omitted for now. */
struct sym
{
  char name[MAXSYM + 1]; /* uppercased symbol, <=30 chars */
  int value;             /* SYM.VAL */
  struct rel r;          /* SYM.R relocation/flags */
  int defined;           /* 0 = undefined reference only, 1 = defined */
  int line;              /* source card # where defined (debug LINENUM) */
  int bf;                /* AVL balance factor -1/0/+1 */
  struct sym *l, *rt;    /* tree links (SYM.LINK.L / .R) */
  int *refs;             /* cross-reference: statement #s referencing it */
  int nref, caprefs;     /* (collected in pass 2 when the XR option is on) */
};
static struct sym *SYMROOT_; /* root of the symbol tree (SYMROOT) */
static int NSYM;             /* number of symbols (for the dump) */

/* Append statement number `ln` to a symbol's cross-reference list (SI61's REF
 * chain), skipping a consecutive duplicate (a statement that references the
 * symbol more than once). */
static void
sym_addref (struct sym *s, int ln)
{
  if (s->nref > 0 && s->refs[s->nref - 1] == ln)
    {
      return;
    }

  if (s->nref >= s->caprefs)
    {
      s->caprefs = s->caprefs ? s->caprefs * 2 : 8;
      s->refs = (int *)realloc (s->refs, (size_t)s->caprefs * sizeof *s->refs);
    }

  s->refs[s->nref++] = ln;
}

/* Character classification tables, built from BMAP_DA1 at startup:
 *   NONBLK  - SI61 NONBLK (DA1 292): 1 for printable ASCII 33..126.
 *   DELTBL  - DELSCAN's DELTBL (SI61 1264): char -> delimiter code. */
static unsigned char NONBLK[256];
static unsigned char DELTBL[256];
static unsigned char DIGITPEDB[256]; /* CONVERT digit/'.'/E/D/B table */
static unsigned char DECTBL[256];   /* DEC/OCT subfield class (3=expr,2=num) */
static unsigned char CONTROLS[256]; /* VFD field-type letter -> code */
static unsigned char NONDGT_[256];  /* EDEC SEARCH: A->9, P->4, digit->0 */
static unsigned char DIGITPMPE_[256]; /* EDEC classifier: digit/+/-/./E */

/* Saved scanner cursor right after READCARD, so the trace can re-walk the
 * variable field without the (stubbed) handlers having consumed it. */
static int VAR_CURRCH0, VAR_DEL0;

/* The whole source (M$SI), read once into memory; each pass replays it.  This
 * stands in for the CP-6 keyed scratch file. */
static char **src_lines;
static int src_n, src_cap, src_cur;

/* portable strdup (strdup is POSIX, not ISO C -- keep -std=c11 clean) */
static char *
dupstr (const char *s)
{
  size_t n = strlen (s) + 1;
  char *p = (char *)malloc (n);

  if (p)
    {
      memcpy (p, s, n);
    }

  return p;
}

/* ------------------------------------------------------ phase 7: macros ---
 * Behavioral C-native macro engine (see BMAP_NOTES "phase 7").  SI61 keeps
 * macro prototype text in the dynamic MPOOL word-pool and a MAC stack; this
 * port keeps each macro's body as plain card strings and expands a call by
 * pushing the #N-substituted body onto an expansion stack that the card reader
 * (next_phys_line) drains ahead of the file.  Expansion is redone each pass --
 * exactly as SI61's READCARD MAC stack does -- so DUP/IF operands that depend
 * on symbol values stay correct, and the shared src_lines[] is never mutated.
 * DUP and IDRP push synthetic frames the same way; IF sets a skip count. */
struct macro
{
  char *name;
  char **body;
  int nbody;
};
static struct macro *MACROS;
static int n_macros, cap_macros;

struct expframe
{
  char **lines;
  int n, cur;
};
#define MAXEXP 64 /* macro/DUP/IDRP nesting depth */
static struct expframe EXPSTK[MAXEXP];
static int exp_depth;
static const struct macro *CUR_MACRO; /* macro SCANOP matched (-> type 21) */
static int SKIPCT; /* IF: # of following cards still to skip */

static struct macro *
macro_find (const char *name)
{
  int i;

  for (i = 0; i < n_macros; i++)
    {
      if (!strcmp (MACROS[i].name, name))
        {
          return &MACROS[i];
        }
    }

  return NULL;
}

/* Register (or, on pass 2, replace) macro `name` with the captured body; takes
 * ownership of body and its strings. */
static void
macro_add (const char *name, char **body, int nbody)
{
  struct macro *m = macro_find (name);
  int k;

  if (m)
    { /* replace an existing definition */
      for (k = 0; k < m->nbody; k++)
        {
          free (m->body[k]);
        }

      free (m->body);
    }
  else
    {
      if (n_macros >= cap_macros)
        {
          cap_macros = cap_macros ? cap_macros * 2 : 16;
          MACROS = (struct macro *)realloc (MACROS, (size_t)cap_macros * sizeof *MACROS);
        }

      m = &MACROS[n_macros++];
      m->name = dupstr (name);
    }

  m->body = body;
  m->nbody = nbody;
}

/* Uppercase op mnemonic of a raw card into buf (the first blank-delimited
 * token after the location field).  Empty if the card has no operation field.
 */
static void
line_op_token (const char *ln, char *buf, size_t cap)
{
  size_t i = 0, n = strlen (ln), o = 0;

  while (i < n && ln[i] != ' ')
    {
      i++; /* skip location field */
    }
  while (i < n && ln[i] == ' ')
    {
      i++; /* skip to operation field */
    }
  while (i < n && ln[i] != ' ' && o + 1 < cap)
    {
      char c = ln[i++];
      buf[o++] = (char)((c >= 'a' && c <= 'z') ? c - 32 : c);
    }
  buf[o] = '\0';
}

/* OPSYN (SI61 CASE 38): "NEW OPSYN OLD" makes NEW assemble as the existing op
 * OLD.  Like SI61 (which inserts the alias into the op tree), the table is
 * built in pass 1 and persists into pass 2; SCANOP consults it when a mnemonic
 * is neither a built-in op nor a macro. */
struct opsyn_ent
{
  char name[MAXSYM + 1];
  const struct bmap_op *op;
};
static struct opsyn_ent *OPSYNS;
static int n_opsyn, cap_opsyn;

static const struct bmap_op *
opsyn_find (const char *name)
{
  int i;

  for (i = 0; i < n_opsyn; i++)
    {
      if (!strcmp (OPSYNS[i].name, name))
        {
          return OPSYNS[i].op;
        }
    }

  return NULL;
}
static void
opsyn_add (const char *name, const struct bmap_op *op)
{
  if (opsyn_find (name))
    {
      return;
    }

  if (n_opsyn >= cap_opsyn)
    {
      cap_opsyn = cap_opsyn ? cap_opsyn * 2 : 16;
      OPSYNS = (struct opsyn_ent *)realloc (OPSYNS, (size_t)cap_opsyn * sizeof *OPSYNS);
    }

  strncpy (OPSYNS[n_opsyn].name, name, MAXSYM);
  OPSYNS[n_opsyn].name[MAXSYM] = '\0';
  OPSYNS[n_opsyn].op = op;
  n_opsyn++;
}

static FILE *LO;      /* listing file (M$LO) */
static int end_synth; /* END had to be synthesised */
static int opt_scan;  /* --scan: phase-2 scanner trace mode */
static int opt_debug; /* -g/--debug: emit the debug schema */

/* ------------------------------------------------------ small helpers -----
 */
static int
upch (int c)
{
  return (c >= 'a' && c <= 'z') ? c - 32 : c;
}

static void
init_tables (void)
{
  int c;

  for (c = 0; c < 256; c++)
    {
      NONBLK[c] = (c >= 33 && c <= 126) ? 1 : 0;
    }

  /* DELTBL (BMAP_SI61.XSI 1264-1268): control chars and blanks are BLANK
   * delimiters; the GMAP operators ( ) * + , - / [ ] are their own codes;
   * everything else (incl. . = $ @ _ digits letters) is a non-delimiter. */
  for (c = 0; c < 256; c++)
    {
      DELTBL[c] = 0;
    }

  for (c = 0; c <= 32; c++)
    {
      DELTBL[c] = D_BLANK;
    }

  for (c = 128; c < 256; c++)
    {
      DELTBL[c] = D_BLANK;
    }

  DELTBL[(int)'('] = D_LPAR;
  DELTBL[(int)')'] = D_RPAR;
  DELTBL[(int)'*'] = D_TIMES;
  DELTBL[(int)'+'] = D_PLUS;
  DELTBL[(int)','] = D_COMMA;
  DELTBL[(int)'-'] = D_MINUS;
  DELTBL[(int)'/'] = D_DIV;
  DELTBL[(int)'['] = D_LB;
  DELTBL[(int)']'] = D_RB;
  /* DIGITPEDB (CONVERT, SI61 1020): digits 0-9 -> 10..19; '.' -> 1;
   * E/e -> 2; D/d -> 3; B/b -> 4; everything else -> 0. */
  for (c = 0; c < 256; c++)
    {
      DIGITPEDB[c] = 0;
    }

  for (c = '0'; c <= '9'; c++)
    {
      DIGITPEDB[c] = (unsigned char)(10 + (c - '0'));
    }

  DIGITPEDB[(int)'.'] = 1;
  DIGITPEDB[(int)'E'] = DIGITPEDB[(int)'e'] = 2;
  DIGITPEDB[(int)'D'] = DIGITPEDB[(int)'d'] = 3;
  DIGITPEDB[(int)'B'] = DIGITPEDB[(int)'b'] = 4;
  /* DECTBL (INST DEC, SI61 2074): class of a DEC/OCT subfield's first
   * significant char -- 3 = letter/$/_ (expression), 2 = digit/'.' (number).
   */
  for (c = 0; c < 256; c++)
    {
      DECTBL[c] = 0;
    }

  for (c = 0; c <= 32; c++)
    {
      DECTBL[c] = 1;
    }

  DECTBL[(int)'$'] = 3;
  DECTBL[(int)','] = 1;
  DECTBL[(int)'.'] = 2;
  for (c = '0'; c <= '9'; c++)
    {
      DECTBL[c] = 2;
    }

  for (c = '@'; c <= 'Z'; c++)
    {
      DECTBL[c] = 3; /* @ A-Z */
    }

  DECTBL[(int)'_'] = 3;
  for (c = 'a'; c <= 'z'; c++)
    {
      DECTBL[c] = 3;
    }

  for (c = 127; c < 256; c++)
    {
      DECTBL[c] = 1;
    }

  /* CONTROLS (VFD, SI61 5633): field-type prefix letter -> code (1=O octal,
   * 2=A/U ascii, 3=H bcd, 4=E ebcdic, 6=Z right-ascii, 7=R right-bcd). */
  for (c = 0; c < 256; c++)
    {
      CONTROLS[c] = 0;
    }

  CONTROLS[(int)'O'] = CONTROLS[(int)'o'] = 1;
  CONTROLS[(int)'A'] = CONTROLS[(int)'a'] = 2;
  CONTROLS[(int)'U'] = CONTROLS[(int)'u'] = 2;
  CONTROLS[(int)'H'] = CONTROLS[(int)'h'] = 3;
  CONTROLS[(int)'E'] = CONTROLS[(int)'e'] = 4;
  CONTROLS[(int)'Z'] = CONTROLS[(int)'z'] = 6;
  CONTROLS[(int)'R'] = CONTROLS[(int)'r'] = 7;
  /* NONDGT (BMAP_DA1 293): the EDEC field-header SEARCH table -- digits map to
   * 0 (keep scanning the leading count), 'A'/'a' -> 9 (9-bit ASCII field),
   * 'P'/'p' -> 4 (4-bit packed field), every other char -> 1 (a terminator
   * that is neither, so INST CASE 42 errors). */
  for (c = 0; c < 256; c++)
    {
      NONDGT_[c] = 1;
    }

  for (c = '0'; c <= '9'; c++)
    {
      NONDGT_[c] = 0;
    }

  NONDGT_[(int)'A'] = NONDGT_[(int)'a'] = 9;
  NONDGT_[(int)'P'] = NONDGT_[(int)'p'] = 4;
  /* DIGITPMPE (SI61 2076): EDEC number classifier -- digit -> 1, '+' -> 2,
   * '-' -> 3, '.' -> 4, 'E'/'e' -> 5, everything else -> 0 (terminator). */
  for (c = 0; c < 256; c++)
    {
      DIGITPMPE_[c] = 0;
    }

  for (c = '0'; c <= '9'; c++)
    {
      DIGITPMPE_[c] = 1;
    }

  DIGITPMPE_[(int)'+'] = 2;
  DIGITPMPE_[(int)'-'] = 3;
  DIGITPMPE_[(int)'.'] = 4;
  DIGITPMPE_[(int)'E'] = DIGITPMPE_[(int)'e'] = 5;
}

/* INDEX1 (PL/6 intrinsic): 0-based index of char `ch` in XCARD at or after
 * `start`, within [start, limit).  Returns limit if not found (the callers
 * search the blank-padded buffer, so the first blank always terminates). */
static int
index1 (int ch, int start, int limit)
{
  int i;

  if (limit > XCSIZE)
    {
      limit = XCSIZE;
    }

  for (i = start; i < limit; i++)
    {
      if (XCARD[i] == ch)
        {
          return i;
        }
    }

  return limit;
}

/* SEARCH (PL/6 intrinsic): scan XCARD[start, limit) for the first char whose
 * `tbl` entry is nonzero.  Returns that index and stores the table value in
 * *val; returns -1 (the ALTRET) when none is found before limit. */
static int
searchtbl (const unsigned char *tbl, int start, int limit, int *val)
{
  int i, v;

  if (limit > XCSIZE)
    {
      limit = XCSIZE;
    }

  for (i = start; i < limit; i++)
    {
      v = tbl[(unsigned char)XCARD[i]];
      if (v)
        {
          if (val)
            {
              *val = v;
            }

          return i;
        }
    }

  return -1;
}

/* ------------------------------------------------------- op-code lookup ---
 * Replaces SCANOP's TREESRCH(OPROOT,...) over the SPOOL AVL tree.  The op
 * field XCARD[s,e) is uppercased (the 6-bit-ASCII fold CONSYM applies maps
 * a-z onto A-Z) into a buffer and looked up by bsearch over the sorted
 * bmap_ops[].  Returns the entry, or NULL if the mnemonic is unknown. */
static int
op_cmp (const void *key, const void *el)
{
  return strcmp ((const char *)key, ((const struct bmap_op *)el)->mnem);
}
static const struct bmap_op *
find_op (int s, int e)
{
  char buf[MAXSYM + 1];
  int n = 0, i;

  for (i = s; i < e && n < MAXSYM; i++)
    {
      buf[n++] = (char)upch ((unsigned char)XCARD[i]);
    }

  buf[n] = '\0';
  if (n == 0)
    {
      return NULL;
    }

  return (struct bmap_op *)bsearch (
          buf, bmap_ops, NBMAPOP, sizeof bmap_ops[0], op_cmp);
}

/* ----------------------------------------------------------- ERROR (1247) -
 * Saves an error number for the current line (ERRNUM/ERRCT) and bumps the
 * totals.  The listing of messages is a later phase; for now negative codes
 * (always counted) and pass-2 codes raise the severity, as in the original. */
static void
error (int code)
{
  int i;

  if (PASS2 != 0 || code < 0)
    {
      TERRCT++;
      if (ERRSEV < 4)
        {
          ERRSEV = 4;
        }
    }

  if (ERRCT == 5)
    {
      return;
    }

  for (i = 0; i < ERRCT; i++)
    {
      if (ERRNUM[i] == code)
        {
          return;
        }
    }

  ERRNUM[ERRCT++] = code;
}

/* ========================================================= symbol table ===
 * TREESRCH/TREESTEP (SI61 5094/5216) as a malloc'd pointer AVL (Horowitz &
 * Sahni insert, the same balance logic already proven in asmdal.c), keyed by
 * strcmp on the uppercased name. */
static struct sym *
new_sym (const char *name)
{
  struct sym *p = (struct sym *)calloc (1, sizeof *p);

  if (!p)
    {
      fprintf (stderr, "bmap: out of memory\n");
      exit (2);
    }

  strncpy (p->name, name, MAXSYM);
  p->name[MAXSYM] = '\0';
  p->r = REL_UNDEF; /* a fresh symbol starts undefined */
  NSYM++;
  return p;
}

/* Find `name`, inserting an undefined node if absent (TREESRCH with INSF=1).
 * Returns the node and sets *is_new.  AVL rebalance is identical to asmdal's
 * add_symbol; A = last unbalanced node on the path, F its parent. */
static struct sym *
sym_find_or_enter (const char *name, int *is_new)
{
  struct sym *Y, *A, *B, *C, *F, *P, *Q;
  int d, c;

  *is_new = 0;
  if (!SYMROOT_)
    {
      *is_new = 1;
      return SYMROOT_ = new_sym (name);
    }

  F = NULL;
  A = SYMROOT_;
  P = SYMROOT_;
  Q = NULL;
  while (P)
    {
      if (P->bf != 0)
        {
          A = P;
          F = Q;
        }

      c = strcmp (name, P->name);
      if (c < 0)
        {
          Q = P;
          P = P->l;
        }
      else if (c > 0)
        {
          Q = P;
          P = P->rt;
        }
      else
        {
          return P; /* already present */
        }
    }
  *is_new = 1;
  Y = new_sym (name);
  if (strcmp (name, Q->name) < 0)
    {
      Q->l = Y;
    }
  else
    {
      Q->rt = Y;
    }

  if (strcmp (name, A->name) > 0)
    {
      P = A->rt;
      B = P;
      d = -1;
    }
  else
    {
      P = A->l;
      B = P;
      d = +1;
    }

  while (P != Y)
    {
      if (strcmp (name, P->name) > 0)
        {
          P->bf = -1;
          P = P->rt;
        }
      else
        {
          P->bf = +1;
          P = P->l;
        }
    }
  if (A->bf == 0)
    {
      A->bf = d;
      return Y;
    }

  if (A->bf + d == 0)
    {
      A->bf = 0;
      return Y;
    }

  if (d == +1)
    {
      if (B->bf == +1)
        { /* LL */
          A->l = B->rt;
          B->rt = A;
          A->bf = 0;
          B->bf = 0;
        }
      else
        { /* LR */
          C = B->rt;
          B->rt = C->l;
          A->l = C->rt;
          C->l = B;
          C->rt = A;
          if (C->bf == +1)
            {
              A->bf = -1;
              B->bf = 0;
            }
          else if (C->bf == -1)
            {
              B->bf = +1;
              A->bf = 0;
            }
          else
            {
              B->bf = 0;
              A->bf = 0;
            }

          C->bf = 0;
          B = C;
        }
    }
  else
    {
      if (B->bf == -1)
        { /* RR */
          A->rt = B->l;
          B->l = A;
          A->bf = 0;
          B->bf = 0;
        }
      else
        { /* RL */
          C = B->l;
          B->l = C->rt;
          A->rt = C->l;
          C->rt = B;
          C->l = A;
          if (C->bf == -1)
            {
              A->bf = +1;
              B->bf = 0;
            }
          else if (C->bf == +1)
            {
              B->bf = -1;
              A->bf = 0;
            }
          else
            {
              B->bf = 0;
              A->bf = 0;
            }

          C->bf = 0;
          B = C;
        }
    }

  if (!F)
    {
      SYMROOT_ = B;
    }
  else if (A == F->l)
    {
      F->l = B;
    }
  else
    {
      F->rt = B;
    }

  return Y;
}

static int
rel_differs (const struct rel *a, const struct rel *b)
{
  return a->opndtyp != b->opndtyp || a->operand != b->operand
         || a->relocop != b->relocop || a->evalop != b->evalop
         || a->s_opndtyp != b->s_opndtyp || a->s_operand != b->s_operand
         || a->value != b->value;
}

/* SYMTAB (SI61 4606), phase-3 core.  def=1: define name = *val with
 * relocation *rel (a conflicting redefinition raises error -10, the S30
 * path).  def=0: reference -- fill in *val and *rel from a known symbol (else
 * leave undefined) and mark it REFED.  The XUO$ emission (phase 6) and the
 * cross-reference (phase 8, the XR option) are live; only the UFR forward-ref
 * fixup packet is deferred (the two-pass model resolves ordinary forward
 * refs). */
static void
symtab (const char *name, int *val, struct rel *rel, int def)
{
  int is_new;
  struct sym *s = sym_find_or_enter (name, &is_new);

  if (def)
    {
      /* SET/SETB symbols (REL.F.SET on both) may be redefined (SI61 S30). */
      if (s->defined && !(s->r.f_set && rel->f_set)
          && (s->value != *val || rel_differs (&s->r, rel)))
        {
          error (-10); /* MULTIPLY-DEFINED SYMBOL */
        }

      s->value = *val;
      { /* merge the EDEF/SDEF/DEFED/REFED flags already seen (SI61 4682) */
        struct rel m = *rel;
        m.f_edef |= s->r.f_edef;
        m.f_sdef |= s->r.f_sdef;
        m.f_defed |= s->r.f_defed;
        m.f_refed |= s->r.f_refed;
        s->r = m;
      }
      s->defined = 1;
      s->line = CARD_COUNT; /* source line, for the debug schema */
      if (PASS2 != 0 && OPTIONS.xr)
        {
          sym_addref (s, CARD_COUNT); /* xref: definition */
        }
    }
  else
    {
      /* SI61 4675: an ENTDEF/SYMDEF reference (incoming REL.F carries the
       * EDEF/SDEF bits = the XDEF mask) merges those def flags onto the
       * symbol; an ordinary reference just marks it REFED. */
      if (rel->f_edef || rel->f_sdef)
        {
          s->r.f_edef |= rel->f_edef;
          s->r.f_sdef |= rel->f_sdef;
        }
      else
        {
          s->r.f_refed = 1;
        }

      if (PASS2 != 0 && OPTIONS.xr)
        {
          sym_addref (s, CARD_COUNT); /* xref: reference */
        }

      if (s->defined)
        {
          *val = s->value;
          *rel = s->r;
        }
      else
        {
          *val = 0;
          *rel = REL_UNDEF;
        }

      /* the returned relocation carries no definition flags (SI61 4849/4856
       * "REL.F='0'B"): EQU/SET/EDEF/SDEF/DEFED/REFED are symbol-table
       * attributes, not part of an expression's relocation -- leaving them
       * set would make an absolute EQU reference look relocatable. */
      rel->f_equ = rel->f_set = rel->f_edef = rel->f_sdef = rel->f_defed
          = rel->f_refed = 0;
    }
}

static void
sym_walk (struct sym *t, void (*fn) (struct sym *))
{
  if (!t)
    {
      return;
    }

  sym_walk (t->l, fn);
  fn (t);
  sym_walk (t->rt, fn);
}

/* ====================================================== number conversion =
 * CONVERT (SI61 1005) + GETSF.  Reads a numeric literal from XCARD at CURRCH
 * (advancing CURRCH) in `base` (only 10 and 8 ever occur -- VARSCAN derives
 * the base as 10 - 2*(type&1)).  Integer conversion is exact (*val).  Floating
 * point -- SP (*type=1, '.'/E) and DP (*type=2, D) -- returns the value in
 * *dval; the caller packs it into DPS-8 hardware FP with dps8_float() (this
 * port's C version of FIX/SCALE).  A scaled-fixed `nBm` literal (B) folds to
 * an integer in *val with *type=0.  (*deferred is vestigial -- always 0 --
 * left from when FP was unimplemented; callers pass it but ignore it.) */
static int
getsf (void) /* GETSF (SI61 1097): signed scale */
{
  int sign = 1, sf = 0, c;

  CURRCH++; /* step past the E / D / B */
  if (XCARD[CURRCH] == '+')
    {
      CURRCH++;
    }
  else if (XCARD[CURRCH] == '-')
    {
      sign = -1;
      CURRCH++;
    }

  while ((c = (unsigned char)XCARD[CURRCH]) >= '0' && c <= '9')
    {
      sf = sf * 10 + (c - '0');
      CURRCH++;
    }
  CURRCH--; /* convert's loop will CURRCH++ */
  return sign * sf;
}
/* Encode a host double as DPS-8 binary floating point: an 8-bit
 * two's-complement exponent in bits 0-7, then a two's-complement fractional
 * mantissa (28 bits single, 64 bits double) normalised to magnitude [0.5,1);
 * zero is all zeros. Returns the word count (1 or 2 in out[]).  This computes
 * the encoding from the host double -- a modernisation of SI61's
 * FIX/SCALE/CONVERTSTEP DPS-8 FP arithmetic; the words match the hardware
 * format to host-double precision. */
static int
dps8_float (double x, int dbl, W *out)
{
  int e;
  double m;

  if (x == 0.0)
    {
      out[0] = 0;
      if (dbl)
        {
          out[1] = 0;
        }

      return dbl ? 2 : 1;
    }

  m = frexp (x, &e); /* x = m * 2^e, |m| in [0.5,1) */
  if (!dbl)
    {
      long mant = lround (ldexp (m, 27)); /* 28-bit signed fraction */
      if (mant >= (1L << 27))
        {
          mant >>= 1;
          e++;
        } /* rounded up to 1.0 */

      out[0] = (((W)e & 0xFF) << 28) | ((W)mant & 0x0FFFFFFF);
      return 1;
    }
  else
    {
      long long mant;
      double t = ldexp (m, 63); /* 64-bit signed fraction */
      if (t >= 9223372036854775808.0)
        {
          mant = 1LL << 62;
          e++;
        }
      else
        {
          mant = llround (t);
        }

      out[0] = (((W)e & 0xFF) << 28) | (((W)(mant >> 36)) & 0x0FFFFFFF);
      out[1] = (W)mant & M36;
      return 2;
    }
}
/* CONVERT (SI61 1005): read a numeric literal from XCARD.  Integers (any base)
 * are exact; base-10 floats accumulate a host double (mantissa * 10^(E - frac
 * digits), the equivalent of CONVERTSTEP + SCALE) returned in *dval with *type
 * 1 (SPFP) or 2 (DPFP); a scaled `nBm` value is fixed to *val (value * 2^m).
 */
static void
convert (int base, int64_t *val, int *type, int *deferred, double *dval)
{
  int64_t v = 0;
  double mant = 0.0;
  int j, frac = 0, sawpt = 0, sf = 0, sh = 0;

  *val = 0;
  *type = 0;
  *deferred = 0;
  *dval = 0.0;
  if (base != 10)
    { /* base 8: simple integer accumulate */
      for (;;)
        {
          j = DIGITPEDB[(unsigned char)XCARD[CURRCH]];
          if (j < 10)
            {
              break;
            }

          v = v * base + (j - 10);
          CURRCH++;
        }

      *val = v & (int64_t)M36;
      return;
    }

  for (;;)
    { /* base 10 */
      j = DIGITPEDB[(unsigned char)XCARD[CURRCH]];
      if (j >= 10)
        {
          v = v * 10 + (j - 10);
          mant = mant * 10.0 + (j - 10);
          if (sawpt)
            {
              frac++;
            }
        } /* digit */
      else if (j == 1)
        {
          if (*type == 0)
            {
              sawpt = 1;
              *type = 1;
            }
        } /* '.' */
      else if (j == 2 || j == 3)
        {
          sf = getsf ();
          if (*type < 2)
            {
              *type = j;
            }
        } /* E,D */
      else if (j == 4)
        {
          sh = getsf ();
          if (*type < 4)
            {
              *type += 4;
            }
        } /* B */
      else
        {
          break;
        }

      CURRCH++;
    }

  if (*type == 0)
    {
      *val = v & (int64_t)M36;
      return;
    } /* integer */

  {
    double value = mant * pow (10.0, (double)(sf - frac));
    if (*type >= 4)
      { /* scaled fixed (B) */
        *val = (int64_t)llround (ldexp (value, sh)) & (int64_t)M36;
        *type = 0;
      }
    else
      { /* SPFP (1/E) or DPFP (D) */
        *dval = value;
        *val
            = (int64_t)value & (int64_t)M36; /* integer part (expr fallback) */
        *type = (*type == 3) ? 2 : 1;
      }
  }
}

/* =============================================================== scanner ==
 */

/* DELSCAN (BMAP_SI61.XSI 1259): from CURRCH, set NEXTCH to the next delimiter
 * and DEL to its code.  A leading '*' is stepped over so it is not itself the
 * delimiter (it is the location-counter term in VARSCAN). */
static void
delscan (void)
{
  int v;

  NEXTCH = CURRCH;
  if (XCARD[NEXTCH] == '*')
    {
      NEXTCH++;
    }

  v = searchtbl (DELTBL, NEXTCH, XCSIZE, &DEL);
  if (v < 0)
    {
      DEL = D_BLANK; /* EOL */
    }
  else
    {
      NEXTCH = v;
    }
}

/* NEXTFLD (BMAP_SI61.XSI 3860): advance to the next variable subfield.
 * Leaves CURRCH at the field start and NEXTCH at the field's end delimiter.
 * Returns 1 (the ALTRET) when there is no further field.  The macro
 * continuation-card path (S10) is a phase-7 concern and is treated as ALTRET
 * here -- with no macros active a trailing comma simply ends the scan. */
static int
nextfld (void)
{
  if (DEL == D_RPAR)
    {
      return 1;
    }

  if (DEL == D_BLANK)
    {
      return 1;
    }

  CURRCH++;
  delscan ();
  if (NEXTCH != CURRCH)
    {
      return 0; /* non-empty field */
    }

  if (DEL < D_LPAR)
    {
      return 0; /* empty field, operator delimiter */
    }

  return 1; /* empty field at end of line */
}

/* SCANOP (BMAP_SI61.XSI 4580): scan the location and operation fields.
 * Sets LOCSZ (size of location field), CURRCH/NEXTCH to bracket the operation
 * field, and OP to the looked-up op-code.  Mirrors the original's results:
 *   - an empty operation field  -> OP = NONOP, no error;
 *   - an unknown mnemonic        -> OP = NONOP and error(2) (INVALID
 * OPERATION).
 */
static void
scanop (void)
{
  int j;
  const struct bmap_op *o;

  LOCSZ = index1 (' ', 0, XCSIZE); /* end of location field */
  /* operation field must start within 15 cols of the location field */
  CURRCH = searchtbl (NONBLK, LOCSZ, LOCSZ + 15, &j);
  if (CURRCH < 0)
    { /* EOL2: no operation */
      CURRCH = LOCSZ;
      NEXTCH = LOCSZ;
      OP = OP_NONOP;
      return;
    }

  NEXTCH = index1 (' ', CURRCH, XCSIZE); /* end of operation field */
  o = find_op (CURRCH, NEXTCH);
  CUR_MACRO = NULL;
  if (o)
    {
      OP = o;
    }
  else
    { /* not a built-in op */
      char nm[MAXSYM + 1];
      int i, k = 0;
      struct macro *m;
      for (i = CURRCH; i < NEXTCH && k < MAXSYM; i++)
        {
          nm[k++] = (char)upch ((unsigned char)XCARD[i]);
        }

      nm[k] = '\0';
      m = macro_find (nm); /* SI61: macros live in the op tree */
      if (m)
        {
          OP = &MACRO_OP;
          CUR_MACRO = m;
        } /* type 21: macro call */
      else
        {
          const struct bmap_op *syn = opsyn_find (nm); /* OPSYN alias */
          if (syn)
            {
              OP = syn;
            }
          else
            {
              OP = OP_NONOP;
              error (2);
            }
        }
    }
}

/* Next physical card: drain the macro-expansion stack (innermost frame first),
 * then the source file.  Returns NULL at end of all input.  Used both by the
 * card reader and by the MACRO/DUP body-capture, so a macro defined or DUP'd
 * inside another expansion sees the expanded text. */
static const char *
next_phys_line (void)
{
  while (exp_depth > 0)
    {
      struct expframe *f = &EXPSTK[exp_depth - 1];
      if (f->cur < f->n)
        {
          return f->lines[f->cur++];
        }

      {
        int k;
        for (k = 0; k < f->n; k++)
          {
            free (f->lines[k]);
          }
      }
      free (f->lines);
      exp_depth--;
    }
  if (src_cur >= src_n)
    {
      return NULL;
    }

  return src_lines[src_cur++];
}

/* Push an expansion frame (a macro/DUP/IDRP body); takes ownership of `lines`
 * and each string.  Drained next by next_phys_line. */
static void
exp_push (char **lines, int n)
{
  int k;

  if (n > 0 && exp_depth < MAXEXP)
    {
      EXPSTK[exp_depth].lines = lines;
      EXPSTK[exp_depth].n = n;
      EXPSTK[exp_depth].cur = 0;
      exp_depth++;
      return;
    }

  for (k = 0; k < n; k++)
    {
      free (lines[k]); /* overflow / empty: discard */
    }

  free (lines);
  if (n > 0)
    {
      error (11); /* MACRO TABLE FULL */
    }
}

/* Discard any live expansion frames (between passes / at start). */
static void
exp_reset (void)
{
  while (exp_depth > 0)
    {
      struct expframe *f = &EXPSTK[--exp_depth];
      int k;
      for (k = 0; k < f->n; k++)
        {
          free (f->lines[k]);
        }

      free (f->lines);
    }
}

/* Load one raw source line into XCARD, blank-padded to 256, and set XCARDL.
 * Returns 0 on success, 1 at end of the in-memory source. */
static int
read_raw (void)
{
  const char *s;
  int n, i;

  s = next_phys_line ();
  if (!s)
    {
      return 1;
    }

  RECORDCT++;
  n = (int)strlen (s);
  if (n > CARDSIZE)
    {
      n = CARDSIZE; /* LINE.CARD is CHAR(140) */
    }

  for (i = 0; i < n; i++)
    {
      XCARD[i] = s[i];
    }

  for (; i < XCSIZE; i++)
    {
      XCARD[i] = ' ';
    }

  XCARD[XCSIZE] = '\0';
  XCARDL = n;
  snprintf (KEY, sizeof KEY, "%d", RECORDCT);
  return 0;
}

/* READCARD (BMAP_SI61.XSI 4260), non-macro path.  Reads the next statement
 * card, skipping (and -- in later phases -- listing) comment cards, runs
 * SCANOP, captures the location field into LOC, and positions CURRCH at the
 * start of the variable field (DEL=0), or sets DEL=BLANK when there is none.
 * At end of input an END card is synthesized so the pass loop terminates.
 *
 * `cont` (the original CONT arg, used by NEXTFLD for continuation cards) is
 * accepted for signature fidelity; continuation is a phase-7 concern. */
static void
read_card (int cont)
{
  int j, var, i;

  (void)cont;

  for (;;)
    {
      if (read_raw ())
        { /* end of source */
          OP = OP_END;
          LOCSZ = 0;
          LOC[0] = '\0';
          CURRCH = NEXTCH = 0;
          DEL = D_BLANK;
          end_synth = 1;
          VAR_CURRCH0 = CURRCH;
          VAR_DEL0 = DEL;
          return;
        }

      /* comment card: column 1 is '*', or the first 16 columns are blank */
      if (XCARD[0] == '*')
        {
          continue;
        }

      for (j = 0; j < 16; j++)
        {
          if (XCARD[j] != ' ')
            {
              break;
            }
        }

      if (j == 16)
        {
          continue;
        }

      break;
    }

  scanop ();

  /* location field -> LOC (CONSYM, DA1 54; uppercased for the 6-bit fold) */
  if (LOCSZ != 0)
    {
      int m = LOCSZ < MAXSYM ? LOCSZ : MAXSYM;
      for (i = 0; i < m; i++)
        {
          LOC[i] = (char)upch ((unsigned char)XCARD[i]);
        }

      LOC[m] = '\0';
    }
  else
    {
      LOC[0] = '\0';
    }

  /* position the variable field (READCARD S50, SI61 4377-4388).  The
   * original searches a base-1 substring, which leaves CURRCH one short so
   * that NEXTFLD's pre-increment lands on the field start; we reproduce that
   * observable result directly. */
  DEL = 0;
  if (OP->type == 8)
    { /* TTL / TTLS column rules */
      if (CURRCH == 7 && NEXTCH <= 13)
        {
          CURRCH = 14;
        }
      else
        {
          CURRCH = NEXTCH;
        }
    }
  else
    {
      var = searchtbl (NONBLK, NEXTCH, NEXTCH + 15, &j);
      if (var < 0)
        {
          CURRCH = NEXTCH;
          DEL = D_BLANK;
        } /* VARNOTFOUND */
      else
        {
          CURRCH = var - 1; /* pre-decrement for NEXTFLD */
        }
    }

  VAR_CURRCH0 = CURRCH;
  VAR_DEL0 = DEL;
}

/* ===================================================== expression eval =====
 * VARSCAN (SI61 5257): evaluate one variable-field subfield to a value AND a
 * filled REL relocation packet.  An operator-precedence (shunting-yard) parse
 * over + - * / ( ) -- and, in an octal field (type&1), the same delimiters
 * reinterpreted as the Boolean OR/EOR/AND/NOT -- while combining the operands'
 * relocations: R1±R2 (with cancellation to absolute), R1*A and R1/A (via the
 * REL EVALOP), per HELP_BMAP's complex-relocation rules.
 *   type bit0   : octal base (else decimal)
 *   (type&3)==2 : modifier mode (address-mod tag field) -- INST, phase 5
 *   type bit2   : literal allowed (=O/=A/...) -- phase 8
 * Leaf: a subfield containing no non-digit is a number (CONVERT); else a
 * symbol (SYMTAB lookup); "*" is the location counter (PC, PCREL).  The
 * modifier (S60) and literal (=) paths are stubbed here. */
#define VOP_PLUS 1
#define VOP_MINUS 2
#define VOP_TIMES 3
#define VOP_DIV 4
#define VOP_OR 5
#define VOP_EOR 6
#define VOP_AND 7
#define VOP_NOT 8
#define VOP_OPAREN 9
#define RSWAP(x) (RELOCOPADD + RELOCOPSUB - (x)) /* ADD<->SUB */
static const int VPREC[15] = { 0, 1, 1, 2, 2, 1, 1, 2, 2, 3, 0, 0, 0, 0, 0 };

static int64_t
sx36 (int64_t x) /* sign-extend a 36-bit value */
{
  return (int64_t)((uint64_t)x << 28) >> 28;
}
static int
rel_nz (const struct rel *r) /* relocatable (REL packet nonzero) */
{
  return r->f_equ || r->f_set || r->f_edef || r->f_sdef || r->f_defed
         || r->f_refed || r->opndtyp || r->evalop || r->relocop || r->operand
         || r->disp || r->stbit || r->endbit || r->value || r->s_opndtyp
         || r->s_relocop || r->s_operand;
}
static int
rel_s_nz (const struct rel *r) /* second relocation word present */
{
  return r->s_opndtyp || r->s_relocop || r->s_operand;
}
static int
rel_eq (const struct rel *a, const struct rel *b)
{
  return a->f_equ == b->f_equ && a->f_set == b->f_set && a->f_edef == b->f_edef
         && a->f_sdef == b->f_sdef && a->f_defed == b->f_defed
         && a->f_refed == b->f_refed && a->opndtyp == b->opndtyp
         && a->evalop == b->evalop && a->relocop == b->relocop
         && a->operand == b->operand && a->disp == b->disp
         && a->stbit == b->stbit && a->endbit == b->endbit
         && a->value == b->value && a->s_opndtyp == b->s_opndtyp
         && a->s_relocop == b->s_relocop && a->s_operand == b->s_operand;
}
static int
all_digits (int s, int e) /* SEARCH(NONDGT) ALTRET: all digits? */
{
  int i;

  if (e <= s)
    {
      return 0;
    }

  for (i = s; i < e; i++)
    {
      if (XCARD[i] < '0' || XCARD[i] > '9')
        {
          return 0;
        }
    }

  return 1;
}
static void
field_sym (char *buf, int s, int e) /* CONSYM: field -> symbol */
{
  int n = 0, i;

  for (i = s; i < e && n < MAXSYM; i++)
    {
      buf[n++] = (char)upch ((unsigned char)XCARD[i]);
    }

  buf[n] = '\0';
}
/* store R[src]'s primary relocation into R[dst]'s second word
 * (R.S(dst)=R(src)) */
static void
rel_set_s (struct rel *dst, const struct rel *src)
{
  dst->s_opndtyp = src->opndtyp;
  dst->s_relocop = src->relocop;
  dst->s_operand = src->operand;
}

/* Address-modification tag mnemonics (VARSCAN MODSYM/MODVAL, SI61 5283).  The
 * tag text is space-padded to 4 chars and binary-searched (the table is sorted
 * by that 4-byte key); MODVAL is the 6-bit DPS-8 tag code. */
static const char MODSYM[38][4] = {
  { '$', ' ', ' ', ' ' }, { '0', ' ', ' ', ' ' }, { '1', ' ', ' ', ' ' },
  { '2', ' ', ' ', ' ' }, { '3', ' ', ' ', ' ' }, { '4', ' ', ' ', ' ' },
  { '5', ' ', ' ', ' ' }, { '6', ' ', ' ', ' ' }, { '7', ' ', ' ', ' ' },
  { 'A', ' ', ' ', ' ' }, { 'A', 'D', ' ', ' ' }, { 'A', 'L', ' ', ' ' },
  { 'A', 'U', ' ', ' ' }, { 'C', 'I', ' ', ' ' }, { 'D', 'I', ' ', ' ' },
  { 'D', 'I', 'C', ' ' }, { 'D', 'L', ' ', ' ' }, { 'D', 'U', ' ', ' ' },
  { 'F', ' ', ' ', ' ' }, { 'I', ' ', ' ', ' ' }, { 'I', 'C', ' ', ' ' },
  { 'I', 'D', ' ', ' ' }, { 'I', 'D', 'C', ' ' }, { 'N', ' ', ' ', ' ' },
  { 'Q', ' ', ' ', ' ' }, { 'Q', 'L', ' ', ' ' }, { 'Q', 'U', ' ', ' ' },
  { 'S', 'C', ' ', ' ' }, { 'S', 'C', 'R', ' ' }, { 'S', 'D', ' ', ' ' },
  { 'X', '0', ' ', ' ' }, { 'X', '1', ' ', ' ' }, { 'X', '2', ' ', ' ' },
  { 'X', '3', ' ', ' ' }, { 'X', '4', ' ', ' ' }, { 'X', '5', ' ', ' ' },
  { 'X', '6', ' ', ' ' }, { 'X', '7', ' ', ' ' },
};
static const int MODVAL[38]
    = { 4,  8,  9,  10, 11, 12, 13, 14, 15, 5,  43, 5, 1,
        40, 44, 45, 7,  3,  32, 41, 4,  46, 47, 0,  6, 6,
        2,  42, 37, 36, 8,  9,  10, 11, 12, 13, 14, 15 };
static void
field_pad4 (char *b4, int s, int e) /* 4-char space-padded tag */
{
  int i, n = e - s;

  for (i = 0; i < 4; i++)
    {
      b4[i] = (i < n) ? (char)upch ((unsigned char)XCARD[s + i]) : ' ';
    }
}

/* ------------------------------------------------------ phase 8: literals --
 * The literal pool (=constants).  A single-word numeric literal "=expr" /
 * "=Oexpr" is interned -- deduped by value -- into the LITERALS section
 * (LITSECT = OSECT index 1, which xuo_sectbuild builds second); a reference to
 * it relocates against that section.  The pool is emitted at END by genlits().
 * The table is rebuilt each pass: literals are interned in encounter order,
 * identical across passes, so an address assigned in pass 1 recurs in pass 2.
 * Typed (=A/U/H/Z/R), float and multi-word literals and the LITORG flush
 * (CASE 45) are supported; =M (instruction) and =V (VFD) literals are not. */
static void xlatev (int nb, W *dest, int destfc, int destnc,
                    const unsigned short *table, int srcoff,
                    int srcnc); /* fwd */
#define LITSECT 1
#define LITMAXW 4 /* widest literal we form (a few words) */
struct lit_ent
{
  int64_t v[LITMAXW];
  int nw;
  int pc;
};
static struct lit_ent *LITTAB;
static int n_lit, cap_lit, LITLOC;

/* Intern an nw-word literal -- deduped by value -- into the LITERALS section,
 * returning its pc (LIT-relative offset).  A 2-word (double) literal is
 * aligned to an even pc, as SI61 does. */
static int
lit_intern (const int64_t *v, int nw)
{
  int i, j;

  for (i = 0; i < n_lit; i++)
    {
      if (LITTAB[i].nw != nw)
        {
          continue;
        }

      for (j = 0; j < nw && LITTAB[i].v[j] == v[j]; j++)
        {
          ;
        }

      if (j == nw)
        {
          return LITTAB[i].pc;
        }
    }

  if (n_lit >= cap_lit)
    {
      cap_lit = cap_lit ? cap_lit * 2 : 32;
      LITTAB = (struct lit_ent *)realloc (LITTAB, (size_t)cap_lit * sizeof *LITTAB);
    }

  if (nw == 2 && (LITLOC & 1))
    {
      LITLOC++; /* even-align a double */
    }

  for (j = 0; j < nw; j++)
    {
      LITTAB[n_lit].v[j] = v[j];
    }

  LITTAB[n_lit].nw = nw;
  LITTAB[n_lit].pc = LITLOC;
  LITLOC += nw;
  return LITTAB[n_lit++].pc;
}

/* Pack a character literal: `count` chars of text (at XCARD[CURRCH+1..],
 * CURRCH being the type letter) translated by `type` (A/U=9-bit ASCII, H=6-bit
 * BCD, Z/R = those right-justified) into w[].  Returns the word count;
 * advances CURRCH past the text. */
static int
lit_char (int type, int count, int64_t *w)
{
  int m = (type == 'A' || type == 'U' || type == 'Z') ? 9 : 6; /* bits/char */
  int sh = (type == 'A' || type == 'U' || type == 'Z')
               ? 2
               : 0; /* asciit: 2=ASCII,0=BCD */
  int right = (type == 'Z' || type == 'R');
  int cpw = 36 / m, nw, i, fc;
  W wb[LITMAXW];

  if (count < 0)
    {
      count = (m == 9) ? 4 : 6; /* leading-form default width */
    }

  if (count < 1)
    {
      count = 1;
    }

  nw = (count + cpw - 1) / cpw;
  if (nw > LITMAXW)
    {
      nw = LITMAXW;
      count = nw * cpw;
    }

  for (i = 0; i < nw; i++)
    {
      wb[i] = 0;
    }

  fc = right ? (nw * cpw - count) : 0; /* right-justify: pad on the left */
  xlatev (m, wb, fc, nw * cpw, bmap_asciit[sh], CURRCH + 1, count);
  for (i = 0; i < nw; i++)
    {
      w[i] = (int64_t)wb[i];
    }

  CURRCH += count + 1; /* past the type letter + text */
  return nw;
}

/* LITERAL (SI61): scan a "=..." operand from the '='.  Forms: =expr / =Oexpr
 * (integer), =1.5 / =1.0D0 (DPS-8 single/double float), =Atext / =Htext and
 * the
 * =<count><A|U|H|Z|R>text suffixed forms (character).  `isconst` (SI61's
 * ~TYPE&4: the field takes no literal address) returns the value itself
 * instead of interning a pool word.  =M (instruction) and =V (VFD) are not yet
 * ported. */
static void
literal (int64_t *out_val, struct rel *out_rel, int isconst)
{
  int64_t w[LITMAXW];
  int64_t v;
  int nw = 1, base = 10, neg = 0, c0, ct = 0, cd = 0;
  double dv = 0.0;

  memset (out_rel, 0, sizeof *out_rel);
  w[0] = 0;
  CURRCH++; /* skip '=' */
  c0 = upch ((unsigned char)XCARD[CURRCH]);
  if (c0 == 'O')
    {
      base = 8;
      CURRCH++;
    } /* =O octal */
  else if (c0 == 'A' || c0 == 'U' || c0 == 'H')
    { /* leading character literal */
      nw = lit_char (c0, -1, w);
      goto done;
    }
  else if (c0 == 'V' || c0 == 'M' || c0 == 'Z' || c0 == 'R')
    {
      error (4);
      delscan ();
      *out_val = 0;
      return; /* =V/=M not ported; Z/R need a count */
    }

  if (XCARD[CURRCH] == '-')
    {
      neg = 1;
      CURRCH++;
    }
  else if (XCARD[CURRCH] == '+')
    {
      CURRCH++;
    }

  if (XCARD[CURRCH] < '0' || XCARD[CURRCH] > '9')
    {
      error (4);
      delscan ();
      *out_val = 0;
      return;
    }

  convert (base, &v, &ct, &cd, &dv);
  if (ct != 0)
    { /* floating point -> 1 or 2 words */
      W fw[2];
      nw = dps8_float (neg ? -dv : dv, ct == 2, fw);
      w[0] = (int64_t)fw[0];
      if (nw > 1)
        {
          w[1] = (int64_t)fw[1];
        }
    }
  else
    {
      int sfx = upch (
          (unsigned char)XCARD[CURRCH]); /* optional A/U/H/Z/R suffix */
      if (sfx == 'A' || sfx == 'U' || sfx == 'H' || sfx == 'Z' || sfx == 'R')
        {
          nw = lit_char (sfx, (int)v, w);
          goto done;
        }

      if (neg)
        {
          v = -v;
        }

      w[0] = sx36 (v);
      nw = 1;
    }

done:
  delscan (); /* DEL/NEXTCH at the value end */
  if (isconst)
    {
      *out_val = w[0];
      return;
    } /* immediate value, not a pool word */

  *out_val = lit_intern (w, nw); /* address of the literal in LITSECT */
  out_rel->opndtyp = OPERSECT;
  out_rel->relocop = RELOCOPADD;
  out_rel->operand = LITSECT;
}

static void
varscan (int64_t *out_val, struct rel *out_rel, int type)
{
  int64_t V[20];
  struct rel R[20];
  int OPSTK[21], UNARY[20];
  int OPX, VX, COP, BASE, TUNARY = 0;
  int64_t VAL = 0, v0 = 0;
  struct rel REL;
  char buf[MAXSYM + 1];

  memset (&REL, 0, sizeof REL);
  OPX = 0;
  OPSTK[0] = 12;
  VX = -1; /* OPSTK[0] = PREC-0 sentinel */
  BASE = (type & 1) ? 8 : 10;

  if (nextfld ())
    {
      if (DEL == D_LPAR)
        {
          goto S20;
        }

      goto S160;
    }

  if ((type & 3) == 2)
    {
      goto S60; /* MODIFIER mode (phase 5) */
    }

  if (XCARD[CURRCH] == '=')
    { /* LITERAL operand (SI61 5322) */
      literal (&VAL, &REL, !(type & 4));
      if (DEL >= D_LPAR)
        {
          goto S160; /* literal is the whole operand */
        }

      VX = 0;
      V[0] = VAL;
      R[0] = REL;
      TUNARY = 0;
      goto S30; /* else it is an expression leaf */
    }

S20:
  TUNARY = (NEXTCH == CURRCH);
  if (!TUNARY)
    {
      VX++;
      if (NEXTCH - CURRCH == 1 && XCARD[CURRCH] == '*')
        { /* location ctr */
          V[VX] = PC;
          R[VX] = PCREL;
        }
      else if (XCARD[CURRCH] == '=')
        { /* literal leaf (immediate) */
          literal (&V[VX], &R[VX], 1);
        }
      else if (all_digits (CURRCH, NEXTCH))
        { /* number */
          int64_t cv;
          int ct, cd;
          double cdv;
          convert (BASE, &cv, &ct, &cd, &cdv);
          if (ct != 0 || CURRCH != NEXTCH)
            {
              error (9); /* float -> error here */
            }

          V[VX] = sx36 (cv);
          memset (&R[VX], 0, sizeof R[VX]);
        }
      else
        { /* symbol */
          int vv = 0;
          struct rel rr = REL_UNDEF;
          field_sym (buf, CURRCH, NEXTCH);
          symtab (buf, &vv, &rr, 0);
          V[VX] = sx36 (vv);
          R[VX] = rr;
        }
    }

S30:
  COP = DEL;
  if ((type & 1) || DEL > D_DIV)
    {
      COP += 4;
    }

S40:
  if (VPREC[OPSTK[OPX]] < VPREC[COP])
    { /* push higher-precedence op */
    S41:
      if (OPX == 20)
        {
          goto S160;
        }

      OPX++;
      OPSTK[OPX] = COP;
      UNARY[OPX] = TUNARY;
      CURRCH = NEXTCH + 1;
      delscan ();
      goto S20;
    }

  if (TUNARY)
    {
      VX++;
      V[VX] = 0;
      memset (&R[VX], 0, sizeof R[VX]);
      TUNARY = 0;
    }

  switch (OPSTK[OPX])
    {
    case VOP_PLUS:
      if (UNARY[OPX])
        {
          goto S51;
        }

      if (!rel_nz (&R[VX - 1]))
        {
          R[VX - 1] = R[VX];
        }
      else if (rel_nz (&R[VX]))
        {
          if (R[VX - 1].opndtyp == R[VX].opndtyp
              && R[VX - 1].evalop == R[VX].evalop
              && R[VX - 1].relocop == RSWAP (R[VX].relocop)
              && R[VX - 1].operand == R[VX].operand
              && R[VX - 1].value == R[VX].value)
            {
              memset (&R[VX], 0, sizeof R[VX]);
            }
          else if (R[VX - 1].evalop != 0 || rel_s_nz (&R[VX - 1])
                   || R[VX].evalop != 0 || rel_s_nz (&R[VX]))
            {
              error (5);
            }
          else
            {
              rel_set_s (&R[VX - 1], &R[VX]);
            }
        }

      V[VX - 1] = sx36 (V[VX - 1] + V[VX]);
      break;

    case VOP_MINUS:
      if (UNARY[OPX])
        {
          V[VX] = sx36 (-V[VX]);
          if (rel_nz (&R[VX]))
            {
              R[VX].relocop = RSWAP (R[VX].relocop);
              if (R[VX].evalop == 0 && rel_s_nz (&R[VX]))
                {
                  R[VX].s_relocop = RSWAP (R[VX].s_relocop);
                }
            }

          goto S51;
        }

      if (!rel_nz (&R[VX - 1]))
        {
          R[VX - 1] = R[VX];
          if (rel_nz (&R[VX - 1]))
            {
              R[VX - 1].relocop = RSWAP (R[VX - 1].relocop);
              if (R[VX - 1].evalop == 0 && rel_s_nz (&R[VX - 1]))
                {
                  R[VX - 1].s_relocop = RSWAP (R[VX - 1].s_relocop);
                }
            }
        }
      else if (rel_nz (&R[VX]))
        {
          if (rel_eq (&R[VX - 1], &R[VX]))
            {
              memset (&R[VX - 1], 0, sizeof R[VX - 1]);
            }
          else if (R[VX - 1].evalop != 0 || rel_s_nz (&R[VX - 1])
                   || R[VX].evalop != 0 || rel_s_nz (&R[VX]))
            {
              error (5);
            }
          else
            {
              rel_set_s (&R[VX - 1], &R[VX]);
              R[VX - 1].s_relocop = RSWAP (R[VX - 1].s_relocop);
            }
        }

      V[VX - 1] = sx36 (V[VX - 1] - V[VX]);
      break;

    case VOP_TIMES:
      if (UNARY[OPX])
        {
          V[VX] = 0;
          memset (&R[VX], 0, sizeof R[VX]);
          goto S51;
        }

      if (rel_nz (&R[VX]))
        {
          if (!rel_nz (&R[VX - 1]))
            {
              int64_t t = V[VX - 1];
              R[VX - 1] = R[VX];
              V[VX - 1] = V[VX];
              V[VX] = t;
            }
          else
            {
              error (5);
            }
        }

      if (V[VX] == 0)
        {
          memset (&R[VX - 1], 0, sizeof R[VX - 1]);
        }

      if (rel_nz (&R[VX - 1]) && V[VX] != 1)
        {
          if (R[VX - 1].evalop != 0 || !rel_s_nz (&R[VX - 1]))
            {
              if (R[VX - 1].evalop == 0)
                {
                  R[VX - 1].evalop = EVALOPMULT;
                  R[VX - 1].value = (int)V[VX];
                }
              else if (R[VX - 1].evalop == EVALOPMULT)
                {
                  R[VX - 1].value *= (int)V[VX];
                }
              else if (R[VX - 1].evalop == EVALOPDIV)
                {
                  if (R[VX - 1].value != 0 && V[VX] % R[VX - 1].value == 0)
                    {
                      R[VX - 1].evalop = EVALOPMULT;
                      R[VX - 1].value = (int)(V[VX] / R[VX - 1].value);
                    }
                  else
                    {
                      error (5);
                    }
                }
            }
          else
            {
              error (5);
            }
        }

      if (R[VX - 1].evalop != 0 && R[VX - 1].value < 0)
        {
          R[VX - 1].relocop = RSWAP (R[VX - 1].relocop);
          R[VX - 1].value = -R[VX - 1].value;
        }

      V[VX - 1] = sx36 (V[VX - 1] * V[VX]);
      break;

    case VOP_DIV:
      if (UNARY[OPX])
        {
          V[VX] = 0;
          memset (&R[VX], 0, sizeof R[VX]);
          goto S51;
        }

      if (rel_nz (&R[VX]))
        {
          error (5);
        }

      if (rel_nz (&R[VX - 1]) && (V[VX] < 0 || V[VX] > 1))
        {
          if (R[VX - 1].evalop != 0 || !rel_s_nz (&R[VX - 1]))
            {
              if (R[VX - 1].evalop == 0)
                {
                  R[VX - 1].evalop = EVALOPDIV;
                  R[VX - 1].value = (int)V[VX];
                }
              else if (R[VX - 1].evalop == EVALOPMULT)
                {
                  if (V[VX] != 0 && R[VX - 1].value % V[VX] == 0)
                    {
                      R[VX - 1].value /= (int)V[VX];
                    }
                  else
                    {
                      error (5);
                    }
                }
              else if (R[VX - 1].evalop == EVALOPDIV)
                {
                  error (5);
                }
            }
          else
            {
              error (5);
            }
        }

      if (R[VX - 1].evalop != 0 && R[VX - 1].value < 0)
        {
          R[VX - 1].relocop = RSWAP (R[VX - 1].relocop);
          R[VX - 1].value = -R[VX - 1].value;
        }

      if (V[VX] != 0)
        {
          V[VX - 1] = sx36 (V[VX - 1] / V[VX]);
        }

      break;

    case VOP_OR:
      if (UNARY[OPX])
        {
          goto S51;
        }

      if ((rel_nz (&R[VX - 1]) && rel_nz (&R[VX]))
          || (rel_nz (&R[VX - 1]) && V[VX] != 0)
          || (V[VX - 1] != 0 && rel_nz (&R[VX])))
        {
          error (5);
          memset (&R[VX - 1], 0, sizeof R[VX - 1]);
        }
      else if (!rel_nz (&R[VX - 1]))
        {
          R[VX - 1] = R[VX];
        }

      V[VX - 1] = sx36 (((uint64_t)V[VX - 1] | (uint64_t)V[VX]) & M36);
      break;

    case VOP_EOR:
      if (UNARY[OPX])
        {
          goto S51;
        }

      if ((rel_nz (&R[VX - 1]) && rel_nz (&R[VX]))
          || (rel_nz (&R[VX - 1]) && V[VX] != 0)
          || (V[VX - 1] != 0 && rel_nz (&R[VX])))
        {
          error (5);
          memset (&R[VX - 1], 0, sizeof R[VX - 1]);
        }
      else if (!rel_nz (&R[VX - 1]))
        {
          R[VX - 1] = R[VX];
        }

      V[VX - 1] = sx36 (((uint64_t)V[VX - 1] ^ (uint64_t)V[VX]) & M36);
      break;

    case VOP_AND:
      if (UNARY[OPX])
        {
          V[VX] = 0;
          memset (&R[VX], 0, sizeof R[VX]);
          goto S51;
        }

      if (rel_nz (&R[VX - 1]) || V[VX - 1] != 0)
        {
          if (!rel_nz (&R[VX]) && V[VX] == 0)
            {
              memset (&R[VX - 1], 0, sizeof R[VX - 1]);
              V[VX - 1] = 0;
            }
          else if ((rel_nz (&R[VX - 1]) && (rel_nz (&R[VX]) || V[VX] != -1))
                   || (V[VX - 1] != -1 && rel_nz (&R[VX])))
            {
              error (5);
              memset (&R[VX - 1], 0, sizeof R[VX - 1]);
            }
          else if (!rel_nz (&R[VX - 1]))
            {
              R[VX - 1] = R[VX];
            }
        }

      V[VX - 1] = sx36 (((uint64_t)V[VX - 1] & (uint64_t)V[VX]) & M36);
      break;

    case VOP_NOT:
      if (UNARY[OPX])
        {
          if (rel_nz (&R[VX]))
            {
              error (5);
              memset (&R[VX], 0, sizeof R[VX]);
            }

          V[VX] = sx36 ((~(uint64_t)V[VX]) & M36);
          goto S51;
        }

      if (rel_nz (&R[VX - 1]) || V[VX - 1] != 0)
        {
          if (!rel_nz (&R[VX]) && V[VX] == -1)
            {
              memset (&R[VX - 1], 0, sizeof R[VX - 1]);
              V[VX - 1] = 0;
            }
          else if (V[VX] != 0)
            {
              if (rel_nz (&R[VX]) || rel_nz (&R[VX - 1]))
                {
                  error (5);
                }

              memset (&R[VX - 1], 0, sizeof R[VX - 1]);
            }
        }

      V[VX - 1] = sx36 (((uint64_t)V[VX - 1] & ~(uint64_t)V[VX]) & M36);
      break;

    case VOP_OPAREN:
      if (!UNARY[OPX])
        {
          goto S160;
        }

      if (COP < D_RPAR + 4)
        {
          goto S41;
        }

      if (COP > D_RPAR + 4)
        {
          goto S160;
        }

      CURRCH = NEXTCH + 1;
      DEL = 0;
      delscan ();
      if (NEXTCH > CURRCH)
        {
          if (XCARD[CURRCH] != '*')
            {
              goto S160;
            }

          NEXTCH = CURRCH;
          DEL = D_TIMES;
        }

      OPX--;
      TUNARY = 0;
      if (OPX == 0 && (type & 3) == 2)
        {
          goto S76;
        }

      goto S30;

    default: /* OPSTK[0] sentinel: end */
      VAL = (VAL | (V[0] & (int64_t)M36)) & (int64_t)M36;
      REL = R[0];
      if ((type & 3) == 2)
        {
          goto S76;
        }

      goto S160;
    }
  VX--;
S51:
  OPX--;
  goto S40;
S60: /* MODIFIER mode (address-mod tag) */
  if (XCARD[CURRCH] == '*')
    { /* indirect modifier */
      CURRCH++;
      VAL = 060; /* '60'O */
      if (NEXTCH > CURRCH)
        {
          *out_val = VAL & (int64_t)M36;
          *out_rel = REL;
          return;
        }

      if (DEL == D_LPAR)
        {
          goto S20;
        }

      VAL = 020; /* '20'O */
      goto S86;
    }

  {
    int lo = 0, hi = 37, ii;
    char t4[4];
    field_pad4 (t4, CURRCH, NEXTCH);
    if (NEXTCH - CURRCH < 4)
      {
        while (hi >= lo)
          {
            int k = (lo + hi) / 2, c = memcmp (t4, MODSYM[k], 4);
            if (c == 0)
              {
                VAL |= (int64_t)MODVAL[k];
                if (k == 0)
                  {
                    REL.opndtyp = OPERREL;
                  }

                goto S85;
              }

            if (c < 0)
              {
                hi = k - 1;
              }
            else
              {
                lo = k + 1;
              }
          }
      }

    for (ii = CURRCH; ii < NEXTCH; ii++)
      {
        if (XCARD[ii] < '0' || XCARD[ii] > '9')
          { /* a symbol tag */
            int vv = 0;
            struct rel rr = REL_UNDEF;
            field_sym (buf, CURRCH, NEXTCH);
            symtab (buf, &vv, &rr, 0);
            v0 = vv;
            REL = rr;
            goto S76;
          }
      }

    for (ii = CURRCH; ii < NEXTCH; ii++)
      {
        v0 = v0 * 10 + (XCARD[ii] - '0'); /* numeric tag */
      }
  }
S76:
  VAL = (VAL | v0 | 010) & (int64_t)M36; /* '10'O */
S85:
  if (DEL == D_TIMES)
    {
      VAL |= 020;
      CURRCH = NEXTCH + 1;
      delscan ();
    }

S86:
  while (DEL < D_LPAR)
    {
      error (3);
      CURRCH = NEXTCH + 1;
      delscan ();
    }
  goto S160;
S160:
  if (OPX > 0 || VX > 0)
    {
      error (21);
    }

  CURRCH = NEXTCH;
  *out_val = VAL & (int64_t)M36;
  *out_rel = REL;
}

/* ===================================================== object writer ======
 * The XUO$* entry points BMAP calls to build the CP-6/GCOS relocatable object
 * unit.  The real object is a keyed file (records keyed by a structured key
 * incl. a UTS timestamp) built by the ~26k-line XUO$BUILD library
 * (.original/XUO$BUILD.txt); since there is no CP-6 linker to consume it, this
 * port instead emits the *record contents* (per .original/B$OBJECT_C.txt) in a
 * clean, deterministic, self-framed stream -- the same approach ASMDAL took.
 *
 * Each 36-bit word is 5 bytes big-endian (top 4 bits zero).  A record is a
 * header word  TYPE(9) | LEN(9) | ARG(18)  (LEN = total words incl. header)
 * followed by LEN-1 payload words; a reader walks by LEN until END.  The TYPE
 * codes match B$OBJECT_C's record subs (TYPHEAD..TYPSREF); the keyed file's
 * head-record name (B$HEADKEY.TEXT) is folded into the HEAD content here, and
 * relocation is PROG with the SUBTYPREL subtype, exactly as the spec models
 * it (B$RELOCSUBS).  END is a synthetic sentinel (the keyed file has none --
 * its keys delimit records).  Records:
 *   0  HEAD : ARG=severity;  payload id, version, start, name(count[+chars])
 *   1  DNAM : ARG=#names;     B$DNAME def names (COUNT(18)+9-bit chars,
 * packed) 2  RNAM : ARG=#names;     B$DNAME ref names 3  SECT : ARG=section#;
 * payload type, bound, size, name(count[+chars]) 4  EDEF : ARG=#entries;
 * B$EDEF 2-word entries (entry defs) 5  EREF : ARG=#entries;   B$EREF 1-word
 * entries (entry refs); entry i = #i 6  SDEF : ARG=#entries;   B$SDEF 2-word
 * entries (symbol defs) 7  SREF : ARG=#entries;   B$SREF 1-word entries
 * (symbol refs); entry i = #i 10  PROG : ARG=offset;     payload SUBTYP,
 * section#, then code words SUBTYP 0 = program words; SUBTYP 1 = a B$RELOC2
 * directive 511  END  : ARG=start address (synthetic end-of-object marker) An
 * EDEF/SDEF/EREF/SREF entry locates its name by word displacement into the
 * DNAM/RNAM record (NPOINTER); a RELOC's OPERAND for an OPEREREF/OPERSREF
 * field is the EREF/SREF entry number.  Parts 1-2 = HEAD/SECT/PROG(+RELOC);
 * part 3 = DNAM/RNAM/EDEF/EREF/SDEF/SREF; the debug records
 * (LOGBLK/EXST/VREBL) part 4. */
enum
{ /* object record types (B$RECORDSUBS) */
  REC_HEAD = 0,
  REC_DNAM = 1,
  REC_RNAM = 2,
  REC_SECT = 3,
  REC_EDEF = 4,
  REC_EREF = 5,
  REC_SDEF = 6,
  REC_SREF = 7,
  REC_SEGDEF = 8,
  REC_SEGREF = 9,
  REC_PROG = 10,
  REC_LOGBLK = 11,
  REC_VREBL = 14,
  REC_EXST = 13,
  REC_DBGNAM = 15,
  REC_END = 0x1FF
};
enum
{
  SUBTYP_PROG = 0,
  SUBTYP_REL = 1
}; /* PROG subtype (B$RELOCSUBS) */

struct obuf
{
  unsigned char *p;
  size_t n, cap;
};
static struct obuf OBJ, PRG,
    REL_; /* final object; PROG records; RELOC records */

static void
ow (struct obuf *b, W w) /* append a 36-bit word, 5 bytes BE */
{
  if (b->n + 5 > b->cap)
    {
      b->cap = b->cap ? b->cap * 2 : 2048;
      b->p = (unsigned char *)realloc (b->p, b->cap);
      if (!b->p)
        {
          fprintf (stderr, "bmap: out of memory\n");
          exit (2);
        }
    }

  b->p[b->n++] = (w >> 32) & 0xFF;
  b->p[b->n++] = (w >> 24) & 0xFF;
  b->p[b->n++] = (w >> 16) & 0xFF;
  b->p[b->n++] = (w >> 8) & 0xFF;
  b->p[b->n++] = w & 0xFF;
}
static W
pack4 (const char *s, int len) /* up to 4 chars -> one 9-bit-packed word */
{
  W w = 0;
  int i;

  for (i = 0; i < 4; i++)
    {
      w = (w << 9) | (i < len ? ((unsigned char)s[i] & 0x1FF) : 0);
    }

  return w;
}
static int
pack_name (W *out, const char *s, int len) /* count word + char words */
{
  int i, nw = 0;

  out[nw++] = (W)len;
  for (i = 0; i < len; i += 4)
    {
      out[nw++] = pack4 (s + i, len - i < 4 ? len - i : 4);
    }

  return nw;
}
/* B$DNAME (B$OBJECT_C 461): a name in the DNAM/RNAM record -- COUNT in the
 * high 18 bits of word 0 (UBIN HALF), then the characters as packed 9-bit
 * bytes (UNAL), the whole thing rounded up to a word boundary.  Because COUNT
 * is 18 bits and each char is 9, every char lands at an offset of 0/9/18/27
 * within a word, so none ever straddles a boundary.  Returns the word count (=
 * SIZEW, which is also the displacement increment XUO$DNAME/RNAME advance by).
 */
static int
pack_dname (W *out, const char *s, int len)
{
  int nw = (18 + 9 * len + 35) / 36, i;

  for (i = 0; i < nw; i++)
    {
      out[i] = 0;
    }

  out[0] |= ((W)(len & 0777777)) << 18; /* COUNT in bits 0-17 (MSB half) */
  for (i = 0; i < len; i++)
    {
      int bitpos = 18 + 9 * i; /* MSB-first stream offset */
      out[bitpos / 36] |= ((W)((unsigned char)s[i] & 0x1FF))
                          << (36 - bitpos % 36 - 9);
    }

  return nw;
}
static void
emit_record (struct obuf *b, int type, int arg, const W *pay, int n)
{
  int i;

  ow (b, ((W)(type & 0x1FF) << 27) | ((W)((n + 1) & 0x1FF) << 18)
             | ((W)arg & M18));
  for (i = 0; i < n; i++)
    {
      ow (b, pay[i]);
    }
}

/* control-section table (XUO$SECTBUILD).  `pc` is the section's saved location
 * counter (B$SECTION.MBZ in the original): when USE switches away, PC is
 * parked here and restored on the next USE of this section. */
struct osect
{
  int type, bound, size, nlen, pc;
  char name[MAXSYM + 1];
};
static struct osect OSECT[64];
static int NOSECT;
static int
xuo_sectbuild (int type, const char *name, int nlen)
{
  struct osect *s = &OSECT[NOSECT];

  s->type = type;
  s->bound = 0;
  s->size = 0;
  s->pc = 0;
  s->nlen = nlen < MAXSYM ? nlen : MAXSYM;
  memcpy (s->name, name, s->nlen);
  s->name[s->nlen] = '\0';
  return NOSECT++;
}

/* program-word accumulator: contiguous (section, offset) words -> one PROG
 * record */
static int PA_sect = -1, PA_off, PA_n;
static W PA_w[500];
static void
prog_flush (void)
{
  int i;

  if (PA_n == 0)
    {
      return;
    }

  ow (&PRG, ((W)REC_PROG << 27) | ((W)((PA_n + 3) & 0x1FF) << 18)
                | ((W)PA_off & M18));
  ow (&PRG, (W)SUBTYP_PROG); /* program-words subtype */
  ow (&PRG, (W)PA_sect);
  for (i = 0; i < PA_n; i++)
    {
      ow (&PRG, PA_w[i]);
    }

  PA_n = 0;
  PA_sect = -1;
}
static void
xuo_prgm (int section, int pc, W word)
{
  if (PA_n > 0 && section == PA_sect && pc == PA_off + PA_n
      && PA_n < (int)(sizeof PA_w / sizeof PA_w[0]))
    {
      PA_w[PA_n++] = word & M36;
    }
  else
    {
      prog_flush ();
      PA_sect = section;
      PA_off = pc;
      PA_w[0] = word & M36;
      PA_n = 1;
    }

  if (section >= 0 && section < NOSECT && pc + 1 > OSECT[section].size)
    {
      OSECT[section].size = pc + 1;
    }
}

/* XUO$RELOC (SI61 GEN 1438): one relocation directive for the field
 * [stbit..endbit] of the word at (section, pc).  GEN calls this per relocated
 * field -- 7-arg when EVALOP=0, 9-arg (with EVALOP and VALUE) otherwise.
 * Emitted as a self-framed RELOC record (type 11) carrying B$OBJECT_C's
 * B$RELOC2 general form: payload[0] = key section; word0 (payload[1]) =
 * MBZ(6)|OPNDTYP(4)|EVALOP(4)|RELOCOP(4)|OPERAND(18); word1 (payload[2]) =
 * DISP(18)|STBIT(9)|ENDBIT(9) with DISP 0 (the directive is at the keyed
 * offset pc); word2 (payload[3]) = VALUE, present only when EVALOP != 0.
 * (The compact 1-word B$RELOC1 form is an encoder optimisation we skip --
 * the general form represents every case losslessly.) */
static void
xuo_reloc (int section, int pc, int opndtyp, int evalop, int relocop,
           int operand, int stbit, int endbit, long value)
{
  W pay[5];
  int n;

  pay[0] = (W)SUBTYP_REL; /* relocation subtype of PROG (type 10) */
  pay[1] = (W)section;
  pay[2] = ((W)(opndtyp & 017) << 26) | ((W)(evalop & 017) << 22)
           | ((W)(relocop & 017) << 18) | ((W)operand & M18);
  pay[3] = ((W)(stbit & 0777) << 9) | (W)(endbit & 0777); /* DISP = 0 */
  n = 4;
  if (evalop != 0)
    {
      pay[4] = (W)value & M36;
      n = 5;
    }

  emit_record (&REL_, REC_PROG, pc, pay, n);
}

/* object-unit head state (set by the END handler before xuo_outterm) */
static int OBJ_severity, OBJ_start;
static char OBJ_name[MAXSYM + 1];
static int OBJ_nlen;
static int OUNAMESW; /* 0 none, 1 SYMDEF-set, 2 ENTDEF/explicit */

/* def/ref record buffers (one record each, accumulated by a symbol-table sweep
 * at xuo_outterm).  DNAM/RNAM = definition/reference names (B$DNAME, dense);
 * EDEF/SDEF = the B$EDEF/B$SDEF (2-word) def entry tables; EREF/SREF = the
 * B$EREF/B$SREF (1-word) ref entry tables.  EREF/SREF entry positions are the
 * 0-based numbers that part-2 RELOC directives reference (assigned by a sweep
 * between the passes; see assign_ext_numbers). */
#define DEFW_CAP 16384
static W DNAM_w[DEFW_CAP];
static int DNAM_n, DNAM_cnt; /* n = words (= next NPOINTER) */
static W RNAM_w[DEFW_CAP];
static int RNAM_n, RNAM_cnt;
static W EDEF_w[DEFW_CAP];
static int EDEF_n, EDEF_cnt;
static W SDEF_w[DEFW_CAP];
static int SDEF_n, SDEF_cnt;
static W EREF_w[DEFW_CAP];
static int EREF_n, EREF_cnt;
static W SREF_w[DEFW_CAP];
static int SREF_n, SREF_cnt;
static W SEGR_w[DEFW_CAP];
static int SEGR_n, SEGR_cnt;
static int EREF_num, SREF_num,
    SEGR_num; /* next 0-based EREF/SREF/SEGREF number */
/* debug schema (part 4, emitted only with -g): DBGNAM debug names, EXST the
 * per-code-symbol statement table, VREBL the data-symbol variable table,
 * LOGBLK the single logical-block descriptor */
static W DBGN_w[DEFW_CAP];
static int DBGN_n, DBGN_cnt;
static W EXST_w[DEFW_CAP];
static int EXST_n, EXST_cnt;
static W VREB_w[DEFW_CAP];
static int VREB_n, VREB_cnt;
static int DBG_STLINE, DBG_ENLINE; /* logical-block line range */

static void
xuo_ouinit (void)
{
  OBJ.n = 0;
  PRG.n = 0;
  REL_.n = 0;
  NOSECT = 0;
  PA_n = 0;
  PA_sect = -1;
  OBJ_severity = 0;
  OBJ_start = 0;
  OUNAMESW = 0;
  DNAM_n = DNAM_cnt = EDEF_n = EDEF_cnt = SDEF_n = SDEF_cnt = 0;
  RNAM_n = RNAM_cnt = EREF_n = EREF_cnt = SREF_n = SREF_cnt = 0;
  SEGR_n = SEGR_cnt = 0;
  DBGN_n = DBGN_cnt = EXST_n = EXST_cnt = VREB_n = VREB_cnt = 0;
  DBG_STLINE = DBG_ENLINE = 0;
  EREF_num = SREF_num = SEGR_num = 0;
  strcpy (OBJ_name, "NO-NAME");
  OBJ_nlen = 7; /* SI61's default OUNAME */
}
static void
xuo_headname (const char *s, int len)
{
  OBJ_nlen = len < MAXSYM ? len : MAXSYM;
  memcpy (OBJ_name, s, OBJ_nlen);
  OBJ_name[OBJ_nlen] = '\0';
}
static void
xuo_head_severity (int sev)
{
  OBJ_severity = sev;
}

/* XUO$DNAME (XUO$BUILD 176): append a name to the DNAM record, return its word
 * displacement (the NPOINTER stored in the EDEF/SDEF entry). */
static int
xuo_dname (const char *s, int len)
{
  int ptr = DNAM_n, k = (18 + 9 * len + 35) / 36;

  if (DNAM_n + k > DEFW_CAP)
    {
      error (8);
      return ptr;
    }

  pack_dname (DNAM_w + DNAM_n, s, len);
  DNAM_n += k;
  DNAM_cnt++;
  return ptr;
}
/* XUO$RNAME: append a name to the RNAM (reference names) record, return its
 * word displacement (the NPOINTER stored in the EREF/SREF entry). */
static int
xuo_rname (const char *s, int len)
{
  int ptr = RNAM_n, k = (18 + 9 * len + 35) / 36;

  if (RNAM_n + k > DEFW_CAP)
    {
      error (8);
      return ptr;
    }

  pack_dname (RNAM_w + RNAM_n, s, len);
  RNAM_n += k;
  RNAM_cnt++;
  return ptr;
}
/* XUO$EDEF (XUO$BUILD 788): a 2-word B$EDEF entry.  word0 = LFLAGS(9, 0 in the
 * object) | SECTNUM(9) | OFFSET(18); word1 = NPOINTER(18) | PRI(1) | ALT(1) |
 * CHECK(1) | CST(4) | NPARAM(11).  SI61 passes CST=1, ALT=CHECK=NPARAM=0. */
static void
xuo_edef (int section, int value, int nptr, int pri)
{
  if (EDEF_n + 2 > DEFW_CAP)
    {
      error (8);
      return;
    }

  EDEF_w[EDEF_n++] = ((W)(section & 0777) << 18) | ((W)value & M18);
  EDEF_w[EDEF_n++]
      = ((W)(nptr & M18) << 18) | ((W)(pri & 1) << 17) | ((W)1 << 11);
  EDEF_cnt++;
}
/* XUO$SDEF / XUO$SDEF_CONST: a 2-word B$SDEF entry.  CONSTNT=1 -> word0 is a
 * 36-bit constant VALUE; CONSTNT=0 -> word0 = LFLAGS(9)|SECTNUM(9)|OFFSET(18).
 * word1 = NPOINTER(18) | CONSTNT(1). */
static void
xuo_sdef (int opndtyp, int section, int value, int nptr)
{
  if (SDEF_n + 2 > DEFW_CAP)
    {
      error (8);
      return;
    }

  if (opndtyp == 0)
    { /* SDEF_CONST */
      SDEF_w[SDEF_n++] = (W)value & M36;
      SDEF_w[SDEF_n++] = ((W)(nptr & M18) << 18) | ((W)1 << 17);
    }
  else
    { /* address SDEF */
      SDEF_w[SDEF_n++] = ((W)(section & 0777) << 18) | ((W)value & M18);
      SDEF_w[SDEF_n++] = ((W)(nptr & M18) << 18);
    }

  SDEF_cnt++;
}
/* XUO$EREF (XUO$BUILD 942): a 1-word B$EREF entry -- NPOINTER(18) | SREF(1) |
 * ALT(1) | CHECK(1) | CST(4) | NPARAM(11); SI61 passes SREF/ALT/CHECK/NPARAM
 * 0, CST 1.  XUO$SREF: a 1-word B$SREF -- NPOINTER(18) | SREF(1) |
 * READ_ONLY(1) | MBZ(16), both flags 0 for an ordinary SYMREF. */
static void
xuo_eref (int nptr)
{
  if (EREF_n + 1 > DEFW_CAP)
    {
      error (8);
      return;
    }

  EREF_w[EREF_n++] = ((W)(nptr & M18) << 18) | ((W)1 << 11); /* CST = 1 */
  EREF_cnt++;
}
static void
xuo_sref (int nptr)
{
  if (SREF_n + 1 > DEFW_CAP)
    {
      error (8);
      return;
    }

  SREF_w[SREF_n++] = ((W)(nptr & M18) << 18);
  SREF_cnt++;
}
/* XUO$SEGREF: a 1-word B$SEGREF -- NPOINTER(18) | SREF(1) | READ_ONLY(1) | ...
 * (the segment-reference name points into the RNAM record, like SREF). */
static void
xuo_segref (int nptr)
{
  if (SEGR_n + 1 > DEFW_CAP)
    {
      error (8);
      return;
    }

  SEGR_w[SEGR_n++] = ((W)(nptr & M18) << 18);
  SEGR_cnt++;
}
/* Assign the 0-based EREF/SREF/SEGREF number to each external reference, in
 * the same in-order (name) walk xuo_emit_defs uses -- so an entry's position
 * equals its number.  The number lands in the symbol's REL.OPERAND, which the
 * part-2 RELOC directives reference.  Run once between the passes (the number
 * must be set before pass 2 emits relocations). */
static void
assign_ext_numbers (struct sym *t)
{
  if (!t)
    {
      return;
    }

  assign_ext_numbers (t->l);
  if (t->r.opndtyp == OPEREREF)
    {
      t->r.operand = EREF_num++;
    }
  else if (t->r.opndtyp == OPERSREF)
    {
      t->r.operand = SREF_num++;
    }
  else if (t->r.opndtyp == OPERSEGREF)
    {
      t->r.operand = SEGR_num++;
    }

  assign_ext_numbers (t->rt);
}
/* Symbol-table sweep (in name order): emit DNAM+EDEF / DNAM+SDEF for every
 * definition and RNAM+EREF / RNAM+SREF for every external reference, mirroring
 * the SI61 SYMTAB block (4660) that fires once a flagged symbol's OPNDTYP is
 * known.  PRI marks the entry whose name is the object-unit name. */
static void
xuo_emit_defs (struct sym *t)
{
  int len;

  if (!t)
    {
      return;
    }

  xuo_emit_defs (t->l);
  len = (int)strlen (t->name);
  if (t->r.opndtyp == OPEREREF)
    {
      xuo_eref (xuo_rname (t->name, len));
    }
  else if (t->r.opndtyp == OPERSREF)
    {
      xuo_sref (xuo_rname (t->name, len));
    }
  else if (t->r.opndtyp == OPERSEGREF)
    {
      xuo_segref (xuo_rname (t->name, len));
    }
  else if (t->r.opndtyp != OPERUNDEF && !t->r.f_set)
    {
      if (t->r.f_edef)
        {
          if (t->r.opndtyp == OPERSECT)
            {
              xuo_edef (t->r.operand, t->value, xuo_dname (t->name, len),
                        !strcmp (t->name, OBJ_name));
            }
          else
            {
              error (5); /* ENTDEF of a non-relocatable symbol */
            }
        }

      if (t->r.f_sdef)
        {
          xuo_sdef (t->r.opndtyp, t->r.opndtyp == OPERSECT ? t->r.operand : 0,
                    t->value, xuo_dname (t->name, len));
        }
    }

  xuo_emit_defs (t->rt);
}

/* ---- debug schema (part 4): the per-code-symbol statement table ----------
 */
/* XUO$DBGNAME: append a name to the debug-name (DBGNAM) record, return its
 * word displacement (the NPOINTER stored in an EXST/LOGBLK entry). */
static int
xuo_dbgname (const char *s, int len)
{
  int ptr = DBGN_n, k = (18 + 9 * len + 35) / 36;

  if (DBGN_n + k > DEFW_CAP)
    {
      error (8);
      return ptr;
    }

  pack_dname (DBGN_w + DBGN_n, s, len);
  DBGN_n += k;
  DBGN_cnt++;
  return ptr;
}
/* XUO$EXST (XUO$BUILD 1908): a 3-word B$EXST statement entry.
 * word0 = LA(1)|SUBSCRIPT(8)|SECTNUM(9)|OFFSET(18); word1 = NPOINTER(18)|
 * LBE(18); word2 = COS(1)|LINENUM(10)|STTYPE(7).  LA/SUBSCRIPT/COS are 0 here.
 */
static void
xuo_exst (int section, int offset, int nptr, int lbe, int sttype, int linenum)
{
  if (EXST_n + 3 > DEFW_CAP)
    {
      error (8);
      return;
    }

  EXST_w[EXST_n++] = ((W)(section & 0777) << 18) | ((W)offset & M18);
  EXST_w[EXST_n++] = ((W)(nptr & M18) << 18) | ((W)lbe & M18);
  EXST_w[EXST_n++] = ((W)(linenum & 01777) << 25) | ((W)(sttype & 0177) << 18);
  EXST_cnt++;
}
/* XUO$VREBL (XUO$BUILD 2403): a 5-word B$VREBL variable entry for a data
 * symbol. word0 ADR.W|C|B|ADDRTYP|DATATYP|REF|MODF; word1
 * LOGSIZ|LEVEL|ALIGNTYP|SZTYP| ARRAYTYP|OPNDTYP; word2
 * IMPTR(-1)|OPERAND(section); word3 ELMNTSIZ|STATUS| SCALE (0); word4
 * DIMS(0)|NPOINTER.  SI61 passes DATATYP=1, LOGSIZ=36, LEVEL=ALIGNTYP=1, the
 * rest 0; ADDRTYP is the per-symbol address type. */
static void
xuo_vrebl (int value, int addrtyp, int opndtyp, int operand, int ref, int nptr)
{
  if (VREB_n + 5 > DEFW_CAP)
    {
      error (8);
      return;
    }

  VREB_w[VREB_n++] = ((W)(value & M18) << 18) | ((W)(addrtyp & 017) << 8)
                     | ((W)1 << 2) | ((W)(ref & 1) << 1); /* DATATYP=1 */
  VREB_w[VREB_n++] = ((W)36 << 18) | ((W)1 << 12) | ((W)1 << 9)
                     | ((W)(opndtyp & 017)); /* LOGSIZ|LEVEL|ALIGN */
  VREB_w[VREB_n++]
      = ((W)M18 << 18) | ((W)operand & M18); /* IMPTR=-1 | OPERAND */
  VREB_w[VREB_n++] = 0;
  VREB_w[VREB_n++] = (W)nptr & M18;
  VREB_cnt++;
}
/* B$ATYPE (SI61 61): the VREBL address type indexed by REL.OPNDTYP. */
static const int ATYPE[16]
    = { 3, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
/* Symbol-table sweep (name order): for every defined non-SET symbol, emit
 * DBGNAM + either EXST (a label in a CODESECTION, SI61 SYMTAB 4736) or VREBL
 * (a data/absolute symbol whose address type K is non-zero, SI61 420).  All
 * entries reference logical block 0. */
static void
xuo_emit_dbg (struct sym *t)
{
  if (!t)
    {
      return;
    }

  xuo_emit_dbg (t->l);
  if (t->defined && !t->r.f_set)
    {
      int len = (int)strlen (t->name);
      int sect = (t->r.opndtyp == OPERSECT && t->r.operand >= 0
                  && t->r.operand < NOSECT)
                     ? OSECT[t->r.operand].type
                     : -1;
      if (sect == CODESECTION)
        { /* code label -> EXST */
          xuo_exst (t->r.operand, t->value, xuo_dbgname (t->name, len), 0, 1,
                    t->line);
          if (t->line > DBG_ENLINE)
            {
              DBG_ENLINE = t->line;
            }
        }
      else
        { /* data/abs -> VREBL if K != 0 */
          int k = (t->r.opndtyp == OPERSECT) ? (sect == DCBSECTION ? 4 : 1)
                                             : ATYPE[t->r.opndtyp & 017];
          if (k)
            {
              xuo_vrebl (t->value, k, t->r.opndtyp,
                         t->r.opndtyp == OPERSECT ? t->r.operand : 0,
                         t->r.f_refed, xuo_dbgname (t->name, len));
            }
        }
    }

  xuo_emit_dbg (t->rt);
}
static void
xuo_outterm (void) /* flush all records to OBJ */
{
  W pay[16];
  int i, n;

  prog_flush ();
  xuo_emit_defs (SYMROOT_); /* build DNAM/RNAM/EDEF/EREF/SDEF/SREF */
  if (opt_debug)
    {
      xuo_emit_dbg (SYMROOT_); /* build DBGNAM/EXST (debug schema) */
    }

  /* HEAD: id, version, start, then the object-unit name.  The keyed file
   * holds the name in the head record's key (B$HEADKEY.TEXT); the self-framed
   * stream serializes it into the head content. */
  pay[0] = pack4 ("GMAP", 4);
  pay[1] = pack4 ("B00", 3);
  pay[2] = (W)OBJ_start & M36;
  n = 3 + pack_name (pay + 3, OBJ_name, OBJ_nlen);
  emit_record (&OBJ, REC_HEAD, OBJ_severity, pay, n);
  if (DNAM_n)
    {
      emit_record (&OBJ, REC_DNAM, DNAM_cnt, DNAM_w, DNAM_n); /* def names */
    }

  if (RNAM_n)
    {
      emit_record (&OBJ, REC_RNAM, RNAM_cnt, RNAM_w, RNAM_n); /* ref names */
    }

  for (i = 0; i < NOSECT; i++)
    { /* SECT records */
      W sp[12];
      int m = 3;
      sp[0] = OSECT[i].type;
      sp[1] = OSECT[i].bound;
      sp[2] = OSECT[i].size;
      m += pack_name (sp + 3, OSECT[i].name, OSECT[i].nlen);
      emit_record (&OBJ, REC_SECT, i, sp, m);
    }

  if (EDEF_n)
    {
      emit_record (&OBJ, REC_EDEF, EDEF_cnt, EDEF_w, EDEF_n); /* entry defs */
    }

  if (EREF_n)
    {
      emit_record (&OBJ, REC_EREF, EREF_cnt, EREF_w, EREF_n); /* entry refs */
    }

  if (SDEF_n)
    {
      emit_record (&OBJ, REC_SDEF, SDEF_cnt, SDEF_w, SDEF_n); /* symbol defs */
    }

  if (SREF_n)
    {
      emit_record (&OBJ, REC_SREF, SREF_cnt, SREF_w, SREF_n); /* symbol refs */
    }

  if (SEGR_n)
    {
      emit_record (&OBJ, REC_SEGREF, SEGR_cnt, SEGR_w,
                   SEGR_n); /* segment refs */
    }

  { /* append the PROG records, then the RELOC records */
    size_t extra = PRG.n + REL_.n;
    if (OBJ.n + extra > OBJ.cap)
      {
        OBJ.cap = OBJ.n + extra + 64;
        OBJ.p = (unsigned char *)realloc (OBJ.p, OBJ.cap);
        if (!OBJ.p)
          {
            fprintf (stderr, "bmap: out of memory\n");
            exit (2);
          }
      }

    if (PRG.n)
      {
        memcpy (OBJ.p + OBJ.n, PRG.p, PRG.n);
        OBJ.n += PRG.n;
      }

    if (REL_.n)
      {
        memcpy (OBJ.p + OBJ.n, REL_.p, REL_.n);
        OBJ.n += REL_.n;
      }
  }
  if (opt_debug && (EXST_n || VREB_n))
    {          /* debug schema (B$OBJECT_C 11/13/14/15) */
      W lb[4]; /* one LOGBLK descriptor (block 0) */
      lb[0] = ((W)(DBG_STLINE & M18) << 18) | ((W)DBG_ENLINE & M18);
      lb[1] = ((W)M18 << 18) | 1; /* NPOINTER = -1 (no name) | LEXLVL = 1 */
      lb[2] = (W)EXST_cnt & M18;  /* STOFFST 0 | STSIZE = #EXST */
      lb[3] = (W)VREB_cnt & M18;  /* VAOFFST 0 | VASIZE = #VREBL */
      emit_record (&OBJ, REC_LOGBLK, 1, lb, 4);
      if (EXST_n)
        {
          emit_record (&OBJ, REC_EXST, EXST_cnt, EXST_w, EXST_n);
        }

      if (VREB_n)
        {
          emit_record (&OBJ, REC_VREBL, VREB_cnt, VREB_w, VREB_n);
        }

      if (DBGN_n)
        {
          emit_record (&OBJ, REC_DBGNAM, DBGN_cnt, DBGN_w, DBGN_n);
        }
    }

  emit_record (&OBJ, REC_END, OBJ_start, NULL, 0); /* synthetic end marker */
}

/* ===================================================== code generation ====
 * GEN (SI61 1326): assemble NF fields of the given bit-widths into a 36-bit
 * word MSB-first (the original's two BITINSERTs = a left-shift + OR), in PASS1
 * just bump PC, in PASS2 record the word + its relocation and bump PC.  In
 * PASS2 a non-forward-reference word is emitted to the object via XUO$PRGM,
 * then each relocated field via XUO$RELOC (the FR build is phase 8).
 * GENLOC/GENVAL format the octal listing (per-field octal + reloc markers). */
static const int IFORM[7]
    = { 3, 15, 12, 6, 18, 12, 6 }; /* non-EIS instruction */
static const int XFORM[11]
    = { 3, 15, 6, 3, 3, 6, 18, 6, 3, 3, 6 }; /* index instruction */
static const int ZFORM[2] = { 18, 18 }; /* ZERO pseudo-op */
static const int RFORM[5] = { 8, 3, 7, 12, 6 }; /* REPEAT instruction */
static int INHIB_BIT28; /* INHIB pseudo-op state (added to opcode) */
static int IDS;         /* EIS modifier-field index (phase 5+) */

static W gen_word;                   /* assembled 36-bit word (last GEN) */
static int gen_has_word;             /* a word was assembled this statement */
static int gen_frf;                  /* a field was a forward reference */
static char gen_octal[160];          /* GENVAL per-field octal text */
static int gen_loc_pc, gen_loc_sect; /* GENLOC location */
static W gen_log[64];                /* words assembled this statement */
static int gen_logn;
static W gen_val; /* EQU/SET value (for the listing) */
static int gen_has_val;

static void
genloc (int has_val, int64_t val, int has_sect, int sect)
{
  if (has_val)
    {
      PC = (int)val;
      if (has_sect)
        {
          PCREL.operand = sect;
        }
    }

  if (PASS2 == 0)
    {
      return;
    }

  gen_loc_pc = PC;
  gen_loc_sect = PCREL.operand;
}
static void
genval (int nf, const int *nb, const int64_t *val, const struct rel *rel)
{
  char *p = gen_octal;
  int i, d;

  if (PASS2 == 0)
    {
      return;
    }

  for (i = 0; i < nf; i++)
    {
      int k = (nb[i] + 2) / 3;
      W fv = (W)val[i] & (nb[i] >= 36 ? M36 : (((W)1 << nb[i]) - 1));
      char mark = ' ';
      if (rel[i].opndtyp == OPERSECT)
        {
          mark = (char)('0' + rel[i].operand % 10);
        }
      else if (rel[i].opndtyp == OPERUNDEF)
        {
          mark = 'F';
        }
      else if (rel_nz (&rel[i]))
        {
          mark = 'X';
        }

      *p++ = ' ';
      *p++ = mark;
      for (d = k - 1; d >= 0; d--)
        {
          *p++ = (char)('0' + (int)((fv >> (3 * d)) & 7));
        }
    }

  *p = '\0';
}
static void
gen (int nf, const int *nb, const int64_t *val, const struct rel *rel)
{
  W tval = 0;
  int i;

  if (PASS2 == 0)
    {
      PC++;
      return;
    }

  gen_frf = 0;
  for (i = 0; i < nf; i++)
    {
      if (rel_nz (&rel[i])
          && (rel[i].opndtyp == OPERUNDEF
              || (rel[i].evalop == 0 && rel[i].s_opndtyp == OPERUNDEF)))
        {
          gen_frf = 1;
        }

      if (nb[i] >= 36)
        {
          tval = (W)val[i] & M36;
        }
      else
        {
          tval = ((tval << nb[i]) | ((W)val[i] & (((W)1 << nb[i]) - 1))) & M36;
        }
    }

  gen_word = tval;
  gen_has_word = 1;
  if (gen_logn < (int)(sizeof gen_log / sizeof gen_log[0]))
    {
      gen_log[gen_logn++] = tval;
    }

  /* emit the word to the object unit (XUO$PRGM), then walk the fields (SI61
   * GEN 1399-1447): each relocated field -> a XUO$RELOC directive, tracking
   * the bit offset FB.  A second additive/subtractive relocation (R1+R2,
   * stored in the field's S word with EVALOP=0) -> a second directive over
   * the same bit range.  A word with an undefined-symbol field (gen_frf) is
   * not emitted here -- its FR fixup is deferred (the two-pass model resolves
   * ordinary forward refs; literals are interned separately by genlits). */
  if (OPTIONS.ou && !gen_frf)
    {
      int fb = 0;
      xuo_prgm (PCREL.operand, PC, tval);
      for (i = 0; i < nf; i++)
        {
          if (rel_nz (&rel[i]))
            {
              xuo_reloc (PCREL.operand, PC, rel[i].opndtyp, rel[i].evalop,
                         rel[i].relocop, rel[i].operand, fb, fb + nb[i] - 1,
                         rel[i].value);
              if (rel[i].evalop == 0 && rel_s_nz (&rel[i]))
                {
                  xuo_reloc (PCREL.operand, PC, rel[i].s_opndtyp, 0,
                             rel[i].s_relocop, rel[i].s_operand, fb,
                             fb + nb[i] - 1, 0);
                }
            }

          fb += nb[i];
        }
    }

  genloc (0, 0, 0, 0);
  PC++;
  genval (nf, nb, val, rel);
}

/* XLATEV (BMAP_SIG.XSI): translate `srcnc` source characters (from XCARD at
 * srcoff) through `table` into `dest` as `nb`-bit characters (36/nb per word,
 * MSB-first), starting at character offset destfc.  Source past the card image
 * reads as blank (the card is space-padded).  Used by ASCII/BCI/EBCDIC and the
 * VFD character fields. */
static void
xlatev (int nb, W *dest, int destfc, int destnc, const unsigned short *table,
        int srcoff, int srcnc)
{
  int cpw = 36 / nb, n = destnc < srcnc ? destnc : srcnc, i;

  for (i = 0; i < n; i++)
    {
      int p = destfc + i, sc = srcoff + i;
      int ch = (sc >= 0 && sc < XCSIZE) ? (unsigned char)XCARD[sc] : ' ';
      unsigned code = table[ch] & ((1u << nb) - 1);
      dest[p / cpw] |= (W)code << (36 - (p % cpw + 1) * nb);
    }
}

/* GENLITS (SI61 875): emit the literal pool to the LITERALS section.  Called
 * at END (pass 2) after the code: switch the GEN section context to LITSECT,
 * emit each interned literal word in pc order, restore, and record the pool
 * size for the SECT record.  In pass 1 GEN only bumps PC, so this is harmless
 * there. */
static void
genlits (void)
{
  static const int NB36[1] = { 36 };
  struct rel saverel = PCREL, r;
  int savepc = PC, i;

  if (n_lit == 0)
    {
      return;
    }

  PCREL.operand = LITSECT; /* GENLOC(0, LITSECT) */
  for (i = 0; i < n_lit; i++)
    {
      int j;
      PC = LITTAB[i].pc;
      for (j = 0; j < LITTAB[i].nw; j++)
        { /* each word (gen bumps PC) */
          int64_t v = LITTAB[i].v[j];
          memset (&r, 0, sizeof r);
          gen (1, NB36, &v, &r);
        }
    }

  PC = savepc;
  PCREL = saverel;
  if (OPTIONS.ou && LITSECT < NOSECT && LITLOC > OSECT[LITSECT].size)
    {
      OSECT[LITSECT].size = LITLOC;
    }
}

/* VFD (SI61 5614): pack a variable-field-definition into 36-bit words.  Each
 * comma-separated field is `[type]width/value`: an optional type letter (O
 * octal, A/U ascii, E ebcdic, H bcd, Z/R right-justified char) then a decimal
 * bit width, '/', then a value -- a VARSCAN expression for octal/no-type, or
 * character text otherwise.  Fields are concatenated MSB-first; a field that
 * crosses a 36-bit boundary is split (a *relocated* split field -> error 5,
 * as in the original).  This reorganises SI61's FB=72-W bit-splice into an
 * equivalent per-word accumulator.  OPD (the opcode-defining form) is
 * deferred. */
static void
vfd (void)
{
  int64_t fval[40];
  int fw[40];
  struct rel frel[40];
  int nf = 0, b = 0, i;

  for (;;)
    { /* L1810: one field per pass */
      int j, m = 0, tx = 0, ww = 0, rem, split = 0;
      int64_t fv = 0, v;
      struct rel frl;
      if (nextfld ())
        {
          break; /* L1835: no more fields */
        }

      j = CONTROLS[(unsigned char)XCARD[CURRCH]];
      switch (j)
        {
        case 1:
          CURRCH++;
          m = 1;
          break; /* O octal */

        case 2:
          CURRCH++;
          m = 9;
          tx = 2;
          break; /* A,U ascii */

        case 6:
          CURRCH++;
          m = -9;
          tx = 2;
          break; /* Z right-ascii */

        case 4:
          CURRCH++;
          m = 9;
          tx = 1;
          break; /* E ebcdic */

        case 3:
          CURRCH++;
          m = 6;
          tx = 0;
          break; /* H bcd */

        case 7:
          CURRCH++;
          m = -6;
          tx = 0;
          break; /* R right-bcd */

        default:
          m = 0;
          break; /* value field */
        }
      if (DEL != D_DIV)
        {
          error (4);
          return;
        } /* width must end at '/' */

      for (i = CURRCH; i < NEXTCH; i++)
        { /* width = decimal bit count */
          if (XCARD[i] < '0' || XCARD[i] > '9')
            {
              error (4);
              break;
            }

          ww = ww * 10 + (XCARD[i] - '0');
        }

      if (ww == 0)
        {
          error (4);
        }

      memset (&frl, 0, sizeof frl);
      if (m > 1 || m < 0)
        { /* character field */
          int am = m < 0 ? -m : m, jc, cpw, fc, ti, p, nc;
          W cw = 0;
          if (ww > 36)
            {
              error (4);
              ww = 36;
            } /* wide (>1 word) char field: deferred */

          jc = (ww + am - 1) / am;
          cpw = 36 / am;
          fc = cpw - jc;
          if (fc < 0)
            {
              fc = 0;
            }

          ti = NEXTCH + 1; /* text: from after '/' to comma/blank */
          p = ti;
          while (p < XCSIZE && XCARD[p] != ',' && XCARD[p] != ' ')
            {
              p++;
            }
          NEXTCH = p;
          DEL = (p < XCSIZE && XCARD[p] == ',') ? D_COMMA : D_BLANK;
          nc = NEXTCH - ti;
          if (m < 0 && nc > jc)
            {
              ti += nc - jc; /* right-justified */
            }

          xlatev (am, &cw, fc, jc, bmap_asciit[tx], ti, nc);
          fv = (int64_t)cw;
          CURRCH = NEXTCH;
        }
      else
        { /* octal / value field */
          if (ww > 36)
            {
              error (4);
              ww = 36;
            }

          CURRCH = NEXTCH;
          varscan (&fv, &frl, m + 4); /* m=1 -> octal+lit, m=0 -> dec+lit */
        }

      rem = ww;
      v = fv & (rem >= 36 ? (int64_t)M36 : (((int64_t)1 << rem) - 1));
      while (rem > 0)
        { /* place ww bits, splitting at word edge */
          int avail = 36 - b, take = rem < avail ? rem : avail;
          fval[nf] = (v >> (rem - take)) & (((int64_t)1 << take) - 1);
          fw[nf] = take;
          if (take == ww && !split)
            {
              frel[nf] = frl; /* whole field */
            }
          else
            {
              if (rel_nz (&frl))
                {
                  error (5);
                }

              memset (&frel[nf], 0, sizeof frel[nf]);
              split = 1;
            }

          nf++;
          b += take;
          rem -= take;
          if (b == 36)
            {
              gen (nf, fw, fval, frel);
              nf = 0;
              b = 0;
            }
        }
      if (DEL != D_COMMA)
        {
          break; /* L1830 */
        }
    }

  if (b != 0)
    { /* L1835: pad partial word + emit */
      fval[nf] = 0;
      memset (&frel[nf], 0, sizeof frel[nf]);
      fw[nf] = 36 - b;
      nf++;
    }

  if (nf > 0)
    {
      gen (nf, fw, fval, frel);
    }
}

/* ===================================================== instruction encode ==
 * INST (SI61 2029): the OP.TYPE encoder.  Phase 5 implements the bulk
 * instruction types 1 (non-EIS), 2 (index) and 6 (no variable field), plus 0
 * (ignore); the data/EIS/descriptor/IO/CLIMB types are deferred. */

/* IC-relative address fix-up shared by the IFORM/XFORM paths (SI61 2167). */
static void
ic_relative (int64_t *val, struct rel *rel, int ai, int ti)
{
  if (rel[ti].opndtyp == OPERREL
      || (LISTING.floatf && val[ti] == 0 && !rel_nz (&rel[ti])
          && (rel_eq (&rel[ai], &PCREL)
              || (rel[ai].opndtyp == OPERUNDEF && rel[ai].relocop == RELOCOPADD
                  && rel[ai].evalop == 0 && !rel_s_nz (&rel[ai])))))
    {
      val[ai] -= PC;
      if (rel_eq (&rel[ai], &PCREL))
        {
          memset (&rel[ai], 0, sizeof rel[ai]);
        }
      else if (rel[ai].evalop == 0 && !rel_s_nz (&rel[ai]))
        {
          rel_set_s (&rel[ai], &PCREL);
          rel[ai].s_relocop = RELOCOPSUB;
        }
      else
        {
          error (5);
        }

      val[ti] = 4;
      memset (&rel[ti], 0, sizeof rel[ti]); /* IC */
    }
}
/* L102 (SI61 2164): scan address + tag, IC fix-up, optional AR field, emit
 * the 3- or 4-field IFORM word.  Caller has set val[2]/rel[2] = opcode. */
static void
inst_l102 (int64_t *val, struct rel *rel)
{
  int k;

  varscan (&val[1], &rel[1], 4);        /* address (literal allowed) */
  varscan (&val[3], &rel[3], OP->mask); /* tag (modifier mode if mask=2) */
  ic_relative (val, rel, 1, 3);
  if (DEL == D_COMMA)
    {
      k = 0;
      varscan (&val[0], &rel[0], 0);
      if (!OP->ar)
        {
          val[2]++;
        }
    }
  else
    {
      k = 1;
      if (OP->ar)
        {
          error (14);
        }
    }

  gen (4 - k, &IFORM[4 * k], &val[k], &rel[k]);
}
/* EIS instruction formats (SI61 2082): six 7-field formats indexed by
 * K = OP.MASK*2 + OP.AR (1-6).  GEN packs val[L..6] using EFORM[7*K-7+L..]. */
static const int EFORM[42] = {
  0, 0, 9, 1, 8, 12, 6, /* K=1 */
  0, 0, 0, 9, 9, 12, 6, /* K=2 */
  1, 4, 4, 1, 8, 12, 6, /* K=3 */
  0, 0, 1, 8, 9, 12, 6, /* K=4 */
  1, 8, 1, 1, 7, 12, 6, /* K=5 */
  0, 2, 7, 2, 7, 12, 6  /* K=6 */
};

/* MFSCAN (SI61): scan one EIS modification field into MF (value + relocation).
 * Two syntaxes: a plain octal value (the raw MF byte), or `(ar,rl,id,reg)`
 * packing AR(bit29)|RL(30)|ID(31)|REG(32-35) -- i.e. AR<<6|RL<<5|ID<<4|REG in
 * the low 7 bits; reg is a register modifier name (type 2).  An empty field
 * leaves MF = 0. */
struct mf
{
  int64_t v;
  struct rel rel;
};
static void
mfscan (struct mf *m)
{
  m->v = 0;
  memset (&m->rel, 0, sizeof m->rel);
  if (!nextfld ())
    { /* a field is present -> plain value */
      CURRCH--;
      DEL = 0;
      varscan (&m->v, &m->rel, 1);
      return;
    }

  if (DEL != D_LPAR)
    {
      CURRCH = NEXTCH;
      return;
    } /* S30: no field, not "(" */

  { /* parenthesized (ar,rl,id,reg) */
    int64_t ar = 0, rl = 0, id = 0, reg = 0, vv;
    int j;
    for (j = 0; j <= 3; j++)
      {
        varscan (&vv, &m->rel,
                 j < 3 ? 0 : 2); /* reg (j=3) is a modifier name */
        if (rel_nz (&m->rel) && j < 3)
          {
            error (5);
          }

        if (j == 0)
          {
            ar = vv;
          }
        else if (j == 1)
          {
            rl = vv;
          }
        else if (j == 2)
          {
            id = vv;
          }
        else
          {
            reg = vv;
          }
      }

    m->v = ((ar & 1) << 6) | ((rl & 1) << 5) | ((id & 1) << 4) | (reg & 017);
    if (DEL == D_RPAR)
      {
        CURRCH++;
        delscan ();
        CURRCH = NEXTCH;
      }
  }
}

/* The EIS modification fields are global so the descriptor directives that
 * follow an EIS instruction (ADSC/VDSC/BDSC/NDSC) can read MF[IDS]'s RL/AR
 * bits.  EIS sets NDS (descriptor count) and resets IDS=-1; each descriptor
 * does IDS++ (capped at slot 3, the all-zero default).  SI61: IDS/NDS/MF. */
static struct mf MF_[4];
static int NDS;
static int
mf_rl (int i)
{
  return (i >= 0 && i < 4) ? (int)((MF_[i].v >> 5) & 1) : 0;
}
static int
mf_ar (int i)
{
  return (i >= 0 && i < 4) ? (int)((MF_[i].v >> 6) & 1) : 0;
}

/* descriptor formats (SI61 2058-2115): K=0 has a 3+15 address-register prefix,
 * K=1 an 18-bit address; both pack to 36 bits. */
static const int ADSCF[11] = { 3, 15, 3, 2, 1, 12, 18, 3, 2, 1, 12 };
static const int VDSCF[9] = { 3, 15, 13, 1, 4, 18, 13, 1, 4 };
static const int BDSCF[9] = { 3, 15, 2, 4, 12, 18, 2, 4, 12 };
static const int NDSCF[13] = { 3, 15, 3, 1, 2, 6, 6, 18, 3, 1, 2, 6, 6 };
/* NSA descriptor / entry-descriptor / vector formats (SI61 2078/2081/2144). */
static const int DSCF[23] = { 18, 2, 9, 3,  4, 20, 9, 3, 4,  0,  18, 2,
                              3,  9, 4, 20, 3, 9,  4, 0, 36, 34, 2 };
static const int EDSCF[10] = { 18, 1, 10, 3, 4, 10, 26, 10, 24, 2 };
static const int VECFORM[30]
    = { 18, 2, 9,  2, 5, 20, 9, 2,  5, 0, 18, 2,  4, 12, 0,
        20, 4, 12, 0, 0, 18, 2, 16, 0, 0, 20, 16, 0, 0,  0 };

/* FLAGS (SI61): scan an optional flag-name list into *tv (a 9-bit field). Each
 * of M/N/P/E/X/B/S/W/R sets bit 2^(name index): M=1, N=2, ... R=256.
 * ALL=0o777, NONE=3, NOT inverts the following names.  If absent (and the next
 * char is not
 * `(`), *tv keeps the caller's default. */
static void
flags_scan (int64_t *tv)
{
  static const char *const FLAG[12]
      = { "M", "N", "P", "E", "X", "B", "S", "W", "R", "ALL", "NONE", "NOT" };
  int notf = 0;

  if (nextfld ())
    { /* S5: no field on the first try */
      if (DEL != D_LPAR)
        {
          return; /* no flags -> keep the caller default */
        }

      *tv = 3;
      if (nextfld ())
        {
          return; /* "(" then nothing */
        }
    }
  else
    {
      *tv = 3; /* a flag field is present */
    }

  for (;;)
    {
      int i, n = NEXTCH - CURRCH, k = 0;
      for (i = 0; i < 12; i++)
        {
          if ((int)strlen (FLAG[i]) == n
              && !memcmp (&XCARD[CURRCH], FLAG[i], (size_t)n))
            {
              k = i + 1;
              break;
            }
        }

      if (k == 0)
        {
          error (4);
        }
      else if (k <= 9)
        {
          int64_t b = (int64_t)1 << (k - 1);
          if (notf)
            {
              *tv &= ~b;
            }
          else
            {
              *tv |= b;
            }
        }
      else if (k == 10)
        {
          *tv = 0777;
        }
      else if (k == 11)
        {
          *tv = 3;
          return;
        }
      else
        {
          notf = 1;
        }

      CURRCH = NEXTCH; /* S40 */
      if (DEL != D_COMMA)
        {
          return;
        }

      if (nextfld ())
        {
          return; /* S10: the next flag */
        }
    }
}

/* CHARBIN (SI61 EDEC helper): decimal value of the digit run XCARD[s, e). */
static int
charbin (int s, int e)
{
  int v = 0, i;

  for (i = s; i < e; i++)
    {
      v = v * 10 + (XCARD[i] - '0');
    }

  return v;
}

/* EDINS (BMAP_SI61 EDEC helper): insert one edit byte CH (its low NB bits)
 * into the current output word *acc at bit FB (MSB-first), as BITINSERT does;
 * when the word fills (FB>=36) emit it via GEN(1,36,...) and start a fresh
 * one.  NB is 9 (one ASCII byte per char) or 4 (a packed nibble -- bit 0 of
 * each 9-bit group is left clear, so a group-aligned FB is bumped past it
 * first).  Each call consumes one of the NC field positions and is a no-op
 * once NC hits 0, which is how the field is padded/truncated to exactly NC
 * characters. */
static void
edec_edins (int CH, int NB, int *NC, int *FB, int64_t *acc, int *NW)
{
  static const int NB36[1] = { 36 };
  struct rel r;
  int64_t w;

  if (*NC <= 0)
    {
      return;
    }

  if (NB == 4 && (*FB % 9) == 0)
    {
      (*FB)++;
    }

  *acc |= (int64_t)(CH & ((1 << NB) - 1)) << (36 - *FB - NB);
  (*NC)--;
  *FB += NB;
  if (*FB >= 36)
    {
      w = *acc & (int64_t)M36;
      memset (&r, 0, sizeof r);
      gen (1, NB36, &w, &r); /* GEN(1,36,VAL.I,0); PRINT; NW=NW+1 */
      (*NW)++;
      *acc = 0;
      *FB = 0;
    }
}

/* DATE (SI61 CASE 44): build the 36-bit date word M$TIME would produce.  The
 * CP-6 monitor returns the date as the 8-char string "MM/DD/YY" (confirmed in
 * the CP-6 listings); CASE 44 then does INSERT(DBUF,6,2,DBUF) -- overwrite the
 * two chars at 1-based position 6 (the second "/" and the first year digit)
 * with the first two chars of DBUF (the month) -- and XLATEV-packs six BCD
 * characters from SUBSTR(DBUF,2) (0-based offset 2) into one word, six bits
 * each, MSB first.  We replicate those operations literally.
 *
 * For reproducible builds the clock is taken from $SOURCE_DATE_EPOCH (the
 * reproducible-builds.org convention: a decimal Unix time, interpreted as UTC)
 * when that variable is set; otherwise the live local clock is used, exactly
 * as the original's non-deterministic M$TIME call did. */
static W
date_word (void)
{
  char dbuf[16];
  const char *epoch = getenv ("SOURCE_DATE_EPOCH");
  struct tm tmv, *tp;
  time_t t;
  W w = 0;
  int i;

  if (epoch && *epoch)
    { /* deterministic: epoch as UTC */
      t = (time_t)strtoll (epoch, NULL, 10);
      tp = gmtime (&t);
    }
  else
    { /* faithful live clock (local) */
      t = time (NULL);
      tp = localtime (&t);
    }

  if (tp)
    {
      tmv = *tp;
    }
  else
    {
      memset (&tmv, 0, sizeof tmv);
    }

  strftime (dbuf, sizeof dbuf, "%m/%d/%y", &tmv); /* DBUF = "MM/DD/YY" */
  dbuf[5] = dbuf[0];      /* INSERT(DBUF,6,2,DBUF): pos 6, */
  dbuf[6] = dbuf[1];      /*   1-based, overwrite with "MM" */
  for (i = 0; i < 6; i++) /* XLATEV 6 BCD from SUBSTR(DBUF,2) */
    {
      w |= (W)(bmap_asciit[0][(unsigned char)dbuf[2 + i]] & 077)
           << (36 - 6 * (i + 1));
    }

  return w & M36;
}

static void
inst (void)
{
  /* two guard slots below index 0: the NSA descriptor (CASE 35, ODSB) can
   * transiently index val[-1], as SI61 does off the front of VAL. */
  int64_t valbuf[40];
  struct rel relbuf[40];
  int64_t *val = valbuf + 2;
  struct rel *rel = relbuf + 2;
  int k;

  switch (OP->type)
    {
    case 0: /* ignore (NONOP/MARK/...) */
      STMNTCT++;
      break;

    case 1: /* NON-EIS INSTRUCTION */
      if (PASS2 == 0)
        {
          PC++;
          return;
        }

      STMNTCT++;
      IDS++;
      val[2] = OP->val + INHIB_BIT28;
      memset (&rel[2], 0, sizeof rel[2]);
      inst_l102 (val, rel);
      break;

    case 2: /* INDEX INSTRUCTION */
      if (PASS2 == 0)
        {
          PC++;
          return;
        }

      STMNTCT++;
      varscan (&val[2], &rel[2], 0);
      if (!rel_nz (&rel[2]))
        { /* absolute index register 0..7 */
          if (val[2] < 0 || val[2] > 7)
            {
              error (3);
              val[2] = ((val[2] % 8) + 8) % 8;
            }

          val[2] = 8 * val[2] + OP->val + INHIB_BIT28;
          memset (&rel[2], 0, sizeof rel[2]);
          inst_l102 (val, rel);
          break;
        }

      val[3] = val[2];
      rel[3] = rel[2]; /* relocatable: 6-field XFORM */
      val[2] = OP->val / 64;
      memset (&rel[2], 0, sizeof rel[2]);
      val[4] = OP->val;
      memset (&rel[4], 0, sizeof rel[4]);
      varscan (&val[1], &rel[1], 4);
      varscan (&val[5], &rel[5], OP->mask);
      ic_relative (val, rel, 1, 5);
      if (DEL == D_COMMA)
        {
          k = 0;
          varscan (&val[0], &rel[0], 0);
          if (!OP->ar)
            {
              val[4]++;
            }
        }
      else
        {
          k = 1;
          if (OP->ar)
            {
              error (14);
            }
        }

      gen (6 - k, &XFORM[6 * k], &val[k], &rel[k]);
      break;

    case 6: /* INSTRUCTION W/O VARIABLE FIELD */
      if (PASS2 == 0)
        {
          PC++;
          return;
        }

      STMNTCT++;
      val[0] = 0;
      memset (&rel[0], 0, sizeof rel[0]);
      val[1] = OP->val + INHIB_BIT28;
      memset (&rel[1], 0, sizeof rel[1]);
      val[2] = 0;
      memset (&rel[2], 0, sizeof rel[2]);
      gen (3, &IFORM[4], &val[0], &rel[0]);
      break;

    case 3: /* TALLY INDIRECT WORD */
      if (PASS2 == 0)
        {
          PC++;
          return;
        }

      STMNTCT++;
      varscan (&val[0], &rel[0], 4);
      varscan (&val[1], &rel[1], 0);
      varscan (&val[2], &rel[2], OP->mask);
      val[2] |= OP->val;
      gen (3, &IFORM[4], &val[0], &rel[0]);
      break;

    case 5: /* RPTX, RPDX */
      if (PASS2 == 0)
        {
          PC++;
          return;
        }

      STMNTCT++;
      varscan (&val[0], &rel[0], 0);
      varscan (&val[2], &rel[2], 0);
      val[0] = 0;
      memset (&rel[0], 0, sizeof rel[0]);
      val[1] = OP->val + INHIB_BIT28;
      memset (&rel[1], 0, sizeof rel[1]);
      gen (3, &IFORM[4], &val[0], &rel[0]);
      break;

    case 4:
      { /* REPEAT (RPT/RPD/...) */
        static const char *const condsym[7]
            = { "TOV", "TNC", "TRC", "TMI", "TPL", "TZE", "TNZ" };
        int64_t cond = 0;
        if (PASS2 == 0)
          {
            PC++;
            return;
          }

        STMNTCT++;
        varscan (&val[0], &rel[0], 0); /* repeat count */
        if (OP->rpl)
          {
            val[4] = 0;
            memset (&rel[4], 0, sizeof rel[4]);
          }
        else
          {
            varscan (&val[4], &rel[4], 0); /* increment */
          }

        for (;;)
          { /* condition list */
            if (nextfld ())
              {
                break;
              }

            if (XCARD[CURRCH] >= '0' && XCARD[CURRCH] <= '9')
              {
                int64_t cv;
                int ct, cd;
                double cdv;
                convert (8, &cv, &ct, &cd, &cdv);
                if (CURRCH != NEXTCH)
                  {
                    error (9);
                  }

                cond |= cv; /* numeric condition */
              }
            else
              {
                int kk, n = NEXTCH - CURRCH, found = 0;
                for (kk = 0; kk < 7; kk++)
                  {
                    if (n == 3
                        && upch ((unsigned char)XCARD[CURRCH])
                               == condsym[kk][0]
                        && upch ((unsigned char)XCARD[CURRCH + 1])
                               == condsym[kk][1]
                        && upch ((unsigned char)XCARD[CURRCH + 2])
                               == condsym[kk][2])
                      {
                        cond |= (1L << (6 - kk));
                        found = 1;
                        break;
                      }
                  }

                if (!found)
                  {
                    error (4);
                  }
              }

            CURRCH = NEXTCH;
          }

        val[1] = OP->mask * 2 + OP->ar;
        memset (&rel[1], 0, sizeof rel[1]);
        val[2] = cond;
        memset (&rel[2], 0, sizeof rel[2]);
        val[3] = OP->val + INHIB_BIT28;
        memset (&rel[3], 0, sizeof rel[3]);
        gen (5, RFORM, &val[0], &rel[0]);
        break;
      }

    case 13:
      { /* EQU, BOOL, SET, SETB */
        static const int W36[1] = { 36 };
        int vv;
        STMNTCT++;
        varscan (&val[0], &rel[0], OP->mask);
        if (DEL != D_BLANK)
          {
            error (4);
          }

        if (LOCSZ == 0)
          {
            error (7);
            return;
          }

        if (OP->val & 0400000)
          {
            rel[0].f_equ = 1; /* REL.F flags from OP.VAL */
          }

        if (OP->val & 0200000)
          {
            rel[0].f_set = 1;
          }

        vv = (int)val[0];
        symtab (LOC, &vv, &rel[0], 1);     /* define label = value */
        genval (1, W36, &val[0], &rel[0]); /* octal listing; no PC bump */
        gen_val = (W)val[0] & M36;
        gen_has_val = 1;
        break;
      }

    case 17: /* ZERO */
      if (PASS2 == 0)
        {
          PC++;
          return;
        }

      STMNTCT++;
      varscan (&val[0], &rel[0], 0);
      varscan (&val[1], &rel[1], 0);
      gen (2, ZFORM, &val[0], &rel[0]);
      break;

    case 18: /* VFD (OPD opcode-definition deferred) */
      STMNTCT++;
      if (OP->val != 0)
        {
          error (-4);
          break;
        } /* OPD: needs a dynamic op table */

      vfd ();
      break;

    case 15:
      { /* ASCII, BCI, EBCDIC, UASCI */
        static const int W36[1] = { 36 };
        int64_t nw;
        struct rel r;
        int m, nb, j, nwi;
        W words[64];
        varscan (&nw, &r, 0); /* word count */
        if (rel_nz (&r))
          {
            error (5);
            return;
          }

        if (PASS2 == 0)
          {
            PC += (int)nw;
            return;
          }

        STMNTCT++;
        m = 4 + 2 * OP->ar; /* chars/word: 4 (9-bit) or 6 (6-bit) */
        nb = 36 / m;
        nwi = (int)nw;
        if (nwi > 64)
          {
            nwi = 64;
          }

        for (j = 0; j < nwi; j++)
          {
            words[j] = 0;
          }

        xlatev (nb, words, 0, m * nwi, bmap_asciit[OP->val], CURRCH + 1,
                m * nwi);
        for (j = 0; j < nwi; j++)
          {
            int64_t v = (int64_t)words[j];
            memset (&rel[0], 0, sizeof rel[0]);
            gen (1, W36, &v, &rel[0]);
          }

        break;
      }

    case 16:
      { /* DEC, OCT (integer, float, scaled) */
        static const int W36[1] = { 36 };
        int base = 10 - 2 * OP->mask, nw = 0;
        STMNTCT++;
        while (DEL <= D_COMMA)
          {
            int si, sj, ty = 0;
            if (nextfld ())
              {
                break; /* DECEND */
              }

            si = searchtbl (DECTBL, CURRCH, XCARDL, &sj);
            if (si >= 0 && sj == 3)
              { /* expression */
                CURRCH--;
                DEL = 0;
                varscan (&val[0], &rel[0], OP->mask);
              }
            else
              { /* number */
                int64_t cv;
                int ct, cd, minus = 0;
                double dv;
                if (XCARD[CURRCH] == '-')
                  {
                    minus = 1;
                    CURRCH++;
                  }
                else if (XCARD[CURRCH] == '+')
                  {
                    CURRCH++;
                  }

                convert (base, &cv, &ct, &cd, &dv);
                memset (&rel[0], 0, sizeof rel[0]);
                memset (&rel[1], 0, sizeof rel[1]);
                if (ct == 1 || ct == 2)
                  { /* SPFP / DPFP -> DPS-8 float */
                    W fw[2];
                    ty = dps8_float (minus ? -dv : dv, ct == 2, fw);
                    val[0] = (int64_t)fw[0];
                    if (ty > 1)
                      {
                        val[1] = (int64_t)fw[1];
                      }
                  }
                else
                  { /* integer / scaled fixed */
                    if (minus)
                      {
                        cv = (base == 10) ? -cv : (cv ^ ((int64_t)1 << 35));
                      }

                    val[0] = cv;
                    ty = 0;
                  }
              }

            {
              int i = 0;
              do
                {
                  gen (1, W36, &val[i], &rel[i]);
                  nw++;
                  i++;
                }
              while (i < ty);
            }
            delscan ();
            if (NEXTCH != CURRCH || DEL < D_COMMA)
              {
                error (9);
              }
          }
        if (ERRCT == 0 && nw == 0)
          {
            error (14);
          }

        break;
      }

    case 23:
      { /* IO: IOTD/IOTP/TDCW/IONTP (SI61 2617) */
        static const int IOFORM[3] = { 18, 6, 12 };
        if (PASS2 == 0)
          {
            PC++;
            break;
          }

        STMNTCT++;
        varscan (&val[0], &rel[0], 4); /* operand (literal allowed) */
        varscan (&val[2], &rel[2], 0); /* tag/extent */
        val[1] = OP->val;
        memset (&rel[1], 0, sizeof rel[1]);
        gen (3, IOFORM, val, rel); /* addr(18) | op(6) | tag(12) */
        break;
      }

    case 32:
      { /* ASCNT (SI61 2903) */
        static const int ASFORM[4] = { 16, 11, 1, 8 };
        if (PASS2 == 0)
          {
            PC++;
            break;
          }

        STMNTCT++;
        varscan (&val[0], &rel[0], 0);
        val[1] = 0;
        val[2] = 1;
        val[3] = 0;
        memset (&rel[1], 0, sizeof rel[1]);
        memset (&rel[2], 0, sizeof rel[2]);
        memset (&rel[3], 0, sizeof rel[3]);
        if (!nextfld () && NEXTCH == CURRCH + 1 && XCARD[CURRCH] == 'N')
          {
            val[2] = 0; /* `,N` clears the count-up flag */
          }

        gen (4, ASFORM, val, rel);
        break;
      }

    case 26:
      { /* CLIMB: ENTER/EXIT/GOTO/PASS/PMME (SI61 2632) */
        static const int CL2FORM[5] = { 1, 9, 8, 6, 12 };
        int K = 1, i;
        if (PASS2 == 0)
          {
            PC += 2;
            return;
          }

        STMNTCT++;
        for (i = 0; i < 9; i++)
          {
            val[i] = 0;
            memset (&rel[i], 0, sizeof rel[i]);
          }

        if (!OP->ar)
          {
            val[8] = 01760 * OP->rpl; /* -> L2610 */
          }
        else
          {
            varscan (&val[8], &rel[8], 0);
            varscan (&val[5], &rel[5], 0);
          }

        /* L2610 */
        val[2] = 07134;
        val[7] = OP->val;
        if (!nextfld ())
          { /* an operand follows */
            val[7] |= 040;
            CURRCH--;
            DEL = 0;
            varscan (&val[1], &rel[1], 0); /* -> L2620 */
          }
        else if (DEL == D_LPAR)
          { /* L2615: parenthesized */
            val[7] |= 040;
            varscan (&val[1], &rel[1], 0);
            if (DEL != D_RPAR)
              {
                varscan (&val[3], &rel[3], 2); /* a register modifier */
              }

            if (DEL != D_RPAR)
              {
                K = 0;
                varscan (&val[0], &rel[0], 0);
                val[2] = 07135;
              }

            if (DEL == D_RPAR)
              {
                CURRCH++;
                delscan ();
                CURRCH = NEXTCH;
              }
          }

        /* L2620 */
        if (!OP->ar)
          {
            varscan (&val[5], &rel[5], 0);
          }

        val[4] = 0;
        if (val[5] != 0 || rel_nz (&rel[5]))
          {
            val[4] = 1;
            val[5]--;
          }

        val[6] = 0;
        if (!nextfld () && NEXTCH == CURRCH + 1 && XCARD[CURRCH] == 'S')
          {
            val[7] &= 057; /* `,S` clears the operand flag */
          }

        gen (4 - K, &IFORM[4 * K], &val[K], &rel[K]); /* word 1 (IFORM) */
        gen (5, CL2FORM, &val[4], &rel[4]);           /* word 2 (CL2FORM) */
        break;
      }

    case 27:
      { /* EIS instructions (SI61 2681) */
        int K = OP->mask * 2 + OP->ar, L = OP->rpl, i;
        if (PASS2 == 0)
          {
            PC++;
            return;
          }

        STMNTCT++;
        for (i = 0; i < 37; i++)
          {
            val[i] = 0;
            memset (&rel[i], 0, sizeof rel[i]);
          }

        mfscan (&MF_[0]);
        mfscan (&MF_[1]);
        memset (&MF_[2], 0, sizeof MF_[2]);
        IDS = -1;
        NDS = 2; /* descriptor state for the ADSC/... that follow */
        switch (K)
          {
          case 1: /* two values after the two MFs */
            varscan (&val[2], &rel[2], 1);
            varscan (&val[3], &rel[3], 1);
            L = 2;
            break;

          case 2: /* a third MF */
            mfscan (&MF_[2]);
            NDS = 3;
            val[3] = MF_[2].v;
            rel[3] = MF_[2].rel;
            L = 3;
            break;

          case 3:
            varscan (&val[2], &rel[2], 1);
            varscan (&val[0], &rel[0], 1);
            varscan (&val[3], &rel[3], 1);
            val[1] = 0;
            L = 0;
            break;

          case 4:
            if (L == 0)
              {
                varscan (&val[2], &rel[2], 1);
                val[3] = 0;
                L = 2;
                break;
              }

            goto eis_l2750; /* L != 0 */

          case 6:
            mfscan (&MF_[2]);
            NDS = 3;
            goto eis_l2750;

          case 5:
          eis_l2750:
            val[1 + L] = MF_[2].v;
            rel[1 + L] = MF_[2].rel;
            if (L == 0)
              {
                varscan (&val[3], &rel[3], 1);
                varscan (&val[0], &rel[0], 1);
                varscan (&val[2], &rel[2], 1);
              }
            else
              {
                val[1] = OP->val / 32768;
                val[3] = OP->val / 4096;
              }

            K = 5 + L;
            break;
          }
        val[4] = MF_[1].v;
        rel[4] = MF_[1].rel;
        val[5] = OP->val + INHIB_BIT28 + mf_ar (0); /* op | I | AR1 */
        val[6] = MF_[0].v;
        rel[6] = MF_[0].rel;
        gen (7 - L, &EFORM[7 * K - 7 + L], &val[L], &rel[L]);
        break;
      }

    case 28:
      { /* BDSC / VDSC / VDSCX (SI61 2747) */
        int kmask = OP->mask, k = 1, i27;
        if (PASS2 == 0)
          {
            PC++;
            return;
          }

        STMNTCT++;
        for (i27 = 0; i27 < 6; i27++)
          {
            val[i27] = 0;
            memset (&rel[i27], 0, sizeof rel[i27]);
          }

        if (++IDS >= NDS)
          {
            IDS = 3;
          }

        if (kmask == 0)
          { /* BDSC */
            int ta = (OP->ar || mf_rl (IDS)) ? 2 : 0;
            varscan (&val[1], &rel[1], 4);
            varscan (&val[4], &rel[4], ta);
            varscan (&val[2], &rel[2], 0);
            varscan (&val[3], &rel[3], 0);
            k = 1;
            if (mf_ar (IDS) || DEL == D_COMMA)
              {
                k = 0;
                varscan (&val[0], &rel[0], 0);
              }

            gen (5 - k, &BDSCF[5 * k], &val[k], &rel[k]);
          }
        else
          { /* VDSC (k=1) / VDSCX (k=2) */
            varscan (&val[1], &rel[1], 4);
            varscan (&val[2], &rel[2], 0);
            val[3] = kmask - 1;
            varscan (&val[4], &rel[4], 2);
            if (kmask > 1)
              {
                val[2] |= 010;
              }

            k = 1;
            if (mf_ar (IDS) || DEL == D_COMMA)
              {
                k = 0;
                varscan (&val[0], &rel[0], 0);
              }

            gen (5 - k, &VDSCF[5 * k], &val[k], &rel[k]);
          }

        break;
      }

    case 29:
      { /* ADSC4/6/9 (+X) (SI61 2786) */
        int k = 1, ta, i27;
        if (PASS2 == 0)
          {
            PC++;
            return;
          }

        STMNTCT++;
        for (i27 = 0; i27 < 6; i27++)
          {
            val[i27] = 0;
            memset (&rel[i27], 0, sizeof rel[i27]);
          }

        if (++IDS >= NDS)
          {
            IDS = 3;
          }

        varscan (&val[1], &rel[1], 4);
        varscan (&val[2], &rel[2], 0);
        if (OP->mask != 0)
          { /* ADSC4 (*2) / ADSC6 (*1) byte scaling */
            val[2] *= OP->mask;
            if (rel_nz (&rel[2]))
              {
                if (rel[2].evalop != 0 || rel_s_nz (&rel[2]))
                  {
                    error (5);
                    memset (&rel[2], 0, sizeof rel[2]);
                  }
                else
                  {
                    rel[2].evalop = EVALOPMULT;
                    rel[2].value = OP->mask;
                  }
              }
          }

        ta = (OP->ar || mf_rl (IDS)) ? 2 : 0;
        varscan (&val[5], &rel[5], ta);
        val[3] = OP->val;
        val[4] = 0;
        k = 1;
        if (mf_ar (IDS) || DEL == D_COMMA)
          {
            k = 0;
            varscan (&val[0], &rel[0], 0);
          }

        gen (6 - k, &ADSCF[6 * k], &val[k], &rel[k]);
        break;
      }

    case 30:
      { /* NDSC4/9 (+X) (SI61 2819) */
        int k = 1, ta, i27;
        if (PASS2 == 0)
          {
            PC++;
            return;
          }

        STMNTCT++;
        for (i27 = 0; i27 < 7; i27++)
          {
            val[i27] = 0;
            memset (&rel[i27], 0, sizeof rel[i27]);
          }

        if (++IDS >= NDS)
          {
            IDS = 3;
          }

        varscan (&val[1], &rel[1], 4);
        varscan (&val[2], &rel[2], 0);
        ta = (OP->ar || mf_rl (IDS)) ? 2 : 0;
        varscan (&val[6], &rel[6], ta);
        varscan (&val[4], &rel[4], 0);
        varscan (&val[5], &rel[5], 0);
        val[3] = OP->val;
        k = 1;
        if (mf_ar (IDS) || DEL == D_COMMA)
          {
            k = 0;
            varscan (&val[0], &rel[0], 0);
          }

        gen (7 - k, &NDSCF[7 * k], &val[k], &rel[k]);
        break;
      }

    case 33:
      { /* PTR -- NSA pointer (SI61 2917) */
        int j;
        if (PASS2 == 0)
          {
            PC++;
            return;
          }

        STMNTCT++;
        for (j = 0; j < 4; j++)
          {
            val[j] = 0;
            memset (&rel[j], 0, sizeof rel[j]);
          }

        varscan (&val[3], &rel[3], 0);
        for (j = 0; j <= 2; j++)
          {
            varscan (&val[j], &rel[j], 0);
          }

        gen (4, &BDSCF[5], val, rel); /* {18,2,4,12} */
        break;
      }

    case 35:
      { /* NSA descriptors ODSC/DDSC/IDSC/ODSB (SI61 2975) */
        int K = OP->mask - 1, K2 = 1 - K, L, j;
        int64_t tv = 043;
        if (PASS2 == 0)
          {
            PC += 2;
            return;
          }

        STMNTCT++;
        for (j = -1; j < 8; j++)
          {
            val[j] = 0;
            memset (&rel[j], 0, sizeof rel[j]);
          }

        varscan (&val[3], &rel[3], 0);
        varscan (&val[5], &rel[5], 0);
        if (K2 != 0)
          { /* a register length unless reloc */
            if (rel_nz (&rel[5]))
              {
                val[6] = 0;
              }
            else
              {
                val[5] = 4 * val[5];
                K2 = 0;
              }
          }

        varscan (&val[K], &rel[K], 0); /* K=-1 (ODSB) -> the guard slot */
        if (val[K] != 0 || rel_nz (&rel[K]))
          {
            val[K]--;
          }

        if (K == 0)
          {
            val[1] = 3;
          }
        else if (!rel_nz (&rel[1]))
          {
            val[0] = val[1] / 4;
            val[1] = val[1] % 4;
            K = 0;
          }

        if (K < 0)
          {
            K = 0; /* exotic ODSB+reloc length: avoid a negative form */
          }

        tv = 043;
        flags_scan (&tv);
        val[2] = tv;
        L = K;
        if (OP->val / 2 == 1)
          {
            val[2] /= 64;
            L += 2;
          }

        val[4] = OP->val;
        gen (5 - K, &DSCF[5 * L], &val[K], &rel[K]); /* word 1 */
        gen (K2, &DSCF[K2 + 19], &val[5],
             &rel[5]); /* word 2 (0 words if K2<=0) */
        break;
      }

    case 36:
      { /* NSA entry descriptors EDSC16/24/64 (SI61 3009) */
        int K = 1, j;
        if (PASS2 == 0)
          {
            PC += 2;
            return;
          }

        STMNTCT++;
        for (j = 0; j < 8; j++)
          {
            val[j] = 0;
            memset (&rel[j], 0, sizeof rel[j]);
          }

        varscan (&val[3], &rel[3], 0);
        varscan (&val[6], &rel[6], 0);
        if (!rel_nz (&rel[6]))
          {
            val[6] *= 4;
          }
        else
          {
            val[7] = 0;
            K = 2;
          }

        varscan (&val[5], &rel[5], 0);
        if (val[5] != 0 || rel_nz (&rel[5]))
          {
            val[5]--;
          }

        varscan (&val[2], &rel[2], 0);
        varscan (&val[0], &rel[0], 0);
        val[1] = 0;
        if (!nextfld () && NEXTCH == CURRCH + 1 && XCARD[CURRCH] == 'S')
          {
            val[1] = 1;
          }

        val[4] = OP->val;
        gen (5, EDSCF, val, rel); /* word 1: EDSCF {18,1,10,3,4} */
        gen (K + 1, &EDSCF[2 * K + 3], &val[5], &rel[5]); /* word 2 */
        break;
      }

    case 34:
      { /* NSA vectors VEC/FVEC/SVEC/CVEC (SI61 2927) */
        static const int W36[1] = { 36 };
        int K = OP->mask / 2, K2 = 5 + K, K3 = 9 + K, j;
        int64_t tv = 0777;
        if (PASS2 == 0)
          {
            PC += OP->val / 256;
            return;
          }

        STMNTCT++;
        for (j = 0; j < 12; j++)
          {
            val[j] = 0;
            memset (&rel[j], 0, sizeof rel[j]);
          }

        varscan (&val[8], &rel[8], 0);
        if (!OP->ar)
          {
            K = 0; /* -> L3420 */
          }
        else
          {
            varscan (&val[K2], &rel[K2], 0);
            if (K2 != 5 && !rel_nz (&rel[6]))
              {
                val[5] = val[6] / 4;
                val[6] %= 4;
                K2 = 5;
              }
          }

        /* L3420 */
        varscan (&val[K], &rel[K], 0);
        if (val[K] != 0 || rel_nz (&rel[K]))
          {
            val[K]--;
          }

        if (K == 0)
          {
            val[1] = 3;
          }
        else if (!rel_nz (&rel[1]))
          {
            val[0] = val[1] / 4;
            val[1] %= 4;
            K = 0;
          }

        /* L3430 */
        if (OP->mask % 2 != 0)
          {
            varscan (&val[K3], &rel[K3], 0);
            if (K3 != 9 && !rel_nz (&rel[10]))
              {
                val[9] = val[10] / 4;
                val[10] %= 4;
                K3 = 9;
              }
          }

        /* L3440 */
        tv = 0777;
        flags_scan (&tv);
        val[2] = tv;
        val[3] = OP->val % 256;
        gen (5 - K, &VECFORM[5 * K], &val[K], &rel[K]);
        gen (9 - K2, &VECFORM[5 * K2 - 15], &val[K2], &rel[K2]);
        if (OP->mask % 2 == 0)
          {
            break; /* 2-word vector */
          }

        gen (12 - K3, &VECFORM[5 * K3 - 25], &val[K3], &rel[K3]);
        gen (1, W36, &val[11], &rel[11]); /* 4-word vector */
        break;
      }

    case 31:
      { /* MICROP: edit micro-ops (SI61 2842) */
        static const int QFORM[4] = { 9, 9, 9, 9 };
        static const char *const MOP[17]
            = { "INSM", "ENF",  "SES",  "MVZB", "MVZA", "MFLS",
                "MFLC", "INSB", "INSA", "INSN", "INSP", "IGN",
                "MVC",  "MSES", "MORS", "LTE",  "CHT" };
        int K, M, L, SH = 0, Jc, ND, j, mi;
        STMNTCT++;
      micro_l3100: /* one group -> one packed word */
        for (j = 0; j < 4; j++)
          {
            val[j] = 0;
            memset (&rel[j], 0, sizeof rel[j]);
          }

        for (K = 0; K <= 3; K++)
          {
            if (nextfld ())
              {
                goto micro_l3130; /* no field present */
              }

            M = 1;
            if (XCARD[CURRCH] >= '0' && XCARD[CURRCH] <= '9')
              {
                M = XCARD[CURRCH] - '0';
                CURRCH++;
              }

            if (XCARD[CURRCH] == 'H')
              {
                Jc = 1;
                SH = 0;
              } /* BCD char */
            else if (XCARD[CURRCH] == 'A' || XCARD[CURRCH] == 'U')
              {
                Jc = 1;
                SH = 2;
              } /* ASCII */
            else
              {
                if (XCARD[CURRCH] != 'O')
                  {
                    error (4);
                  }

                Jc = 0;
              } /* octal */

            for (L = 1; L <= M; L++)
              { /* L3135: repeat M times */
                if (Jc)
                  {
                    CURRCH++;
                    val[K] = bmap_asciit[SH][(unsigned char)XCARD[CURRCH]];
                  }
                else
                  {
                    for (ND = 1; ND <= 3; ND++)
                      {
                        if (XCARD[CURRCH + 1] < '0' || XCARD[CURRCH + 1] > '7')
                          {
                            break;
                          }

                        CURRCH++;
                        val[K] = 8 * val[K] + (XCARD[CURRCH] - '0');
                      }
                  }

                memset (&rel[K], 0, sizeof rel[K]); /* L3150 */
                if (L < M)
                  {
                    K++;
                  }

                if (K > 3)
                  { /* group full -> emit, restart */
                    gen (4, QFORM, val, rel);
                    K = 0;
                    for (j = 0; j < 4; j++)
                      {
                        val[j] = 0;
                        memset (&rel[j], 0, sizeof rel[j]);
                      }
                  }
              }

            CURRCH++;
            continue;  /* L3180 */
          micro_l3130: /* a "(" MOP form, or empty */
            if (DEL != D_LPAR)
              {
                continue; /* L3170: empty field -> 0 */
              }

            if (nextfld ())
              {
                continue; /* L3170 */
              }

            M = 0;
            for (mi = 1; mi <= 16; mi++)
              {
                if ((int)strlen (MOP[mi]) == NEXTCH - CURRCH
                    && !memcmp (&XCARD[CURRCH], MOP[mi],
                                (size_t)(NEXTCH - CURRCH)))
                  {
                    M = mi;
                    break;
                  }
              }

            if (M == 0)
              {
                error (4);
              }

            CURRCH = NEXTCH;
            varscan (&val[K], &rel[K], 0);
            if (rel_nz (&rel[K]))
              {
                error (5);
                memset (&rel[K], 0, sizeof rel[K]);
              }

            val[K] += M * 16;
            if (DEL == D_RPAR)
              {
                CURRCH++;
                delscan ();
                CURRCH = NEXTCH;
              }
          }

        gen (4, QFORM, val, rel);
        if (DEL != D_BLANK)
          {
            goto micro_l3100;
          }

        break;
      }

    case 42:
      { /* EDEC: decimal-edit compiler (SI61 3067) */
        int NW = 0, FB = 0, NB = 0, NC = 0, CT, DP = 0, SF, FC, CC, I, cls;
        int FP, LEFT, MINUS;
        int64_t acc = 0; /* VAL.I(0): the word being filled */
        STMNTCT++;
        while (DEL <= D_COMMA)
          { /* one comma-separated field per turn */
            FP = 0;
            if (nextfld ())
              {
                goto edec_end; /* ALTRET(EDECEND): no more fields */
              }

            /* field header: <count><A|P>[L] -- SEARCH(NONDGT) gives the A/P
             * boundary and NB (9 or 4); the leading digits are the count NC.
             */
            I = searchtbl (NONDGT_, CURRCH, XCSIZE, &NB);
            if (I < 0)
              {
                I = XCSIZE;
              }

            NC = (I > CURRCH) ? charbin (CURRCH, I) : 0;
            if (NB == 9)
              {
                FB = ((FB + 8) / 9) * 9; /* round to a byte boundary */
              }
            else if (NB != 4)
              {
                error (9);
                goto edec_scannext;
              }

            CURRCH = I + 1;
            LEFT = (XCARD[CURRCH] == 'L');
            if (LEFT)
              {
                CURRCH++;
              }

            FC = CURRCH;
            /* count phase: tally the significant positions CT and the decimal
             * scale DP (negated fraction-digit count, adjusted by any E exp).
             */
            CT = 0;
            for (;;)
              {
                switch (DIGITPMPE_[(unsigned char)XCARD[CURRCH]])
                  {
                  case 1:
                    CT++;
                    DP--;
                    break; /* digit */

                  case 2:
                  case 3:
                    CT++;
                    break; /* +, - */

                  case 4:
                    FP = 1;
                    CT += (NB == 4) ? 2 : 1;
                    DP = 0;
                    break; /* . */

                  case 5: /* E exponent */
                    CURRCH++;
                    MINUS = (XCARD[CURRCH] == '-');
                    if (MINUS || XCARD[CURRCH] == '+')
                      {
                        CURRCH++;
                      }

                    I = searchtbl (NONDGT_, CURRCH, XCSIZE, &SF);
                    if (I < 0)
                      {
                        I = XCSIZE;
                      }

                    SF = (I > CURRCH) ? charbin (CURRCH, I) : 0;
                    CURRCH = I;
                    if (!FP)
                      {
                        FP = 1;
                        CT += (NB == 4) ? 2 : 1;
                        DP = 0;
                      }

                    if (MINUS)
                      {
                        DP -= SF;
                      }
                    else
                      {
                        DP += SF;
                      }

                    if (DP > 127 || DP < -128)
                      {
                        error (9);
                      }

                    goto edec_endscan;

                  default:
                    goto edec_endscan;
                  }
                CURRCH++;
              }

          edec_endscan:
            if (CT > NC)
              {
                error (9); /* the number does not fit the field */
              }

            /* emit phase: re-walk the number, inserting each position as an
             * edit byte -- leading zero-fill (unless L), the sign, the digits,
             * and (for floating point) the trailing scale byte(s). */
            CC = FC;
            for (;;)
              {
                cls = DIGITPMPE_[(unsigned char)XCARD[CC]];
                switch (cls)
                  {
                  case 1: /* digit */
                    if (CC == FC)
                      {
                        if (FP)
                          {
                            edec_edins (NB == 4 ? 014 : '+', NB, &NC, &FB,
                                        &acc, &NW);
                          }

                        if (!LEFT)
                          {
                            while (NC > CT)
                              {
                                edec_edins (0, NB, &NC, &FB, &acc, &NW);
                              }
                          }
                      }

                    if (CT <= NC)
                      {
                        edec_edins ((unsigned char)XCARD[CC], NB, &NC, &FB,
                                    &acc, &NW);
                      }

                    CT--;
                    break;

                  case 2: /* + */
                  case 3: /* - */
                    edec_edins (cls == 2 ? (NB == 4 ? 014 : '+') : '-', NB,
                                &NC, &FB, &acc, &NW);
                    CT--;
                    if (!LEFT)
                      {
                        while (NC > CT)
                          {
                            edec_edins (0, NB, &NC, &FB, &acc, &NW);
                          }
                      }

                    break;

                  case 4: /* . */
                    if (CC == FC)
                      {
                        if (FP)
                          {
                            edec_edins (NB == 4 ? 014 : '+', NB, &NC, &FB,
                                        &acc, &NW);
                          }

                        if (!LEFT)
                          {
                            while (NC > CT)
                              {
                                edec_edins (0, NB, &NC, &FB, &acc, &NW);
                              }
                          }
                      }

                    break;

                  default:
                    goto edec_endfield;
                  }
                CC++;
                if (CT <= 0)
                  {
                    goto edec_endfield;
                  }
              }

          edec_endfield:
            if (FP)
              { /* append the scale byte(s) */
                if (NB == 4)
                  {
                    edec_edins ((DP + 256) / 16, NB, &NC, &FB, &acc, &NW);
                  }

                edec_edins (DP & 0377, NB, &NC, &FB, &acc, &NW);
              }

            while (NC > 0)
              {
                edec_edins (0, NB, &NC, &FB, &acc, &NW); /* pad the field */
              }
          edec_scannext:
            delscan ();
            if (NEXTCH != CURRCH || DEL < D_COMMA)
              {
                error (9);
              }

            if (DEL < D_COMMA)
              {
                error (9);
                CURRCH = NEXTCH + 1;
                goto edec_scannext;
              }
          }
      edec_end:
        if (FB != 0)
          { /* flush the last partial word */
            static const int NB36[1] = { 36 };
            int64_t w = acc & (int64_t)M36;
            struct rel r;
            memset (&r, 0, sizeof r);
            gen (1, NB36, &w, &r);
            NW++;
          }

        if (ERRCT == 0 && NW == 0)
          {
            error (14); /* nothing generated */
          }

        break;
      }

    case 44:
      { /* DATE: emit the date word (SI61 3194) */
        int64_t v;
        struct rel r;
        static const int NB36[1] = { 36 };
        STMNTCT++;
        if (PASS2 == 0)
          {
            PC++;
            break;
          }

        v = (OP->val == 0) ? (int64_t)date_word () : (int64_t)TTLDAT;
        memset (&r, 0, sizeof r);
        gen (1, NB36, &v, &r);
        break;
      }

    default: /* truly unknown type: ignore */
      STMNTCT++;
      break;
    }
}

/* ===================================================== mainline handlers ===
 * Pseudo-ops handled in the BMAP mainline (not INST): BOUNDARY/BSS/ORG/OUNAME,
 * DEF/REF, the control-section machinery (USE/BLOCK), macros, literals/LITORG,
 * OPSYN, and the listing/cross-reference directives. */

/* BOUNDARY (SI61 814): align PC to a multiple of NW (offset ODD), emitting one
 * padding word -- a NOP for word alignment, or a TRA over the gap otherwise.
 */
static void
boundary (int nw, int odd, int cdf)
{
  int newpc;
  int64_t val[3];
  struct rel rel[3];

  (void)cdf;
  if (PC % nw == odd)
    {
      return;
    }

  newpc = ((PC + nw - 1) / nw) * nw + odd;
  memset (rel, 0, sizeof rel);
  if (nw == 2)
    {
      val[0] = 0;
      val[1] = 0110 | INHIB_BIT28;
      val[2] = 0;
    } /* NOP */
  else
    {
      val[0] = newpc - PC;
      val[1] = 07100 | INHIB_BIT28;
      val[2] = 4;
    } /* TRA *+d,IC */

  gen (3, &IFORM[4], val, rel); /* emits 1 word, PC++ */
  genloc (1, newpc, 0, 0);      /* PC = newpc */
}
static void
case_boundary (void) /* EVEN/ODD/EIGHT/PAGE (type 11) */
{
  STMNTCT++;
  boundary ((int)OP->val, (int)OP->mask, LOCSZ == 0);
  if (LOCSZ != 0)
    {
      int v = PC;
      struct rel r = PCREL;
      symtab (LOC, &v, &r, 1);
    }
}
static void
case_org (void) /* ORG (type 12) */
{
  int64_t v;
  struct rel r;

  STMNTCT++;
  varscan (&v, &r, 0);
  if (r.opndtyp == OPERUNDEF)
    {
      error (-6);
    }

  genloc (1, v, 0, 0); /* PC = v */
  if (LOCSZ != 0)
    {
      int pv = PC;
      struct rel pr = PCREL;
      symtab (LOC, &pv, &pr, 1);
    }
}
static void
case_bss (void) /* BSS (type 24) */
{
  int64_t cnt;
  struct rel r;

  STMNTCT++;
  varscan (&cnt, &r, 0);
  if (rel_nz (&r))
    {
      error (-5); /* count must be absolute */
    }

  genloc (0, 0, 0, 0); /* record location (start of block) */
  PC += (int)cnt;      /* label was defined by the pre-dispatch */
}
static void
case_ouname (void) /* OUNAME (type 40): object-unit name */
{
  char nm[MAXSYM + 1];
  int m = 0, i;

  STMNTCT++;
  if (nextfld ())
    {
      return; /* no name field */
    }

  for (i = CURRCH; i < NEXTCH && m < MAXSYM; i++)
    {
      nm[m++] = XCARD[i];
    }

  if (OPTIONS.ou)
    {
      xuo_headname (nm, m);
      OUNAMESW = 2;
    } /* explicit name wins */
}

/* ENTDEF, ENTREF, SEGREF, SYMDEF, SYMREF (SI61 CASE 14).  A comma-separated
 * list of symbol names; OP.VAL is the first 18 bits of the REL packet
 * (F | OPNDTYP | EVALOP | RELOCOP) to stamp on each.  ENTDEF/SYMDEF (OP.AR=0)
 * are processed as references (DEF=false) so the flag merges onto the
 * locally-defined label; ENTREF/SYMREF/SEGREF (OP.AR=1) define the symbol as
 * an external reference (DEF=true).  The first ENTDEF (else first SYMDEF)
 * names the object unit.  Processed in pass 1 only, as in the original.  The
 * reference records (RNAM/EREF/SREF) for OP.AR=1 are part 3c. */
static void
case_defref (void)
{
  if (PASS2 > 0)
    {
      return;
    }

  STMNTCT++;
  for (;;)
    {
      char buf[MAXSYM + 1];
      struct rel r;
      int v = 0;
      if (nextfld ())
        {
          break; /* ALTRET: no (more) name field */
        }

      memset (&r, 0, sizeof r);
      if (OP->val & 0100000)
        {
          r.f_edef = 1; /* OP.VAL bit 2 = F.EDEF */
        }

      if (OP->val & 0040000)
        {
          r.f_sdef = 1; /* OP.VAL bit 3 = F.SDEF */
        }

      r.opndtyp = (OP->val >> 8) & 017;
      r.evalop = (OP->val >> 4) & 017;
      r.relocop = OP->val & 017;
      r.operand = 0777777; /* SI61: REL.OPERAND = 777777 */
      field_sym (buf, CURRCH, NEXTCH);
      if (r.f_edef && OUNAMESW < 2)
        { /* first ENTDEF names the OU */
          OUNAMESW = 2;
          if (OPTIONS.ou)
            {
              xuo_headname (&XCARD[CURRCH], NEXTCH - CURRCH);
            }
        }
      else if (r.f_sdef && OUNAMESW == 0)
        { /* else first SYMDEF */
          OUNAMESW = 1;
          if (OPTIONS.ou)
            {
              xuo_headname (&XCARD[CURRCH], NEXTCH - CURRCH);
            }
        }

      symtab (buf, &v, &r, OP->ar); /* DEF = OP.AR */
      if (DEL != D_COMMA)
        {
          break;
        }

      CURRCH = NEXTCH; /* advance past comma; loop */
    }
}

/* USE / BLOCK (SI61 CASE 7): switch the current control section.  Each section
 * keeps its own location counter (OSECT[].pc); USE parks the current PC there
 * and resumes the target's.  The section name is the operand (`USE name` /
 * `BLOCK name`); `USE` alone selects the blank default section (0); `USE
 * PREVIOUS` returns to the section used just before.  USE makes a CODESECTION
 * (OP.VAL=1), BLOCK a labeled-common LCOMSECTION (OP.VAL=4).  A section is
 * created on first mention (pass 1 only -- a new section in pass 2 is error 6,
 * as in the original).  A trailing `,type` overrides the section type. */
static int PREVIOUS_SECT;
static int
find_section (const char *nm, int nlen)
{
  int i;

  for (i = 0; i < NOSECT; i++)
    {
      if (OSECT[i].nlen == nlen && !memcmp (OSECT[i].name, nm, nlen))
        {
          return i;
        }
    }

  return -1;
}
static void
case_use (void)
{
  int cur = PCREL.operand, target = -1, i, nlen = 0;
  char nm[MAXSYM + 1];

  STMNTCT++;
  if (cur >= 0 && cur < NOSECT)
    { /* park the current section's PC */
      OSECT[cur].pc = PC;
      if (PC > OSECT[cur].size)
        {
          OSECT[cur].size = PC;
        }
    }

  nm[0] = '\0';
  if (nextfld ())
    { /* no operand -> blank section */
      nlen = 0;
    }
  else if (NEXTCH - CURRCH == 8 && !memcmp (&XCARD[CURRCH], "PREVIOUS", 8))
    {
      target = PREVIOUS_SECT;
    }
  else
    {
      nlen = NEXTCH - CURRCH;
      if (nlen > MAXSYM)
        {
          nlen = MAXSYM;
        }

      for (i = 0; i < nlen; i++)
        {
          nm[i] = (char)upch ((unsigned char)XCARD[CURRCH + i]);
        }

      nm[nlen] = '\0';
    }

  if (target < 0)
    {
      target = find_section (nm, nlen);
      if (target < 0)
        { /* first mention -> create it */
          if (PASS2 > 0)
            {
              error (6);
              target = cur;
            } /* INVALID SECTION */
          else
            {
              target = xuo_sectbuild (OP->val, nm, nlen);
            }
        }
    }

  PREVIOUS_SECT = cur;
  PCREL.operand = target;
  PC = (target >= 0 && target < NOSECT) ? OSECT[target].pc : 0;
  if (DEL == D_COMMA && target >= 0 && target < NOSECT)
    { /* `,type` override */
      int64_t tv;
      struct rel tr;
      CURRCH = NEXTCH; /* step to the comma; varscan skips it */
      varscan (&tv, &tr, 0);
      if (rel_nz (&tr))
        {
          error (5);
        }
      else
        {
          OSECT[target].type = (int)tv;
        }
    }
}

/* Listing-mode dispatch on OP.TYPE: INST encodes instructions + data, the
 * mainline handlers above cover BOUNDARY/ORG/BSS/USE; the not-yet-ported
 * mainline types fall through to a no-op (no code, no PC change). */
/* ----------------------------------------------- phase-7 macro handlers ---
 * line_is_endm -- does this raw card's operation field terminate a body? */
static int
line_is_endm (const char *ln)
{
  char op[MAXSYM + 1];

  line_op_token (ln, op, sizeof op);
  return !strcmp (op, "ENDM") || !strcmp (op, "ENDOP");
}

/* MACRO (SI61 CASE 20): capture the body cards up to ENDM and register the
 * macro under its location-field name.  Runs in both passes (consuming the
 * body each time so the main loop skips it); pass 2 just replaces the body. */
static void
case_macro_def (void)
{
  char **body = NULL;
  int nbody = 0, cap = 0;

  STMNTCT++;
  for (;;)
    {
      const char *ln = next_phys_line ();
      if (!ln)
        {
          error (1);
          break;
        } /* end of input before ENDM */

      if (line_is_endm (ln))
        {
          break;
        }

      if (nbody >= cap)
        {
          cap = cap ? cap * 2 : 8;
          body = (char * *)realloc (body, (size_t)cap * sizeof *body);
        }

      body[nbody++] = dupstr (ln);
    }

  if (LOCSZ == 0)
    {
      error (7);
    } /* MACRO with no name */
  else if (find_op (0, LOCSZ))
    {
      error (-13);
    } /* name collides with a built-in op */
  else
    {
      macro_add (LOC, body, nbody);
      return;
    }

  {
    int k;
    for (k = 0; k < nbody; k++)
      {
        free (body[k]);
      }

    free (body);
  } /* rejected: discard */
}

/* Split the macro-call variable field (XCARD from `start`) into
 * comma-separated arguments, honouring (..)/[..] grouping (which protects
 * embedded commas and blanks) and stripping one outer ()/[] from each argument
 * -- SI61 MACROPARAM. Returns the argument count; text is kept in argbuf,
 * args[i] points into it. */
static int
macro_parse_args (int start, char *argbuf, size_t cap, char **args,
                  int maxargs)
{
  int i = start, nargs = 0;
  size_t o = 0;

  if (start < 0 || start >= XCSIZE || XCARD[start] == ' ')
    {
      return 0;
    }

  for (;;)
    {
      int astart = (int)o, depth = 0, len;
      char *a;
      for (; i < XCSIZE; i++)
        {
          char c = XCARD[i];
          if (c == '(' || c == '[')
            {
              depth++;
            }
          else if ((c == ')' || c == ']') && depth > 0)
            {
              depth--;
            }
          else if (depth == 0 && (c == ',' || c == ' '))
            {
              break;
            }

          if (o + 1 < cap)
            {
              argbuf[o++] = c;
            }
        }

      if (o < cap)
        {
          argbuf[o++] = '\0';
        }

      a = &argbuf[astart];
      len = (int)strlen (a);
      if (len >= 2
          && ((a[0] == '(' && a[len - 1] == ')')
              || (a[0] == '[' && a[len - 1] == ']')))
        {
          a[len - 1] = '\0'; /* strip one outer ()/[] */
          memmove (a, a + 1, (size_t)(len - 1));
        }

      if (nargs < maxargs)
        {
          args[nargs++] = a;
        }

      if (i >= XCSIZE || XCARD[i] == ' ')
        {
          break; /* blank ends the field */
        }

      i++; /* skip the comma */
    }

  return nargs;
}

/* CRSM (SI61 CREATESYMBOL): with CRSM on, a macro reference #N beyond the
 * supplied arguments expands to a generated unique label "_NNNN_" instead of
 * nothing -- so a macro that defines a local label can be expanded repeatedly
 * without colliding.  The map below is per expansion: the same #N reuses one
 * symbol within it; CRSMNO advances globally and is reset each pass so the
 * generated names are pass-stable (the same call yields the same label). */
static long CRSMNO;
struct crsm_ctx
{
  int num[32];
  char *sym[32];
  int n;
};
static const char *
crsm_sym (struct crsm_ctx *c, int n)
{
  int i;

  for (i = 0; i < c->n; i++)
    {
      if (c->num[i] == n)
        {
          return c->sym[i];
        }
    }

  if (c->n < 32)
    {
      char buf[16];
      snprintf (buf, sizeof buf, "_%04ld_", CRSMNO++);
      c->num[c->n] = n;
      c->sym[c->n] = dupstr (buf);
      return c->sym[c->n++];
    }

  return "";
}
static void
crsm_free (struct crsm_ctx *c)
{
  int i;

  for (i = 0; i < c->n; i++)
    {
      free (c->sym[i]);
    }

  c->n = 0;
}

/* Substitute #N references in one body card (N = 1-based argument number)
 * using args[0..nargs-1] (SI61 MACRO EXPANSION, S100).  "#0" and a "#" not
 * followed by a digit are literal; an argument beyond nargs expands to a
 * CRSM-generated symbol when `cc` is non-NULL (CRSM on), else to nothing. */
static void
macro_subst (const char *src, char **args, int nargs, struct crsm_ctx *cc,
             char *out, size_t cap)
{
  const char *p = src;
  size_t o = 0;

  while (*p && o + 1 < cap)
    {
      if (*p == '#' && p[1] >= '0' && p[1] <= '9')
        {
          int n = 0;
          const char *q = p + 1;
          while (*q >= '0' && *q <= '9')
            {
              n = n * 10 + (*q - '0');
              q++;
            }
          if (n == 0)
            {
              out[o++] = *p++;
              continue;
            } /* "#0": leave the # */

          if (n >= 1 && n <= nargs)
            {
              const char *a = args[n - 1];
              while (*a && o + 1 < cap)
                {
                  out[o++] = *a++;
                }
            }
          else if (cc)
            { /* CRSM: generated label */
              const char *s = crsm_sym (cc, n);
              while (*s && o + 1 < cap)
                {
                  out[o++] = *s++;
                }
            }

          p = q; /* consume #N */
        }
      else
        {
          out[o++] = *p++;
        }
    }
  out[o] = '\0';
}

/* Split a top-level comma list (e.g. an IDRP argument) into elements,
 * honouring
 * (..)/[..] nesting.  Text kept in buf; elems[i] points into it. */
static int
split_list (const char *s, char *buf, size_t cap, char **elems, int maxe)
{
  const char *p = s;
  int ne = 0;
  size_t o = 0;

  if (!*s)
    {
      return 0;
    }

  for (;;)
    {
      int estart = (int)o, depth = 0;
      while (*p)
        {
          char c = *p;
          if (c == '(' || c == '[')
            {
              depth++;
            }
          else if ((c == ')' || c == ']') && depth > 0)
            {
              depth--;
            }
          else if (depth == 0 && c == ',')
            {
              break;
            }

          if (o + 1 < cap)
            {
              buf[o++] = c;
            }

          p++;
        }
      if (o < cap)
        {
          buf[o++] = '\0';
        }

      if (ne < maxe)
        {
          elems[ne++] = &buf[estart];
        }

      if (*p == ',')
        {
          p++;
          continue;
        }

      break;
    }

  return ne;
}

/* IDRP line classification.  Returns 1 if `ln`'s operation is IDRP and copies
 * its operand (the first blank-delimited token after IDRP) into `buf` -- empty
 * for the operand-less closing form; 0 if `ln` is not IDRP. */
static int
idrp_line (const char *ln, char *buf, size_t cap)
{
  char op[MAXSYM + 1];
  size_t i = 0, n = strlen (ln), o = 0;

  line_op_token (ln, op, sizeof op);
  if (strcmp (op, "IDRP"))
    {
      return 0;
    }

  while (i < n && ln[i] != ' ')
    {
      i++; /* skip location */
    }
  while (i < n && ln[i] == ' ')
    {
      i++; /* to op */
    }
  while (i < n && ln[i] != ' ')
    {
      i++; /* skip op */
    }
  while (i < n && ln[i] == ' ')
    {
      i++; /* to operand */
    }
  while (i < n && ln[i] != ' ' && o + 1 < cap)
    {
      buf[o++] = ln[i++];
    }
  buf[o] = '\0';
  return 1;
}

/* First "dummy" parameter (#N with N beyond the supplied args) referenced in
 * body[lo,hi) -- what a literal-list IDRP binds its elements to. */
static int
first_dummy (char **body, int lo, int hi, int nargs)
{
  int b;

  for (b = lo; b < hi; b++)
    {
      const char *p = body[b];
      for (; *p; p++)
        {
          if (*p == '#' && p[1] >= '0' && p[1] <= '9')
            {
              int n = 0;
              const char *q = p + 1;
              while (*q >= '0' && *q <= '9')
                {
                  n = n * 10 + (*q++ - '0');
                }
              if (n > nargs)
                {
                  return n;
                }
            }
        }
    }

  return 0;
}

/* Push line `s` onto a growable line list (helper for macro expansion). */
static void
push_line (char ***v, int *n, int *cap, const char *s)
{
  if (*n >= *cap)
    {
      *cap = *cap ? *cap * 2 : 16;
      *v = (char * *)realloc (*v, (size_t)*cap * sizeof **v);
    }

  (*v)[(*n)++] = dupstr (s);
}

/* Expand macro body lines [lo,hi) into exp, substituting #N from args[] and
 * handling IDRP blocks recursively (so nesting works): an "IDRP <list>" opens
 * a block (matched to its operand-less close by depth) emitted once per comma
 * element of the list, with the iterated parameter bound to that element.  The
 * list is parameter #k for "IDRP #k", or a literal "IDRP (a,b,c)" bound to the
 * block's first dummy parameter. */
static void
expand_range (char **body, int lo, int hi, char **args, int nargs,
              struct crsm_ctx *ccp, char ***exp, int *ne, int *cape)
{
  char line[XCSIZE * 2], opnd[XCSIZE], dummy[XCSIZE];
  int i = lo;

  while (i < hi)
    {
      if (idrp_line (body[i], opnd, sizeof opnd) && opnd[0])
        { /* IDRP open */
          char listbuf[XCSIZE * 2];
          char *elems[64];
          int depth = 1, j = i + 1, close = hi, bound, nel, e;
          for (; j < hi; j++) /* matching close (depth-tracked) */
            {
              if (idrp_line (body[j], dummy, sizeof dummy))
                {
                  if (dummy[0])
                    {
                      depth++;
                    }
                  else if (--depth == 0)
                    {
                      close = j;
                      break;
                    }
                }
            }

          if (opnd[0] == '#' && opnd[1] >= '0' && opnd[1] <= '9')
            { /* IDRP #k */
              bound = atoi (opnd + 1);
              nel = split_list (
                  (bound >= 1 && bound <= nargs) ? args[bound - 1] : "",
                  listbuf, sizeof listbuf, elems, 64);
            }
          else
            { /* IDRP (literal list) */
              int L = (int)strlen (opnd);
              char *p = opnd;
              if (L >= 2
                  && ((opnd[0] == '(' && opnd[L - 1] == ')')
                      || (opnd[0] == '[' && opnd[L - 1] == ']')))
                {
                  opnd[L - 1] = '\0';
                  p = opnd + 1;
                }

              nel = split_list (p, listbuf, sizeof listbuf, elems, 64);
              bound = first_dummy (body, i + 1, close, nargs);
            }

          for (e = 0; e < nel; e++)
            {
              char *saved = NULL;
              int eff = nargs, g;
              if (bound >= 1 && bound <= 64)
                {
                  for (g = nargs; g < bound - 1; g++)
                    {
                      args[g] = (char *)""; /* gap-fill */
                    }

                  if (bound > nargs)
                    {
                      eff = bound;
                    }

                  saved = args[bound - 1];
                  args[bound - 1] = elems[e];
                }

              expand_range (body, i + 1, close, args, eff, ccp, exp, ne, cape);
              if (bound >= 1 && bound <= 64)
                {
                  args[bound - 1] = saved;
                }
            }

          i = (close < hi) ? close + 1 : close; /* past the closing IDRP */
        }
      else
        {
          macro_subst (body[i], args, nargs, ccp, line, sizeof line);
          push_line (exp, ne, cape, line);
          i++;
        }
    }
}

/* macro call (SI61 CASE 21): parse the arguments and build the expanded body
 * (substitution is a single pass, so a concatenation like `#1#2` expands
 * atomically; IDRP blocks -- including nested and literal-list forms -- are
 * handled by expand_range), then push it as a frame for the card reader. */
static void
case_macro_call (void)
{
  const struct macro *m = CUR_MACRO;
  char argbuf[XCSIZE * 2];
  char *args[64];
  char **exp = NULL;
  int nargs, start, n_exp = 0, cap_exp = 0;
  struct crsm_ctx cc = { { 0 }, { 0 }, 0 };
  struct crsm_ctx *ccp = LISTING.crsm ? &cc : NULL;

  STMNTCT++;
  if (!m)
    {
      error (2);
      return;
    }

  start = (DEL == D_BLANK) ? -1 : CURRCH + 1; /* variable field = the args */
  nargs = macro_parse_args (start, argbuf, sizeof argbuf, args, 64);
  if (m->nbody == 0)
    {
      return;
    }

  expand_range (m->body, 0, m->nbody, args, nargs, ccp, &exp, &n_exp,
                &cap_exp);
  crsm_free (&cc);
  exp_push (exp, n_exp);
}

/* DUP (SI61 CASE 19): "DUP ncards,nreps" -- capture the next `ncards` cards
 * and push them repeated `nreps` times.  Both operands must be absolute;
 * either being zero is a no-op (SI61). */
static void
case_dup (void)
{
  int64_t ncards = 0, nreps = 0;
  struct rel r0, r1;
  char **block, **exp;
  int i, j, got;

  STMNTCT++;
  varscan (&ncards, &r0, 0);
  varscan (&nreps, &r1, 0);
  if (rel_nz (&r0) || rel_nz (&r1))
    {
      error (5);
      return;
    }

  if (ncards <= 0 || nreps <= 0)
    {
      return;
    }

  block = (char * *)malloc ((size_t)ncards * sizeof *block);
  for (got = 0; got < ncards; got++)
    { /* capture the block */
      const char *ln = next_phys_line ();
      if (!ln)
        {
          error (1);
          break;
        }

      block[got] = dupstr (ln);
    }

  if (got > 0)
    { /* emit it nreps times */
      exp = (char * *)malloc ((size_t)got * (size_t)nreps * sizeof *exp);
      for (j = 0; j < nreps; j++)
        {
          for (i = 0; i < got; i++)
            {
              exp[j * got + i] = dupstr (block[i]);
            }
        }

      exp_push (exp, got * (int)nreps);
    }

  for (i = 0; i < got; i++)
    {
      free (block[i]);
    }

  free (block);
}

/* IF (SI61 CASE 22): IFE/IFG/IFL/INE (OP.VAL 0/1/2/3) compare two operands; if
 * the relation holds the following code assembles, otherwise the next N cards
 * are skipped (N from a trailing field, default 1).  Operands are either two
 * VARSCAN expressions or two strings (each `'quoted'` or a bareword), matching
 * SI61's two compare paths. */
static void
case_if (void)
{
  int64_t skip = 1;
  int cond = 0, sp = CURRCH + 1, qx;

  STMNTCT++;
  /* string-compare form (SI61 L2210): the first or second operand is quoted */
  qx = (XCARD[sp] == '\'');
  if (!qx)
    {
      int q = sp;
      while (q < XCSIZE && XCARD[q] != ',' && XCARD[q] != ' ')
        {
          q++;
        }
      qx = (XCARD[q] == ',' && XCARD[q + 1] == '\'');
    }

  if (qx)
    {
      char s1[XCSIZE], s2[XCSIZE];
      int p = sp, n1 = 0, n2 = 0, c;
      if (XCARD[p] == '\'')
        {
          p++;
          while (p < XCSIZE && XCARD[p] != '\'')
            {
              s1[n1++] = XCARD[p++];
            }
          if (XCARD[p] == '\'')
            {
              p++;
            }
        }
      else
        {
          while (p < XCSIZE && XCARD[p] != ',')
            {
              s1[n1++] = XCARD[p++];
            }
        }

      s1[n1] = '\0';
      if (XCARD[p] != ',')
        {
          error (4);
          SKIPCT = 0;
          return;
        }

      p++;
      if (XCARD[p] == '\'')
        {
          p++;
          while (p < XCSIZE && XCARD[p] != '\'')
            {
              s2[n2++] = XCARD[p++];
            }
          if (XCARD[p] == '\'')
            {
              p++;
            }
        }
      else
        {
          while (p < XCSIZE && XCARD[p] != ',' && XCARD[p] != ' ')
            {
              s2[n2++] = XCARD[p++];
            }
        }

      s2[n2] = '\0';
      c = strcmp (s1, s2); /* 6-bit-ASCII compare == strcmp */
      switch (OP->val)
        {
        case 0:
          cond = (c == 0);
          break; /* IFE */

        case 1:
          cond = (c > 0);
          break; /* IFG */

        case 2:
          cond = (c < 0);
          break; /* IFL */

        case 3:
          cond = (c != 0);
          break; /* INE */
        }
      if (cond)
        {
          return;
        }

      if (XCARD[p] == ',')
        { /* trailing skip count (decimal) */
          int q = p + 1, v = 0, got = 0;
          while (XCARD[q] >= '0' && XCARD[q] <= '9')
            {
              v = v * 10 + (XCARD[q] - '0');
              q++;
              got = 1;
            }
          if (got)
            {
              skip = v;
            }
        }

      SKIPCT = (int)skip;
      return;
    }

  /* numeric form: two VARSCAN expressions */
  {
    int64_t a = 0, b = 0;
    struct rel ra, rb, rs;
    varscan (&a, &ra, 0);
    varscan (&b, &rb, 0);
    switch (OP->val)
      {
      case 0:
        cond = (a == b && rel_eq (&ra, &rb));
        break; /* IFE */

      case 1:
        cond = (a > b && rel_eq (&ra, &rb));
        break; /* IFG */

      case 2:
        cond = (a < b && rel_eq (&ra, &rb));
        break; /* IFL */

      case 3:
        cond = (a != b || !rel_eq (&ra, &rb));
        break; /* INE */
      }
    if (cond)
      {
        return; /* relation holds: assemble next */
      }

    if (!nextfld ())
      { /* optional skip count (default 1) */
        CURRCH--;
        DEL = 0;
        varscan (&skip, &rs, 0);
        if (skip < 0)
          {
            error (4);
            skip = 0;
          }

        if (rel_nz (&rs))
          {
            error (5);
          }
      }
  }
  SKIPCT = (int)skip;
}

/* OPSYN (SI61 CASE 38): "NEW OPSYN OLD" -- alias NEW to the existing op OLD.
 * Pass-1 only (the alias persists to pass 2); errors if OLD is unknown or NEW
 * already names a built-in op or alias. */
static void
case_opsyn (void)
{
  const struct bmap_op *o;

  STMNTCT++;
  if (PASS2 != 0)
    {
      return;
    }

  if (nextfld ())
    {
      error (-4);
      return;
    } /* operand: the existing op */

  o = find_op (CURRCH, NEXTCH);
  if (!o)
    {
      error (-4);
      return;
    }

  if (LOCSZ == 0)
    {
      error (7);
      return;
    } /* no new name in the location field */

  if (find_op (0, LOCSZ) || opsyn_find (LOC))
    {
      error (-13);
      return;
    } /* already defined */

  opsyn_add (LOC, o);
}

/* The listing flag LISTCTL's OP.VAL selects (DETAIL/LIST/PMC/HEXFP/CRSM/FLOAT/
 * PCC/REF/REFMA = 0..8), plus a one-deep SAVE/RESTORE stack per flag. */
static int *
listing_flag (int i)
{
  switch (i)
    {
    case 0:
      return &LISTING.detail;

    case 1:
      return &LISTING.list;

    case 2:
      return &LISTING.pmc;

    case 3:
      return &LISTING.hexfp;

    case 4:
      return &LISTING.crsm;

    case 5:
      return &LISTING.floatf;

    case 6:
      return &LISTING.pcc;

    case 7:
      return &LISTING.ref;

    case 8:
      return &LISTING.refma;

    default:
      return NULL;
    }
}
static int LISTING_STACK[9];

/* LISTCTL (SI61 CASE 39): a listing flag is set from ON/OFF/SAVE/RESTORE
 * operands (several may follow).  Processed in both passes so an
 * assembly-affecting flag (FLOAT/CRSM) stays consistent; LISTING is reset each
 * pass, so the directive re-applies at the same point. */
static void
case_listctl (void)
{
  int i = (int)OP->val, *flag = listing_flag (i);

  STMNTCT++;
  if (!flag)
    {
      return;
    }

  while (!nextfld ())
    {
      char w[16];
      int k = 0, c;
      for (c = CURRCH; c < NEXTCH && k < 15; c++)
        {
          w[k++] = (char)upch ((unsigned char)XCARD[c]);
        }

      w[k] = '\0';
      if (!strcmp (w, "ON"))
        {
          *flag = 1;
        }
      else if (!strcmp (w, "OFF"))
        {
          *flag = 0;
        }
      else if (!strcmp (w, "SAVE"))
        {
          LISTING_STACK[i] = *flag;
        }
      else if (!strcmp (w, "RESTORE"))
        {
          *flag = LISTING_STACK[i];
        }
      else
        {
          error (4);
          break;
        }

      CURRCH = NEXTCH;
    }
}

/* EJECT / TTL / TTLS (SI61 CASE 8, OP.VAL 0/1/2): listing pagination.  This
 * port's listing is not paginated, so EJECT emits a form feed and TTL/TTLS
 * emit the title text as a marker line -- visible without a page-header
 * system. */
static void
case_eject (void)
{
  STMNTCT++;
  if (PASS2 == 0 || !OPTIONS.ls)
    {
      return;
    }

  if (OP->val == 0)
    {
      fputc ('\f', LO);
      return;
    } /* EJECT: page break */

  { /* TTL / TTLS: title text */
    int e = XCARDL;
    while (e > CURRCH && XCARD[e - 1] == ' ')
      {
        e--;
      }
    if (e > CURRCH)
      {
        fprintf (LO, "* %s: %.*s\n", OP->val == 1 ? "TTL" : "TTLS", e - CURRCH,
                 &XCARD[CURRCH]);
      }
  }
}

/* LITORG / LIT (SI61 CASE 45): flush the accumulated literal pool to the
 * LITERALS section here (instead of only at END) and begin a fresh batch.  An
 * optional label is defined as the pool's current LITSECT location.  genlits
 * emits in pass 2; clearing the table happens in both passes so the LIT
 * offsets stay pass-stable (LITLOC keeps advancing, so batches do not
 * overlap). */
static void
case_litorg (void)
{
  STMNTCT++;
  if (LOCSZ != 0)
    { /* LBL LITORG -> LBL = pool location */
      int v = LITLOC;
      struct rel r;
      memset (&r, 0, sizeof r);
      r.opndtyp = OPERSECT;
      r.relocop = RELOCOPADD;
      r.operand = LITSECT;
      symtab (LOC, &v, &r, 1);
    }

  if (PASS2 != 0)
    {
      genlits (); /* emit the accumulated pool */
    }

  n_lit = 0; /* fresh batch (LITLOC unchanged) */
}

static void
dispatch_real (void)
{
  switch (OP->type)
    {
    case 11:
      case_boundary ();
      break;

    case 12:
      case_org ();
      break;

    case 24:
      case_bss ();
      break;

    case 40:
      case_ouname ();
      break;

    case 14:
      case_defref ();
      break; /* ENTDEF/ENTREF/SYMDEF/SYMREF/SEGREF */

    case 7:
      case_use ();
      break; /* USE / BLOCK (control sections) */

    case 19:
      case_dup ();
      break; /* DUP (repeat cards) */

    case 20:
      case_macro_def ();
      break; /* MACRO definition */

    case 21:
      case_macro_call ();
      break; /* macro call (expand) */

    case 22:
      case_if ();
      break; /* IFE/IFG/IFL/INE conditional */

    case 37:
      STMNTCT++;
      error (2);
      break; /* IDRP outside a macro (SI61 MLVL=1) */

    case 38:
      case_opsyn ();
      break; /* OPSYN (opcode synonym) */

    case 8:
      case_eject ();
      break; /* EJECT / TTL / TTLS */

    case 39:
      case_listctl ();
      break; /* DETAIL/LIST/PMC/.../REF/REFMA */

    case 45:
      case_litorg ();
      break; /* LITORG / LIT (flush literal pool) */

    case 41:
    case 43:
      STMNTCT++;
      break; /* LODM/...: deferred */

    default:
      inst (); /* instruction + data types */
    }
}

/* =============================================================== trace ====
 * Phase-2 hand-verification artefact: one line per statement card.  Re-walks
 * the variable field from the cursor READCARD left, using NEXTFLD/DELSCAN, and
 * prints each subfield as "<text>"/<DELIM>.  Deterministic (no dates/paths) so
 * it can be snapshotted into a tests/expected/ .scan file. */
static void
trace_var (void)
{
  int first = 1;

  CURRCH = VAR_CURRCH0;
  DEL = VAR_DEL0;
  while (!nextfld ())
    {
      int i;
      fputs (first ? "  | " : " ", LO);
      first = 0;
      fputc ('"', LO);
      for (i = CURRCH; i < NEXTCH; i++)
        {
          fputc (XCARD[i], LO);
        }

      fprintf (LO, "\"/%s", del_name[DEL]);
      CURRCH = NEXTCH;
    }
}

static void
trace_card (void)
{
  if (!OPTIONS.ls)
    {
      return;
    }

  fprintf (LO, "%04d L=%-8s O=%-8s t%02d:%-12s v%06o", CARD_COUNT, LOC,
           OP->mnem, OP->type, bmap_optype[OP->type], OP->val);
  trace_var ();
  fputc ('\n', LO);
}

/* Real octal listing line (listing mode): location + assembled word(s) + the
 * source card.  This is the phase-5 stand-in for the BMAP PRINT/LIST listing
 * (which, with its title/page formatting, is phase 8). */
static void
listing_line (int pc0)
{
  int i;
  const char *p = "", *pcont = ""; /* XR: statement-# prefix */
  char pbuf[12];

  if (!OPTIONS.ls || !LISTING.list)
    {
      return; /* LIST OFF suppresses listing */
    }

  if (OPTIONS.xr)
    { /* line-number column for the xref */
      snprintf (pbuf, sizeof pbuf, "%4d ", CARD_COUNT);
      p = pbuf;
      pcont = "     ";
    }

  if (gen_logn > 0)
    { /* code: location + word(s) */
      fprintf (LO, "%s%d %06o  %012llo  | %.*s\n", p, PCREL.operand % 10, pc0,
               (unsigned long long)gen_log[0], XCARDL, XCARD);
      for (i = 1; i < gen_logn; i++)
        {
          fprintf (LO, "%s  %06o  %012llo  |\n", pcont, pc0 + i,
                   (unsigned long long)gen_log[i]);
        }
    }
  else if (gen_has_val)
    { /* EQU/SET: value, no location */
      fprintf (LO, "%s         =%012llo  | %.*s\n", p,
               (unsigned long long)gen_val, XCARDL, XCARD);
    }
  else if (OP->type == 24 || OP->type == 12)
    { /* BSS/ORG: located, no word */
      fprintf (LO, "%s%d %06o               | %.*s\n", p, PCREL.operand % 10,
               pc0, XCARDL, XCARD);
    }
  else
    { /* no code (comment, etc.) */
      fprintf (LO, "%s                       | %.*s\n", p, XCARDL, XCARD);
    }
}

/* ===================================================== dispatch stubs =====
 * The OP.TYPE handlers (BMAP_SI61.XSI line 215 DO CASE).  Phase 2 routes them
 * all to counting stubs; only END drives the pass machinery.  STMNTCT mirrors
 * the original (each real case bumps it).  INST (the 46-way encoder), the
 * object writer and the symbol table are later phases. */
static void
inst_stub (void)
{
  STMNTCT++;
} /* CASE(ELSE) -> INST (SI61 806) */
static void
mainline_stub (void)
{
  STMNTCT++;
} /* USE/EJECT/ORG/.../LIT cases */

/* GENLOC/BOUNDARY placeholders for the L10 pre-dispatch (SI61 210-214).
 * GENLOC emits the location word and BOUNDARY forces alignment -- both need
 * GEN (phase 5).  Symbol definition (SYMTAB) is live as of phase 3. */
static void
genloc_stub (void)
{
}
static void
boundary_stub (void)
{
}

/* ============================================================ pass loop ===
 * BMAP_SI61.XSI: initialisation (line 129), per-pass init at L5 (164),
 * statement loop at L10 (207), END handling in CASE(9) (306). */
static void
init_once (void)
{
  PASS2 = 0;
  TERRCT = 0;
  ERRSEV = 0;
  RECORDCT = 0;
  STMNTCT = 0;
  CRD_COUNT = -1;
  SYMROOT_ = NULL;
  NSYM = 0;    /* the symbol tree persists across passes */
  n_opsyn = 0; /* OPSYN aliases: built in pass 1, kept for pass 2 */
  /* XUO$OUINIT + the standard sections.  Done even when OU is off so the
   * control-section table (used for USE/BLOCK PC tracking and the listing's
   * section column) is always present; the object records themselves stay
   * OU-gated (xuo_prgm / xuo_outterm). */
  xuo_ouinit ();
  xuo_sectbuild (CODESECTION, "", 0);       /* section 0: code (blank name) */
  xuo_sectbuild (ROSECTION, "LITERALS", 8); /* section 1: literals */
}

static void
init_per_pass (void)
{
  int i;

  src_cur = 0;
  exp_reset (); /* discard any macro expansion in flight */
  SKIPCT = 0;
  CRSMNO = 0; /* CRSM symbol counter: pass-stable names */
  n_lit = 0;
  LITLOC = 0; /* literal pool restarts each pass */
  PC = 0;
  ERRCT = 0;
  CARD_COUNT = 0;
  /* each control section's location counter restarts at 0 each pass; the
   * section table itself (built in pass 1) persists across the passes */
  for (i = 0; i < NOSECT; i++)
    {
      OSECT[i].pc = 0;
    }

  PREVIOUS_SECT = 0;
  /* PCREL = section relocation, operand = current section (SI61 182) */
  PCREL = (struct rel){ 0, 0, 0, 0, 0, 0, OPERSECT, 0, RELOCOPADD,
                        0, 0, 0, 0, 0, 0, 0,        0 };
  LISTING.detail = 1;
  LISTING.list = 1;
  LISTING.pmc = 0;
  LISTING.hexfp = 1;
  LISTING.crsm = 0;
  LISTING.floatf = 0;
  LISTING.pcc = 1;
  LISTING.ref = 1;
  LISTING.refma = 1;
}

/* One pass over the source.  Returns 1 when END was seen in pass 1 (caller
 * starts pass 2), 0 when the assembly is complete (END in pass 2).
 *
 * Two modes: --scan keeps the phase-2 scanner trace (handlers are counting
 * stubs, PC stays 0); the default listing mode runs the real encoder (INST +
 * the mainline handlers), advances PC and emits an octal listing. */
static int
do_pass (void)
{
  for (;;)
    {
      int pc0;
      CARD_COUNT++;
      ERRCT = 0;
      gen_logn = 0;
      gen_has_val = 0;
      read_card (0);

      /* IF skip (SI61 MAC.SKIP): a failed IF skips the next N cards -- they
       * assemble to nothing.  END is never skipped (it must terminate). */
      if (SKIPCT > 0 && OP->type != 9)
        {
          SKIPCT--;
          if (PASS2 == 1)
            {
              if (opt_scan)
                {
                  trace_card ();
                }
              else
                {
                  listing_line (PC);
                }
            }

          continue;
        }

      /* L10 pre-dispatch (SI61 210-214): an op with the location print-flag
       * and a label defines that label = PC with the PC relocation. */
      if (OP->type >= 34 && OP->type <= 36)
        {
          if (opt_scan)
            {
              boundary_stub ();
            }
          else
            {
              boundary (2, 0, 0);
            }
        }

      if ((OP->prfs & 1) && LOCSZ != 0)
        {
          int v = PC;
          struct rel r = PCREL;
          symtab (LOC, &v, &r, 1);
          if (opt_scan)
            {
              genloc_stub ();
            }
          else
            {
              genloc (0, 0, 0, 0);
            }
        }

      if (OP->type == 9)
        { /* END (CASE 9) */
          STMNTCT++;
          if (PASS2 == 0)
            {
              PASS2 = 1;
              CRD_COUNT = CARD_COUNT;
              return 1; /* -> pass 2 (GOTO L5) */
            }

          if (PASS2 > 0 && CRD_COUNT != CARD_COUNT && CRD_COUNT >= 0)
            {
              error (20); /* 2 PASS INPUT UNBALANCED */
            }

          if (PASS2 == 1)
            {
              if (opt_scan)
                {
                  trace_card ();
                }
              else
                {
                  listing_line (PC);
                }

              if (!opt_scan)
                {
                  genlits (); /* emit the literal pool (LITSECT) */
                }

              if (OPTIONS.ou)
                { /* finalize the object unit */
                  xuo_head_severity (ERRSEV);
                  xuo_outterm ();
                }
            }

          return 0; /* assembly complete */
        }

      pc0 = PC;
      if (opt_scan)
        {
          /* scan mode: counting stubs (the original mainline-vs-INST split) */
          switch (OP->type)
            {
            case 0:
              break;

            case 7:
            case 8:
            case 11:
            case 12:
            case 20:
            case 24:
            case 38:
            case 39:
            case 40:
            case 41:
            case 43:
            case 45:
              mainline_stub ();
              break;

            default:
              inst_stub ();
              break;
            }
          if (PASS2 == 1)
            {
              trace_card ();
            }
        }
      else
        {
          dispatch_real (); /* INST + mainline handlers */
          /* track each section's extent: the high-water PC, so a trailing BSS
           * (which reserves words without emitting any) still grows the SECT
           * size -- xuo_prgm only sees emitted words. */
          if (OPTIONS.ou && PASS2 == 1 && PCREL.operand >= 0
              && PCREL.operand < NOSECT && PC > OSECT[PCREL.operand].size)
            {
              OSECT[PCREL.operand].size = PC;
            }

          if (PASS2 == 1) /* L90 print (pass 2) */
            {
              listing_line (OP->type == 12 ? PC
                                           : pc0); /* ORG shows new origin */
            }
        }
    }
}

static void
assemble (void)
{
  init_once ();
  init_per_pass (); /* L5, pass 1 */
  if (do_pass ())
    { /* END in pass 1 */
      if (OPTIONS.ou)
        {
          assign_ext_numbers (SYMROOT_); /* EREF/SREF numbers */
        }

      init_per_pass (); /* L5, pass 2 */
      do_pass ();
    }
}

/* ============================================================== summary ===
 */
/* Sorted symbol-table dump (in-order walk = TREESTEP order).  Until GEN
 * tracks PC (phase 5) defined-label values are 0; this verifies the AVL
 * insert/sort/dedup and the SYMTAB define path. */
static void
dump_one_sym (struct sym *s)
{
  fprintf (LO, "  %-8s v%06o  %s  opndtyp=%d\n", s->name,
           (unsigned)(s->value & 0777777), s->defined ? "DEF" : "UND",
           s->r.opndtyp);
}
/* Cross-reference line for one symbol (XR option): value, defined/undefined,
 * then the statement numbers that touch it -- the definition line marked `*`.
 */
static void
dump_one_xref (struct sym *s)
{
  int i;

  fprintf (LO, "  %-8s v%06o  %s ", s->name, (unsigned)(s->value & 0777777),
           s->defined ? "DEF" : "UND");
  for (i = 0; i < s->nref; i++)
    {
      fprintf (LO, " %d%s", s->refs[i],
               (s->defined && s->refs[i] == s->line) ? "*" : "");
    }

  fputc ('\n', LO);
}
static void
write_summary (void)
{
  if (!OPTIONS.ls)
    {
      return;
    }

  fprintf (LO, "----\n");
  fprintf (LO, "passes=%d statements=%d records=%d errors=%d severity=%d\n",
           OPTIONS.p2 ? 2 : 1, STMNTCT, RECORDCT, TERRCT, ERRSEV);
  if (OPTIONS.xr)
    { /* XR: symbol cross-reference */
      fprintf (LO, "---- cross-reference (%d symbols; *=definition) ----\n",
               NSYM);
      sym_walk (SYMROOT_, dump_one_xref);
    }
  else
    {
      fprintf (LO, "---- symbols (%d) ----\n", NSYM);
      sym_walk (SYMROOT_, dump_one_sym);
    }

  if (end_synth)
    {
      fprintf (LO, "note: END synthesised (no END statement in source)\n");
    }
}

/* ============================================================= self-test ==
 * `bmap -t` exercises CONVERT and the symbol table directly.  No source-level
 * caller reaches them until VARSCAN (phase 4) / the INST handlers (phase 5),
 * so this is the phase-3 verification artefact (snapshot to a fixture). */
static void
set_xcard (const char *s)
{
  int i, n = (int)strlen (s);

  for (i = 0; i < n && i < XCSIZE; i++)
    {
      XCARD[i] = s[i];
    }

  for (; i < XCSIZE; i++)
    {
      XCARD[i] = ' ';
    }

  XCARD[XCSIZE] = '\0';
  XCARDL = n;
  CURRCH = 0;
}
/* Lay out an expression for varscan exactly as read_card leaves the cursor:
 * a leading blank so CURRCH=0 is one short and NEXTFLD's pre-increment lands
 * on the first char. */
static void
set_xcard_expr (const char *s)
{
  int i, n = (int)strlen (s);

  XCARD[0] = ' ';
  for (i = 0; i < n && i + 1 < XCSIZE; i++)
    {
      XCARD[i + 1] = s[i];
    }

  for (i = n + 1; i < XCSIZE; i++)
    {
      XCARD[i] = ' ';
    }

  XCARD[XCSIZE] = '\0';
  XCARDL = n + 1;
  CURRCH = 0;
  NEXTCH = 0;
  DEL = 0;
}
static void
print_rel (const struct rel *r)
{
  if (!rel_nz (r))
    {
      fputs ("abs", LO);
      return;
    }

  fprintf (LO, "reloc(opndtyp=%d operand=%d relocop=%d", r->opndtyp,
           r->operand, r->relocop);
  if (r->evalop)
    {
      fprintf (LO, " evalop=%d value=%d", r->evalop, r->value);
    }

  if (rel_s_nz (r))
    {
      fprintf (LO, " S[opndtyp=%d operand=%d relocop=%d]", r->s_opndtyp,
               r->s_operand, r->s_relocop);
    }

  fputc (')', LO);
}
/* Load a full card and position the cursor at the variable field exactly as
 * READCARD does, so INST can VARSCAN it (no TTL/comment handling needed). */
static void
setup_card (const char *card)
{
  int n = (int)strlen (card), i, var, j;

  for (i = 0; i < n && i < XCSIZE; i++)
    {
      XCARD[i] = card[i];
    }

  for (; i < XCSIZE; i++)
    {
      XCARD[i] = ' ';
    }

  XCARD[XCSIZE] = '\0';
  XCARDL = n;
  scanop ();
  if (LOCSZ != 0)
    {
      int m = LOCSZ < MAXSYM ? LOCSZ : MAXSYM;
      for (i = 0; i < m; i++)
        {
          LOC[i] = (char)upch ((unsigned char)XCARD[i]);
        }

      LOC[m] = '\0';
    }
  else
    {
      LOC[0] = '\0';
    }

  DEL = 0;
  var = searchtbl (NONBLK, NEXTCH, NEXTCH + 15, &j);
  if (var < 0)
    {
      CURRCH = NEXTCH;
      DEL = D_BLANK;
    }
  else
    {
      CURRCH = var - 1;
    }
}
static void
run_selftests (void)
{
  static const struct
  {
    const char *s;
    int base;
  } ct[] = {
    { "0", 10 },   { "5", 10 },   { "123", 10 },  { "68719476735", 10 },
    { "7", 8 },    { "10", 8 },   { "777", 8 },   { "777777777777", 8 },
    { "1.5", 10 }, { "1E3", 10 }, { "3B17", 10 },
  };
  static const struct
  {
    const char *n;
    int v;
  } defs[] = {
    { "GAMMA", 15 }, { "ALPHA", 5 }, { "BETA", 10 },
    { "MIDDLE", 7 }, { "AAA", 1 },   { "DELTA", 4 },
  };
  int i;
  int64_t v;
  int type, def;
  double dv;
  struct rel r, cst;

  fprintf (LO, "convert:\n");
  for (i = 0; i < (int)(sizeof ct / sizeof ct[0]); i++)
    {
      set_xcard (ct[i].s);
      convert (ct[i].base, &v, &type, &def, &dv);
      if (type == 1 || type == 2)
        { /* SPFP / DPFP */
          W fw[2];
          int n = dps8_float (dv, type == 2, fw);
          if (n == 1)
            {
              fprintf (LO, "  %-13s base %2d -> float %012llo type=%d\n",
                       ct[i].s, ct[i].base, (unsigned long long)fw[0], type);
            }
          else
            {
              fprintf (LO,
                       "  %-13s base %2d -> float %012llo %012llo type=%d\n",
                       ct[i].s, ct[i].base, (unsigned long long)fw[0],
                       (unsigned long long)fw[1], type);
            }
        }
      else
        {
          fprintf (LO, "  %-13s base %2d -> %lld = 0%llo type=%d\n", ct[i].s,
                   ct[i].base, (long long)v, (unsigned long long)v, type);
        }
    }

  fprintf (LO, "symtab:\n");
  SYMROOT_ = NULL;
  NSYM = 0;
  TERRCT = 0;
  ERRSEV = 0;
  ERRCT = 0;
  PASS2 = 1;
  memset (&cst, 0, sizeof cst);
  cst.opndtyp = OPERCONST;
  for (i = 0; i < (int)(sizeof defs / sizeof defs[0]); i++)
    {
      int val = defs[i].v;
      r = cst;
      symtab (defs[i].n, &val, &r, 1);
    }

  fprintf (LO, "  sorted:\n");
  sym_walk (SYMROOT_, dump_one_sym);
  { /* reference: a hit and a miss (the miss creates an undefined entry) */
    int val = -1;
    r = cst;
    symtab ("ALPHA", &val, &r, 0);
    fprintf (LO, "  lookup ALPHA -> val=%d opndtyp=%d\n", val, r.opndtyp);
    val = -1;
    r = cst;
    symtab ("ZZZ", &val, &r, 0);
    fprintf (LO, "  lookup ZZZ   -> val=%d opndtyp=%d (undefined)\n", val,
             r.opndtyp);
  }
  { /* redefinition: same value is benign, a conflict raises error -10 */
    int val = 5;
    ERRCT = 0;
    r = cst;
    symtab ("ALPHA", &val, &r, 1);
    val = 999;
    ERRCT = 0;
    r = cst;
    symtab ("ALPHA", &val, &r, 1);
    fprintf (LO, "  redefine ALPHA=5 then ALPHA=999 -> errors=%d\n", TERRCT);
  }

  fprintf (LO, "varscan:\n");
  SYMROOT_ = NULL;
  NSYM = 0;
  PASS2 = 1;
  { /* two relocatable (section) symbols and one absolute, for reloc tests */
    struct rel rs;
    int vv;
    memset (&rs, 0, sizeof rs);
    rs.opndtyp = OPERSECT;
    rs.relocop = RELOCOPADD;
    vv = 100;
    rs.operand = 0;
    {
      struct rel t = rs;
      symtab ("L1", &vv, &t, 1);
    }
    vv = 40;
    rs.operand = 0;
    {
      struct rel t = rs;
      symtab ("L2", &vv, &t, 1);
    }
    memset (&rs, 0, sizeof rs);
    vv = 8;
    symtab ("K", &vv, &rs, 1);
  }
  {
    static const struct
    {
      const char *e;
      int type;
    } ve[] = {
      { "5+3", 0 },   { "10-2", 0 },    { "5*3", 0 },   { "17/3", 0 },
      { "2+3*4", 0 }, { "(2+3)*4", 0 }, { "-5", 0 },    { "L1", 0 },
      { "L1+5", 0 },  { "L1+L2", 0 },   { "L1-L2", 0 }, { "K*4", 0 },
      { "L1*2", 0 },  { "L1*L2", 0 },   { "5+2", 1 },
    };
    int k;
    for (k = 0; k < (int)(sizeof ve / sizeof ve[0]); k++)
      {
        int64_t val;
        struct rel rr;
        TERRCT = 0;
        ERRSEV = 0;
        ERRCT = 0;
        set_xcard_expr (ve[k].e);
        varscan (&val, &rr, ve[k].type);
        fprintf (LO, "  %-9s t%d -> %lld = 0%012llo  ", ve[k].e, ve[k].type,
                 (long long)sx36 (val),
                 (unsigned long long)((uint64_t)val & M36));
        print_rel (&rr);
        if (TERRCT)
          {
            fprintf (LO, "  [err]");
          }

        fputc ('\n', LO);
      }
  }

  fprintf (LO, "inst:\n");
  SYMROOT_ = NULL;
  NSYM = 0;
  PASS2 = 1;
  PC = 0;
  INHIB_BIT28 = 0;
  LISTING.floatf = 0;
  PCREL = (struct rel){ 0, 0, 0, 0, 0, 0, OPERSECT, 0, RELOCOPADD,
                        0, 0, 0, 0, 0, 0, 0,        0 };
  { /* one relocatable label and one absolute constant for the operands */
    struct rel r;
    int v;
    memset (&r, 0, sizeof r);
    r.opndtyp = OPERSECT;
    r.relocop = RELOCOPADD;
    v = 0100;
    symtab ("LOOP", &v, &r, 1);
    memset (&r, 0, sizeof r);
    v = 5;
    symtab ("KON", &v, &r, 1);
  }
  {
    static const char *const prog[] = {
      "       LDA   5",       /* type 1: numeric address */
      "       ADA   KON",     /* type 1: absolute symbol */
      "       STA   LOOP",    /* type 1: relocatable symbol */
      "       LDA   10,1",    /* type 1: numeric address + index tag 1 */
      "       STQ   LOOP,QU", /* type 1: register modifier QU */
      "       LDA   5,QU,1",  /* type 1: address-register (AR) 4-field form */
      "       ADX   3,5",     /* type 2: index instr, absolute register 3 */
      "       NOP",           /* type 6: no variable field */
      "       TRA   UNDEF",   /* type 1: forward (undefined) reference */
    };
    int k;
    for (k = 0; k < (int)(sizeof prog / sizeof prog[0]); k++)
      {
        const char *disp = prog[k];
        while (*disp == ' ')
          {
            disp++;
          }
        TERRCT = 0;
        ERRSEV = 0;
        ERRCT = 0;
        gen_has_word = 0;
        gen_octal[0] = '\0';
        setup_card (prog[k]);
        inst ();
        if (gen_has_word)
          {
            fprintf (LO, "  %-12s %02o/%06o: %012llo %s%s\n", disp,
                     gen_loc_sect, gen_loc_pc, (unsigned long long)gen_word,
                     gen_octal, gen_frf ? " (fref)" : "");
          }
        else
          {
            fprintf (LO, "  %-12s (no word)\n", disp);
          }
      }
  }

  fprintf (LO, "data:\n");
  SYMROOT_ = NULL;
  NSYM = 0;
  PASS2 = 1;
  PC = 0;
  INHIB_BIT28 = 0;
  PCREL = (struct rel){ 0, 0, 0, 0, 0, 0, OPERSECT, 0, RELOCOPADD,
                        0, 0, 0, 0, 0, 0, 0,        0 };
  {
    static const char *const data[] = {
      "FIVE   EQU   5",     /* type 13: define a constant */
      "MASK   BOOL  777",   /* type 13: octal-base equate */
      "CTR    SET   1",     /* type 13: SET */
      "CTR    SET   2",     /* type 13: SET redefinition (no error) */
      "       DEC   1,2,3", /* type 16: three decimal words */
      "       OCT   777,0", /* type 16: two octal words */
      "       DEC   -7",    /* type 16: negative decimal */
      "       ZERO  4,5",   /* type 17: ZERO two-field word */
    };
    int k, i;
    for (k = 0; k < (int)(sizeof data / sizeof data[0]); k++)
      {
        const char *disp = data[k];
        while (*disp == ' ')
          {
            disp++;
          }
        TERRCT = 0;
        ERRSEV = 0;
        ERRCT = 0;
        gen_logn = 0;
        setup_card (data[k]);
        inst ();
        if (gen_logn)
          {
            fprintf (LO, "  %-12s ->", disp);
            for (i = 0; i < gen_logn; i++)
              {
                fprintf (LO, " %012llo", (unsigned long long)gen_log[i]);
              }

            if (TERRCT)
              {
                fprintf (LO, "  [err]");
              }

            fputc ('\n', LO);
          }
        else
          {
            fprintf (LO, "  %-12s (defined)%s\n", disp,
                     TERRCT ? "  [err]" : "");
          }
      }

    fprintf (LO, "  symbols:\n");
    sym_walk (SYMROOT_, dump_one_sym);
  }
}

/* =============================================================== driver ===
 */
static int
read_source (const char *name)
{
  FILE *f = fopen (name, "r");
  char buf[1024];

  if (!f)
    {
      return 1;
    }

  while (fgets (buf, sizeof buf, f))
    {
      int n = (int)strlen (buf);
      while (n > 0 && (buf[n - 1] == '\n' || buf[n - 1] == '\r'))
        {
          buf[--n] = '\0';
        }
      if (src_n == src_cap)
        {
          src_cap = src_cap ? src_cap * 2 : 256;
          src_lines = (char * *)realloc (src_lines, src_cap * sizeof *src_lines);
          if (!src_lines)
            {
              fprintf (stderr, "bmap: out of memory\n");
              exit (2);
            }
        }

      src_lines[src_n] = (char *)malloc ((size_t)n + 1);
      memcpy (src_lines[src_n], buf, (size_t)n + 1);
      src_n++;
    }
  fclose (f);
  return 0;
}

/* Apply one GMAP option token (HELP_BMAP.XSI lines 23-46). */
static void
set_option (const char *t)
{
  if (!strcmp (t, "1P"))
    {
      OPTIONS.p2 = 0;
    }
  else if (!strcmp (t, "2P"))
    {
      OPTIONS.p2 = 1;
    }
  else if (!strcmp (t, "LU"))
    {
      OPTIONS.lu = 1;
    }
  else if (!strcmp (t, "LS"))
    {
      OPTIONS.ls = 1;
    }
  else if (!strcmp (t, "NLS"))
    {
      OPTIONS.ls = 0;
    }
  else if (!strcmp (t, "OU"))
    {
      OPTIONS.ou = 1;
    }
  else if (!strcmp (t, "NOU"))
    {
      OPTIONS.ou = 0;
    }
  else if (!strcmp (t, "SC") || !strcmp (t, "SCHEMA"))
    {
      OPTIONS.nd = 0;
    }
  else if (!strcmp (t, "NSC") || !strcmp (t, "NSCHEMA"))
    {
      OPTIONS.nd = 1;
    }
  else if (!strcmp (t, "SO"))
    {
      OPTIONS.so = 1;
    }
  else if (!strcmp (t, "UI"))
    {
      OPTIONS.ui = 1;
    }
  else if (!strcmp (t, "NUI"))
    {
      OPTIONS.ui = 0;
    }
  else if (!strcmp (t, "XR") || !strcmp (t, "XREF"))
    {
      OPTIONS.xr = 1;
    }
  else if (!strncmp (t, "SRCH", 4) || !strncmp (t, "SR(", 3))
    {
      OPTIONS.nacs = 1; /* SRCH(fid,...) accounts: phase 7 */
    }
  else
    {
      fprintf (stderr, "bmap: unknown option '%s' (ignored)\n", t);
    }
}

/* Parse a comma/space/paren separated option list ("1P,LS,OU" or "(1P LS)").
 */
static void
parse_options (const char *s)
{
  char tok[32];
  int n = 0;

  for (;;)
    {
      int c = *s++;
      if (c == '(' || c == ')' || c == ' ' || c == ',' || c == '\0')
        {
          if (n)
            {
              tok[n] = '\0';
              set_option (tok);
              n = 0;
            }

          if (c == '\0')
            {
              break;
            }
        }
      else if (n < (int)sizeof tok - 1)
        {
          tok[n++] = (char)upch (c);
        }
    }
}

static char *
derive (const char *src, const char *ext)
{
  const char *slash = strrchr (src, '/');
  const char *base = slash ? slash + 1 : src;
  const char *dot = strrchr (base, '.');
  size_t n = dot ? (size_t)(dot - src) : strlen (src);
  char *out = (char *)malloc (n + strlen (ext) + 1);

  memcpy (out, src, n);
  strcpy (out + n, ext);
  return out;
}

static void
resolve_sentinels (void)
{
  /* find_op needs XCARD set up; resolve the few sentinels by direct search */
  static const char *names[] = { "NONOP", "END" };
  const struct bmap_op *found[2] = { NULL, NULL };
  int i;

  for (i = 0; i < 2; i++)
    {
      found[i] = (struct bmap_op *)bsearch (
          names[i], bmap_ops, NBMAPOP, sizeof bmap_ops[0], op_cmp);
    }

  OP_NONOP = found[0];
  OP_END = found[1];
  if (!OP_NONOP || !OP_END)
    {
      fprintf (stderr,
               "bmap: internal error: missing NONOP/END in op table\n");
      exit (2);
    }
}

int
main (int argc, char **argv)
{
  const char *srcname = NULL, *objname = NULL, *lstname = NULL;
  const char *updname = NULL, *soname = NULL, *optstr = NULL;
  char *dlst = NULL;
  int i, gave_options = 0, testmode = 0;

  for (i = 1; i < argc; i++)
    {
      const char *a = argv[i];
      if (!strcmp (a, "-o") && i + 1 < argc)
        {
          objname = argv[++i];
        }
      else if (!strcmp (a, "-l") && i + 1 < argc)
        {
          lstname = argv[++i];
        }
      else if (!strcmp (a, "-u") && i + 1 < argc)
        {
          updname = argv[++i];
        }
      else if (!strcmp (a, "-s") && i + 1 < argc)
        {
          soname = argv[++i];
        }
      else if (!strcmp (a, "-O") && i + 1 < argc)
        {
          optstr = argv[++i];
          gave_options = 1;
        }
      else if (!strcmp (a, "-t") || !strcmp (a, "--selftest"))
        {
          testmode = 1;
        }
      else if (!strcmp (a, "-S") || !strcmp (a, "--scan"))
        {
          opt_scan = 1;
        }
      else if (!strcmp (a, "-g") || !strcmp (a, "--debug"))
        {
          opt_debug = 1;
        }
      else if (a[0] == '(')
        {
          optstr = a;
          gave_options = 1;
        } /* "(1P,LS,OU)" */
      else if (!srcname)
        {
          srcname = a;
        }
      else if (!objname)
        {
          objname = a;
        }
      else if (!lstname)
        {
          lstname = a;
        }
      else
        {
          fprintf (stderr, "bmap: extra argument '%s' ignored\n", a);
        }
    }

  if (!srcname && !testmode)
    {
      fprintf (
          stderr,
          "usage: bmap source [object [listing]] [-o obj] [-l lst]\n"
          "            [-u update] [-s srcout] [-O opts | (opts)] [-S] [-t]\n"
          "  opts: 1P 2P LU LS NLS OU NOU SC NSC SO UI NUI XR SRCH(...)\n"
          "  -S:   scanner trace only (no encoding)\n"
          "  -t:   run the CONVERT/symtab/varscan/inst self-test\n");
      return 2;
    }

  /* -t: phase-3 self-test of CONVERT + the symbol table (no source). */
  if (testmode)
    {
      init_tables ();
      resolve_sentinels ();
      LO = lstname ? fopen (lstname, "w") : stdout;
      if (!LO)
        {
          fprintf (stderr, "bmap: cannot create %s\n", lstname);
          return 2;
        }

      run_selftests ();
      if (LO != stdout)
        {
          fclose (LO);
        }

      return 0;
    }

  /* Options (CTLCRD, SI61 1150): always two-pass (1P/2P ignored per the
   * HELP file's star 24631); default to LS+OU when none are given. */
  OPTIONS.p2 = 1;
  if (lstname)
    {
      OPTIONS.ls = 1;
    }

  if (objname)
    {
      OPTIONS.ou = 1;
    }

  if (updname)
    {
      OPTIONS.ui = 1;
    }

  if (gave_options)
    {
      parse_options (optstr);
    }

  if (!gave_options && !lstname && !objname)
    {
      OPTIONS.ls = 1;
      OPTIONS.ou = 1;
    }

  OPTIONS.p2 = 1; /* enforced two-pass */

  init_tables ();
  resolve_sentinels ();

  if (read_source (srcname))
    {
      fprintf (stderr, "bmap: cannot open %s\n", srcname);
      return 2;
    }

  if (!lstname)
    {
      lstname = dlst = derive (srcname, ".scan");
    }

  LO = fopen (lstname, "w");
  if (!LO)
    {
      fprintf (stderr, "bmap: cannot create %s\n", lstname);
      return 2;
    }

  fprintf (stderr, "BMAP here (%s) -- %s\n",
           opt_scan ? "scanner trace" : "phase-5/6 assembler", srcname);
  (void)updname;
  (void)soname; /* M$UI/M$SO: later phases */

  assemble ();
  write_summary ();

  /* object unit (M$OU): assembled by xuo_outterm during pass-2 END */
  if (OPTIONS.ou && !opt_scan)
    {
      char *dobj = objname ? NULL : derive (srcname, ".obj");
      const char *on = objname ? objname : dobj;
      FILE *of = fopen (on, "wb");
      if (!of)
        {
          fprintf (stderr, "bmap: cannot create %s\n", on);
          return 2;
        }

      fwrite (OBJ.p, 1, OBJ.n, of);
      fclose (of);
      free (dobj);
    }

  fclose (LO);
  for (i = 0; i < src_n; i++)
    {
      free (src_lines[i]);
    }

  free (src_lines);
  free (dlst);
  return ERRSEV > 0 ? 1 : 0;
}
