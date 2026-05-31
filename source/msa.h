/* msa.h - shared interface between the MSA disassembler engine
 * (msa_engine.c) and the per-CPU decoders (msa6800.c, msa8085.c, ...).
 * The engine owns the 64K memory, the object-unit loader, the flow tracer
 * and the source emitter; each CPU provides decode(). */
#ifndef MSA_H
#define MSA_H

#define RESV 256                 /* memory cell value for reserved (BSS) */

extern int val[65536];           /* 0..255 data, 256 reserved, -1 absent */
extern unsigned char some[65536], rd[65536], wr[65536], jmp[65536];
extern unsigned char jsr[65536], exec[65536], visit[65536], bno[65536];

enum { FALLTHRU, BR_COND, BR_UNCOND, CALL_, STOP };
struct ins {
    int  len;        /* instruction length */
    int  flow;       /* FALLTHRU/BR_COND/BR_UNCOND/CALL_/STOP */
    int  target;     /* branch/call target, or -1 */
    int  rdref;      /* read  data-ref address, or -1 */
    int  wrref;      /* write data-ref address, or -1 */
    char text[80];   /* "MNEMONIC operands" (re-assemblable) */
};

struct ins decode(int a);        /* provided by the per-CPU decoder */

const char *symat(int v);        /* symbol name defined at address v, or 0 */
void lbl(char *b, int a);        /* label operand: symbol name or Lxxxx */
int  byte(int a);                /* memory byte (0 if absent/reserved) */
int  word(int a);                /* little-endian 16-bit at a (a, a+1) */

#endif
