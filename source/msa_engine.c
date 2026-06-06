/*
 * msa_engine - shared engine for the MSA reverse-assemblers: loads the
 * ASM* object-unit format into 64K memory, flow-traces from entry points to
 * separate code from data, and emits re-assemblable source.  CPU-specific
 * instruction decoding is provided by decode() in a per-CPU file.  See
 * MSAZ80_NOTES.md for the reverse-engineering of the original PL/6 MSA_C1..C5.
 */

#include "msa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int val[65536];
unsigned char some[65536], rd[65536], wr[65536], jmp[65536];
unsigned char jsr[65536], exec[65536], visit[65536], bno[65536];

struct sym
{
  char name[9];
  int type;
  int value;
};
static struct sym syms[4096];
static int nsym = 0;
static int begin = -1;

static int
hx (const char *p, int n)
{
  int v = 0, i, c, d;

  for (i = 0; i < n; i++)
    {
      c = (unsigned char)p[i];
      if (c >= '0' && c <= '9')
        {
          d = c - '0';
        }
      else if (c >= 'A' && c <= 'F')
        {
          d = c - 'A' + 10;
        }
      else if (c >= 'a' && c <= 'f')
        {
          d = c - 'a' + 10;
        }
      else
        {
          return -1;
        }

      v = v * 16 + d;
    }

  return v;
}

static void
load (const char *fn)
{
  FILE *f = fopen (fn, "r");
  char line[1024];
  int i;

  for (i = 0; i < 65536; i++)
    {
      val[i] = -1;
    }

  if (!f)
    {
      perror (fn);
      exit (1);
    }

  while (fgets (line, sizeof line, f))
    {
      int cnt, addr, type, k, sum, len = (int)strlen (line);
      while (len && (line[len - 1] == '\n' || line[len - 1] == '\r'))
        {
          line[--len] = 0;
        }
      if (line[0] != ':' || len < 11)
        {
          continue;
        }

      cnt = hx (line + 1, 2);
      addr = hx (line + 3, 4);
      type = hx (line + 7, 2);
      if (cnt < 0 || addr < 0 || type < 0)
        {
          continue;
        }

      for (sum = 0, k = 1; k + 1 < len; k += 2)
        {
          int b = hx (line + k, 2);
          if (b < 0)
            {
              break;
            }

          sum += b;
        }

      if (sum & 0xFF)
        {
          fprintf (stderr, "msa: checksum error: %s\n", line);
          continue;
        }

      if (type & 0x80)
        {
          if (nsym < 4096)
            {
              struct sym *s = &syms[nsym++];
              s->type = type - 128;
              s->value = (type == 0x82) ? 0 : addr;
              for (k = 0; k < 8; k++)
                {
                  s->name[k] = (char)hx (line + 9 + 2 * k, 2);
                }

              s->name[8] = 0;
            }

          continue;
        }

      switch (type & 0x0F)
        {
        case 0:
          for (k = 0; k < cnt; k++)
            {
              int b = hx (line + 9 + 2 * k, 2), a = (addr + k) & 0xFFFF;
              if (b < 0)
                {
                  break;
                }

              val[a] = b;
              some[a] = 1;
            }

          break;

        case 1:
          begin = addr;
          break;

        case 5:
          {
            int n2 = hx (line + 9, 4), a;
            for (k = 0; k < n2; k++)
              {
                a = (addr + k) & 0xFFFF;
                if (!some[a])
                  {
                    val[a] = RESV;
                    some[a] = 1;
                    visit[a] = 1;
                  }
              }

            break;
          }
        }
    }
  fclose (f);
}

const char *
symat (int v)
{
  int i;

  for (i = 0; i < nsym; i++)
    {
      if (syms[i].type != 2 && syms[i].value == v)
        {
          return syms[i].name;
        }
    }

  return NULL;
}
void
lbl (char *b, int a)
{
  const char *s = symat (a);

  if (s)
    {
      int n = 0;
      while (s[n] && s[n] != ' ')
        {
          n++;
        }
      sprintf (b, "%.*s", n, s);
    }
  else
    {
      sprintf (b, "L%04X", a & 0xFFFF);
    }
}
int
byte (int a)
{
  int v = val[a & 0xFFFF];

  return (v < 0 || v > 255) ? 0 : v;
}
int
word (int a)
{
  return byte (a) | (byte (a + 1) << 8);
}

static int wl[70000][2];
static int wn = 0;
static void
enq (int a, int how)
{
  a &= 0xFFFF;
  if (how == 1)
    {
      jmp[a] = 1;
    }
  else
    {
      jsr[a] = 1;
    }

  if (wn < 70000)
    {
      wl[wn][0] = a;
      wl[wn][1] = how;
      wn++;
    }
}

static void
trace (void)
{
  while (wn > 0)
    {
      int a = wl[--wn][0];
      for (;;)
        {
          struct ins in;
          int i;
          if (!some[a] || visit[a])
            {
              break;
            }

          in = decode (a);
          for (i = 0; i < in.len; i++)
            {
              int c = (a + i) & 0xFFFF;
              if (!some[c])
                {
                  in.len = 0;
                  break;
                }

              visit[c] = 1;
              exec[c] = 1;
              bno[c] = i;
            }

          if (in.len == 0)
            {
              break;
            }

          if (in.rdref >= 0)
            {
              rd[in.rdref & 0xFFFF] = 1;
            }

          if (in.wrref >= 0)
            {
              wr[in.wrref & 0xFFFF] = 1;
            }

          if (in.target >= 0)
            {
              if (in.flow == CALL_)
                {
                  jsr[in.target & 0xFFFF] = 1;
                }
              else
                {
                  jmp[in.target & 0xFFFF] = 1;
                }
            }

          if (in.flow == STOP)
            {
              break;
            }

          if (in.flow == BR_UNCOND)
            {
              a = in.target & 0xFFFF;
              continue;
            }

          if (in.flow == CALL_ && in.target >= 0)
            {
              enq (in.target, 2);
            }

          if (in.flow == BR_COND && in.target >= 0)
            {
              enq (in.target, 1);
            }

          a = (a + in.len) & 0xFFFF;
        }

      if (wn == 0)
        {
          int b;
          for (b = 0; b < 65536; b++)
            {
              if (some[b] && !visit[b] && (jsr[b] || jmp[b]))
                {
                  enq (b, jmp[b] ? 1 : 2);
                  break;
                }
            }
        }
    }
}

static void
emit (FILE *f)
{
  int a, i;

  for (i = 0; i < nsym; i++)
    {
      if (syms[i].type != 2)
        {
          int v = syms[i].value & 0xFFFF;
          if (!rd[v] && !wr[v] && !jmp[v] && !jsr[v] && !exec[v])
            {
              rd[v] = 1;
            }
        }
    }

  fprintf (f, "         NAME     DISASM\n");
  for (i = 0; i < 65536; i++)
    {
      if (!some[i] && (rd[i] || wr[i] || jmp[i] || jsr[i]))
        {
          char l[16];
          lbl (l, i);
          fprintf (f, "%-8s EQU      $%04X\n", l, i);
        }
    }

  a = 0;
  {
    int lo = -1;
    for (i = 0; i < 65536; i++)
      {
        if (some[i])
          {
            lo = i;
            break;
          }
      }

    if (lo < 0)
      {
        return;
      }

    a = lo;
    fprintf (f, "         ORG      $%04X\n", a);
  }
  while (a < 65536)
    {
      char label[16] = "";
      if (!some[a])
        {
          a++;
          continue;
        }

      if (rd[a] || wr[a] || jmp[a] || jsr[a])
        {
          lbl (label, a);
        }

      if (exec[a] && bno[a] == 0)
        {
          struct ins in = decode (a);
          fprintf (f, "%-8s %s\n", label, in.text);
          a += in.len ? in.len : 1;
        }
      else
        {
          char line[120];
          int n = 0, first = 1;
          line[0] = 0;
          if (val[a] == RESV)
            {
              int c = 0;
              while (a < 65536 && val[a] == RESV)
                {
                  c++;
                  a++;
                }
              fprintf (f, "%-8s DEFS     %d\n", label, c);
              continue;
            }

          strcat (line, "DEFB     ");
          while (a < 65536 && some[a] && val[a] != RESV
                 && !(exec[a] && bno[a] == 0) && n < 8
                 && !(n > 0 && (rd[a] || wr[a] || jmp[a] || jsr[a])))
            {
              char t[8];
              sprintf (t, "%s$%02X", first ? "" : ",", val[a] & 0xFF);
              strcat (line, t);
              first = 0;
              n++;
              a++;
            }
          fprintf (f, "%-8s %s\n", label, line);
        }
    }
  if (begin >= 0)
    {
      char e[16];
      lbl (e, begin);
      fprintf (f, "         END      %s\n", e);
    }
  else
    {
      fprintf (f, "         END\n");
    }
}

int
main (int argc, char **argv)
{
  const char *in = NULL, *out = NULL;
  int ent[64], ne = 0, i;
  char cmd[64];
  FILE *of;

  for (i = 1; i < argc; i++)
    {
      if (!strcmp (argv[i], "-e") && i + 1 < argc)
        {
          if (ne < 64)
            {
              ent[ne++] = (int)strtol (argv[++i], NULL, 16);
            }
        }
      else if (!strcmp (argv[i], "-o") && i + 1 < argc)
        {
          out = argv[++i];
        }
      else
        {
          in = argv[i];
        }
    }

  if (!in)
    {
      fprintf (stderr, "usage: msa<cpu> file.obj [-e entry] [-o out]\n");
      return 1;
    }

  load (in);
  for (i = 0; i < ne; i++)
    {
      enq (ent[i], 1);
    }

  if (begin >= 0)
    {
      enq (begin, 1);
    }

  while (fgets (cmd, sizeof cmd, stdin))
    {
      if (cmd[0] == 'Q' || cmd[0] == 'q')
        {
          break;
        }

      if (cmd[0] == 'E' || cmd[0] == 'e')
        {
          break;
        }

      if (cmd[0] == 'J' || cmd[0] == 'j')
        {
          enq ((int)strtol (cmd + 1, NULL, 16), 1);
        }

      if (cmd[0] == 'C' || cmd[0] == 'c')
        {
          enq ((int)strtol (cmd + 1, NULL, 16), 2);
        }
    }
  if (wn == 0)
    {
      for (i = 0; i < 65536; i++)
        {
          if (some[i] && val[i] != RESV)
            {
              enq (i, 1);
              break;
            }
        }
    }

  trace ();
  of = out ? fopen (out, "w") : stdout;
  if (!of)
    {
      perror (out);
      return 1;
    }

  emit (of);
  if (out)
    {
      fclose (of);
    }

  return 0;
}
