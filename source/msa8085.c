/*
 * msa8085 - Intel 8080/8085 reverse-assembler decode, a C port of
 * MSA8085_SI6 (the per-CPU half).  Links with msa_engine.c.  Little-endian.
 * Validated by disassembling hand-built object records.
 */

#include "msa.h"
#include <stdio.h>
#include <string.h>

static const char *R[8] = { "B", "C", "D", "E", "H", "L", "M", "A" };
static const char *RP[4] = { "B", "D", "H", "SP" };
static const char *RP3[4] = { "B", "D", "H", "PSW" };
static const char *ALU[8]
    = { "ADD", "ADC", "SUB", "SBB", "ANA", "XRA", "ORA", "CMP" };
/* 0x00-0x3F */
static const char *TLO[64]
    = { "NOP", "LXI", "STAX", "INX", "INR", "DCR", "MVI", "RLC",
        "..",  "DAD", "LDAX", "DCX", "INR", "DCR", "MVI", "RRC",
        "..",  "LXI", "STAX", "INX", "INR", "DCR", "MVI", "RAL",
        "..",  "DAD", "LDAX", "DCX", "INR", "DCR", "MVI", "RAR",
        "RIM", "LXI", "SHLD", "INX", "INR", "DCR", "MVI", "DAA",
        "..",  "DAD", "LHLD", "DCX", "INR", "DCR", "MVI", "CMA",
        "SIM", "LXI", "STA",  "INX", "INR", "DCR", "MVI", "STC",
        "..",  "DAD", "LDA",  "DCX", "INR", "DCR", "MVI", "CMC" };
/* 0xC0-0xFF */
static const char *THI[64]
    = { "RNZ", "POP",  "JNZ", "JMP",  "CNZ", "PUSH", "ADI", "RST",
        "RZ",  "RET",  "JZ",  "..",   "CZ",  "CALL", "ACI", "RST",
        "RNC", "POP",  "JNC", "OUT",  "CNC", "PUSH", "SUI", "RST",
        "RC",  "..",   "JC",  "IN",   "CC",  "..",   "SBI", "RST",
        "RPO", "POP",  "JPO", "XTHL", "CPO", "PUSH", "ANI", "RST",
        "RPE", "PCHL", "JPE", "XCHG", "CPE", "..",   "XRI", "RST",
        "RP",  "POP",  "JP",  "DI",   "CP",  "PUSH", "ORI", "RST",
        "RM",  "SPHL", "JM",  "EI",   "CM",  "..",   "CPI", "RST" };

static void
put (char *t, const char *m, const char *o)
{
  if (o && o[0])
    {
      sprintf (t, "%-8s %s", m, o);
    }
  else
    {
      strcpy (t, m);
    }
}

struct ins
decode (int a)
{
  struct ins in;
  int op = byte (a), hi = op >> 6, lo = op & 15, rp = (op >> 4) & 3;
  char o[24];

  in.len = 1;
  in.flow = FALLTHRU;
  in.target = in.rdref = in.wrref = -1;
  in.text[0] = 0;
  o[0] = 0;
  if (hi == 1)
    { /* 40-7F: MOV r,r' / HLT */
      if (op == 0x76)
        {
          strcpy (in.text, "HLT");
          in.flow = STOP;
        }
      else
        {
          sprintf (o, "%s,%s", R[(op >> 3) & 7], R[op & 7]);
          put (in.text, "MOV", o);
        }

      return in;
    }

  if (hi == 2)
    { /* 80-BF: ALU A,r */
      put (in.text, ALU[(op >> 3) & 7], R[op & 7]);
      return in;
    }

  if (hi == 0)
    { /* 00-3F */
      const char *m = TLO[op];
      if (m[0] == '.')
        {
          sprintf (in.text, "DB       $%02X", op);
          return in;
        }

      switch (lo)
        {
        case 1:
          in.len = 3;
          {
            char lb[16];
            sprintf (lb, "$%04X", word (a + 1));
            sprintf (o, "%s,%s", RP[rp], lb);
          }
          put (in.text, m, o);
          break;

        case 2:
        case 10:
          if (rp < 2)
            {
              put (in.text, m, RP[rp]); /* STAX/LDAX B|D */
            }
          else
            {
              char lb[16];
              int w = word (a + 1);
              in.len = 3;
              lbl (lb, w);
              if (op == 0x22 || op == 0x32)
                {
                  in.wrref = w;
                }
              else
                {
                  in.rdref = w;
                }

              put (in.text, m, lb);
            }

          break;

        case 3:
        case 9:
        case 11:
          put (in.text, m, RP[rp]);
          break; /* INX/DAD/DCX */

        case 4:
        case 5:
        case 12:
        case 13:
          put (in.text, m, R[(op >> 3) & 7]);
          break; /* INR/DCR */

        case 6:
        case 14:
          in.len = 2;
          sprintf (o, "%s,$%02X", R[(op >> 3) & 7], byte (a + 1));
          put (in.text, m, o);
          break;

        default:
          strcpy (in.text, m); /* 0,7,8,15: no operand */
        }
      return in;
    }

  /* hi == 3: C0-FF */
  {
    const char *m = THI[op - 0xC0];
    if (m[0] == '.')
      {
        sprintf (in.text, "DB       $%02X", op);
        return in;
      }

    switch (lo)
      {
      case 1:
      case 5:
        put (in.text, m, RP3[rp]);
        break; /* POP/PUSH */

      case 2:
      case 10:
        in.len = 3;
        {
          char lb[16];
          int w = word (a + 1);
          in.target = w;
          in.flow = BR_COND;
          lbl (lb, w);
          put (in.text, m, lb);
        }
        break; /* Jcc */

      case 4:
      case 12:
        in.len = 3;
        {
          char lb[16];
          int w = word (a + 1);
          in.target = w;
          in.flow = CALL_;
          lbl (lb, w);
          put (in.text, m, lb);
        }
        break; /* Ccc */

      case 6:
      case 14:
        in.len = 2;
        sprintf (o, "$%02X", byte (a + 1));
        put (in.text, m, o);
        break; /* ALU imm */

      case 7:
      case 15:
        in.target = ((op >> 3) & 7) * 8;
        in.flow = CALL_;
        sprintf (o, "%d", (op >> 3) & 7);
        put (in.text, m, o);
        break; /* RST */

      case 3:
      case 9:
      case 11:
      case 13:
        if (op == 0xC3)
          {
            in.len = 3;
            char lb[16];
            int w = word (a + 1);
            in.target = w;
            in.flow = BR_UNCOND;
            lbl (lb, w);
            put (in.text, m, lb);
          }
        else if (op == 0xCD)
          {
            in.len = 3;
            char lb[16];
            int w = word (a + 1);
            in.target = w;
            in.flow = CALL_;
            lbl (lb, w);
            put (in.text, m, lb);
          }
        else if (op == 0xD3 || op == 0xDB)
          {
            in.len = 2;
            sprintf (o, "$%02X", byte (a + 1));
            put (in.text, m, o);
          } /* OUT/IN */
        else
          {
            strcpy (in.text, m);
            if (op == 0xC9 || op == 0xE9)
              {
                in.flow = STOP;
              }
          } /* RET/PCHL stop; others bare */

        break;

      default:
        strcpy (in.text, m);
        break; /* 0,8: conditional RET (continues) */
      }
  }
  return in;
}
