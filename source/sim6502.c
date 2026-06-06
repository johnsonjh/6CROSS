/*
 * sim6502 - a small, self-contained MOS 6502 emulator for running and
 * testing programs produced by the ported ASM6502 assembler.
 *
 * It loads a raw binary at a load address, sets PC there, and executes
 * until a BRK (treated as halt) or an instruction-count limit.  Writes to
 * an optional output port are echoed to stdout; a memory byte can be
 * dumped at exit for test assertions.
 *
 *   sim6502 file.bin [-l load] [-d dumpaddr] [-o outport] [-n maxinsns]
 *
 * Addresses accept 0x.., $.. or decimal.  The full documented NMOS 6502
 * instruction set is implemented (binary arithmetic; decimal mode is not
 * emulated).  This file is original and may be used freely.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint8_t mem[65536];
static uint8_t A, X, Y, SP, P;
static uint16_t PC;
static int outport = -1;

#define FC 0x01u
#define FZ 0x02u
#define FI 0x04u
#define FD 0x08u
#define FB 0x10u
#define FU 0x20u
#define FV 0x40u
#define FN 0x80u

static uint8_t
rd (uint16_t a)
{
  return mem[a];
}
static void
wr (uint16_t a, uint8_t v)
{
  if (outport >= 0 && a == (uint16_t)outport)
    {
      putchar (v);
      fflush (stdout);
      return;
    }

  mem[a] = v;
}
static uint8_t
rb (void)
{
  return rd (PC++);
}
static uint16_t
rw (void)
{
  uint8_t lo = rb ();
  uint8_t hi = rb ();

  return lo | (hi << 8);
}
static void
push (uint8_t v)
{
  wr (0x100 + SP, v);
  SP--;
}
static uint8_t
pop (void)
{
  SP++;
  return rd (0x100 + SP);
}
static void
setNZ (uint8_t v)
{
  P = (P & ~(FN | FZ)) | (v & FN) | (v ? 0 : FZ);
}
static void
setC (int c)
{
  P = c ? (P | FC) : (P & ~FC);
}

/* effective-address helpers (advance PC past the operand) */
static uint16_t
a_zp (void)
{
  return rb ();
}
static uint16_t
a_zpx (void)
{
  return (uint8_t)(rb () + X);
}
static uint16_t
a_zpy (void)
{
  return (uint8_t)(rb () + Y);
}
static uint16_t
a_abs (void)
{
  return rw ();
}
static uint16_t
a_absx (void)
{
  return rw () + X;
}
static uint16_t
a_absy (void)
{
  return rw () + Y;
}
static uint16_t
a_indx (void)
{
  uint8_t z = rb () + X;

  return rd (z) | (rd ((uint8_t)(z + 1)) << 8);
}
static uint16_t
a_indy (void)
{
  uint8_t z = rb ();

  return (rd (z) | (rd ((uint8_t)(z + 1)) << 8)) + Y;
}

static void
adc (uint8_t m)
{
  unsigned t = A + m + (P & FC);

  P = (P & ~FV) | ((~(A ^ m) & (A ^ t) & 0x80) ? FV : 0);
  setC (t > 0xFF);
  A = t & 0xFF;
  setNZ (A);
}
static void
cmp (uint8_t r, uint8_t m)
{
  unsigned t = r - m;

  setC (r >= m);
  setNZ (t & 0xFF);
}
static uint8_t
asl (uint8_t v)
{
  setC (v & 0x80);
  v <<= 1;
  setNZ (v);
  return v;
}
static uint8_t
lsr (uint8_t v)
{
  setC (v & 0x01);
  v >>= 1;
  setNZ (v);
  return v;
}
static uint8_t
rol (uint8_t v)
{
  int c = P & FC;

  setC (v & 0x80);
  v = (v << 1) | c;
  setNZ (v);
  return v;
}
static uint8_t
ror (uint8_t v)
{
  int c = P & FC;

  setC (v & 0x01);
  v = (v >> 1) | (c ? 0x80 : 0);
  setNZ (v);
  return v;
}
static void
br (int cond)
{
  int8_t d = (int8_t)rb ();

  if (cond)
    {
      PC += d;
    }
}

/* run one instruction; return 0 to keep going, 1 on BRK/halt */
static int
step (void)
{
  uint8_t op = rb ();
  uint16_t a;
  uint8_t m;

  switch (op)
    {
    /* ---- load / store ---- */
    case 0xA9:
      A = rb ();
      setNZ (A);
      break;

    case 0xA5:
      A = rd (a_zp ());
      setNZ (A);
      break;

    case 0xB5:
      A = rd (a_zpx ());
      setNZ (A);
      break;

    case 0xAD:
      A = rd (a_abs ());
      setNZ (A);
      break;

    case 0xBD:
      A = rd (a_absx ());
      setNZ (A);
      break;

    case 0xB9:
      A = rd (a_absy ());
      setNZ (A);
      break;

    case 0xA1:
      A = rd (a_indx ());
      setNZ (A);
      break;

    case 0xB1:
      A = rd (a_indy ());
      setNZ (A);
      break;

    case 0xA2:
      X = rb ();
      setNZ (X);
      break;

    case 0xA6:
      X = rd (a_zp ());
      setNZ (X);
      break;

    case 0xB6:
      X = rd (a_zpy ());
      setNZ (X);
      break;

    case 0xAE:
      X = rd (a_abs ());
      setNZ (X);
      break;

    case 0xBE:
      X = rd (a_absy ());
      setNZ (X);
      break;

    case 0xA0:
      Y = rb ();
      setNZ (Y);
      break;

    case 0xA4:
      Y = rd (a_zp ());
      setNZ (Y);
      break;

    case 0xB4:
      Y = rd (a_zpx ());
      setNZ (Y);
      break;

    case 0xAC:
      Y = rd (a_abs ());
      setNZ (Y);
      break;

    case 0xBC:
      Y = rd (a_absx ());
      setNZ (Y);
      break;

    case 0x85:
      wr (a_zp (), A);
      break;

    case 0x95:
      wr (a_zpx (), A);
      break;

    case 0x8D:
      wr (a_abs (), A);
      break;

    case 0x9D:
      wr (a_absx (), A);
      break;

    case 0x99:
      wr (a_absy (), A);
      break;

    case 0x81:
      wr (a_indx (), A);
      break;

    case 0x91:
      wr (a_indy (), A);
      break;

    case 0x86:
      wr (a_zp (), X);
      break;

    case 0x96:
      wr (a_zpy (), X);
      break;

    case 0x8E:
      wr (a_abs (), X);
      break;

    case 0x84:
      wr (a_zp (), Y);
      break;

    case 0x94:
      wr (a_zpx (), Y);
      break;

    case 0x8C:
      wr (a_abs (), Y);
      break;

    /* ---- transfers ---- */
    case 0xAA:
      X = A;
      setNZ (X);
      break;

    case 0xA8:
      Y = A;
      setNZ (Y);
      break;

    case 0x8A:
      A = X;
      setNZ (A);
      break;

    case 0x98:
      A = Y;
      setNZ (A);
      break;

    case 0xBA:
      X = SP;
      setNZ (X);
      break;

    case 0x9A:
      SP = X;
      break;

    /* ---- stack ---- */
    case 0x48:
      push (A);
      break;

    case 0x68:
      A = pop ();
      setNZ (A);
      break;

    case 0x08:
      push (P | FB | FU);
      break;

    case 0x28:
      P = (pop () & ~FB) | FU;
      break;

    /* ---- logic ---- */
    case 0x29:
      A &= rb ();
      setNZ (A);
      break;

    case 0x25:
      A &= rd (a_zp ());
      setNZ (A);
      break;

    case 0x35:
      A &= rd (a_zpx ());
      setNZ (A);
      break;

    case 0x2D:
      A &= rd (a_abs ());
      setNZ (A);
      break;

    case 0x3D:
      A &= rd (a_absx ());
      setNZ (A);
      break;

    case 0x39:
      A &= rd (a_absy ());
      setNZ (A);
      break;

    case 0x21:
      A &= rd (a_indx ());
      setNZ (A);
      break;

    case 0x31:
      A &= rd (a_indy ());
      setNZ (A);
      break;

    case 0x09:
      A |= rb ();
      setNZ (A);
      break;

    case 0x05:
      A |= rd (a_zp ());
      setNZ (A);
      break;

    case 0x15:
      A |= rd (a_zpx ());
      setNZ (A);
      break;

    case 0x0D:
      A |= rd (a_abs ());
      setNZ (A);
      break;

    case 0x1D:
      A |= rd (a_absx ());
      setNZ (A);
      break;

    case 0x19:
      A |= rd (a_absy ());
      setNZ (A);
      break;

    case 0x01:
      A |= rd (a_indx ());
      setNZ (A);
      break;

    case 0x11:
      A |= rd (a_indy ());
      setNZ (A);
      break;

    case 0x49:
      A ^= rb ();
      setNZ (A);
      break;

    case 0x45:
      A ^= rd (a_zp ());
      setNZ (A);
      break;

    case 0x55:
      A ^= rd (a_zpx ());
      setNZ (A);
      break;

    case 0x4D:
      A ^= rd (a_abs ());
      setNZ (A);
      break;

    case 0x5D:
      A ^= rd (a_absx ());
      setNZ (A);
      break;

    case 0x59:
      A ^= rd (a_absy ());
      setNZ (A);
      break;

    case 0x41:
      A ^= rd (a_indx ());
      setNZ (A);
      break;

    case 0x51:
      A ^= rd (a_indy ());
      setNZ (A);
      break;

    case 0x24:
      m = rd (a_zp ());
      P = (P & ~(FN | FV | FZ)) | (m & (FN | FV)) | ((A & m) ? 0 : FZ);
      break;

    case 0x2C:
      m = rd (a_abs ());
      P = (P & ~(FN | FV | FZ)) | (m & (FN | FV)) | ((A & m) ? 0 : FZ);
      break;

    /* ---- arithmetic ---- */
    case 0x69:
      adc (rb ());
      break;

    case 0x65:
      adc (rd (a_zp ()));
      break;

    case 0x75:
      adc (rd (a_zpx ()));
      break;

    case 0x6D:
      adc (rd (a_abs ()));
      break;

    case 0x7D:
      adc (rd (a_absx ()));
      break;

    case 0x79:
      adc (rd (a_absy ()));
      break;

    case 0x61:
      adc (rd (a_indx ()));
      break;

    case 0x71:
      adc (rd (a_indy ()));
      break;

    case 0xE9:
      adc (rb () ^ 0xFF);
      break;

    case 0xE5:
      adc (rd (a_zp ()) ^ 0xFF);
      break;

    case 0xF5:
      adc (rd (a_zpx ()) ^ 0xFF);
      break;

    case 0xED:
      adc (rd (a_abs ()) ^ 0xFF);
      break;

    case 0xFD:
      adc (rd (a_absx ()) ^ 0xFF);
      break;

    case 0xF9:
      adc (rd (a_absy ()) ^ 0xFF);
      break;

    case 0xE1:
      adc (rd (a_indx ()) ^ 0xFF);
      break;

    case 0xF1:
      adc (rd (a_indy ()) ^ 0xFF);
      break;

    case 0xC9:
      cmp (A, rb ());
      break;

    case 0xC5:
      cmp (A, rd (a_zp ()));
      break;

    case 0xD5:
      cmp (A, rd (a_zpx ()));
      break;

    case 0xCD:
      cmp (A, rd (a_abs ()));
      break;

    case 0xDD:
      cmp (A, rd (a_absx ()));
      break;

    case 0xD9:
      cmp (A, rd (a_absy ()));
      break;

    case 0xC1:
      cmp (A, rd (a_indx ()));
      break;

    case 0xD1:
      cmp (A, rd (a_indy ()));
      break;

    case 0xE0:
      cmp (X, rb ());
      break;

    case 0xE4:
      cmp (X, rd (a_zp ()));
      break;

    case 0xEC:
      cmp (X, rd (a_abs ()));
      break;

    case 0xC0:
      cmp (Y, rb ());
      break;

    case 0xC4:
      cmp (Y, rd (a_zp ()));
      break;

    case 0xCC:
      cmp (Y, rd (a_abs ()));
      break;

    /* ---- inc / dec ---- */
    case 0xE6:
      a = a_zp ();
      m = rd (a) + 1;
      wr (a, m);
      setNZ (m);
      break;

    case 0xF6:
      a = a_zpx ();
      m = rd (a) + 1;
      wr (a, m);
      setNZ (m);
      break;

    case 0xEE:
      a = a_abs ();
      m = rd (a) + 1;
      wr (a, m);
      setNZ (m);
      break;

    case 0xFE:
      a = a_absx ();
      m = rd (a) + 1;
      wr (a, m);
      setNZ (m);
      break;

    case 0xC6:
      a = a_zp ();
      m = rd (a) - 1;
      wr (a, m);
      setNZ (m);
      break;

    case 0xD6:
      a = a_zpx ();
      m = rd (a) - 1;
      wr (a, m);
      setNZ (m);
      break;

    case 0xCE:
      a = a_abs ();
      m = rd (a) - 1;
      wr (a, m);
      setNZ (m);
      break;

    case 0xDE:
      a = a_absx ();
      m = rd (a) - 1;
      wr (a, m);
      setNZ (m);
      break;

    case 0xE8:
      X++;
      setNZ (X);
      break;

    case 0xC8:
      Y++;
      setNZ (Y);
      break;

    case 0xCA:
      X--;
      setNZ (X);
      break;

    case 0x88:
      Y--;
      setNZ (Y);
      break;

    /* ---- shifts (accumulator / memory) ---- */
    case 0x0A:
      A = asl (A);
      break;

    case 0x06:
      a = a_zp ();
      wr (a, asl (rd (a)));
      break;

    case 0x16:
      a = a_zpx ();
      wr (a, asl (rd (a)));
      break;

    case 0x0E:
      a = a_abs ();
      wr (a, asl (rd (a)));
      break;

    case 0x1E:
      a = a_absx ();
      wr (a, asl (rd (a)));
      break;

    case 0x4A:
      A = lsr (A);
      break;

    case 0x46:
      a = a_zp ();
      wr (a, lsr (rd (a)));
      break;

    case 0x56:
      a = a_zpx ();
      wr (a, lsr (rd (a)));
      break;

    case 0x4E:
      a = a_abs ();
      wr (a, lsr (rd (a)));
      break;

    case 0x5E:
      a = a_absx ();
      wr (a, lsr (rd (a)));
      break;

    case 0x2A:
      A = rol (A);
      break;

    case 0x26:
      a = a_zp ();
      wr (a, rol (rd (a)));
      break;

    case 0x36:
      a = a_zpx ();
      wr (a, rol (rd (a)));
      break;

    case 0x2E:
      a = a_abs ();
      wr (a, rol (rd (a)));
      break;

    case 0x3E:
      a = a_absx ();
      wr (a, rol (rd (a)));
      break;

    case 0x6A:
      A = ror (A);
      break;

    case 0x66:
      a = a_zp ();
      wr (a, ror (rd (a)));
      break;

    case 0x76:
      a = a_zpx ();
      wr (a, ror (rd (a)));
      break;

    case 0x6E:
      a = a_abs ();
      wr (a, ror (rd (a)));
      break;

    case 0x7E:
      a = a_absx ();
      wr (a, ror (rd (a)));
      break;

    /* ---- branches ---- */
    case 0x10:
      br (!(P & FN));
      break;

    case 0x30:
      br (P & FN);
      break;

    case 0x50:
      br (!(P & FV));
      break;

    case 0x70:
      br (P & FV);
      break;

    case 0x90:
      br (!(P & FC));
      break;

    case 0xB0:
      br (P & FC);
      break;

    case 0xD0:
      br (!(P & FZ));
      break;

    case 0xF0:
      br (P & FZ);
      break;

    /* ---- jumps / calls ---- */
    case 0x4C:
      PC = a_abs ();
      break;

    case 0x6C:
      {
        uint16_t p = a_abs ();
        PC = rd (p) | (rd ((p & 0xFF00) | ((p + 1) & 0xFF)) << 8);
      }
      break;

    case 0x20:
      {
        uint16_t t = rw ();
        uint16_t r = PC - 1;
        push (r >> 8);
        push (r & 0xFF);
        PC = t;
      }
      break;

    case 0x60:
      {
        uint8_t lo = pop ();
        uint8_t hi = pop ();
        PC = (lo | (hi << 8)) + 1;
      }
      break;

    case 0x40:
      {
        P = (pop () & ~FB) | FU;
        uint8_t lo = pop ();
        uint8_t hi = pop ();
        PC = lo | (hi << 8);
      }
      break;

    /* ---- flags ---- */
    case 0x18:
      P &= ~FC;
      break;

    case 0x38:
      P |= FC;
      break;

    case 0x58:
      P &= ~FI;
      break;

    case 0x78:
      P |= FI;
      break;

    case 0xB8:
      P &= ~FV;
      break;

    case 0xD8:
      P &= ~FD;
      break;

    case 0xF8:
      P |= FD;
      break;

    case 0xEA:
      break; /* NOP */

    case 0x00:
      return 1; /* BRK -> halt */

    default:
      fprintf (stderr, "sim6502: unimplemented opcode %02X at %04X\n", op,
               (PC - 1) & 0xFFFF);
      return 1;
    }
  return 0;
}

static long
parsenum (const char *s)
{
  if (s[0] == '$')
    {
      return strtol (s + 1, NULL, 16);
    }

  return strtol (s, NULL, 0);
}

int
main (int argc, char **argv)
{
  const char *fn = NULL;
  long load = 0x0200, dump = -1, maxins = 10000000, i;
  FILE *f;
  size_t n;

  for (i = 1; i < argc; i++)
    {
      if (!strcmp (argv[i], "-l") && i + 1 < argc)
        {
          load = parsenum (argv[++i]);
        }
      else if (!strcmp (argv[i], "-d") && i + 1 < argc)
        {
          dump = parsenum (argv[++i]);
        }
      else if (!strcmp (argv[i], "-o") && i + 1 < argc)
        {
          outport = (int)parsenum (argv[++i]);
        }
      else if (!strcmp (argv[i], "-n") && i + 1 < argc)
        {
          maxins = parsenum (argv[++i]);
        }
      else
        {
          fn = argv[i];
        }
    }

  if (!fn)
    {
      fprintf (
          stderr,
          "usage: sim6502 file.bin [-l load] [-d addr] [-o port] [-n max]\n");
      return 1;
    }

  f = fopen (fn, "rb");
  if (!f)
    {
      perror (fn);
      return 1;
    }

  n = fread (mem + (load & 0xFFFF), 1, 65536 - (load & 0xFFFF), f);
  fclose (f);
  (void)n;
  A = X = Y = 0;
  SP = 0xFF;
  P = FU | FI;
  PC = load & 0xFFFF;
  for (i = 0; i < maxins; i++)
    {
      if (step ())
        {
          break;
        }
    }

  if (dump >= 0)
    {
      printf ("%02X\n", mem[dump & 0xFFFF]);
    }

  return 0;
}
