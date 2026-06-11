/*
 * bin2hex - wrap a raw binary image (e.g. a CP/M .COM file) in the Intel
 * HEX object format that the MSA reverse-assemblers consume, so they can
 * disassemble arbitrary files.  (ouconv goes the other way -- it turns an
 * assembled .obj into a raw binary or HEX.)  The output feeds any of the
 * MSA disassemblers -- msaz80, msa6502, msa6800, msa8085, msa8748 -- since
 * they share one loader; only the per-CPU decode differs, so choose the
 * load address to match the target.
 *
 * Usage: bin2hex input [-o out] [-a base]
 *   -a base  load address in hex.  Default 100 -- the CP/M TPA, where Z80/
 *            8080 .COM files load and begin executing.  For other CPUs give
 *            the image's real base (e.g. a 6502 ROM/cartridge address).
 *   -o out   output file (default: input + ".hex").
 *
 * The disassembler starts its flow trace at the lowest loaded address; use
 * its own -e <entry> option to add further entry points (e.g. a 6502 reset
 * vector).  Only the low 64K is emitted -- the address space the engine
 * models; a larger file is truncated with a warning.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned char img[65536];

int
main (int argc, char **argv)
{
  const char *in = NULL, *out = NULL;
  int base = 0x100, i, n;
  char defout[300];
  FILE *f, *of;

  for (i = 1; i < argc; i++)
    {
      if (!strcmp (argv[i], "-o") && i + 1 < argc)
        {
          out = argv[++i];
        }
      else if (!strcmp (argv[i], "-a") && i + 1 < argc)
        {
          base = (int)strtol (argv[++i], NULL, 16) & 0xFFFF;
        }
      else
        {
          in = argv[i];
        }
    }

  if (!in)
    {
      fprintf (stderr, "usage: bin2hex input [-o out] [-a base_hex]\n");
      return 1;
    }

  f = fopen (in, "rb");
  if (!f)
    {
      perror (in);
      return 1;
    }

  n = (int)fread (img, 1, (size_t)(65536 - base), f);
  if (n > 0 && fgetc (f) != EOF)
    {
      fprintf (stderr,
               "bin2hex: %s: truncated to %d bytes (image exceeds 64K from "
               "base $%04X)\n",
               in, n, base);
    }

  fclose (f);
  if (n <= 0)
    {
      fprintf (stderr, "bin2hex: %s: empty or unreadable\n", in);
      return 1;
    }

  if (!out)
    {
      snprintf (defout, sizeof defout, "%s.hex", in);
      out = defout;
    }

  of = fopen (out, "w");
  if (!of)
    {
      perror (out);
      return 1;
    }

  for (i = 0; i < n; i += 16)
    {
      int c = n - i, k, sum, a = (base + i) & 0xFFFF;
      if (c > 16)
        {
          c = 16;
        }

      sum = c + ((a >> 8) & 0xFF) + (a & 0xFF);
      fprintf (of, ":%02X%04X00", c, a);
      for (k = 0; k < c; k++)
        {
          fprintf (of, "%02X", img[i + k]);
          sum += img[i + k];
        }

      fprintf (of, "%02X\n", (-sum) & 0xFF);
    }

  fprintf (of, ":00000001FF\n"); /* Intel HEX EOF record */
  fclose (of);
  fprintf (stderr, "bin2hex: %s -> %s (%d bytes, $%04X-$%04X)\n", in, out, n,
           base, (base + n - 1) & 0xFFFF);
  return 0;
}
