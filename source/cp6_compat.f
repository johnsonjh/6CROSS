C*T***********************************************************
C*T* CP-6 FORTRAN compatibility layer for the Linux/gfortran  *
C*T* port of the ASMZ80 cross-assembler.                      *
C*T*                                                          *
C*T* Reproduces CP-6 FORTRAN intrinsics with exact 36-bit     *
C*T* word semantics on 64-bit host integers, plus helpers to  *
C*T* pack/unpack the 9-bit-per-character word representation   *
C*T* used throughout the assembler (4 chars per 36-bit word,  *
C*T* leftmost char in bits 27-35).                            *
C*T***********************************************************
C
C     ISL - Integer Shift Logical.  Positive count shifts left,
C     negative count shifts right with zero fill, all within a
C     36-bit word (matches CP-6 FORTRAN ISL).
C
      INTEGER*8 FUNCTION ISL(IV,ISH)
      IMPLICIT INTEGER*8 (A-Z)
      PARAMETER (M36=68719476735)
      ISL=IAND(ISHFT(IAND(IV,M36),ISH),M36)
      RETURN
      END
C
C     UCH - return the single character stored left-justified (bits
C     27-35) of word W, as CHARACTER*1.
C
      CHARACTER FUNCTION UCH(W)
      INTEGER*8 W
      UCH=CHAR(IAND(ISHFT(W,-27),255))
      RETURN
      END
C
C     ISA - Integer Shift Arithmetic.  Right shift sign-extends
C     from bit 35; left shift same as ISL.
C
      INTEGER*8 FUNCTION ISA(IV,ISH)
      IMPLICIT INTEGER*8 (A-Z)
      PARAMETER (M36=68719476735)
      PARAMETER (SB =34359738368)
      IF (ISH.GE.0) THEN
         ISA=IAND(ISHFT(IAND(IV,M36),ISH),M36)
      ELSE
         V=IAND(IV,M36)
         IF (IAND(V,SB).NE.0) V=IOR(V,NOT(M36))
         ISA=IAND(XSHIFTA(V,-ISH),M36)
      END IF
      RETURN
      END
C
C     ISC - Integer Shift Circular (rotate) within a 36-bit word.
C
      INTEGER*8 FUNCTION ISC(IV,ISH)
      IMPLICIT INTEGER*8 (A-Z)
      PARAMETER (M36=68719476735)
      V=IAND(IV,M36)
      N=MOD(ISH,36_8)
      IF (N.LT.0) N=N+36
      ISC=IAND(IOR(ISHFT(V,N),ISHFT(V,N-36)),M36)
      RETURN
      END
C
C     INOT / ICOMPL - 36-bit one's complement.  IEXCLR - exclusive OR.
C
      INTEGER*8 FUNCTION INOT(IV)
      IMPLICIT INTEGER*8 (A-Z)
      PARAMETER (M36=68719476735)
      INOT=IAND(NOT(IV),M36)
      RETURN
      END
      INTEGER*8 FUNCTION ICOMPL(IV)
      IMPLICIT INTEGER*8 (A-Z)
      PARAMETER (M36=68719476735)
      ICOMPL=IAND(NOT(IV),M36)
      RETURN
      END
      INTEGER*8 FUNCTION IEXCLR(IA,IB)
      IMPLICIT INTEGER*8 (A-Z)
      IEXCLR=IEOR(IA,IB)
      RETURN
      END
C
C     ACPU - return total CPU time in seconds (REAL), as CP-6 ACPU.
C
      SUBROUTINE ACPU(T)
      REAL T
      CALL CPU_TIME(T)
      RETURN
      END
C
C     CLK_ - store the time of day as 8 characters "HH:MM:SS"
C     packed (4 chars/word) into TIME(2).
C
      SUBROUTINE CLK_(TIME)
      INTEGER*8 TIME(2)
      CHARACTER*8 S
      INTEGER V(8)
      CALL DATE_AND_TIME(VALUES=V)
      WRITE(S,900) V(5),V(6),V(7)
  900 FORMAT(I2.2,':',I2.2,':',I2.2)
      CALL S2W(S,2,4,TIME)
      RETURN
      END
C
C     DAT_ - store the date as 8 characters "MM/DD/YY" packed into
C     DATE(2).
C
      SUBROUTINE DAT_(DATE)
      INTEGER*8 DATE(2)
      CHARACTER*8 S
      INTEGER V(8)
      CALL DATE_AND_TIME(VALUES=V)
      WRITE(S,900) V(2),V(3),MOD(V(1),100)
  900 FORMAT(I2.2,'/',I2.2,'/',I2.2)
      CALL S2W(S,2,4,DATE)
      RETURN
      END
C
C     S2W - pack a character string into NW words, CPW chars per word,
C     leftmost char in bits 27-35 (CP-6 9-bit packing).
C
      SUBROUTINE S2W(S,NW,CPW,W)
      CHARACTER*(*) S
      INTEGER*8 W(*),WORD
      INTEGER NW,CPW,I,K,P
      P=0
      DO I=1,NW
         WORD=0
         DO K=1,CPW
            P=P+1
            WORD=IOR(WORD,ISHFT(INT(ICHAR(S(P:P)),8),27-9*(K-1)))
         END DO
         W(I)=WORD
      END DO
      RETURN
      END
C
C     W2S - unpack NW words (CPW chars per word, leftmost char in
C     bits 27-35) into a character string.
C
      SUBROUTINE W2S(W,NW,CPW,S)
      INTEGER*8 W(*),C
      CHARACTER*(*) S
      INTEGER NW,CPW,I,K,P
      P=0
      DO I=1,NW
         DO K=1,CPW
            P=P+1
            C=IAND(ISHFT(W(I),-(27-9*(K-1))),511_8)
            S(P:P)=CHAR(IAND(C,255_8))
         END DO
      END DO
      RETURN
      END
