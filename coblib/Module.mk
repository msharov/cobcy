################ Source files ##########################################

COBLIB		:= $Ocoblib/libcobcy.a
coblib/SRCS	:= $(wildcard coblib/*.c)
coblib/INCS	:= $(wildcard coblib/*.h)
coblib/OBJS	:= $(addprefix $O,${coblib/SRCS:.c=.o})
coblib/DEPS	:= ${coblib/OBJS:.o=.d}

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
	@if [ -f ${COBLIBI} ]; then\
	    echo "Removing ${COBLIBI} ...";\
	    rm -f ${COBLIBI};\
	fi
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
	@if [ -d ${LIDIR} ]; then\
	    echo "Removing ${LIDIR}/ ...";\
	    rm -f ${coblib/INCSI};\
	    ${RMPATH} ${LIDIR};\
	fi
endif

################ Housekeeping ##########################################

clean:		coblib/clean
coblib/clean:
	@if [ -d $Ocoblib ]; then\
	    rm -f ${coblib/OBJS} ${coblib/DEPS} ${COBLIB} $Ocoblib/.d;\
	    rmdir $Ocoblib;\
	fi

$Ocoblib/.d:	$O.d
	@[ -d $Ocoblib ] || mkdir $Ocoblib
	@touch $@

${coblib/OBJS}:	${MKDEPS} coblib/Module.mk $Ocoblib/.d

-include ${coblib/DEPS}
