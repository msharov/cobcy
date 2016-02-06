// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#include "config.h"

enum {
    MAX_SYMBOL_LENGTH	= 50,
    MAX_PREFIX_LENGTH	= 80,
    MAX_FULLNAME_LENGTH	= (MAX_SYMBOL_LENGTH + MAX_PREFIX_LENGTH)
};

enum CobolSymbolType {
    CS_Undefined,
    CS_Record, 
    CS_Variable,
    CS_Procedure,
    CS_FileDesc,
    CS_Label,
    CS_Constant,
    CS_Picture
};

/// Defines an abstract symbol table entry.
class CobolSymbol {
public:
			CobolSymbol (void);
    virtual		~CobolSymbol (void);
    void		SetName (const char* name);
    inline auto&	GetName (void) const		{ return _cobolName; }
    inline auto&	GetCName (void) const		{ return _cName; }
    inline auto&	GetFullCName (void) const	{ return _cNameFull; }
    void		SetParent (const char* pname);
    virtual CobolSymbolType	Kind (void) const	{ return CS_Undefined; }
    virtual void	text_write (ostringstream& os) const;
private:
    string		_prefix;
    string		_cobolName;	// Cobol name
    string		_parentCobolName;
    string		_cName;		// C name
    string		_cNameFull;	// C name with prefix
};

class SymbolTable {
public:
    using value_type	= CobolSymbol*;
    using key_type	= string;
    using value_ptr	= unique_ptr<CobolSymbol>;
private:
    using keyvec_t	= vector<key_type>;
    using datavec_t	= vector<value_ptr>;
public:
			SymbolTable (void);
    value_type		find (const key_type& key) const noexcept;
    template <typename Sym>
    inline auto		lookup (const key_type& key) const noexcept	{ return dynamic_cast<Sym*>(find(key)); }
    value_type		insert (const key_type& key, value_type sym);
    void		erase (const key_type& key);
    template <typename Sym, typename... Args>
    inline auto		emplace (const key_type& key, Args&&... args)	{ return static_cast<Sym*>(insert (key, new Sym (forward<Args>(args)...))); }
    inline bool		empty (void) const	{ return _names.empty(); }
private:
    keyvec_t		_names;
    datavec_t		_syms;
};

extern SymbolTable g_Symbols;
