identification division.
    program-id. test-program.

environment division.

configuration section.
source-computer.	Intel486.
object-computer.	Intel486.

input-output section.
file-control.
	select print-out	assign to output-file.

data division.
file section.
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

working-storage section.
01	weird-record.
	05	id-number	pic	is	999.
	05	filler		pic	is	X	value is space.
	05	name		pic 	is 	X(15).
	05	filler		pic	is	X	value is space.
	05	address		pic 	is 	X(35).
01	counter			pic 	is 	9(3).

procedure division.
start-x.
    open output print-out.
    move 0 to counter.
loop-x.
    add 1 to counter.
    if counter is greater than 50 stop run.

    move counter to id-number.
    move "Weirdo" to name.
    move "nowhere.loopback.edu" to address.

    write print-out-record from weird-record.
    go to loop-x.

