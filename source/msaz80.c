/* msaz80 - Z80 reverse-assembler decode (the per-CPU half), a C port of the
 * PL/6 MSAZ80_SI6.  Links with msa_engine.c (shared load/trace/emit).
 * asmz80 -> .obj -> msaz80 -> source -> asmz80 round-trips.  See
 * MSAZ80_NOTES.md for the reverse-engineering. */
#include <stdio.h>
#include <string.h>
#include "msa.h"

static const char *R[8]  = {"B","C","D","E","H","L","(HL)","A"};
static const char *RP[4] = {"BC","DE","HL","SP"};
static const char *RP2[4]= {"BC","DE","HL","AF"};
static const char *CC[8] = {"NZ","Z","NC","C","PO","PE","P","M"};
static const char *ALU[8]= {"ADD","ADC","SUB","SBC","AND","XOR","OR","CP"};
static const char *ROT[8]= {"RLC","RRC","RL","RR","SLA","SRA","SLL","SRL"};

struct ins decode(int a)
{
    struct ins in;
    int op = byte(a), x = op >> 6, y = (op >> 3) & 7, z = op & 7, p = y >> 1, q = y & 1;
    char o1[16], o2[16];
    in.len = 1; in.flow = FALLTHRU; in.target = in.rdref = in.wrref = -1;
    in.text[0] = 0;
    if (op == 0xCB) {                                   /* ---- CB prefix */
        int o = byte(a+1); int xx=o>>6, yy=(o>>3)&7, zz=o&7;
        in.len = 2;
        if (xx == 0) sprintf(in.text, "%-8s %s", ROT[yy], R[zz]);
        else sprintf(in.text, "%-8s %d,%s", xx==1?"BIT":xx==2?"RES":"SET", yy, R[zz]);
        return in;
    }
    if (op == 0xED) {                                   /* ---- ED prefix */
        int o = byte(a+1), yy=(o>>3)&7, pp=yy>>1, qq=yy&1;
        in.len = 2;
        if (o == 0x44) strcpy(in.text, "NEG");
        else if (o == 0x45) { strcpy(in.text, "RETN"); in.flow = STOP; }
        else if (o == 0x4D) { strcpy(in.text, "RETI"); in.flow = STOP; }
        else if (o == 0x46) strcpy(in.text, "IM       0");
        else if (o == 0x56) strcpy(in.text, "IM       1");
        else if (o == 0x5E) strcpy(in.text, "IM       2");
        else if (o == 0x47) strcpy(in.text, "LD       I,A");
        else if (o == 0x4F) strcpy(in.text, "LD       R,A");
        else if (o == 0x57) strcpy(in.text, "LD       A,I");
        else if (o == 0x5F) strcpy(in.text, "LD       A,R");
        else if (o == 0x67) strcpy(in.text, "RRD");
        else if (o == 0x6F) strcpy(in.text, "RLD");
        else if ((o & 0xC7) == 0x40) sprintf(in.text, "IN       %s,(C)", R[yy]);
        else if ((o & 0xC7) == 0x41) sprintf(in.text, "OUT      (C),%s", R[yy]);
        else if ((o & 0xCF) == 0x42) sprintf(in.text, "SBC      HL,%s", RP[pp]);
        else if ((o & 0xCF) == 0x4A) sprintf(in.text, "ADC      HL,%s", RP[pp]);
        else if ((o & 0xCF) == 0x43) { lbl(o1, word(a+2)); sprintf(in.text,"LD       (%s),%s",o1,RP[pp]); in.len=4; in.wrref=word(a+2); }
        else if ((o & 0xCF) == 0x4B) { lbl(o1, word(a+2)); sprintf(in.text,"LD       %s,(%s)",RP[pp],o1); in.len=4; in.rdref=word(a+2); }
        else if (o >= 0xA0 && (o & 4) == 0) {           /* block ops */
            static const char *bl[4][4] = {
                {"LDI","CPI","INI","OUTI"},{"LDD","CPD","IND","OUTD"},
                {"LDIR","CPIR","INIR","OTIR"},{"LDDR","CPDR","INDR","OTDR"}};
            strcpy(in.text, bl[(o>>3)&3][o&3]);
        }
        else strcpy(in.text, "DB       $ED");           /* undecoded ED */
        (void)qq;
        return in;
    }
    if (op == 0xDD || op == 0xFD) {                     /* ---- IX / IY */
        const char *ix = (op == 0xDD) ? "IX" : "IY";
        int o = byte(a+1);
        in.len = 2;
        if (o == 0x21) { sprintf(in.text,"LD       %s,$%04X",ix,word(a+2)); in.len=4; }
        else if (o == 0x22) { lbl(o1,word(a+2)); sprintf(in.text,"LD       (%s),%s",o1,ix); in.len=4; in.wrref=word(a+2); }
        else if (o == 0x2A) { lbl(o1,word(a+2)); sprintf(in.text,"LD       %s,(%s)",ix,o1); in.len=4; in.rdref=word(a+2); }
        else if (o == 0x23) sprintf(in.text,"INC      %s",ix);
        else if (o == 0x2B) sprintf(in.text,"DEC      %s",ix);
        else if (o == 0xE5) sprintf(in.text,"PUSH     %s",ix);
        else if (o == 0xE1) sprintf(in.text,"POP      %s",ix);
        else if (o == 0xE9) { sprintf(in.text,"JP       (%s)",ix); in.flow=STOP; }
        else if (o == 0xF9) sprintf(in.text,"LD       SP,%s",ix);
        else if (o == 0xE3) sprintf(in.text,"EX       (SP),%s",ix);
        else if ((o & 0xC7) == 0x46 && o != 0x76) { int d=byte(a+2); sprintf(in.text,"LD       %s,(%s%+d)",R[(o>>3)&7],ix,(signed char)d); in.len=3; }
        else if ((o & 0xF8) == 0x70) { int d=byte(a+2); sprintf(in.text,"LD       (%s%+d),%s",ix,(signed char)d,R[o&7]); in.len=3; }
        else if (o == 0x34) { int d=byte(a+2); sprintf(in.text,"INC      (%s%+d)",ix,(signed char)d); in.len=3; }
        else if (o == 0x35) { int d=byte(a+2); sprintf(in.text,"DEC      (%s%+d)",ix,(signed char)d); in.len=3; }
        else if (o == 0x36) { int d=byte(a+2),n=byte(a+3); sprintf(in.text,"LD       (%s%+d),$%02X",ix,(signed char)d,n); in.len=4; }
        else if ((o & 0xC0) == 0x80 && (o & 7) == 6) { int d=byte(a+2); sprintf(in.text,"%-8s (%s%+d)",ALU[(o>>3)&7],ix,(signed char)d); in.len=3; }
        else if (o == 0xCB) { int d=byte(a+2),o2b=byte(a+3),xx=o2b>>6,yy=(o2b>>3)&7; in.len=4;
            if (xx==0) sprintf(in.text,"%-8s (%s%+d)",ROT[yy],ix,(signed char)d);
            else sprintf(in.text,"%-8s %d,(%s%+d)",xx==1?"BIT":xx==2?"RES":"SET",yy,ix,(signed char)d); }
        else { sprintf(in.text,"DB       $%02X",op); in.len=1; }   /* not an IX/IY form */
        return in;
    }
    /* ---- main page (no prefix) */
    switch (x) {
    case 0:
        switch (z) {
        case 0:
            if (y==0) strcpy(in.text,"NOP");
            else if (y==1) strcpy(in.text,"EX       AF,AF'");
            else if (y==2) { int d=byte(a+1); in.len=2; in.target=(a+2+(signed char)d)&0xFFFF; in.flow=BR_COND; lbl(o1,in.target); sprintf(in.text,"DJNZ     %s",o1); }
            else if (y==3) { int d=byte(a+1); in.len=2; in.target=(a+2+(signed char)d)&0xFFFF; in.flow=BR_UNCOND; lbl(o1,in.target); sprintf(in.text,"JR       %s",o1); }
            else { int d=byte(a+1); in.len=2; in.target=(a+2+(signed char)d)&0xFFFF; in.flow=BR_COND; lbl(o1,in.target); sprintf(in.text,"JR       %s,%s",CC[y-4],o1); }
            break;
        case 1:
            if (q==0) { in.len=3; sprintf(in.text,"LD       %s,$%04X",RP[p],word(a+1)); }
            else sprintf(in.text,"ADD      HL,%s",RP[p]);
            break;
        case 2:
            if (q==0) { if (p==0) strcpy(in.text,"LD       (BC),A"); else if (p==1) strcpy(in.text,"LD       (DE),A");
                        else { in.len=3; lbl(o1,word(a+1)); in.wrref=word(a+1); sprintf(in.text, p==2?"LD       (%s),HL":"LD       (%s),A",o1); } }
            else { if (p==0) strcpy(in.text,"LD       A,(BC)"); else if (p==1) strcpy(in.text,"LD       A,(DE)");
                   else { in.len=3; lbl(o1,word(a+1)); in.rdref=word(a+1); sprintf(in.text, p==2?"LD       HL,(%s)":"LD       A,(%s)",o1); } }
            break;
        case 3: sprintf(in.text,"%-8s %s",q==0?"INC":"DEC",RP[p]); break;
        case 4: sprintf(in.text,"INC      %s",R[y]); break;
        case 5: sprintf(in.text,"DEC      %s",R[y]); break;
        case 6: in.len=2; sprintf(in.text,"LD       %s,$%02X",R[y],byte(a+1)); break;
        case 7: { static const char *sp[8]={"RLCA","RRCA","RLA","RRA","DAA","CPL","SCF","CCF"}; strcpy(in.text,sp[y]); } break;
        }
        break;
    case 1:
        if (op==0x76) { strcpy(in.text,"HALT"); in.flow=STOP; }
        else sprintf(in.text,"LD       %s,%s",R[y],R[z]);
        break;
    case 2:
        sprintf(in.text,"%-8s %s",ALU[y],R[z]);
        break;
    case 3:
        switch (z) {
        case 0: sprintf(in.text,"RET      %s",CC[y]); break;           /* cond ret: continues */
        case 1:
            if (q==0) sprintf(in.text,"POP      %s",RP2[p]);
            else if (p==0) { strcpy(in.text,"RET"); in.flow=STOP; }
            else if (p==1) strcpy(in.text,"EXX");
            else if (p==2) { strcpy(in.text,"JP       (HL)"); in.flow=STOP; }
            else strcpy(in.text,"LD       SP,HL");
            break;
        case 2: in.len=3; in.target=word(a+1); in.flow=BR_COND; lbl(o1,in.target); sprintf(in.text,"JP       %s,%s",CC[y],o1); break;
        case 3:
            if (y==0) { in.len=3; in.target=word(a+1); in.flow=BR_UNCOND; lbl(o1,in.target); sprintf(in.text,"JP       %s",o1); }
            else if (y==1) { in.len=2; sprintf(in.text,"DB       $CB"); }   /* CB handled above */
            else if (y==2) { in.len=2; sprintf(in.text,"OUT      ($%02X),A",byte(a+1)); }
            else if (y==3) { in.len=2; sprintf(in.text,"IN       A,($%02X)",byte(a+1)); }
            else if (y==4) strcpy(in.text,"EX       (SP),HL");
            else if (y==5) strcpy(in.text,"EX       DE,HL");
            else if (y==6) strcpy(in.text,"DI");
            else strcpy(in.text,"EI");
            break;
        case 4: in.len=3; in.target=word(a+1); in.flow=CALL_; lbl(o1,in.target); sprintf(in.text,"CALL     %s,%s",CC[y],o1); break;
        case 5:
            if (q==0) sprintf(in.text,"PUSH     %s",RP2[p]);
            else if (p==0) { in.len=3; in.target=word(a+1); in.flow=CALL_; lbl(o1,in.target); sprintf(in.text,"CALL     %s",o1); }
            else in.len = 1;   /* DD/ED/FD handled above */
            break;
        case 6: in.len=2; sprintf(in.text,"%-8s $%02X",ALU[y],byte(a+1)); break;
        case 7: in.target=y*8; in.flow=CALL_; sprintf(in.text,"RST      %d",y*8); break;
        }
        break;
    }
    (void)o2;
    return in;
}
