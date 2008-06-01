identification division.
    program-id. test-program.
environment division.
data division.
working-storage section.
01	varx 	occurs 10 times indexed by varx-index.
	05	vary	picture is 9999.
	05	varz	picture is 9(4) value is 60.
	05	varl	occurs 50 times indexed by varl-index.
		10	varl1	occurs 10 times indexed by varl1-index
				picture is 9(7) value is 10.
		10	varl2	occurs 13 times indexed by varl2-index
				picture is 9(6) value is 13.
	05	name	picture is X(18).
01	varx-index	picture is 99 value 0.
01	varl-index	picture is 99 value 0.
01	varl1-index	picture is 99 value 0.
01	varl2-index	picture is 99 value 0.

procedure division.
    display "This is a cobol program testing arrays".
    compute varx-index = 0.
    display "varx[0] = ", varx.

