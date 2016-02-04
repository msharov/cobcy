// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "semextern.h"
#include "symbase.h"
#include <stdio.h>

CobolSymbol::CobolSymbol (void)
:_prefix()
,_cobolName()
,_parentCobolName()
,_cName()
,_cNameFull()
{
}

// Virtual dtor
CobolSymbol::~CobolSymbol (void) {}

void CobolSymbol::SetName (const char* name)
{
    _cName = _cobolName = name;
    replace (_cName, '-', '_');	// C name made from Cobol name by converting dashes
    _cNameFull = _prefix;
    _cNameFull += _cName;
}

void CobolSymbol::SetParent (const char* pname)
{
    if (!pname) {
	_parentCobolName.clear();
	_prefix.clear();
	_cNameFull = _cName;
    } else {
	auto pattr = g_Symbols.find (pname);
	if (!pattr)
	    return WriteError ("cannot find parent %s", pname);
	_prefix = pattr->_prefix;
	_prefix += pattr->_cName;
	_prefix += '.';
	_cNameFull = _prefix;
	_cNameFull += _cName;
    }
}

void CobolSymbol::text_write (ostringstream& os) const
{
    os << _cNameFull;
}

//----------------------------------------------------------------------

SymbolTable::SymbolTable (void)
:_names()
,_syms()
{
}

auto SymbolTable::find (const key_type& key) const noexcept -> value_type
{
    auto ikey = lower_bound (_names, key);
    if (ikey == _names.end() || *ikey != key)
	return nullptr;
    return ibyi (ikey, _names, _syms)->get();
}

auto SymbolTable::insert (const key_type& key, value_type sym) -> value_type
{
    auto ikey = lower_bound (_names, key);
    auto isym = ibyi (ikey, _names, _syms);
    if (ikey == _names.end() || *ikey != key) {
	_names.insert (ikey, key);
	isym = _syms.emplace (isym, move(sym));
    } else
	WriteError ("symbol %s redefined", key.c_str());
    return isym->get();
}

void SymbolTable::erase (const key_type& key)
{
    auto ikey = lower_bound (_names, key);
    if (ikey == _names.end() || *ikey != key)
	return;
    _syms.erase (ibyi (ikey, _names, _syms));
}

//----------------------------------------------------------------------

SymbolTable	 	g_Symbols;
