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
** Dispatcher.c
*/


#include "Dispatcher.h"
#include <proto/exec.h>
#include <clib/alib_protos.h>


/*
** create the dispatcher
*/
struct Dispatcher *CreateDispatcher( struct Context *MyContext )
{
  struct ExecBase *SysBase ;
  struct Dispatcher *NewDispatcher ;
  
  NewDispatcher = NULL ;

  if( NULL != MyContext )
  {  /* requirements ok */
    SysBase = MyContext->c_SysBase ;
    NewDispatcher = AllocVec( sizeof( struct Dispatcher ), MEMF_ANY | MEMF_CLEAR ) ;
    if( NULL != NewDispatcher )
    {  /* dispatcher context ok */
      NewDispatcher->d_Context = MyContext ;
      NewList( &NewDispatcher->d_HandlerList ) ;
      NewDispatcher->d_SignalMask = 0 ;
      NewDispatcher->d_SignalMaskDiff = 0 ;
    }
    else
    {  /* dispatcher context not ok */
    }
  }
  else
  {  /* requirements not ok */
  }
  
  return( NewDispatcher ) ;
}


/*
** delete the dispatcher
*/
void DeleteDispatcher( struct Dispatcher *OldDispatcher )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  
  if( NULL != OldDispatcher )
  {  /* dispatcher context needs to be freed */
    MyContext = OldDispatcher->d_Context ;
    SysBase = MyContext->c_SysBase ;
    FreeVec( OldDispatcher ) ;
  }
}


/*
** add a handler node to the list of installed handlers
*/
void AddDispatcherNode( struct Dispatcher *MyDispatcher, struct DispatcherNode *AddNode )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;

  if( NULL != MyDispatcher )
  {
    MyContext = MyDispatcher->d_Context ;
    SysBase = MyContext->c_SysBase ;
    if( NULL != AddNode )
    {
      AddHead( &MyDispatcher->d_HandlerList, &AddNode->dn_Node ) ;
      MyDispatcher->d_SignalMaskDiff |= AddNode->dn_Signals ;
    }
  }
}


/*
** remove a handler from the list of installed handlers
*/
void RemDispatcherNode( struct Dispatcher *MyDispatcher, struct DispatcherNode *RemNode )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;

  if( NULL != MyDispatcher )
  {
    MyContext = MyDispatcher->d_Context ;
    SysBase = MyContext->c_SysBase ;
    if( NULL != RemNode )
    {
      Remove( &RemNode->dn_Node ) ;
      MyDispatcher->d_SignalMaskDiff |= RemNode->dn_Signals ;
    }
  }
}


/*
** run the dispatcher
*/
void Dispatcher( struct Dispatcher *MyDispatcher, enum DispatchMode MyMode )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  ULONG ReceivedSignals, NewSignals ;
  struct Node *NextNode ;
  struct DispatcherNode *CurrentNode ;

  if( NULL != MyDispatcher )
  {
    MyContext = MyDispatcher->d_Context ;
    SysBase = MyContext->c_SysBase ;
    
    if( MyDispatcher->d_SignalMaskDiff )
    {  /* mask updated required */
      MyDispatcher->d_SignalMask = 0 ;
      NextNode = MyDispatcher->d_HandlerList.lh_Head ;
      while( NULL != NextNode->ln_Succ )
      {  /* check all handlers */
        CurrentNode = ( struct DispatcherNode * )NextNode ;
        NextNode = NextNode->ln_Succ ;
        MyDispatcher->d_SignalMask |= ( CurrentNode->dn_Signals ) ;
      }
      MyDispatcher->d_SignalMaskDiff = 0 ;
    }
    
    if( BLOCKING_MODE == MyMode )
    {  /* wait for signals to arrive */
      ReceivedSignals = Wait( MyDispatcher->d_SignalMask ) ;
    }
    else if( NONBLOCKING_MODE == MyMode )
    {  /* just check signals */
      ReceivedSignals = SetSignal( 0L, 0L ) ;  /* just get pending signals */
      if( 0 != ReceivedSignals )
      {  /* clear the signals this way, as doing this with SetSignal is considered dangerous */
        ReceivedSignals = Wait( ReceivedSignals ) ; 
      }
      else
      {  /* nothing to do */
        return ;
      }
    }

    NextNode = MyDispatcher->d_HandlerList.lh_Head ;
    while( NULL != NextNode->ln_Succ )
    {  /* check all handlers */
      CurrentNode = ( struct DispatcherNode * )NextNode ;
      NextNode = NextNode->ln_Succ ;
      if( ReceivedSignals & CurrentNode->dn_Signals ) 
      {  /* action for this handler */
        if( NULL != CurrentNode->dn_DoFunction )
        {  /* handler can be called */
          NewSignals = ( CurrentNode->dn_DoFunction )( CurrentNode->dn_DoData, ( ReceivedSignals & CurrentNode->dn_Signals ) ) ;
          if( 0 != NewSignals )
          {  /* node did not kill itself */
            MyDispatcher->d_SignalMaskDiff |= ( CurrentNode->dn_Signals ^ NewSignals ) ;
            CurrentNode->dn_Signals = NewSignals ;
          }
          else
          {  /* node did kill itself */
            MyDispatcher->d_SignalMaskDiff |= 1 ;
          }
        }
      }
    }
  }
}
