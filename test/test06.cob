        identification division.
            program-id. test-program.
        environment division.
        data division.
        working-storage section.
        01 various-numbers.
            05 entry-one   pic is 9,999.99 value is 12345.
            05 filler      pic is X value is space.
            05 entry-two   pic is 9,999.ZZ value is 12345.
            05 filler      pic is X value is space.
            05 entry-three pic is 9999.99 value is 12345.
            05 filler      pic is X value is space.
            05 entry-four  pic is 9,999.99 value is 12.3.
            05 filler      pic is X value is space.
            05 entry-five  pic is $9,999,999.99 value is 239847.
        01 other-numbers.
            05 other-one   pic is 9,999V99 value is 12345.
            05 filler      pic is X value is space.
            05 other-two   pic is 9,999V00 value is 12345.
            05 filler      pic is X value is space.
            05 other-three pic is 9999V99 value is 12345.
            05 filler      pic is X value is space.
            05 other-four  pic is 9,999V99 value is 12.3.
            05 filler      pic is X value is space.
            05 other-five  pic is $9,999,999V99 value is 239847.

        procedure division.
            display " ,123.45  ,123.00  123.45  ,   .12 $ ,  2,398.47".
            display "------------------------------------------------".
            display various-numbers.
            display " ".
            display "2,345.00 2,345.00 2345.00  , 12.30 $ ,239,847.00".
            display "------------------------------------------------".
            display other-numbers.
