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
** ab_stdio.h
*/


#ifndef _AB_STDIO_H
#define _AB_STDIO_H


#ifdef _USE_AB_LIB


#define snprintf ab_snprintf
#define vsnprintf ab_vsnprintf


#else


#include <stdio.h>


#endif


#include <stdarg.h>


int ab_vsprintf( char *Buffer, const char *Format, va_list Arguments ) ;
int ab_sprintf( char *Buffer, const char *Format, ... ) ;
int ab_vsnprintf( char *Buffer, int Length, const char * Format, va_list Arguments ) ;
int ab_snprintf( char *Buffer, int Length, const char * Format, ... ) ;




#endif  /* _AB_STDIO_H */
