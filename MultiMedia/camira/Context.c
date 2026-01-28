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
** Context.c
*/

#include "Context.h"
#include <proto/exec.h>


/*
** delete the context
*/
void DeleteContext( struct Context *MyContext )
{
  struct ExecBase *SysBase ;

  if( NULL != MyContext )
  {
    SysBase = MyContext->c_SysBase ;

    FreeVec( MyContext ) ;
  }
}


/*
** create the context
*/
struct Context *CreateContext( struct ExecBase *SysBase )
{
  struct Context *MyContext ;

  MyContext = NULL ;
  
  if( NULL != SysBase )
  {  /* requirements ok */
    MyContext = AllocVec( sizeof( struct Context ), MEMF_ANY | MEMF_CLEAR ) ;
    if( NULL != MyContext )
    {  /* memory for context ok */
      MyContext->c_SysBase = SysBase ;
      MyContext->c_Process = ( struct Process * )FindTask( NULL ) ;
    }
  }
  else
  {  /* requirements not ok */
  }

  return( MyContext ) ;
}
