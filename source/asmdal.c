/* asmdal.c - CP-6 ASMDAL: a two-pass assembler for a subset of DAL (DEC
 * Assembly Language).  C port of Dave Wagner's PL/6 ASMDAL_SI61.XSI
 * (Copyright (c) Bull HN Information Systems Inc., 1989), part of the CP-6
 * cross-assembler suite Linux port.
 *
 * Faithful to the original two-pass design: PASS 1 tokenises the column-
 * oriented source (LABEL[0:5] MNEMONIC[7:12] OPERAND[15:30] COMMENT[31:]),
 * builds an AVL symbol table and a token list; PASS 2 walks the tokens,
 * assembles 36-bit PDP-10 words and emits a CP-6 object unit plus a listing.
 * No object is produced when the source has errors (matching the original).
 *
 * The opcode/JSYS tables live in asmdal_tables.h, generated verbatim from the
 * PL/6 source, so instruction encodings match bit-for-bit.  The 9-bit octal
 * opcodes are the real PDP-10 opcodes (ADD=270, MOVE=200, ...), so each
 * assembled word is a valid PDP-10 instruction and can be hand-verified.
 *
 * Object-unit serialisation (see ASMDAL_NOTES.md): the original writes M$OU
 * records of 36-bit words; here each 36-bit word is emitted big-endian as 5
 * bytes (top 4 bits zero).  Records are self-framing via WORD0:
 *     WORD0 = TYPE(9) | LENGTH(9) | ADDRESS(18)
 * where LENGTH is the record's total word count (incl. WORD0).  TYPE 0 = code
 * (word1 = 18x 2-bit relocation codes, then up to 18 instruction words),
 * 1 = external defs, 3 = local defs (9 symbols/record, 2 words each),
 * 2 = trailer (ADDRESS = start address).  A reader walks records by LENGTH
 * until it sees TYPE 2.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#include "asmdal_tables.h"

typedef uint64_t W;                       /* a 36-bit CP-6 word */
#define M36 0xFFFFFFFFFULL
#define M18 0777777ULL

/* %EQU flag values (ASMDAL_C61): ABS#/REL#, EXT#/LOC#, DEF#/UND# */
enum { ABS_ = 1, REL_ = 0, EXT_ = 1, LOC_ = 0, DEF_ = 1, UND_ = 0 };

/* Error message indices.  These are exactly the bit positions of the single-
 * bit %EQU masks in the original BIT(72) error word, and index err_msg[]. */
enum {
    E_BAD_OPCODE, E_NO_OPCODE, E_BAD_LABEL, E_DUP_LABEL, E_BAD_OPERAND,
    E_BAD_NUMBER, E_CANT_LABEL, E_NO_OPERAND, E_BAD_DECIMAL, E_BAD_OCTAL,
    E_NOT_TITLE, E_MULT_TITLE, E_BAD_TITLE, E_BAD_SYMBOL, E_NO_LABEL,
    E_NO_SYMB, E_NO_END, E_DC1_NOT_NUM, E_DC1_TOO_BIG, E_DC2_TOO_BIG,
    E_BAD_SEMI, E_CANT_OPERAND, E_COLUMN, E_BAD_BIAS, E_BAD_JSYS,
    E_ACC_NOT_ABS, E_ACC_UND, E_ACC_BAD, E_XR_NOT_ABS, E_XR_UND, E_XR_BAD,
    E_DEV_NOT_ABS, E_DEV_UND, E_DEV_BAD, E_NO_DEV, E_VALUE_UND, E_VALUE_BAD,
    E_INV_ACC, E_INV_XR, E_INV_DEV, E_NO_PREV_SYMB, E_EXPRESS_EXT
};
static const char *err_msg[] = {
    "Invalid opcode mnemonic", "Missing opcode mnemonic", "Invalid label",
    "Multiply defined label", "Bad operand", "Bad number",
    "Label not allowed", "No operand where one was expected",
    "Bad decimal number", "Bad octal number",
    "Statment not within the scope of a TITLE/END block",
    "Multiple TITLE found", "Bad title", "Bad symbol in operand field",
    "Missing label", "Symbol not defined",
    "END statement provided courtesy of the assembler",
    "First half of split DC must be numeric", "First half of split DC too big",
    "Second half of split DC too big", "Badly placed semicolon",
    "Operand not allowed", "Columnation error", "Bad value offset",
    "Bad JSYS number", "Accumulator symbol must be absolute",
    "Undefined accumulator symbol", "Bad accumulator form",
    "Index register symbol must be absolute",
    "Undefined index register symbol", "Bad index register form",
    "Device symbol must be absolute", "Undefined device symbol",
    "Bad device form", "No device found in i/o statement",
    "Undefined value symbol", "Bad value form",
    "Accumulator number out of range", "Index register number out of range",
    "Device number out of range", "Symbol not previously defined",
    "Expressions may not contain external symbols"
};
#define NERR ((int)(sizeof err_msg / sizeof err_msg[0]))

/* Token: the per-line intermediate the original spools to a scratch file;
 * here just an in-memory array entry. */
struct token {
    uint64_t errors;        /* set of (1<<E_*) flags */
    W   pseudo_word;        /* UBIN(36) assembled pseudo-op word */
    int opcode;             /* UBIN(9) */
    int type;               /* 0 normal, 1 I/O, 2 JSYS, 3 no-code/pseudo */
    int indirect;           /* '@' present */
    int no_code;            /* line generates no object word */
    int reloc;              /* RELOCATION_BITS: 0=abs 1=rel 2=ext 3=block */
    char value[6];          /* value symbol/number text (CHAR 6) */
    int value_bias;         /* SBIN(18) +/- offset */
    char acc[6];            /* accumulator field (DEV redefines this) */
    char xr[6];             /* index register field */
    int jsys;               /* UBIN(18) JSYS number */
    int location;           /* UBIN(18) */
    W   ou_word;            /* assembled word (pass 2) */
    char source_line[80];   /* original source line */
};

/* AVL symbol-table node (Horowitz & Sahni, as in the original) */
struct node {
    char symbol[6];
    int  value;             /* SBIN(18) */
    struct node *l, *r;
    int  bf;                /* balance factor -1/0/+1 */
    int  abs_rel;           /* ABS_/REL_ */
    int  ext_loc;           /* EXT_/LOC_ */
    int  def_und;           /* DEF_/UND_ */
};

/* ------------------------------------------------------------------ globals */
static struct token *toks;          /* token list */
static int ntok, captok;
static struct token *T;             /* token currently being built/processed */
static char si[80];                 /* current source line (SI_BUFFER) */
static int comments;                /* COMMENTS field index 0..3 */
static int assembly_switch, title_found, jsys_processed;
static int location, start_address = 0777777, number_errors;
static char title_g[6] = {'n', 'u', 'l', 'l', ' ', ' '};
static struct node *root;

/* object unit accumulates here; flushed to file only if no errors */
static unsigned char *objbuf;
static size_t objlen, objcap;
static W oubuf[20];                 /* OU_BUFFER as 20 36-bit words */
static int count;                   /* COUNT: words/symbols in current record */
static int rec_addr;                /* address of first word in current record */

#define ERR(i) (T->errors |= (1ULL << (i)))

/* ----------------------------------------------------------- small helpers */
static int is_sp(const char *p, int n)
{
    for (int i = 0; i < n; i++) if (p[i] != ' ') return 0;
    return 1;
}
static void pad6(char *d, const char *s, int n)     /* copy <=6, blank pad */
{
    int i = 0, m = n < 6 ? n : 6;
    for (; i < m; i++) d[i] = s[i];
    for (; i < 6; i++) d[i] = ' ';
}
static void padn(char *d, int dn, const char *s, int n)  /* copy <=dn, pad */
{
    int i = 0, m = n < dn ? n : dn;
    for (; i < m; i++) d[i] = s[i];
    for (; i < dn; i++) d[i] = ' ';
}
static int eq6(const char *a, const char *b) { return memcmp(a, b, 6) == 0; }

/* CP-6 INDEX: 0-based offset of c in s[start..len), or len if not found. */
static int idx_ch(const char *s, int len, char c, int start)
{
    for (int i = start; i < len; i++) if (s[i] == c) return i;
    return len;
}

/* CHAR_TO_SBIN: optional +/- sign then base-`base` digits until a blank.
 * Returns 0 and *out on success, 1 (altret) on a bad digit. */
static int char_to_sbin(long *out, const char *s, int len, int base)
{
    int i = 0, sign = 1;
    long v = 0;
    if (len > 0 && s[0] == '-') { sign = -1; i = 1; }
    else if (len > 0 && s[0] == '+') { sign = 1; i = 1; }
    for (; i < len; i++) {
        int c = (unsigned char)s[i];
        if (c == ' ') break;                 /* trailing blank terminates */
        int d = c - '0';
        if (d < 0 || d >= base) { *out = 0; return 1; }
        v = v * base + d;
    }
    *out = sign * v;
    return 0;
}

/* VALIDATE_SYMBOL: first char A-Z, rest A-Z/0-9, a blank ends it (nothing
 * non-blank may follow).  Returns 0 valid, 1 invalid (altret). */
static int validate_symbol(const char *s)
{
    int t = (unsigned char)s[0];
    if (t < 65 || t > 90) return 1;
    for (int i = 1; i < 6; i++) {
        t = (unsigned char)s[i];
        if (t == 32) {
            for (int k = i; k < 6; k++) if (s[k] != ' ') return 1;
            return 0;
        }
        if ((t < 65 || t > 90) && (t < 48 || t > 57)) return 1;
    }
    return 0;
}

/* DECIMAL_TO_OCTAL: 18-bit value -> 6 octal digit chars (MSD first). */
static void oct6(char *d, int v)
{
    for (int i = 0; i < 6; i++) d[i] = '0' + ((v >> (15 - 3 * i)) & 7);
}

/* ------------------------------------------------------------- symbol tree */
static struct node *new_node(void)
{
    struct node *p = calloc(1, sizeof *p);
    if (!p) { fprintf(stderr, "asmdal: out of memory\n"); exit(2); }
    return p;
}

/* ADD_SYMBOL - AVL insert (Horowitz & Sahni), faithful to the original. */
static void add_symbol(const char *symbol, int value, int ext_loc,
                       int abs_rel, int def_und)
{
    struct node *Y, *A, *B, *C, *F, *P, *Q;
    int d;

    if (!root) {
        Y = new_node();
        memcpy(Y->symbol, symbol, 6);
        root = Y;
        Y->value = value; Y->ext_loc = ext_loc; Y->abs_rel = abs_rel;
        Y->def_und = def_und; Y->bf = 0; Y->l = Y->r = NULL;
        return;
    }
    /* Phase 1: locate, tracking the last unbalanced node A (parent F). */
    F = NULL; A = root; P = root; Q = NULL;
    while (P) {
        if (P->bf != 0) { A = P; F = Q; }
        if (memcmp(symbol, P->symbol, 6) < 0) { Q = P; P = P->l; }
        else if (memcmp(symbol, P->symbol, 6) > 0) { Q = P; P = P->r; }
        else {                                   /* already present */
            if (P->def_und == UND_) { P->def_und = def_und; P->value = value; }
            else if (ext_loc == EXT_) P->ext_loc = EXT_;
            else ERR(E_DUP_LABEL);
            return;
        }
    }
    /* Phase 2: insert new node Y as child of Q. */
    Y = new_node();
    memcpy(Y->symbol, symbol, 6);
    Y->value = value; Y->ext_loc = ext_loc; Y->abs_rel = abs_rel;
    Y->def_und = def_und; Y->l = Y->r = NULL; Y->bf = 0;
    if (memcmp(symbol, Q->symbol, 6) < 0) Q->l = Y; else Q->r = Y;
    /* set balance factors from A down to Y */
    if (memcmp(symbol, A->symbol, 6) > 0) { P = A->r; B = P; d = -1; }
    else { P = A->l; B = P; d = +1; }
    while (P != Y) {
        if (memcmp(symbol, P->symbol, 6) > 0) { P->bf = -1; P = P->r; }
        else { P->bf = +1; P = P->l; }
    }
    if (A->bf == 0) { A->bf = d; return; }
    if (A->bf + d == 0) { A->bf = 0; return; }
    if (d == +1) {
        if (B->bf == +1) {                       /* LL */
            A->l = B->r; B->r = A; A->bf = 0; B->bf = 0;
        } else {                                 /* LR */
            C = B->r; B->r = C->l; A->l = C->r; C->l = B; C->r = A;
            if (C->bf == +1) { A->bf = -1; B->bf = 0; }
            else if (C->bf == -1) { B->bf = +1; A->bf = 0; }
            else { B->bf = 0; A->bf = 0; }
            C->bf = 0; B = C;
        }
    } else {
        if (B->bf == -1) {                       /* RR */
            A->r = B->l; B->l = A; A->bf = 0; B->bf = 0;
        } else {                                 /* RL */
            C = B->l; B->l = C->r; A->r = C->l; C->r = B; C->l = A;
            if (C->bf == -1) { A->bf = +1; B->bf = 0; }
            else if (C->bf == +1) { B->bf = -1; A->bf = 0; }
            else { B->bf = 0; A->bf = 0; }
            C->bf = 0; B = C;
        }
    }
    if (!F) root = B;
    else if (A == F->l) F->l = B;
    else if (A == F->r) F->r = B;
}

/* LOOK_UP_SYMBOL: returns 0 and fills any non-NULL outs, or 1 if absent. */
static int look_up(const char *symbol, int *value, int *ext_loc,
                   int *abs_rel, int *def_und)
{
    struct node *p = root;
    while (p) {
        int c = memcmp(symbol, p->symbol, 6);
        if (c < 0) p = p->l;
        else if (c > 0) p = p->r;
        else {
            if (value) *value = p->value;
            if (ext_loc) *ext_loc = p->ext_loc;
            if (abs_rel) *abs_rel = p->abs_rel;
            if (def_und) *def_und = p->def_und;
            return 0;
        }
    }
    return 1;
}

/* CHANGE_SYMBOL_VALUE: returns 0 if changed, 1 if absent. */
static int change_value(const char *symbol, int value)
{
    struct node *p = root;
    while (p) {
        int c = memcmp(symbol, p->symbol, 6);
        if (c < 0) p = p->l;
        else if (c > 0) p = p->r;
        else { p->value = value; return 0; }
    }
    return 1;
}

/* ------------------------------------------------------------ object output */
static W mkword0(int type, int length, int address)
{
    return ((W)(type & 0777) << 27) | ((W)(length & 0777) << 18)
         | ((W)(address & M18));
}
static void emit_word(W w)              /* append one 36-bit word, 5 bytes BE */
{
    if (objlen + 5 > objcap) {
        objcap = objcap ? objcap * 2 : 4096;
        objbuf = realloc(objbuf, objcap);
        if (!objbuf) { fprintf(stderr, "asmdal: out of memory\n"); exit(2); }
    }
    objbuf[objlen++] = (w >> 32) & 0xFF;
    objbuf[objlen++] = (w >> 24) & 0xFF;
    objbuf[objlen++] = (w >> 16) & 0xFF;
    objbuf[objlen++] = (w >> 8) & 0xFF;
    objbuf[objlen++] = w & 0xFF;
}

/* PREPARE_OU_RECORD: stash a code word + its 2-bit relocation; flush at 18. */
static void prepare_ou_record(W ou_word, int reloc, int loc)
{
    if (count == 0) { rec_addr = loc; oubuf[1] = 0; }
    oubuf[1] |= ((W)(reloc & 3)) << (34 - 2 * count);
    oubuf[2 + count] = ou_word & M36;
    count++;
    if (count == 18) {
        count = 0;
        oubuf[0] = mkword0(0, 20, rec_addr);
        for (int i = 0; i < 20; i++) emit_word(oubuf[i]);
    }
}

/* WRITE_EXTERNALS: in-order walk emitting symbols whose ext_loc matches. */
static void write_externals(struct node *t, int ext_loc)
{
    if (!t) return;
    write_externals(t->l, ext_loc);
    if (t->ext_loc == ext_loc) {
        if (count == 0) oubuf[1] = 0;
        oubuf[1] |= ((W)(t->def_und ? 0 : 1)) << (32 - 4 * count);
        W wa = 0, wb = 0;
        for (int k = 0; k < 4; k++)
            wa |= ((W)(unsigned char)t->symbol[k]) << (27 - 9 * k);
        wb |= ((W)(unsigned char)t->symbol[4]) << 27;
        wb |= ((W)(unsigned char)t->symbol[5]) << 18;
        wb |= ((W)t->value & M18);
        oubuf[2 + 2 * count] = wa;
        oubuf[2 + 2 * count + 1] = wb;
        count++;
        if (count == 9) {
            count = 0;
            oubuf[0] = mkword0(ext_loc == EXT_ ? 1 : 3, 20, 0);
            for (int i = 0; i < 20; i++) emit_word(oubuf[i]);
        }
    }
    write_externals(t->r, ext_loc);
}

/* WRITE_REST_OF_FILE: tail text record, external/local symbol records, and
 * the trailer.  Called only when the assembly had no errors. */
static void write_rest_of_file(void)
{
    if (count != 0) {
        oubuf[0] = mkword0(0, count + 2, rec_addr);
        for (int i = 0; i < count + 2; i++) emit_word(oubuf[i]);
    }
    count = 0;
    write_externals(root, EXT_);
    if (count != 0) {
        oubuf[0] = mkword0(1, 2 + 2 * count, 0);
        for (int i = 0; i < 2 + 2 * count; i++) emit_word(oubuf[i]);
    }
    count = 0;
    write_externals(root, LOC_);
    if (count != 0) {
        oubuf[0] = mkword0(3, 2 + 2 * count, 0);
        for (int i = 0; i < 2 + 2 * count; i++) emit_word(oubuf[i]);
    }
    emit_word(mkword0(2, 1, start_address));        /* trailer */
}

/* ============================================================= PASS 1 ===== */
/* forward decls */
static void process_label(void);

static void process_comments(void)
{
    int I = idx_ch(si, 80, ';', 0);
    if (I < 7) {
        comments = 0;
        if (I != 0) ERR(E_BAD_SEMI);
    } else if (I < 15) {
        comments = 1;
        if (is_sp(si, 7)) comments = 0;
        if (I != 7) ERR(E_BAD_SEMI);
    } else if (I < 31) {
        comments = 2;
        if (is_sp(si, 15)) comments = 0;
        if (I != 15) ERR(E_BAD_SEMI);
    } else {
        comments = 3;
        if (is_sp(si, 31)) comments = 0;
        if (I != 31 && I != 80) ERR(E_BAD_SEMI);
    }
}

/* --- pass-1 pseudo-op handlers ------------------------------------------- */
static void block_1(void)
{
    long n;
    process_label();
    if (is_sp(si + 15, 16)) { ERR(E_NO_OPERAND); return; }
    if (char_to_sbin(&n, si + 15, 16, 10)) { ERR(E_BAD_DECIMAL); return; }
    if (n < 0) { ERR(E_BAD_OPERAND); return; }
    T->pseudo_word = (W)n & M36;
    T->reloc = 3;                       /* '11'B */
    T->no_code = 0;
    location += (int)n;
}
static void dc_1(void)
{
    process_label();
    T->no_code = 0;
    location += 1;
}
static void end_1(void)
{
    assembly_switch = 0;
    if (!is_sp(si, 6)) { ERR(E_CANT_LABEL); process_label(); }
    if (is_sp(si + 15, 16)) return;
    if (!is_sp(si + 21, 9)) { ERR(E_BAD_SYMBOL); return; }
    char sym[6]; pad6(sym, si + 15, 6);
    if (validate_symbol(sym) == 0) {
        int v;
        if (look_up(sym, &v, NULL, NULL, NULL) == 0) start_address = v;
        else ERR(E_NO_PREV_SYMB);
    } else ERR(E_BAD_SYMBOL);
}
static void entry_extern_1(void)        /* ENTRY and EXTERN are identical */
{
    if (!is_sp(si, 6)) { ERR(E_CANT_LABEL); process_label(); }
    if (is_sp(si + 15, 16)) { ERR(E_NO_OPERAND); return; }
    if (!is_sp(si + 21, 9)) { ERR(E_BAD_SYMBOL); return; }
    char sym[6]; pad6(sym, si + 15, 6);
    if (validate_symbol(sym)) { ERR(E_BAD_SYMBOL); return; }
    add_symbol(sym, 0, EXT_, REL_, UND_);
}
static void equ_1(void)
{
    long v;
    if (is_sp(si, 6)) { ERR(E_NO_LABEL); return; }
    char lab[6]; memcpy(lab, si, 6);
    if (validate_symbol(lab)) { ERR(E_BAD_LABEL); return; }
    if (!is_sp(si + 21, 9)) { ERR(E_BAD_SYMBOL); return; }
    if (char_to_sbin(&v, si + 15, 16, 10)) {            /* operand is a symbol */
        char sym[6]; pad6(sym, si + 15, 6);
        if (validate_symbol(sym)) { ERR(E_BAD_SYMBOL); return; }
        int vv;
        if (look_up(sym, &vv, NULL, NULL, NULL)) { ERR(E_NO_PREV_SYMB); return; }
        v = vv;
    }
    add_symbol(lab, (int)v, LOC_, ABS_, DEF_);
}
static void oct_1(void)
{
    long n = 0;
    process_label();
    if (is_sp(si + 15, 16)) ERR(E_NO_OPERAND);
    else if (char_to_sbin(&n, si + 15, 16, 8)) ERR(E_BAD_OCTAL);
    T->no_code = 0;
    T->pseudo_word = (W)n & M36;
    location += 1;
}
static void title_1(void)
{
    if (title_found) { ERR(E_MULT_TITLE); return; }
    assembly_switch = 1;
    title_found = 1;
    if (!is_sp(si, 6)) { ERR(E_CANT_LABEL); process_label(); }
    if (is_sp(si + 15, 16)) { ERR(E_NO_OPERAND); return; }
    if (!is_sp(si + 21, 9)) { ERR(E_BAD_SYMBOL); return; }
    char sym[6]; pad6(sym, si + 15, 6);
    if (validate_symbol(sym)) { ERR(E_BAD_SYMBOL); return; }
    add_symbol(sym, 0, EXT_, REL_, UND_);
    memcpy(title_g, sym, 6);
}
static void z_1(void)
{
    process_label();
    if (!is_sp(si + 15, 16)) ERR(E_CANT_OPERAND);
    T->no_code = 0;
    location += 1;
}

static void process_mnemonic(void)
{
    if (!assembly_switch && memcmp(si + 7, "TITLE ", 6) != 0) {
        T->no_code = 1; T->type = 3; ERR(E_NOT_TITLE); return;
    }
    if (comments < 2) { T->no_code = 1; T->type = 3; return; }
    if (si[6] != ' ' || (comments > 1 && !is_sp(si + 13, 2))) {
        T->no_code = 1; ERR(E_COLUMN); return;
    }
    if (is_sp(si + 7, 6)) { T->no_code = 1; ERR(E_NO_OPCODE); return; }

    jsys_processed = 0;

    int top = NOPC - 1, bot = 0;        /* binary search opcode table */
    while (bot <= top) {
        int mid = (top + bot) / 2, c = memcmp(si + 7, op_mnem[mid], 6);
        if (c > 0) bot = mid + 1;
        else if (c < 0) top = mid - 1;
        else { T->opcode = op_val[mid]; T->type = op_type[mid]; return; }
    }
    top = NJSYS - 1; bot = 0;           /* binary search JSYS table */
    while (bot <= top) {
        int mid = (top + bot) / 2, c = memcmp(si + 7, jsys_name[mid], 6);
        if (c > 0) bot = mid + 1;
        else if (c < 0) top = mid - 1;
        else {
            T->opcode = 0104; T->type = 2; T->jsys = jsys_num[mid];
            jsys_processed = 1; return;
        }
    }
    static const char *pn[9] = { "BLOCK ", "DC    ", "END   ", "ENTRY ",
                                 "EQU   ", "EXTERN", "OCT   ", "TITLE ", "Z     " };
    for (int i = 0; i < 9; i++)
        if (memcmp(si + 7, pn[i], 6) == 0) {
            T->type = 3; T->no_code = 1;
            switch (i) {
            case 0: block_1(); break;
            case 1: dc_1(); break;
            case 2: end_1(); break;
            case 3: entry_extern_1(); break;
            case 4: equ_1(); break;
            case 5: entry_extern_1(); break;
            case 6: oct_1(); break;
            case 7: title_1(); break;
            case 8: z_1(); break;
            }
            return;
        }
    ERR(E_BAD_OPCODE);
    T->no_code = 1;
}

static void process_label(void)
{
    if (comments == 0) return;
    if (is_sp(si, 6)) return;
    char lab[6]; memcpy(lab, si, 6);
    if (validate_symbol(lab)) { ERR(E_BAD_LABEL); return; }
    add_symbol(lab, location, LOC_, REL_, DEF_);
}

static void process_operand(void)
{
    const char *op = si + 15;           /* OPERAND CHAR(16) */
    if (T->type == 2) {                 /* JSYS */
        if (jsys_processed) {
            if (!is_sp(op, 16)) ERR(E_CANT_OPERAND);
        } else if (is_sp(op, 16)) ERR(E_NO_OPERAND);
        else {
            long vb;
            if (char_to_sbin(&vb, op, 16, 8) == 0) {
                if (vb < 0 || vb > (long)M18) ERR(E_BAD_JSYS);
                else T->jsys = (int)vb;
            } else ERR(E_BAD_OCTAL);
        }
        return;
    }
    int start = 0, I;
    char value[13];
    I = idx_ch(op, 16, ',', start);                  /* accumulator */
    if (I < 16) { pad6(T->acc, op + start, I - start); start = I + 1; }
    I = idx_ch(op, 16, '@', start);                  /* indirect */
    if (I < 16) { T->indirect = 1; start = I + 1; }
    I = idx_ch(op, 16, '(', start);                  /* value */
    padn(value, 13, op + start, I - start);
    if (I < 16) start = I + 1;
    I = idx_ch(op, 16, ')', start);                  /* index register */
    if (I < 16) { pad6(T->xr, op + start, I - start); start = I + 1; }

    int J = idx_ch(value, 13, '+', 0);               /* split value +/- bias */
    if (J < 13) {
        pad6(T->value, value, J);
        long vb;
        if (char_to_sbin(&vb, value + J, 13 - J, 10) == 0) T->value_bias = (int)vb;
        else { ERR(E_BAD_BIAS); return; }
    } else {
        J = idx_ch(value, 13, '-', 0);
        if (J < 13) {
            pad6(T->value, value, J);
            long vb;
            if (char_to_sbin(&vb, value + J, 13 - J, 10) == 0) T->value_bias = (int)vb;
            else { ERR(E_BAD_BIAS); return; }
        } else pad6(T->value, value, 13);
    }
}

/* ============================================================= PASS 2 ===== */
/* CHECK_SYMBOL_OR_NUMBER: resolve a field to an 18-bit value.
 * error_flag: 0 ok, 1 not-absolute, 2 undefined, 3 bad form.
 * reloc may be NULL (acc/xr/dev pass no relocation word). */
static void check_symbol_or_number(int *out, const char *symbol,
                                   int *error_flag, int *reloc)
{
    long n;
    *error_flag = 0;
    if (char_to_sbin(&n, symbol, 6, 10) == 0) { *out = (int)n; return; }
    if (validate_symbol(symbol)) {                  /* not a valid symbol */
        if (eq6(symbol, ".     ")) {
            *out = T->location;
            if (reloc) *reloc = 1;                  /* '01'B */
        } else { *out = 0; *error_flag = 3; }
        return;
    }
    int val, ext_loc, abs_rel;
    if (look_up(symbol, &val, &ext_loc, &abs_rel, NULL) == 0) {
        *out = val;
        if (abs_rel != ABS_) {
            *error_flag = 1;
            if (reloc && ext_loc == LOC_) *reloc = 1;       /* '01'B */
            else if (reloc) {
                *reloc = 2;                                  /* '10'B */
                change_value(symbol, T->location);
            }
        }
    } else { *out = 0; *error_flag = 2; }
}

static void prepare_ou_word(void)
{
    W w = 0;
    int sb, ef;
    switch (T->type) {
    case 0:                                          /* NORMAL */
        w |= ((W)(T->opcode & 0777)) << 27;
        check_symbol_or_number(&sb, T->acc, &ef, NULL);     /* accumulator */
        if (ef == 1) { sb = 0; ERR(E_ACC_NOT_ABS); }
        else if (ef == 2) ERR(E_ACC_UND);
        else if (ef == 3) ERR(E_ACC_BAD);
        if (sb < 0 || sb > 15) { ERR(E_INV_ACC); sb = 0; }
        w |= ((W)(sb & 017)) << 23;
        w |= ((W)(T->indirect & 1)) << 22;
        check_symbol_or_number(&sb, T->xr, &ef, NULL);      /* index register */
        if (ef == 1) { sb = 0; ERR(E_XR_NOT_ABS); }
        else if (ef == 2) ERR(E_XR_UND);
        else if (ef == 3) ERR(E_XR_BAD);
        if (sb < 0 || sb > 15) { ERR(E_INV_XR); sb = 0; }
        w |= ((W)(sb & 017)) << 18;
        {
            int rl = 0;
            check_symbol_or_number(&sb, T->value, &ef, &rl);   /* value */
            T->reloc = rl;
            if (ef == 2) ERR(E_VALUE_UND);
            else if (ef == 3) ERR(E_VALUE_BAD);
            if (T->value_bias != 0 && T->reloc == 2) ERR(E_EXPRESS_EXT);
            else w |= ((W)((sb + T->value_bias) & M18));
        }
        break;
    case 1:                                          /* I/O */
        w |= (W)7 << 33;
        if (is_sp(T->acc, 6)) ERR(E_NO_DEV);         /* DEV redefines ACC */
        check_symbol_or_number(&sb, T->acc, &ef, NULL);     /* device */
        if (ef == 1) { sb = 0; ERR(E_DEV_NOT_ABS); }
        else if (ef == 2) ERR(E_DEV_UND);
        else if (ef == 3) ERR(E_DEV_BAD);
        if (sb < 0 || sb > 127) { ERR(E_INV_DEV); sb = 0; }
        w |= ((W)(sb & 0177)) << 26;
        w |= ((W)(T->opcode & 7)) << 23;
        w |= ((W)(T->indirect & 1)) << 22;
        check_symbol_or_number(&sb, T->xr, &ef, NULL);      /* index register */
        if (ef == 1) { sb = 0; ERR(E_XR_NOT_ABS); }
        else if (ef == 2) ERR(E_XR_UND);
        else if (ef == 3) ERR(E_XR_BAD);
        if (sb < 0 || sb > 15) { ERR(E_INV_XR); sb = 0; }
        w |= ((W)(sb & 017)) << 18;
        {
            int rl = 0;
            check_symbol_or_number(&sb, T->value, &ef, &rl);
            T->reloc = rl;
            if (ef == 2) ERR(E_VALUE_UND);
            else if (ef == 3) ERR(E_VALUE_BAD);
            if (T->value_bias != 0 && T->reloc == 2) ERR(E_EXPRESS_EXT);
            else w |= ((W)((sb + T->value_bias) & M18));
        }
        break;
    case 2:                                          /* JSYS */
        w = ((W)0104 << 27) | ((W)T->jsys & M18);
        break;
    case 3: {                                        /* pseudo-op word */
        memcpy(si, T->source_line, 80);
        if (memcmp(si + 7, "DC    ", 6) == 0) {      /* DC resolved here */
            const char *op = si + 15;
            W half1 = 0, half2 = 0, pw = 0;
            int I = idx_ch(op, 16, ':', 0);
            if (I != 16) {                           /* split halves */
                long v;
                if (char_to_sbin(&v, op, I, 10) == 0) {
                    if (v > (long)M18) ERR(E_DC1_TOO_BIG);
                    else half1 = (W)v & M18;
                } else ERR(E_DC1_NOT_NUM);
                if (char_to_sbin(&v, op + I + 1, 16 - (I + 1), 10) == 0) {
                    if (v > (long)M18) ERR(E_DC2_TOO_BIG);
                    else half2 = (W)v & M18;
                } else {
                    char sym[6]; pad6(sym, op + I + 1, 16 - (I + 1));
                    if (validate_symbol(sym) == 0) {
                        int val, el, ar, du;
                        if (look_up(sym, &val, &el, &ar, &du) == 0) {
                            half2 = (W)val & M18;
                            if (ar == REL_ && el == EXT_ && du == UND_) {
                                T->reloc = 2; change_value(sym, T->location);
                            } else T->reloc = (ar == ABS_) ? 0 : 1;
                        } else ERR(E_NO_SYMB);
                    } else ERR(E_BAD_SYMBOL);
                }
                pw = (half1 << 18) | half2;
            } else {                                 /* single word */
                long v;
                if (char_to_sbin(&v, op, 16, 10) != 0) {
                    char sym[6]; pad6(sym, op, 6);
                    if (validate_symbol(sym) == 0) {
                        int val, el, ar, du;
                        if (look_up(sym, &val, &el, &ar, &du) == 0) {
                            pw = (W)val & M36;
                            if (ar == REL_ && el == EXT_ && du == UND_) {
                                T->reloc = 2; change_value(sym, T->location);
                            } else T->reloc = (ar == ABS_) ? 0 : 1;
                        } else ERR(E_NO_SYMB);
                    } else ERR(E_BAD_SYMBOL);
                } else pw = (W)v & M36;
            }
            T->pseudo_word = pw & M36;
        }
        w = T->pseudo_word & M36;
        break;
    }
    }
    T->ou_word = w & M36;
}

/* --------------------------------------------------------------- listing -- */
static FILE *lst;
static int line_count = 0777, page_count = 0;
#define MAX_LINES 51

static void headings(void)
{
    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    char buf[64];
    line_count = 0;
    page_count++;
    fprintf(lst, "\fASMDAL.A01   Title=%.6s   ", title_g);
    strftime(buf, sizeof buf, "%a %d-%b-%y %H:%M:%S", tm);
    fprintf(lst, "%s   Page=%2d\n\n", buf, page_count);
    fprintf(lst, "Location  Machine text    Source line\n\n");
}

static void print_errors(void)
{
    int found = 0;
    for (int i = 0; i < NERR; i++)
        if (T->errors & (1ULL << i)) {
            number_errors++;
            found = 1;
            if (line_count > MAX_LINES) headings();
            fprintf(lst, "                          ** %s\n", err_msg[i]);
            line_count++;
        }
    if (found && line_count <= MAX_LINES) { fprintf(lst, "\n"); line_count++; }
}

static void print_lo_line(void)
{
    char loc[6], c1[6], c2[6];
    oct6(loc, T->location);
    if (line_count > MAX_LINES) headings();
    if (T->no_code)
        fprintf(lst, " %.6s                 %.80s\n", loc, T->source_line);
    else {
        oct6(c1, (int)((T->ou_word >> 18) & M18));
        oct6(c2, (int)(T->ou_word & M18));
        fprintf(lst, " %.6s   %.6s %.6s   %.80s\n", loc, c1, c2, T->source_line);
    }
    line_count++;
    print_errors();
}

static void print_end_line(void)
{
    fprintf(lst, "\n");
    if (number_errors == 0)
        fprintf(lst, "           no errors encountered in procedure %.6s\n", title_g);
    else
        fprintf(lst, "          %3d errors encountered in procedure %.6s\n",
                number_errors, title_g);
}

static void in_order(struct node *t)    /* symbol table listing, sorted */
{
    if (!t) return;
    in_order(t->l);
    char v[6]; oct6(v, t->value);
    fprintf(lst, "%.6s   %s/%s/%s   %.6s\n", t->symbol,
            t->ext_loc ? "Ext" : "Loc", t->abs_rel ? "Abs" : "Rel",
            t->def_und ? "Def" : "Und", v);
    in_order(t->r);
}

/* =============================================================== driver === */
static struct token *next_token(void)
{
    if (ntok == captok) {
        captok = captok ? captok * 2 : 256;
        toks = realloc(toks, captok * sizeof *toks);
        if (!toks) { fprintf(stderr, "asmdal: out of memory\n"); exit(2); }
    }
    struct token *t = &toks[ntok++];
    memset(t, 0, sizeof *t);
    memset(t->value, ' ', 6);
    memset(t->acc, ' ', 6);
    memset(t->xr, ' ', 6);
    memset(t->source_line, ' ', 80);
    return t;
}

static void pass_1(FILE *src)
{
    char line[512];
    location = 0;
    while (fgets(line, sizeof line, src)) {
        int n = (int)strlen(line);
        while (n > 0 && (line[n - 1] == '\n' || line[n - 1] == '\r')) n--;
        memset(si, ' ', 80);
        for (int i = 0; i < n && i < 80; i++) si[i] = line[i];
        T = next_token();
        memcpy(T->source_line, si, 80);
        T->location = location;
        process_comments();
        process_mnemonic();
        if (T->type != 3) {
            process_label();
            if (!T->no_code) { process_operand(); location++; }
        }
    }
    if (assembly_switch) {               /* synthesize an END line */
        T = next_token();
        memcpy(T->source_line, "       END", 10);
        T->location = location;
        T->errors |= (1ULL << E_NO_END);
        T->no_code = 1; T->type = 3;
    }
}

static void pass_2(void)
{
    location = 0;
    for (int i = 0; i < ntok; i++) {
        T = &toks[i];
        if (!T->no_code) {
            prepare_ou_word();
            prepare_ou_record(T->ou_word, T->reloc, T->location);
        }
        print_lo_line();
    }
    print_end_line();
    if (number_errors == 0) write_rest_of_file();
    else
        fprintf(lst, "          No code generated for procedure %.6s\n", title_g);
    fprintf(lst, "\fSymbol      Type       Value\n");
    in_order(root);
}

static char *derive(const char *src, const char *ext)
{
    const char *slash = strrchr(src, '/');
    const char *base = slash ? slash + 1 : src;
    const char *dot = strrchr(base, '.');
    size_t n = dot ? (size_t)(dot - src) : strlen(src);
    char *out = malloc(n + strlen(ext) + 1);
    memcpy(out, src, n);
    strcpy(out + n, ext);
    return out;
}

int main(int argc, char **argv)
{
    const char *srcname = NULL, *objname = NULL, *lstname = NULL;
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-o") && i + 1 < argc) objname = argv[++i];
        else if (!strcmp(argv[i], "-l") && i + 1 < argc) lstname = argv[++i];
        else if (!srcname) srcname = argv[i];
        else if (!objname) objname = argv[i];
        else if (!lstname) lstname = argv[i];
    }
    if (!srcname) {
        fprintf(stderr, "usage: asmdal source [object [listing]] "
                        "[-o object] [-l listing]\n");
        return 2;
    }
    FILE *src = fopen(srcname, "r");
    if (!src) { fprintf(stderr, "asmdal: cannot open %s\n", srcname); return 2; }

    char *dobj = NULL, *dlst = NULL;
    if (!objname) objname = dobj = derive(srcname, ".obj");
    if (!lstname) lstname = dlst = derive(srcname, ".lst");
    lst = fopen(lstname, "w");
    if (!lst) { fprintf(stderr, "asmdal: cannot create %s\n", lstname); return 2; }

    /* salutation to the terminal (M$ME) */
    {
        time_t now = time(NULL);
        char buf[32];
        strftime(buf, sizeof buf, "%H:%M %d-%b-%y", localtime(&now));
        fprintf(stderr, "ASMDAL A01 here at %s\n", buf);
    }

    pass_1(src);
    fclose(src);
    pass_2();

    if (number_errors == 0) {
        FILE *of = fopen(objname, "wb");
        if (!of) { fprintf(stderr, "asmdal: cannot create %s\n", objname); return 2; }
        fwrite(objbuf, 1, objlen, of);
        fclose(of);
    }
    fclose(lst);
    free(dobj); free(dlst);
    return number_errors ? 1 : 0;
}
