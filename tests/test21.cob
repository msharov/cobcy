identification division.
    program-id. test-program.
environment division.
data division.
working-storage section.
01	charstr	picture is X(15) value is "Blacksburg".
01	intstr	picture is 9(15) value is 12345.
01	floatstr	picture is 9(7)V9(7) value is 123.45.

screen section.
01	test-screen.
	05	line 5 column 10 pic X(15) using charstr.
	05	line 6 column 10 pic 9(15) using intstr.
	05	line 7 column 10 pic 9(7)V9(7) using floatstr.

procedure division.
	accept test-screen.
	stop run.

