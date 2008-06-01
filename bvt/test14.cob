identification division.
    program-id. test-program.
environment division.

input-output section.
file-control.
	select sample-rel-file
		assign to disk
		organization is relative
		access mode is sequential.

data division.
file section.
fd sample-rel-file
	label records are standard
	value of file-id is "relative.dbf".
01 sample-rel-record.
	05	name	picture is X(25).
	05	address	picture	is X(25).
	05	aptnum	picture is 9(4).
	05	city	picture is X(15).
	05	zip	picture is 99999B9999.

working-storage section.
01	count-var	picture is 999 value is zero.

procedure division.
    display "This program will write a file with relative organization".
    display "The file will be called 'relative.dbf'".
    open input sample-rel-file.

read-loop.
    read sample-rel-record at end go to end-loop.
    display sample-rel-record.
    add 1 to count-var.
    go to read-loop.
end-loop.
    close sample-rel-file.
    display "Read ", count-var, " records".
    exit program.

