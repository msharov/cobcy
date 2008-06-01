identification division.
    program-id. test-program.
environment division.
data division.
working-storage section.
01	varx.
	05	vary	picture is 9999.
	05	varz	picture is 9(4) value is 60.
	05	tok1	picture is X(18).

procedure division.
    move 8 to vary.
    if vary = 8 then
	display "if1-1"
	display "if1-2"
	if vary = 8 then
	    display "if2-1"
	    if varz = 60 then
		display "if3-1"
	    end-if
	    display "if2-2"
	else
	    display "if2-3"
	* Only outermost IFs can end in a period.
	* all inner IFs must end in an end-if
	* The reason is syntax ambiguity:
	* is that next elsif a part of the outer or the inner IF?
	end-if
    elsif varz = 60
        display "if1-3"
    else
	display "if1-4".
    if vary = 8
	display "if1-5"
	if vary = 8
	    display "if4-1".

