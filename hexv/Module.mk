################ Source files ##########################################

hexv/EXE	:= $Ohexv/hexv
hexv/SRCS	:= $(wildcard hexv/*.c)
hexv/OBJS	:= $(addprefix $O,$(hexv/SRCS:.c=.o))
hexv/LIBS	:= ${COBLIB} -lncurses
hexv/DEPS	:= ${hexv/OBJS:.o=.d}

################ Compilation ###########################################

all:		hexv/all
hexv/all:	${hexv/EXE}
${hexv/EXE}:	${hexv/OBJS} ${COBLIB}
	@echo "Linking $@ ..."
	@${LD} ${LDFLAGS} -o $@ $^ ${hexv/LIBS}

################ Installation ##########################################

.PHONY:		hexv/install hexv/uninstall hexv/clean

ifdef BINDIR
hexv/EXEI	:= ${BINDIR}/hexv
install:	hexv/install
hexv/install:	${hexv/EXEI}
${hexv/EXEI}:	${hexv/EXE}
	@echo "Installing $@ ..."
	@${INSTALLEXE} $< $@
uninstall:	hexv/uninstall
hexv/uninstall:
	@if [ -f ${hexv/EXEI} ]; then\
	    echo "Removing ${hexv/EXEI} ...";\
	    rm -f ${hexv/EXEI};\
	fi
endif

################ Compilation ###########################################

clean:		hexv/clean
hexv/clean:
	@if [ -d $Ohexv ]; then\
	    rm -f ${hexv/EXE} ${hexv/OBJS} ${hexv/DEPS} $Ohexv/.d;\
	    rmdir $Ohexv;\
	fi

$Ohexv/.d:	$O.d
	@[ -d $Ohexv ] || mkdir $Ohexv
	@touch $@

${hexv/OBJS}:	${MKDEPS} hexv/Module.mk $Ohexv/.d
-include ${hexv/DEPS}
