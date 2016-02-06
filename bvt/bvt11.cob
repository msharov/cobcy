        identification division.
            program-id. test-program.
        environment division.
        data division.
        working-storage section.
            01 name       pic XXXXX.
            01 name-print pic X0BX.
            01 name-3     pic B(3)X.
            01 getal      pic 9,999.
            01 getal2     pic 999v99.
            01 teller     pic 99.

        procedure division.
            Move "a cde" to name.
            Move name to name-3.
            Move name to name-print.
            move 5000 to getal.
            Move 53 to getal2.
            add 5 to getal.
            Display name.
            Display name-print.
            Display name-3.
            display getal.
            display getal2.
            display QUOTE "hello" QUOTES Quote quotes.
            Display all "hello all".
            Move 7 to teller.
            if teller = 7 and teller is equal to 10
                display "yes".
            If name is alphabetic
                move 12 to teller.
            if teller is equal to 12
                Display "teller is 12"
            else
                Display "teller is not 12 !".
            stop run.
