        identification division.
            program-id. test-program.
        environment division.
        data division.
        working-storage section.
            01 var1 pic 9(4) value 10.
            01 var2 pic 9(4) value 11.
            01 var3 pic 9(4).

        procedure division.
            add var1 to var2 giving var3.
            display "And the answer is (10 + 11 = 21):".
            display var3.

            move 3 to var1.
            move 5 to var2.
            divide var1 by var2 giving var3.
            display "3 / 5 unrounded = ", var3.
            divide var1 by var2 giving var3 rounded.
            display "3 / 5 rounded = ", var3.
