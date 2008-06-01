/* semscreen.cc
**
**	Defines screen semantic actions for COBOL compiler.
*/

#include "semscreen.h"
#include "symbase.h"
#include "semextern.h"
#include "symscreen.h"

/*----------------------------------------------------------------------*/

static CobolScreenField* g_CurrentParent = NULL;
static CobolScreenField* g_CurrentField = NULL;

/*----------------------------------------------------------------------*/

void BeginScreenField (void)
{
    assert (g_CurrentField == NULL);
    g_CurrentField = new CobolScreenField;
    assert (g_CurrentField != NULL);

    StackEntryPtr DeclLevelEntry = SemStack.Pop();
    g_CurrentField->SetDeclLevel (DeclLevelEntry->ival);
    delete DeclLevelEntry;

    if (g_CurrentParent != NULL && g_CurrentParent->GetDeclLevel() < g_CurrentField->GetDeclLevel())
	g_CurrentParent->AddChild (g_CurrentField);
    else {
	if (g_CurrentParent != NULL)
	    g_CurrentParent->GenDeclare();
	g_CurrentParent = g_CurrentField;
    }
}

void EndScreenField (void)
{
    assert (g_CurrentField != NULL);
    SymTable.Insert (g_CurrentField->GetName(), g_CurrentField);
    g_CurrentField = NULL;
}

void CloseScreenSection (void)
{
    assert (g_CurrentField == NULL);
    if (g_CurrentParent != NULL) {
        g_CurrentParent->GenDeclare();
	g_CurrentParent = NULL;
    }
}

void SetScreenFieldName (void)
{
    assert (g_CurrentField != NULL);
    StackEntry* nameEntry = SemStack.Pop();
    g_CurrentField->SetName (nameEntry->ident);
    delete nameEntry;
}

void SetScreenFieldLine (void)
{
    assert (g_CurrentField != NULL);
    StackEntry* valEntry = SemStack.Pop();
    g_CurrentField->SetFieldLine (valEntry->ival);
    delete valEntry;
}

void SetScreenFieldColumn (void)
{
    assert (g_CurrentField != NULL);
    StackEntry* valEntry = SemStack.Pop();
    g_CurrentField->SetFieldColumn (valEntry->ival);
    delete valEntry;
}

void SetScreenForeColor (void)
{
    assert (g_CurrentField != NULL);
    StackEntry* valEntry = SemStack.Pop();
    g_CurrentField->SetForeColor (valEntry->ival);
    delete valEntry;
}

void SetScreenBackColor (void)
{
    assert (g_CurrentField != NULL);
    StackEntry* valEntry = SemStack.Pop();
    g_CurrentField->SetBackColor (valEntry->ival);
    delete valEntry;
}

void SetScreenDefaultValue (void)
{
    assert (g_CurrentField != NULL);
    StackEntry* valEntry = SemStack.Pop();
    g_CurrentField->SetDefaultValue (valEntry);
}

void SetScreenFieldPicture (void)
{
    assert (g_CurrentField != NULL);
    StackEntry* valEntry = SemStack.Pop();
    delete valEntry;
}

void SetScreenFieldSource (void)
{
    assert (g_CurrentField != NULL);
    StackEntry* valEntry = SemStack.Pop();
    g_CurrentField->SetDataVarName (valEntry->ident);
    delete valEntry;
}

void SetScreenFieldAttr (SCRAttr /*newAttr*/)
{
    assert (g_CurrentField != NULL);
}

void SetScreenBlankFlag (BOOL newValue)
{
    assert (g_CurrentField != NULL);
    g_CurrentField->SetScreenBlankFlag (newValue);
}

