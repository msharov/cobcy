################ Source files ##########################################

bvt/COBS:= $(wildcard bvt/bvt*.cob)
bvt/SRCS:= $(bvt/COBS:.cob=.c)
bvt/INCS:= $(bvt/COBS:.cob=.h)
bvt/OBJS:= $(bvt/SRCS:.c=.o)
bvt/BVTS:= $(bvt/COBS:.cob=)
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
	    diff $$i.std $$i.out && rm $$i.out;				\
	done)

${bvt/BVTS}:	bvt/bvt%:	$Obvt/bvt%.o ${COBLIB}
	@echo "Linking $@ ..."
	@${LD} ${LDFLAGS} -o $@ $< ${bvt/LIBS}

%.c:	%.cob ${EXE}
	@echo "    Converting $< ..."
	@./${EXE} -o $@ $<

clean:	bvt/clean
bvt/clean:
	@echo "Removing generated files ..."
	@rm -f ${bvt/BVTS} ${bvt/OBJS} $(bvt/OBJS:.o=.d) ${bvt/SRCS} ${bvt/INCS}
	@rmdir $Obvt &> /dev/null || true
	@rm -f bvt/output-file bvt/*.dbf bvt/*.ndx

${bvt/OBJS}:	Makefile Config.mk bvt/Module.mk

-include ${bvt/OBJS:.o=.d}
