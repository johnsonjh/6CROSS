/* cp6link - linker for ASMZ80/ASM6502 object units.
 *
 * A C reimplementation of the CP-6 BASIC BAS_LINK ("a cheesy linker for
 * ASMZ80/6502 OUs", D. Griesser, LADC, 1983).  It reads printable-hex
 * object-unit records of the form
 *
 *     :aabbbbcc[dd]...ee
 *
 * (aa = byte count, bbbb = address MSB-first, cc = record type/command,
 * dd = data bytes, ee = checksum), resolves external references (type 82)
 * against an optional DEF schema, applies type-03 long relocations, loads
 * type-00 data into a 64K image, and writes a run-unit of type-00 load
 * records (with optional symbol/schema records).
 *
 * Usage: cp6link [-s schema.obj] object.obj [object2.obj ...] [-o out.ru]
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int hx(const char *p, int n)
{
    int v = 0, i, c, d;
    for (i = 0; i < n; i++) {
        c = (unsigned char)p[i];
        if      (c >= '0' && c <= '9') d = c - '0';
        else if (c >= 'A' && c <= 'F') d = c - 'A' + 10;
        else if (c >= 'a' && c <= 'f') d = c - 'a' + 10;
        else return -1;
        v = v * 16 + d;
    }
    return v;
}

#define MAXDEF 128
static char defname[MAXDEF][17];
static int  defval[MAXDEF];
static int  ndef = 0;
static int  extmap[256];                 /* external number -> 1+def index */

#define MAXREL 4096
static int relA[MAXREL], relB[MAXREL], nrel = 0;

static unsigned char img[65536];
static int lo = 65536, hi = -1;

/* Symbol schema collected from type-80 records and -D options, optionally
 * re-emitted into the run-unit (BAS_LINK's "include schema in the RU"). */
#define MAXSYM 2048
static char symname[MAXSYM][9];
static int  symval[MAXSYM];
static int  nsym = 0;
static int  emit_schema = 0;

static void add_sym(const char *nm, int val)
{
    int i;
    for (i = 0; i < nsym; i++)
        if (!memcmp(symname[i], nm, 8)) { symval[i] = val; return; }
    if (nsym < MAXSYM) {
        memcpy(symname[nsym], nm, 8); symname[nsym][8] = 0;
        symval[nsym] = val; nsym++;
    }
}

static void load_schema(const char *fn)
{
    FILE *f = fopen(fn, "r");
    char line[1024];
    if (!f) { perror(fn); exit(1); }
    while (fgets(line, sizeof line, f)) {
        if (line[0] != ':' || strlen(line) < 9) continue;
        /* type 80 or 81 define a symbol: name (8 chars) at +9, value at +3 */
        if ((line[7]=='8') && (line[8]=='0' || line[8]=='1')) {
            if (ndef < MAXDEF) {
                memcpy(defname[ndef], line + 9, 16);
                defname[ndef][16] = 0;
                defval[ndef] = hx(line + 3, 4);
                ndef++;
            }
        }
    }
    fclose(f);
}

static void process(const char *fn)
{
    FILE *f = fopen(fn, "r");
    char line[1024];
    if (!f) { perror(fn); exit(1); }
    while (fgets(line, sizeof line, f)) {
        int cnt, addr, k, L;
        if (line[0] != ':' || strlen(line) < 9) continue;
        cnt  = hx(line + 1, 2);
        addr = hx(line + 3, 4);
        if (cnt < 0 || addr < 0) continue;
        if ((line[7]=='0'&&line[8]=='0') || (line[7]=='1'&&line[8]=='0')) {
            for (k = 0; k < cnt; k++) {
                int b = hx(line + 9 + 2 * k, 2);
                int a = (addr + k) & 0xFFFF;
                if (b < 0) break;
                img[a] = (unsigned char)b;
                if (a < lo) lo = a;
                if (a > hi) hi = a;
            }
        } else if (line[7]=='8' && line[8]=='2') {          /* external ref */
            int i;
            for (i = 0; i < ndef; i++)
                if (!memcmp(defname[i], line + 9, 16)) { extmap[addr & 0xFF] = i + 1; break; }
        } else if (line[7]=='0' && line[8]=='3') {          /* relocate long */
            for (L = 0; L + 3 <= cnt; L += 3) {
                if (nrel < MAXREL) {
                    relA[nrel] = hx(line + 9 + 2 * L, 4);
                    relB[nrel] = hx(line + 9 + 2 * (L + 2), 2);
                    nrel++;
                }
            }
        } else if (line[7]=='8' && (line[8]=='0' || line[8]=='1')) {  /* symbol */
            char nm[9];
            int k;
            for (k = 0; k < 8; k++) nm[k] = (char)hx(line + 9 + 2 * k, 2);
            add_sym(nm, addr);
        }
        /* 01 (entry) records carry no image data */
    }
    fclose(f);
}

static void relocate(void)
{
    int i;
    for (i = 0; i < nrel; i++) {
        int a = relA[i] & 0xFFFF;
        int v = img[a] | (img[(a + 1) & 0xFFFF] << 8);
        int base = relB[i];
        if (base > 127) { int d = extmap[base - 128]; if (d) v -= defval[d - 1]; }
        else            { int d = extmap[base];       if (d) v += defval[d - 1]; }
        v &= 0xFFFF;
        img[a] = v & 0xFF;
        img[(a + 1) & 0xFFFF] = (v >> 8) & 0xFF;
    }
}

static void write_rununit(const char *fn)
{
    FILE *f;
    int a;
    if (hi < lo) { fprintf(stderr, "cp6link: nothing to link\n"); return; }
    f = fopen(fn, "w");
    if (!f) { perror(fn); exit(1); }
    for (a = lo; a <= hi; a += 32) {
        int c = hi - a + 1, k, sum;
        if (c > 32) c = 32;
        sum = c + ((a >> 8) & 0xFF) + (a & 0xFF);
        fprintf(f, ":%02X%04X00", c, a & 0xFFFF);
        for (k = 0; k < c; k++) { fprintf(f, "%02X", img[a + k]); sum += img[a + k]; }
        fprintf(f, "%02X\n", (-sum) & 0xFF);
    }
    fclose(f);
}

/* Append type-80 symbol (schema) records to the run-unit. */
static void write_schema(const char *fn)
{
    FILE *f;
    int i, k, sum;
    if (nsym == 0) return;
    f = fopen(fn, "a");
    if (!f) { perror(fn); return; }
    for (i = 0; i < nsym; i++) {
        sum = 0x09 + ((symval[i] >> 8) & 0xFF) + (symval[i] & 0xFF) + 0x80 + 0x01;
        fprintf(f, ":09%04X80", symval[i] & 0xFFFF);
        for (k = 0; k < 8; k++) {
            unsigned char c = (unsigned char)symname[i][k];
            fprintf(f, "%02X", c);
            sum += c;
        }
        fprintf(f, "01%02X\n", (-sum) & 0xFF);
    }
    fclose(f);
}

int main(int argc, char **argv)
{
    const char *out = "a.ru", *schema = NULL;
    const char *objs[64];
    int nobj = 0, i;
    for (i = 1; i < argc; i++) {
        if      (!strcmp(argv[i], "-o") && i + 1 < argc) out = argv[++i];
        else if (!strcmp(argv[i], "-s") && i + 1 < argc) schema = argv[++i];
        else if (!strcmp(argv[i], "--schema")) emit_schema = 1;
        else if (!strcmp(argv[i], "-D") && i + 1 < argc) {
            char *a = argv[++i], *eq = strchr(a, '=');
            if (eq) {
                char nm[9];
                *eq = 0;
                snprintf(nm, sizeof nm, "%-8.8s", a);
                add_sym(nm, (int)strtol(eq + 1, NULL, 16));
            }
        }
        else if (nobj < 64) objs[nobj++] = argv[i];
    }
    if (nobj == 0) {
        fprintf(stderr, "usage: cp6link [-s schema] object.obj ... [-o out.ru]\n");
        return 1;
    }
    if (schema) load_schema(schema);
    for (i = 0; i < nobj; i++) process(objs[i]);
    relocate();
    write_rununit(out);
    if (emit_schema) write_schema(out);
    if (hi >= lo)
        fprintf(stderr, "cp6link: memory used %04X-%04X H (%d bytes) -> %s\n",
                lo, hi, hi - lo + 1, out);
    return 0;
}
