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
    display "This program will write a file with indexed organization".
    display "The file will be called 'indexed.dbf'".
    open output sample-idx-file.

write-loop.
    move "Whoever" to name.
    move "Wherever" to address.
    move count-var to aptnum.
    move "I don't know" to city.
    move 837480985 to zip.
    write sample-idx-record.
    add 1 to count-var.
    if count-var is greater than 10 go to end-loop.
    go to write-loop.
end-loop.
    close sample-idx-file.
    display "All done.".
    exit program.

