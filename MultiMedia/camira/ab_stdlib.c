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
** ab_stdlib.c
*/


#include "ab_stdlib.h"


long ab_atol( char *Buffer )
{
  long Result ;
  unsigned long BufferIndex ;

  Result = 0 ;
  BufferIndex = 0 ;
  while( ( '0' <= Buffer[ BufferIndex ] ) && ( '9' >= Buffer[ BufferIndex ] ) )
  {
    Result *= 10 ;
    Result += Buffer[ BufferIndex ] - '0' ;
    BufferIndex++ ;
  }

  return( Result ) ;
}
