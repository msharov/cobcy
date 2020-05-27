        identification division.
            program-id. test-program.

        environment division.

        configuration section.
        source-computer. Intel486.
        object-computer. Intel486.

        data division.

        working-storage section.
        01 counter-var picture is 999 value is zero.

        procedure division.
            display "Listing speed limits starting with zero:".
        count-loop.
            display counter-var.
            add 5 to counter-var.
            if counter-var is greater than 100 go to done-counting.
            go to count-loop.
        done-counting.
