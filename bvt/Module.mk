################ Source files ##########################################

bvt/COBS:= $(sort $(wildcard bvt/bvt*.cob))
bvt/SRCS:= $(addprefix $O,${bvt/COBS:.cob=.c})
bvt/INCS:= $(addprefix $O,${bvt/COBS:.cob=.h})
bvt/BVTS:= $(addprefix $O,${bvt/COBS:.cob=})
bvt/OBJS:= ${bvt/SRCS:.c=.o}
bvt/DEPS:= ${bvt/OBJS:.o=.d}
bvt/LIBS:= ${COBLIB}
CFLAGS	+= -I.
bvt/SEP	:= "------------------------------------------------------------"

################ Compilation ###########################################

.PHONY:	check bvt/run bvt/clean

.SECONDARY: ${bvt/INCS} ${bvt/SRCS}

bvt/all:	${bvt/BVTS}

check:	bvt/run
bvt/run:	${bvt/BVTS}
	@(cd bvt; for i in $(notdir ${bvt/BVTS}); do			\
	    echo "Running $$i";		echo ${bvt/SEP} > $$i.out;	\
	    cat $$i.h >> $$i.out;	echo ${bvt/SEP} >> $$i.out;	\
	    cat $$i.c >> $$i.out;	echo ${bvt/SEP} >> $$i.out;	\
	    ./$$i >> $$i.out;		echo ${bvt/SEP} >> $$i.out;	\
	    diff $$i.std $$i.out && rm -f $$i.h $$i.c $$i.out;		\
	done)

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
	    rm -f ${bvt/BVTS} ${bvt/OBJS} ${bvt/DEPS} ${bvt/SRCS} ${bvt/INCS} $Obvt/output-file $Obvt/*.dbf $Obvt/*.ndx $Obvt/.d;\
	    rmdir $Obvt;\
	fi

$Obvt/.d:	$O.d
	@[ -d $Obvt ] || mkdir $Obvt
	@touch $@

${bvt/SRCS}:	${MKDEPS} bvt/Module.mk $Obvt/.d
-include ${bvt/DEPS}
