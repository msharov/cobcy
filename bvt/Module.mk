################ Source files ##########################################

bvt/COBS:= $(sort $(wildcard bvt/bvt*.cob))
bvt/SRCS:= $(addprefix $O,${bvt/COBS:.cob=.c})
bvt/BVTS:= $(addprefix $O,${bvt/COBS:.cob=})
bvt/OBJS:= ${bvt/SRCS:.c=.o}
bvt/DEPS:= ${bvt/OBJS:.o=.d}
bvt/LIBS:= ${COBLIB}
CFLAGS	+= -I.
bvt/SEP	:= "------------------------------------------------------------"

################ Compilation ###########################################

.PHONY:	check bvt/run bvt/clean bvt/standardize

.SECONDARY: ${bvt/SRCS}

bvt/all:	${bvt/BVTS}

check:	bvt/run
bvt/run:	${bvt/BVTS}
	@cp bvt/card-reader $Obvt/;\
	for i in $(notdir ${bvt/BVTS}); do\
	    echo "Running $$i";		echo ${bvt/SEP} > $Obvt/$$i.out;		\
	    cat $Obvt/$$i.c >> $Obvt/$$i.out;	echo ${bvt/SEP} >> $Obvt/$$i.out;	\
	    (cd $Obvt; ./$$i >> $$i.out 2>&1);	echo ${bvt/SEP} >> $Obvt/$$i.out;	\
	    diff bvt/$$i.std $Obvt/$$i.out;\
	done

bvt/standardize:
	@for i in $(notdir ${bvt/BVTS}); do\
	    cp -f $Obvt/$$i.out bvt/$$i.std;\
	done

${bvt/BVTS}:	$Obvt/bvt%:	$Obvt/bvt%.o ${COBLIB}
	@echo "Linking $@ ..."
	@${LD} ${LDFLAGS} -o $@ $< ${bvt/LIBS}

$O%.c:	%.cob ${EXE}
	@echo "    Converting $< ..."
	@${EXE} -o $@ $<

$Obvt/%.o:	$Obvt/%.c
	@echo "    Compiling $< ..."
	@${CC} ${CFLAGS} -MMD -MT "$(<:.c=.s) $@" -o $@ -c $<

clean:	bvt/clean
bvt/clean:
	@if [ -d $Obvt ]; then\
	    rm -f ${bvt/BVTS} ${bvt/OBJS} ${bvt/DEPS} ${bvt/SRCS} $Obvt/card-reader $Obvt/output-file $Obvt/*.dbf $Obvt/*.ndx $Obvt/*.out $Obvt/.d;\
	    rmdir $Obvt;\
	fi

$Obvt/.d:	$O.d
	@[ -d $Obvt ] || mkdir $Obvt
	@touch $@

${bvt/SRCS}:	${MKDEPS} bvt/Module.mk $Obvt/.d
-include ${bvt/DEPS}
