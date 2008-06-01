/* symscreen.cc
**
**	Base class for screen elements.
*/

#include "symscreen.h"
#include "semutil.h"
#include "semextern.h"
#include "symconst.h"
#include "symvar.h"

CobolScreenField :: CobolScreenField (void)
{
    DeclLevel = 0;
    FieldLine = -1;
    FieldColumn = -1;
    ForeColor = 7;
    BackColor = 0;
    DataVarName[0] = 0;
    BlankScreen = FALSE;
    nChildren = 0;
    DefaultValue = NULL;
}

CobolScreenField :: ~CobolScreenField (void)
{
    if (DefaultValue != NULL)
	delete DefaultValue;
}

void CobolScreenField :: GenRead (ostream& os, const char* /*stream*/)
{
    GenIndent();
    os << "_AcceptScreen (" << *this << ", " << nChildren << ");\n";
    SetCursesUsed (TRUE);
}

void CobolScreenField :: GenWrite (ostream& os, const char* /*stream*/)
{
    GenIndent();
    os << "_DisplayScreen (" << *this << ", " << nChildren << ");\n";
    SetCursesUsed (TRUE);
}

void CobolScreenField::GenDeclare (void)
{
    if (nChildren > 0) {
	GenIndent();
	codef << "static _SScreenField " << *this;
	codef << " [" << nChildren << "] = {\n";
	Children.Head();
	++ NestingLevel;
	for (int i = 0; i < nChildren; ++ i) {
	    CobolScreenField* pChild = Children.LookAt();
	    assert (pChild != NULL);
	    pChild->GenDeclare();
	    if (i < nChildren - 1)
		codef << ",";
	    codef << "\n";
	    Children.Next();
	}
	-- NestingLevel;
	GenIndent();
	codef << "};\n\n";
    }
    else {
	GenIndent();
	codef << "{";
	codef << FieldColumn << ", ";
	codef << FieldLine << ", ";
	codef << ForeColor << ", ";
	codef << BackColor << ", ";
	if (strlen (DataVarName) > 0) {
	    CobolSymbol* pData = SymTable.Lookup (DataVarName);
	    if (pData == NULL)
		WriteError ("unknown field data variable");
	    else if (pData != NULL && pData->Kind() != CS_Variable)
		WriteError ("invalid field data variable");
	    else {
		CobolVar* pVar = (CobolVar*) pData;
		if (!pVar->IsNumeric())
		    codef << "_dt_String, ";
		else if (pVar->GetPicture().IsInteger())
		    codef << "_dt_Integer, &";
		else
		    codef << "_dt_Float, &";
		codef << *pVar << ", \"";
		pVar->WritePicture (codef);
		codef << "\", ";
	    }
	}
	else {
	    if (DefaultValue != NULL) {
		CobolConstant val = DefaultValue;
		if (!val.IsNumeric())
		    codef << "_dt_String, ";
		else if (val.IsInteger())
		    codef << "_dt_Integer, ";
		else
		    codef << "_dt_Float, ";
		codef << val << ", NULL, ";
	    }
	    else
		codef << "NULL, ";
	}
	codef << (int) BlankScreen;
	codef << "}";
    }
}

