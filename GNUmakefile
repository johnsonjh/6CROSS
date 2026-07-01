# Makefile for the CP-6 cross-assembler suite, Linux port.
#
#   make            build everything
#   make asm        build the asmz80 + asm6502 assemblers (default: gfortran;
#                   use FC=ifx/flang/nvfortran/f90/lfortran for others)
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
# Intel IFX or IFORT: make FC=ifx
#
# LLVM flang: make FC=flang
#
# NVIDIA FORTRAN: make FC=nvfortran
#
# Oracle FORTRAN: make FC=f90
#
# Work-in-progress:
#   LFortran (currently still has problems with v0.63.0-881)
#     make FC=lfortran \
#       FFLAGS="--fixed-form -fdefault-integer-8 --std=legacy"

FFLAGS  = -std=legacy -ffixed-form -ffixed-line-length-none \
          -fdefault-integer-8 -fno-automatic -fno-range-check -fdollar-ok \
          -O3 -w

# Compute a basename for more reliable matching when users pass FC as an
# absolute path (e.g. FC=/opt/.../nvfortran) rather than just the command name.
FC_BASENAME := $(notdir $(FC))

# Provide compiler-appropriate FFLAGS when a non-default FC is chosen on
# the command line and the user did not also pass an explicit FFLAGS=.
# This makes "make FC=ifx" (etc.) work out of the box.  FC may be a bare
# command name or a full path (e.g. FC=/opt/nvidia/.../nvfortran); we match
# on both $(FC) and $(FC_BASENAME).
ifneq ($(origin FFLAGS),command line)
  ifneq (,$(findstring ifx,$(FC))$(findstring ifort,$(FC))$(findstring ifx,$(FC_BASENAME))$(findstring ifort,$(FC_BASENAME)))
    FFLAGS := -fixed -i8 -save -extend-source 132 -O3 -w
  else ifneq (,$(findstring flang,$(FC))$(findstring flang,$(FC_BASENAME)))
    FFLAGS := -ffixed-form -ffixed-line-length=1000 -fdefault-integer-8 -fno-automatic -w
  else ifneq (,$(findstring nvfortran,$(FC))$(findstring nvfortran,$(FC_BASENAME))$(findstring pgf90,$(FC))$(findstring pgf90,$(FC_BASENAME)))
    FFLAGS := -Mfixed -i8 -Msave -Mextend -O3 -w
  else ifneq (,$(findstring f90,$(FC))$(findstring f90,$(FC_BASENAME))$(findstring f95,$(FC))$(findstring f95,$(FC_BASENAME)))
    FFLAGS := -fixed -e -xtypemap=integer:64 -O3 -w
  else ifneq (,$(findstring lfortran,$(FC))$(findstring lfortran,$(FC_BASENAME)))
    FFLAGS := --fixed-form -fdefault-integer-8 --std=legacy
  endif
endif

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

# Special handling for LFortran (alpha) which requires INTEGER*4 for the
# COMMAND_ARGUMENT_COUNT/GET_COMMAND_ARGUMENT position argument when
# -fdefault-integer-8 is active. We auto-patch a temp copy of cp6_io.f
# in the $(OBJDIR) but only when the FC basename contains "lfortran".
$(OBJDIR)/cp6_io.o: $(SRCDIR)/cp6_io.f $(INC) | $(OBJDIR)
	@if echo "$(notdir $(FC))" | grep -qi lfortran; then \
	  sed -e 's/      INTEGER NARG,I,L,IDOT/      INTEGER*4 NARG4,I4\n      INTEGER NARG,I,L,IDOT/' \
	      -e 's/NARG=COMMAND_ARGUMENT_COUNT()/NARG4=COMMAND_ARGUMENT_COUNT()\n      NARG=NARG4/' \
	      -e 's/CALL GET_COMMAND_ARGUMENT(1,SRCNAM)/I4=1\n      CALL GET_COMMAND_ARGUMENT(I4,SRCNAM)/' \
	      -e 's/         CALL GET_COMMAND_ARGUMENT(I,ARG)/         I4 = I\n         CALL GET_COMMAND_ARGUMENT(I4,ARG)/' \
	      $(SRCDIR)/cp6_io.f > $(OBJDIR)/cp6_io_lf.f ; \
	  $(FC) $(FFLAGS) -I$(SRCDIR) -c $(OBJDIR)/cp6_io_lf.f -o $@ ; \
	else \
	  $(FC) $(FFLAGS) -I$(SRCDIR) -c $< -o $@ ; \
	fi

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

scc: README.md
	"$${MAKE:-$(MAKE)}" distclean
	awk '/<!-- scc-start -->/ { \
		print; system("scc \
			--count-as-pattern \"*.f:FORTRAN:FORTRAN Legacy\" \
			--exclude-ext md --count-as \"gmap:Assembly\" \
			--count-as \"dal:Assembly\" --count-as \"z80:Assembly\" \
			--exclude-dir .original --exclude-file README.awk \
			--no-cocomo -u --no-size -s lines -f html-table; \
			printf \"\n%s\n\" \"<!-- scc-end -->\""); \
			skip=1; next } \
		skip && /<!-- scc-end -->/ { skip=0; next } \
		!skip' README.md > README.awk && \
	mv -f README.awk README.md && \
	expand README.md > README.out && \
	mv -f README.out README.md

################################################################################

.PHONY: all asm tools test clean distclean scc
