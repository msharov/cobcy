include Config.mk

SRCS	= $(wildcard *.cc) cobol_lex.cc cobol_yacc.cc
OBJS	= $(SRCS:.cc=.o)

CLIB	= lib/libcobol.a
DBFOBJS	= dbf/dbf.o dbf/ndx.o

#############################################

all:	${EXE} ${CLIB}

${CLIB}:
	@echo "Building cobcy runtime library ..."
	@+make -C lib

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

check:	${EXE} ${CLIB}
	@+make -C bvt run

depend:
	@echo Generating dependencies ...
	@${CXX} ${CXXFLAGS} -MM ${SRCS} > .depend
	@+make -C dbf depend

clean:
	@+make -C dbf clean
	@+make -C lib clean
	@+make -C bvt clean
	@rm -f $(OBJS) *.bak *~ y.tab.h cobol_lex.cc cobol_yacc.cc
	@rm -f $(COBJS)

realclean: clean
	@rm -f $(EXE) $(CLIB)

-include .depend
