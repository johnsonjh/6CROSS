# Makefile for the CP-6 cross-assembler suite, Linux port.
#
#   make            build everything
#   make asm        build the asmz80 + asm6502 assemblers (gfortran)
#   make tools      build the C tools (cp6link ouconv sim6502 msa* asmdal bmap)
#   make test       build everything and run the test suite
#   make clean      remove build artifacts
#
# All source lives in source/; the original CP-6 sources are in .original/.

FC = gfortran
CC = cc

# Faithful-port flags: legacy fixed-form, 64-bit integers (a CP-6 36-bit
# word fits), static locals, and '$' allowed in identifiers (CLK$, DAT$).
#
# -w: the FORTRAN build was audited with warnings ON.  The only actionable
# findings were fixed (a dropped OBJOUT byte-count argument in asm6502_sif1,
# and four assumed-size dummy buffers declared (1)/(4) -> (*) in asmz80_sif2).
# The rest -- ~38 "rank mismatch (rank-1 and scalar)" notes -- are valid
# FORTRAN-77 sequence association: a 1-word scalar passed as a 1-element buffer
# to the string packers S2W/W2S/PACK (whose dummy is assumed-size W(*)), used
# as a scalar elsewhere.  gfortran 14 has no targeted flag for these (only -w;
# even -std=legacy implies -fallow-argument-mismatch and still warns), so -w
# is the only way to suppress the irreducible idiom.  Drop -w to re-audit.
#
# Alternative Fortran compilers are supported:
#
# flang / flang-new (LLVM):
#   make FC=flang \
#     FFLAGS="-ffixed-form -ffixed-line-length=1000 -fdefault-integer-8 -fno-automatic -w"
#
# NVFORTRAN (NVIDIA HPC SDK):
#   make FC=nvfortran \
#     FFLAGS="-Mfixed -i8 -Msave -Mextend -w"
#
# Oracle Developer Studio f90 (or f77):
#   make FC=f90 \
#     FFLAGS="-fixed -e -xtypemap=integer:64 -w"

FFLAGS  = -std=legacy -ffixed-form -ffixed-line-length-none \
          -fdefault-integer-8 -fno-automatic -fno-range-check -fdollar-ok \
          -O3 -w

CFLAGS  = -std=gnu9x -Wall -O3

SRCDIR  = source
OBJDIR  = build
INC     = $(SRCDIR)/asmz80_c1.finc $(SRCDIR)/asm6502_c1.finc

# Link order: leaf utilities, then BLOCK DATA, driver and support.
FOBJS = $(OBJDIR)/shifta.o      $(OBJDIR)/cp6_compat.o \
        $(OBJDIR)/cp6_init.o    $(OBJDIR)/cp6_io.o \
        $(OBJDIR)/cp6_startup.o $(OBJDIR)/asmz80_sif0.o \
        $(OBJDIR)/asmz80_sif1.o $(OBJDIR)/asmz80_sif2.o

all: asm tools

asm: asmz80 asm6502

asmz80: $(FOBJS)
	$(FC) $(FFLAGS) -o $@ $(FOBJS)

# ASM6502 reuses the shared support (asmz80_sif2) and the compat/io/startup
# layers; only its BLOCK DATA, driver and opcode-init are 6502-specific.
A6OBJS = $(OBJDIR)/shifta.o        $(OBJDIR)/cp6_compat.o \
         $(OBJDIR)/cp6_io.o        $(OBJDIR)/cp6_startup.o \
         $(OBJDIR)/cp6_init_6502.o $(OBJDIR)/asm6502_sif0.o \
         $(OBJDIR)/asm6502_sif1.o  $(OBJDIR)/asmz80_sif2.o

asm6502: $(A6OBJS)
	$(FC) $(FFLAGS) -o $@ $(A6OBJS)

$(OBJDIR)/%.o: $(SRCDIR)/%.f $(INC) | $(OBJDIR)
	$(FC) $(FFLAGS) -I$(SRCDIR) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

tools: cp6link ouconv bin2hex sim6502 msaz80 msa6502 msa6800 msa8085 msa8748 asmdal bmap

cp6link: $(SRCDIR)/cp6link.c
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/cp6link.c

# ASMDAL: two-pass DAL (PDP-10) assembler, C port of ASMDAL_SI61 (PL/6).
# asmdal_tables.h is generated from the PL/6 source by asmdal_tables.py.
asmdal: $(SRCDIR)/asmdal.c $(SRCDIR)/asmdal_tables.h
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/asmdal.c

# BMAP: GMAP (Honeywell DPS-8, 36-bit) macro assembler, C port of BMAP_SI61
# (PL/6).  Assembles to an octal listing and a relocatable object unit; full
# instruction set + macros + literals.  bmap_opcodes.h and bmap_asciitbl.h are
# generated from .original/BMAP_DA2.XSI / BMAP_DA1.XSI by their committed .py
# generators.  See source/BMAP_NOTES.md.
bmap: $(SRCDIR)/bmap.c $(SRCDIR)/bmap_opcodes.h $(SRCDIR)/bmap_asciitbl.h
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/bmap.c -lm

ouconv: $(SRCDIR)/ouconv.c
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/ouconv.c

# bin2hex: wrap a raw binary (e.g. a CP/M .COM) in the Intel HEX object format
# the MSA disassemblers read, so they can disassemble arbitrary files.
bin2hex: $(SRCDIR)/bin2hex.c
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/bin2hex.c

sim6502: $(SRCDIR)/sim6502.c
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/sim6502.c

# disassemblers: one shared msa_engine.o + a per-CPU decode object
$(OBJDIR)/msa%.o: $(SRCDIR)/msa%.c $(SRCDIR)/msa.h | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

msaz80:  $(OBJDIR)/msa_engine.o $(OBJDIR)/msaz80.o
	$(CC) $(CFLAGS) -o $@ $^

msa6502: $(OBJDIR)/msa_engine.o $(OBJDIR)/msa6502.o
	$(CC) $(CFLAGS) -o $@ $^

msa6800: $(OBJDIR)/msa_engine.o $(OBJDIR)/msa6800.o
	$(CC) $(CFLAGS) -o $@ $^

msa8085: $(OBJDIR)/msa_engine.o $(OBJDIR)/msa8085.o
	$(CC) $(CFLAGS) -o $@ $^

msa8748: $(OBJDIR)/msa_engine.o $(OBJDIR)/msa8748.o
	$(CC) $(CFLAGS) -o $@ $^

# `make test` builds the full toolchain (both assemblers + the C tools) so it
# is correct from a clean tree, then runs the suite.
test: all tools
	tests/run_tests.sh

clean distclean:
	rm -rf $(OBJDIR) asmz80 asm6502 cp6link ouconv bin2hex sim6502 msaz80 msa6502 msa6800 msa8085 msa8748 asmdal bmap

.PHONY: all asm tools test clean distclean
