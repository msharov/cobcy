identification division.
	program-id. Hello-World.
environment division.
data division.
working-storage section.
01	varx	picture is 9(4).
procedure division.
	display (15,3) "Hello world!".
	display (17,10) "Hello world!".
	display (4,20) "Hello world!".
	display (19,34) "Hello world!".
	display (15,36) "Hello world!".
	display (5,22) "Type a number: ".
	accept (20,22) varx from console.
	display (5,23) "You typed ", varx, "!".

