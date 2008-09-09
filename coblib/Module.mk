################ Source files ##########################################

COBLIB		:= $Ocoblib/libcobcy.a
coblib/SRCS	:= $(wildcard coblib/*.c)
coblib/INCS	:= $(wildcard coblib/*.h)
coblib/OBJS	:= $(addprefix $O,${coblib/SRCS:.c=.o})

################ Compilation ###########################################

.PHONY:	coblib/all coblib/clean

all:		coblib/all
coblib/all:	${COBLIB}
${COBLIB}:	${coblib/OBJS}
	@echo "Linking $@ ..."
	@${AR} rc $@ $?
	@${RANLIB} $@

################ Installation ##########################################

.PHONY:	coblib/install coblib/uninstall coblib/install-incs coblib/uninstall-incs

ifdef LIBDIR
COBLIBI	:= $(addprefix ${LIBDIR}/,$(notdir ${COBLIB}))
install:	coblib/install
coblib/install:	${COBLIBI}
${COBLIBI}:	${COBLIB}
	@echo "Installing $@ ..."
	@${INSTALLLIB} $< $@
uninstall:	coblib/uninstall
coblib/uninstall:
	@echo "Removing ${COBLIBI} ..."
	@rm -f ${COBLIBI}
endif
ifdef INCDIR
LIDIR		:= ${INCDIR}/coblib
coblib/INCSI	:= $(addprefix ${INCDIR}/,${coblib/INCS})
coblib/install:	coblib/install-incs
coblib/install-incs:	${coblib/INCSI}
${coblib/INCSI}:	${LIDIR}/%.h: coblib/%.h
	@echo "Installing $@ ..."
	@${INSTALLDATA} $< $@
coblib/uninstall:	coblib/uninstall-incs
coblib/uninstall-incs:
	@echo "Removing ${LIDIR}/ ..."
	@rm -f ${coblib/INCSI}
	@rmdir ${LIDIR} &> /dev/null || true
endif

################ Housekeeping ##########################################

clean:		coblib/clean
coblib/clean:
	@rm -f ${coblib/OBJS} $(coblib/OBJS:.o=.d) ${COBLIB}
	@rmdir $Ocoblib &> /dev/null || true

${coblib/OBJS}:	Makefile Config.mk coblib/Module.mk
-include ${coblib/OBJS:.o=.d}
