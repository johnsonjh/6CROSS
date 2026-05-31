C*T***********************************************************
C*T* PEEKAT - Linux replacement for the CP-6 PL/6 SI63 shim.   *
C*T* The original fetched the option list from the run command  *
C*T* (the text between '(' and ')') in the JIT and queried the  *
C*T* listing DCB for page geometry.  Here the option text is    *
C*T* taken from the saved command-line arguments (see CP6ARG).  *
C*T* OPTLST is filled one character per word (code<<27) as the  *
C*T* assembler's STOWOP expects; W is the page width and D the  *
C*T* lines per page.                                            *
C*T***********************************************************
      SUBROUTINE PEEKAT(OPTLST,W,D)
      INTEGER*8 OPTLST(*)
      INTEGER W,D
      CHARACTER*256 OPTSTR
      COMMON /CP6OPT/ OPTSTR
      CHARACTER*256 S
      INTEGER I,L,P1,P2
C     default page geometry (overridable later if desired)
      W=80
      D=60
C     option text: the part between '(' and ')' if present,
C     otherwise the whole argument string.
      L=LEN_TRIM(OPTSTR)
      IF(L.EQ.0)THEN
         S=' '
      ELSE
         P1=INDEX(OPTSTR(1:L),'(')
         P2=INDEX(OPTSTR(1:L),')',BACK=.TRUE.)
         IF(P1.GT.0 .AND. P2.GT.P1)THEN
            S=OPTSTR(P1+1:P2-1)
            L=P2-P1-1
         ELSE
            S=OPTSTR(1:L)
         END IF
      END IF
C     pack into OPTLST (one char per word, code<<27), blank padded.
      DO I=1,128
         OPTLST(I)=ISHFT(INT(ICHAR(' '),8),27)
      END DO
      DO I=1,MIN(L,128)
         OPTLST(I)=ISHFT(INT(ICHAR(S(I:I)),8),27)
      END DO
      RETURN
      END
