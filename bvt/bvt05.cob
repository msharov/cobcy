identification division.
    program-id. test-program.

environment division.

configuration section.
source-computer.	Intel486.
object-computer.	Intel486.

input-output section.
file-control.
	select card-in		assign to card-reader.
	select print-out	assign to output-file.

data division.
file section.
fd	card-in	label	record	is omitted.
01	card-in-record.
	05	card-name	picture is 	X(35).
	05	card-social-security-number.
		10	digits-one-to-three	picture is 	X(3).
		10	digits-four-and-five	picture is 	X(2).
		10	digits-six-to-nine	picture is 	X(4).
	05	filler		picture is X(36).
fd	print-out	label	record	is omitted.
01	print-out-record.
	05	print-social-security-number.
		10	print-one	picture	is	X(3).
		10	print-two	picture	is	X.
		10	print-three	picture	is	X(2).
		10	print-four	picture	is	X.
		10	print-five	picture	is	X(4).
	05	space-x		picture	is	X(10).
	05	print-name	picture	is	X(35).

procedure division.
start-x.
    open input card-in output print-out.
loop-x.
    read card-in record at end close card-in
    	print-out stop run.
    move digits-one-to-three to print-one.
    move "-" to print-two.
    move digits-four-and-five to print-three.
    move "-" to print-four.
    move digits-six-to-nine to print-five.
    move spaces to space-x.
    move card-name to print-name.
    write print-out-record.
    display print-out-record.
    go to loop-x.

