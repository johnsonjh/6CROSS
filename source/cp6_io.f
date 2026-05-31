C*T***********************************************************
C*T* cp6_io - I/O abstraction for the ASMZ80 Linux/gfortran    *
C*T* port.  Replaces CP-6 monitor file services and the        *
C*T* A-format-of-packed-word convention with portable          *
C*T* gfortran I/O.  Source/listing/object files come from the  *
C*T* command line; the assembler's character buffers are       *
C*T* packed 1 char per word (code<<27) by S2W/W2S.             *
C*T***********************************************************
C
C     CP6ARG - parse the command line, derive file names, and open
C     the source (105), listing (106) units.  The option text (the
C     remaining arguments) is saved for PEEKAT/GETOPT.
C
      SUBROUTINE CP6ARG
      CHARACTER*128 SRCNAM,LSTNAM,OBJNAM
      COMMON /CP6FN/ SRCNAM,LSTNAM,OBJNAM
      CHARACTER*256 OPTSTR
      COMMON /CP6OPT/ OPTSTR
      CHARACTER*256 ARG,BASE
      INTEGER NARG,I,L,IDOT
      NARG=COMMAND_ARGUMENT_COUNT()
      IF(NARG.LT.1)THEN
         WRITE(0,'(A)')'usage: asmz80 source.z80 [options]'
         STOP 1
      END IF
      CALL GET_COMMAND_ARGUMENT(1,SRCNAM)
C     remaining arguments form the CP-6 option string
      OPTSTR=' '
      L=0
      DO I=2,NARG
         CALL GET_COMMAND_ARGUMENT(I,ARG)
         OPTSTR(L+1:)=ARG
         L=L+LEN_TRIM(ARG)+1
      END DO
C     derive listing/object names from the source base name
      BASE=SRCNAM
      IDOT=INDEX(BASE,'.',BACK=.TRUE.)
      IF(IDOT.GT.1)BASE=BASE(1:IDOT-1)
      LSTNAM=TRIM(BASE)//'.lst'
      OBJNAM=TRIM(BASE)//'.obj'
      OPEN(105,FILE=TRIM(SRCNAM),STATUS='OLD',FORM='FORMATTED')
      OPEN(106,FILE=TRIM(LSTNAM),STATUS='REPLACE',FORM='FORMATTED')
      RETURN
      END
C
C     RDLIN - read one text line from UNIT, packing it 1 char/word
C     (code<<27, blank padded) into W(1..N).  IEOF=1 at end of file.
C
      SUBROUTINE RDLIN(UNIT,N,W,IEOF)
      INTEGER UNIT,N,IEOF
      INTEGER*8 W(*)
      CHARACTER*512 LINE
      IEOF=0
      LINE=' '
      READ(UNIT,'(A)',END=900)LINE
      CALL S2W(LINE,N,1,W)
      RETURN
  900 IEOF=1
      RETURN
      END
C
C     RDLINR - direct-access version of RDLIN, record REC.  IERR=1
C     on error.
C
      SUBROUTINE RDLINR(UNIT,REC,N,W,IERR)
      INTEGER UNIT,REC,N,IERR
      INTEGER*8 W(*)
      CHARACTER*512 LINE
      IERR=0
      LINE=' '
      READ(UNIT,'(A)',REC=REC,ERR=900)LINE
      CALL S2W(LINE,N,1,W)
      RETURN
  900 IERR=1
      RETURN
      END
C
C     WRLIN - unpack W(1..N) (1 char/word) and write as a text line.
C
      SUBROUTINE WRLIN(UNIT,N,W)
      INTEGER UNIT,N
      INTEGER*8 W(*)
      CHARACTER*512 LINE
      CALL W2S(W,N,1,LINE)
      WRITE(UNIT,'(A)')LINE(1:N)
      RETURN
      END
C
C     WRLINR - direct-access version of WRLIN, record REC.
C
      SUBROUTINE WRLINR(UNIT,REC,N,W)
      INTEGER UNIT,REC,N
      INTEGER*8 W(*)
      CHARACTER*512 LINE
      CALL W2S(W,N,1,LINE)
      WRITE(UNIT,'(A)',REC=REC)LINE(1:N)
      RETURN
      END
C
C     WROBJ - write a FLUSH object-unit record: ':' followed by two hex
C     characters for each of the NB words in BUF, then the checksum X.
C
      SUBROUTINE WROBJ(UNIT,BUF,NB,X)
      INTEGER UNIT,NB
      INTEGER*8 BUF(*),X
      CHARACTER*120 OLINE
      CALL W2S(BUF,NB,2,OLINE)
      CALL W2S(X,1,2,OLINE(2*NB+1:2*NB+2))
      WRITE(UNIT,'(A)')':'//OLINE(1:2*NB+2)
      RETURN
      END
C
C     WRLST - write a listing line from LOBUF (4 chars/word), N words,
C     with a leading blank (carriage control).
C
      SUBROUTINE WRLST(UNIT,LOBUF,N)
      INTEGER UNIT,N
      INTEGER*8 LOBUF(*)
      CHARACTER*160 OLINE
      CALL W2S(LOBUF,N,4,OLINE)
      WRITE(UNIT,'(1X,A)')OLINE(1:4*N)
      RETURN
      END
C
C     WRHDR - write a listing page header: time, date, page number and
C     title (N single-char words).  Leading '1' = new-page control.
C
      SUBROUTINE WRHDR(UNIT,TIME,DATE,PAGE,TITLE,N)
      INTEGER UNIT,PAGE,N
      INTEGER*8 TIME(*),DATE(*),TITLE(*)
      CHARACTER*8 TS,DS
      CHARACTER*160 TT
      CALL W2S(TIME,2,4,TS)
      CALL W2S(DATE,2,4,DS)
      CALL W2S(TITLE,N,1,TT)
      WRITE(UNIT,900)TS,DS,PAGE,TT(1:N)
  900 FORMAT('1',A8,1X,A8,1X,'PAGE',I4,1X,A)
      RETURN
      END
