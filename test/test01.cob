        identification division.
            program-id. test-program.
        environment division.
        data division.
        working-storage section.
        01 varx.
            05 vary picture is 9999.
            05 varz picture is 9(4) value is 60.
            05 name picture is X(18) value is "Hello world!".

        procedure division.
            display "This is a cobol program".
            display varx.
