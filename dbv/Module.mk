################ Source files ##########################################

dbv/EXE		:= $Odbv/dbv
dbv/SRCS	:= $(wildcard dbv/*.c)
dbv/OBJS	:= $(addprefix $O,$(dbv/SRCS:.c=.o))
dbv/LIBS	:= ${COBLIB} -lncurses
dbv/DEPS	:= ${dbv/OBJS:.o=.d}

################ Compilation ###########################################

all:		dbv/all
dbv/all:	${dbv/EXE}
${dbv/EXE}:	${dbv/OBJS} ${COBLIB}
	@echo "Linking $@ ..."
	@${LD} ${LDFLAGS} -o $@ $^ ${dbv/LIBS}

################ Installation ##########################################

.PHONY:		dbv/install dbv/uninstall dbv/clean

ifdef BINDIR
dbv/EXEI	:= ${BINDIR}/$(notdir ${dbv/EXE})
install:	dbv/install
dbv/install:	${dbv/EXEI}
${dbv/EXEI}:	${dbv/EXE}
	@echo "Installing $@ ..."
	@${INSTALLBIN} $< $@
uninstall:	dbv/uninstall
dbv/uninstall:
	@if [ -f ${dbv/EXEI} ]; then\
	    echo "Removing ${dbv/EXEI} ..."\
	    rm -f ${dbv/EXEI}\
	fi
endif

################ Compilation ###########################################

clean:		dbv/clean
dbv/clean:
	@if [ -d $Odbv ]; then\
	    rm -f ${dbv/EXE} ${dbv/OBJS} ${dbv/DEPS} $Odbv/.d;\
	    rmdir $Odbv;\
	fi

$Odbv/.d:	$O.d
	@[ -d $Odbv ] || mkdir $Odbv
	@touch $@

${dbv/OBJS}:	${MKDEPS} dbv/Module.mk $Odbv/.d
-include ${dbv/DEPS}
