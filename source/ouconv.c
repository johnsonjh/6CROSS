/*
 * ouconv - convert ASMZ80 object-unit / run-unit hex records
 * (:aabbbbcc[dd]...ee) into a raw binary image or Intel HEX, suitable for
 * loading into a Z80 emulator.
 *
 * Usage: ouconv input.obj [-o out] [--bin | --ihex]
 *   --bin   (default) raw binary: bytes from the lowest to the highest
 *           loaded address (gaps filled with 0x00).
 *   --ihex  Intel HEX (16 bytes/record + EOF record).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static unsigned char img[65536];
static int lo = 65536, hi = -1;

static void
load (const char *fn)
{
  FILE *f = fopen (fn, "r");
  char line[1024];

  if (!f)
    {
      perror (fn);
      exit (1);
    }

  while (fgets (line, sizeof (line), f))
    {
      int cnt, addr, k;
      if (line[0] != ':' || strlen (line) < 9)
        {
          continue;
        }

      /* only load data records: cc = 00 (absolute) or 10 (relocatable) */
      if (!((line[7] == '0' && line[8] == '0')
            || (line[7] == '1' && line[8] == '0')))
        {
          continue;
        }

      cnt = hx (line + 1, 2);
      addr = hx (line + 3, 4);
      if (cnt < 0 || addr < 0)
        {
          continue;
        }

      for (k = 0; k < cnt; k++)
        {
          int b = hx (line + 9 + 2 * k, 2);
          int a = (addr + k) & 0xFFFF;
          if (b < 0)
            {
              break;
            }

          img[a] = (unsigned char)b;
          if (a < lo)
            {
              lo = a;
            }

          if (a > hi)
            {
              hi = a;
            }
        }
    }
  fclose (f);
}

static void
write_bin (const char *fn)
{
  FILE *f = fopen (fn, "wb");

  if (!f)
    {
      perror (fn);
      exit (1);
    }

  if (hi >= lo)
    {
      fwrite (img + lo, 1, hi - lo + 1, f);
    }

  fclose (f);
}

static void
write_ihex (const char *fn)
{
  FILE *f = fopen (fn, "w");
  int a;

  if (!f)
    {
      perror (fn);
      exit (1);
    }

  for (a = lo; a <= hi; a += 16)
    {
      int c = hi - a + 1, k, sum;
      if (c > 16)
        {
          c = 16;
        }

      sum = c + ((a >> 8) & 0xFF) + (a & 0xFF) + 0x00;
      fprintf (f, ":%02X%04X00", c, a & 0xFFFF);
      for (k = 0; k < c; k++)
        {
          fprintf (f, "%02X", img[a + k]);
          sum += img[a + k];
        }

      fprintf (f, "%02X\n", (-sum) & 0xFF);
    }

  fprintf (f, ":00000001FF\n"); /* Intel HEX EOF record */
  fclose (f);
}

int
main (int argc, char **argv)
{
  const char *in = NULL, *out = NULL;
  int ihex = 0, i;
  char defout[300];

  for (i = 1; i < argc; i++)
    {
      if (!strcmp (argv[i], "-o") && i + 1 < argc)
        {
          out = argv[++i];
        }
      else if (!strcmp (argv[i], "--ihex"))
        {
          ihex = 1;
        }
      else if (!strcmp (argv[i], "--bin"))
        {
          ihex = 0;
        }
      else
        {
          in = argv[i];
        }
    }

  if (!in)
    {
      fprintf (stderr, "usage: ouconv input.obj [-o out] [--bin|--ihex]\n");
      return 1;
    }

  load (in);
  if (hi < lo)
    {
      fprintf (stderr, "ouconv: no data records found\n");
      return 1;
    }

  if (!out)
    {
      snprintf (defout, sizeof (defout), "%s.%s", in, ihex ? "hex" : "bin");
      out = defout;
    }

  if (ihex)
    {
      write_ihex (out);
    }
  else
    {
      write_bin (out);
    }

  fprintf (stderr, "ouconv: %04X-%04X H (%d bytes) -> %s\n", lo, hi,
           hi - lo + 1, out);
  return 0;
}
