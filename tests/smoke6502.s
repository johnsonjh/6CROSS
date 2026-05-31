        ORG 0200H
START   LDA #5
        STA 10H
        LDX #0
        INX
        CPX #10
        BNE START
        BRK
        END
