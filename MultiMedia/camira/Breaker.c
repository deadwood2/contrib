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
** BreakHandler.c
*/


#include "Breaker.h"
#include <proto/exec.h>


/*
** callback function for breaker signal handler
*/
static ULONG DoBreaker( struct Breaker *MyBreaker, ULONG TriggerSignals )
{
  struct Context *MyContext ;

  if( NULL != MyBreaker )
  {  /* breaker node seems ok */
    MyContext = MyBreaker->b_Context ;
    MyContext->c_ShutdownRequest = 1 ;
  }
  else
  {  /* breaker node not ok */
  }

  TriggerSignals = SIGBREAKF_CTRL_C ;
  
  return( TriggerSignals ) ;
}


/*
** create the breaker
*/
struct Breaker *CreateBreaker( struct Context *MyContext )
{
  struct ExecBase *SysBase ;
  struct Breaker *NewBreaker ;
  
  NewBreaker = NULL ;

  if( ( NULL != MyContext ) && ( NULL != MyContext->c_Dispatcher ) )
  {  /* requirements ok */
    SysBase = MyContext->c_SysBase ;
    NewBreaker = AllocVec( sizeof( struct Breaker ), MEMF_ANY | MEMF_CLEAR ) ;
    if( NULL != NewBreaker )
    {  /* breaker context ok */
      NewBreaker->b_Context = MyContext ;
      NewBreaker->b_DispatcherNode.dn_Signals = SIGBREAKF_CTRL_C ;
      NewBreaker->b_DispatcherNode.dn_DoFunction = ( DISPATCHER_DOFUNCTION )DoBreaker ;
      NewBreaker->b_DispatcherNode.dn_DoData = NewBreaker ;
      AddDispatcherNode( MyContext->c_Dispatcher, &NewBreaker->b_DispatcherNode ) ;
    }
    else
    {  /* breaker context ok */
    }
  }
  else
  {  /* requirements not ok */
  }

  return( NewBreaker ) ;
}


/*
** delete the breaker
*/
void DeleteBreaker( struct Breaker *OldBreaker )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  
  if( NULL != OldBreaker )
  {  /* breaker context needs to be freed */
    MyContext = OldBreaker->b_Context ;
    SysBase = MyContext->c_SysBase ;
    RemDispatcherNode( MyContext->c_Dispatcher, &OldBreaker->b_DispatcherNode ) ;
    FreeVec( OldBreaker ) ;
  }
}
