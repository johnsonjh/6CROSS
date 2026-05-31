        ORG 0200H
        LDA #0
        LDX #5
LOOP    CLC
        ADC #3
        DEX
        BNE LOOP
        STA 80H
        BRK
        END
