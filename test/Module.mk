################ Source files ##########################################

test/cobs	:= $(sort $(wildcard test/test*.cob))
test/srcs	:= $(addprefix $O,${test/cobs:.cob=.c})
test/tests	:= $(addprefix $O,${test/cobs:.cob=})
test/objs	:= ${test/srcs:.c=.o}
test/deps	:= ${test/objs:.o=.d}
test/sep	:= "------------------------------------------------------------"

################ Compilation ###########################################

.PHONY:		check test/run test/clean
.SECONDARY:	${test/srcs}

test/all:	${test/tests}

check:	test/run
test/run:	${test/tests}
	@cp test/card-reader $Otest/;\
	for i in $(notdir ${test/tests}); do\
	    echo "Running $$i";		echo ${test/sep} > $Otest/$$i.out;		\
	    cat $Otest/$$i.c >> $Otest/$$i.out;	echo ${test/sep} >> $Otest/$$i.out;	\
	    (cd $Otest; ./$$i >> $$i.out 2>&1);	echo ${test/sep} >> $Otest/$$i.out;	\
	    diff test/$$i.std $Otest/$$i.out;\
	done

${test/tests}:	$Otest/test%:	$Otest/test%.o ${liba}
	@echo "Linking $@ ..."
	@${CC} ${ldflags} -o $@ $^

$O%.c:	%.cob | ${exe}
	@echo "    Converting $< ..."
	@${exe} -o $@ $<

$Otest/%.o:	$Otest/%.c
	@echo "    Compiling $< ..."
	@${CC} ${cflags} -I. -MMD -MT "$(<:.c=.s) $@" -o $@ -c $<

clean:	test/clean
test/clean:
	@if [ -d ${builddir}/test ]; then\
	    rm -f ${test/tests} ${test/objs} ${test/deps} ${test/srcs} $Otest/card-reader $Otest/output-file $Otest/*.dbf $Otest/*.ndx $Otest/*.out $Otest/.d;\
	    rmdir ${builddir}/test;\
	fi

${test/srcs}:	Makefile test/Module.mk ${confs} $Otest/.d

-include ${test/deps}
