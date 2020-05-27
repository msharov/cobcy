identification division.
    program-id. test-program.
environment division.

input-output section.
file-control.
	select sample-idx-file
		assign to disk
		organization is indexed
		record key is aptnum
		access mode is random.

data division.
file section.
fd sample-idx-file
	label records are standard
	value of file-id is "indexed.dbf".
01 sample-idx-record.
	05	name	picture is X(25).
	05	address	picture	is X(25).
	05	aptnum	picture is 9(4).
	05	city	picture is X(15).
	05	zip	picture is 99999B9999.

working-storage section.
01	count-var	picture is 999 value is 1.

procedure division.
    display "This program will read a file with indexed organization".
    display "The file is called 'indexed.dbf'".
    open input sample-idx-file.

read-loop.
    * count-var is the key
    move count-var to aptnum.
    * No at end clause here because it is random access
    read sample-idx-record.
    display sample-idx-record.
    if count-var is not less than 10 go to end-loop.
    add 1 to count-var.
    go to read-loop.
end-loop.
    close sample-idx-file.
    display "Read ", count-var, " records".
    exit program.

