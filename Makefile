include Config.mk

SRCS	= main.cc semdecl.cc semutil.cc semarith.cc seminfo.cc \
	seminit.cc semfile.cc semconio.cc semcontrol.cc symbase.cc \
	symconst.cc symfile.cc sympic.cc symrec.cc symvar.cc \
	cobfunc.c
OBJS	= main.o semdecl.o semutil.o semarith.o seminfo.o \
	seminit.o semfile.o semconio.o semcontrol.o \
	symbase.o symconst.o symdata.o symfile.o symlabel.o \
	sympic.o symrec.o symvar.o \
	cobol_lex.o cobol_yacc.o 

CLIB	= libcobol.a
COBJS	= cobfunc.o
DBFOBJS	= dbf/dbf.o dbf/ndx.o
ADTLIB	= adtlib/libadt.a
LIBS	+= ${ADTLIB}

#############################################

all:	$(ADTLIB) $(EXE) $(CLIB)

$(ADTLIB):
	@echo Building base classes ...
	@make -C adtlib

${EXE}:	${OBJS}
	@echo "Linking $@ ..."
	@${CXX} ${LDFLAGS} -o $@ $^ ${LIBS}

cobol_lex.cc:	cobol.l y.tab.h
	@echo "    Building lexical analyzer ..."
	@${LEX} -o $@ $<

cobol_yacc.cc:	cobol.y
	@echo "    Building parser ..."
	@${YACC} -o $@ $<

y.tab.h:
	@echo Updating parser symbol defs ...
	@$(YACC) -d cobol.y
	@rm -f y.tab.c

$(CLIB):	$(COBJS)
	@echo Making DBF objects ...
	@make -C dbf
	@echo Building cobcy runtime library ...
	@rm -f $(CLIB)
	@$(AR) rs $(CLIB) $(COBJS) $(DBFOBJS)
	@$(RANLIB) $(CLIB)

cobfunc.o:	cobfunc.c
	@echo Compiling $< ...
	@$(CC) $(CFLAGS) -c $<

%.o:	%.c
	@echo "    Compiling $< ..."
	@${CC} ${CFLAGS} -o $@ -c $<

%.o:	%.cc
	@echo "    Compiling $< ..."
	@${CXX} ${CXXFLAGS} -o $@ -c $<

%.s:	%.cc
	@echo "    Compiling $< to assembly ..."
	@${CXX} ${CXXFLAGS} -S -o $@ -c $<

checkin:
	@echo Checking in everything ...
	@ci -l $(RCSED)

revert:
	@echo Swash! Dang! Boooom!
	@echo Canceling all recent changes.
	@co $(RCSED)
	@chmod u+w $(RCSED)

depend:
	@echo Generating dependencies ...
	@${CXX} ${CXXFLAGS} -MM ${SRCS} > .depend
	@+make -C adtlib depend
	@+make -C dbf depend

clean:
	@+make -C adtlib clean
	@+make -C dbf clean
	@rm -f $(OBJS) *.bak *~ y.tab.h cobol_lex.cc cobol_yacc.cc
	@rm -f $(COBJS)

spotless: clean
	@./cleantest
	@rm -f $(EXE) $(CLIB)
	@rm -f $(ADTLIB)

-include .depend
