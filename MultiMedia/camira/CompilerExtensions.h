/*****************************************************************************

    This file is part of cAMIra - a small webcam application for the AMIGA.
    
    Copyright (C) 2012-2020 Andreas (supernobby) Barth

    cAMIra is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    cAMIra is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with cAMIra. If not, see <http://www.gnu.org/licenses/>.
    
*****************************************************************************/


#ifndef _COMPILEREXTENSIONS_H
#define _COMPILEREXTENSIONS_H


/*
** a macro to define register parameters
*/
#ifdef __GNUC__

#define REGFUNC
#if defined(__mc68000__)
#define REG( _reg, _var ) _var __asm(#_reg)
#else
#define REG( _reg, _var ) _var
#endif

#elif __STORM__

#define REGFUNC
#define REG( _reg, _var ) register __##_reg _var

#elif __MAXON__

#define REGFUNC
#define REG( _reg, _var ) register __##_reg _var

#elif __SASC

#define REGFUNC __asm
#define REG( _reg, _var ) register __##_reg _var

#endif


#endif  /* _COMPILEREXTENSIONS_H */
