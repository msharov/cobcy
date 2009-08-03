// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#ifndef SEMARITH_H_503A913B4ADC70F064EDF1806F474404
#define SEMARITH_H_503A913B4ADC70F064EDF1806F474404

void GenMove (void);
void GenAdd (void);
void GenSubtract (void);
void GenMultiply (void);
void GenDivide (void);
void GenCompute (void);
void SetResultRounding (void);

#endif
