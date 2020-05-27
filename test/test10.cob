        identification division.
            program-id. test-program.
        environment division.
        data division.
        working-storage section.
            01 counter     picture 9(5) value is 0.
            01 some-number picture 9(9) value is 1.
        procedure division.
            display "Should get 4, 32, 8, 16384".
            perform proc-double 2 times.
            display some-number.
            move 1 to some-number.
            perform proc-double varying counter from 1 to 5.
            display some-number.
            move 1 to some-number.
            perform proc-double varying counter from 1 to 6 by 2.
            display some-number.
            move 1 to some-number.
            perform proc-double
                varying counter from 1 by 2
                until some-number > 10000.
            display some-number.
            display "All done".
            stop run.
        proc-double.
            multiply some-number by 2.
