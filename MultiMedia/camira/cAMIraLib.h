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


/*
** cAMIraLib.h
*/


#ifndef _CAMIRALIB_H
#define _CAMIRALIB_H


#include <utility/date.h>


/*
** available specifier
*/
#define EXPAND_SPECIFIER_YEAR ( 'Y' )
#define EXPAND_SPECIFIER_MONTH ( 'm' )
#define EXPAND_SPECIFIER_DAY ( 'd' )
#define EXPAND_SPECIFIER_HOUR ( 'H' )
#define EXPAND_SPECIFIER_MINUTE ( 'M' )
#define EXPAND_SPECIFIER_SECOND ( 'S' )
#define EXPAND_SPECIFIER_COUNTER ( 'N' )


/*
** data to expand strings
*/
struct ExpandData
{
  struct ClockData *ed_ClockData ;
  LONG *ed_Number ;
} ;


/*
** module functions
*/
LONG ExpandString( STRPTR MyString, LONG MyLength, STRPTR MyTemplate, struct ExpandData *MyExpandData ) ;


#endif  /* !_CAMIRALIB_H */
