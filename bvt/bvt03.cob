identification division.
    program-id. test-program.
environment division.
data division.
working-storage section.
01	varx.
	05	vary	picture is 9999 value is zeroes.
	05	varz	picture is 9(5) value is zeros.
	05	varf	picture is 9(5) value is zero.
	05	filler	picture is X value is " ".
	05	name	picture is X(18) value is spaces.
	05	filler	picture is X value is " ".
	05	names	picture is X(18) value is space.
	05	filler	picture is X value is " ".
	05	city	picture is X(15) value is "Blacksburg".

procedure division.
    display "This is a cobol program".
    display varx.
    move 5 to vary, varz.
    move "Mike" to name, names.
    display varx.

