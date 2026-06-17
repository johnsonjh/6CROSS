      INTEGER*8 FUNCTION XSHIFTA(I, SHIFT)

      INTEGER*8 I, SHIFT
      INTEGER*8 K, TMP

      IF (SHIFT .LE. 0) THEN
         XSHIFTA = I
         RETURN
      ENDIF

      TMP = I
      DO 10 K = 1, SHIFT
         IF (TMP .EQ. 0 .OR. TMP .EQ. -1) THEN
            GOTO 20
         ENDIF

         IF (TMP .GE. 0) THEN
            TMP = TMP / 2
         ELSE
            IF (MOD(TMP, 2_8) .NE. 0) THEN
               TMP = TMP / 2 - 1
            ELSE
               TMP = TMP / 2
            ENDIF
         ENDIF
   10 CONTINUE

   20 XSHIFTA = TMP
      RETURN
      END
