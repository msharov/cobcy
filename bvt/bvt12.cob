        identification division.
            program-id. test-program.
        environment division.
        data division.
        working-storage section.
        01 varx.
            05 vary picture is 9999.
            05 varz picture is 9(4) value is 60.
            05 tok1 picture is X(18).

        procedure division.
            move 8 to vary.
            if vary = 8
               then move "vary is eight" to tok1
                    move 55 to varz
                    if varz = 55
                       then display varz
                            display tok1.
            compute varz = (6 + vary) / 5.
            display varz, " is not rounded".
            compute varz rounded = (6 + vary) / 5.
            display varz, " is rounded".
