-include Config.mk

################ Source files ##########################################

EXE	:= $O${NAME}
SRCS	:= $(sort $(wildcard *.cc) $Oscanner.cc $Oparser.cc)
OBJS	:= $(addprefix $O,$(notdir ${SRCS:.cc=.o}))
DEPS	:= ${OBJS:.o=.d}
MKDEPS	:= Makefile Config.mk config.h $O.d
ONAME	:= $(notdir $(abspath $O))

################ Compilation ###########################################

.PHONY: all clean dist distclean maintainer-clean

ALLTGTS	:= ${MKDEPS} ${EXE}
all:	${ALLTGTS}

${EXE}:	${OBJS}
	@echo "Linking $@ ..."
	@${LD} ${LDFLAGS} -o $@ ${OBJS} ${LIBS}

$O%.o:	%.cc
	@echo "    Compiling $< ..."
	@${CXX} ${CXXFLAGS} -MMD -MT "$(<:.cc=.s) $@" -o $@ -c $<

$Oscanner.o $Oparser.o:	%.o: %.cc
	@echo "    Compiling $< ..."
	@${CXX} ${CXXFLAGS} -I. -MMD -MT "$(<:.cc=.s) $@" -o $@ -c $<

$O%.o:	%.c
	@echo "    Compiling $< ..."
	@${CC} ${CFLAGS} -MMD -MT "$(<:.c=.s) $@" -o $@ -c $<

%.s:	%.cc
	@echo "    Compiling $< to assembly ..."
	@${CXX} ${CXXFLAGS} -S -o $@ -c $<

%.s:	%.c
	@echo "    Compiling $< to assembly ..."
	@${CC} ${CFLAGS} -S -o $@ -c $<

$Oscanner.cc:	scanner.l $Oparser.cc
	@echo "    Generating $@ ..."
	@${FLEX} -o $@ $<

$Oparser.cc:	parser.y
	@echo "    Generating $@ ..."
	@${BISON} -d -o $@ $<

################ Modules ###############################################

include coblib/Module.mk
include bvt/Module.mk
include hexv/Module.mk
include dbv/Module.mk

################ Installation ##########################################

.PHONY:	install uninstall

ifdef BINDIR
EXEI	:= ${BINDIR}/${NAME}

install:	${EXEI}
${EXEI}:	${EXE}
	@echo "Installing $< as $@ ..."
	@${INSTALLEXE} $< $@

uninstall:
	@if [ -f ${EXEI} ]; then\
	    echo "Removing ${EXEI} ...";\
	    rm -f ${EXEI};\
	    ${RMPATH} ${BINDIR};\
	fi
endif

################ Maintenance ###########################################

clean:
	@if [ -h ${ONAME} ]; then\
	    rm -f ${EXE} ${OBJS} ${DEPS} $Oparser.cc $Oparser.hh $Oscanner.cc $O.d ${ONAME};\
	    ${RMPATH} ${BUILDDIR};\
	fi

distclean:	clean
	@rm -f Config.mk config.h config.status

maintainer-clean: distclean

$O.d:	${BUILDDIR}/.d
	@[ -h ${ONAME} ] || ln -sf ${BUILDDIR} ${ONAME}
${BUILDDIR}/.d:	Makefile
	@mkdir -p ${BUILDDIR} && touch ${BUILDDIR}/.d

${OBJS}:		${MKDEPS}
Config.mk:		Config.mk.in
config.h:		config.h.in
Config.mk config.h:	configure
	@if [ -x config.status ]; then			\
	    echo "Reconfiguring ..."; ./config.status;	\
	else						\
	    echo "Running configure ..."; ./configure;	\
	fi

-include ${DEPS}
