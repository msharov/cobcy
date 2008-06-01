identification division.
	program-id. Hello-World.
environment division.
data division.
procedure division.
	display "Hello world from a perform thru test program!".
	display " The following should perform proc1 thru proc3, no proc4".
	perform proc1 thru proc3.
	display " test recursion: call proc2 thru proc7 with proc5".
	display "    calling proc6 thru proc 8".
	perform proc2 thru proc7.
	stop run.
proc1.
	display "proc1".
proc2.
	display "proc2".
proc3.
	display "proc3".
proc4.
	display "proc4".
proc5.
	display "proc5".
	perform proc6 thru proc8.
proc6.
	display "proc6".
proc7.
	display "proc7".
proc8.
	display "proc8".

