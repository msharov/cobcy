        identification division.
            program-id. test-program.
        environment division.
      * All of the above is required and
      *        no abbreviations are allowed.
        data division.
        working-storage section.
        01 varx.
            05 vary picture is 9999.
      * This is a test of expanded pictures
            05 varz picture is 9(5) value is 60.
      * This is a test of X(4) type pictures
            05 address.
                07 filler picture is X value is " ".
                07 street picture is X(20) value is "Main st.".
                07 filler picture is X value is " ".
                07 house picture is 9(6)        value is 301.
            05 name picture is X(18).
        01 city picture is X(15) value is "Baltimore".

        procedure division.
            display "This is a cobol program".
            display varx.
            move 768 to vary, varz.
            add vary to varz.
            subtract 5 from vary.
            multiply vary by 2.
            divide varz by 3.
      * This should display 768 in first two fields
            display varx.
