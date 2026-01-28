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
** MemoryManager.c
*/


#include "MemoryManager.h"
#include <proto/exec.h>


/*
** processed allocation requests
*/
static ULONG DoMemoryManager( struct MemoryManager *MyMemoryManager, ULONG TriggerSignals )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct MemoryMessage *ClientRequest ;
  
  if( NULL != MyMemoryManager )
  {  /* this seems to be a valid memory manager */
    MyContext = MyMemoryManager->mm_Context ;
    SysBase = MyContext->c_SysBase ;
    while( NULL != ( ClientRequest = ( struct MemoryMessage * )GetMsg( MyMemoryManager->mm_MsgPort ) ) )
    {  /* precess all allocation requests */
      if( NULL == ClientRequest->mm_MemoryChunk )
      {  /* an alloc request */
        //print_f( "alloc request %ld\n", ClientRequest->mm_Size ) ;
        ClientRequest->mm_MemoryChunk = AllocVec( ClientRequest->mm_Size, ClientRequest->mm_Attributes ) ;
      }
      else
      {  /* a free request */
        //print_f( "free request\n" ) ;
        FreeVec( ClientRequest->mm_MemoryChunk ) ;
        ClientRequest->mm_MemoryChunk = NULL ;
      }
      ReplyMsg( ( struct Message * )ClientRequest ) ;
    }
  }
  
  TriggerSignals = ( 1UL << MyMemoryManager->mm_MsgPort->mp_SigBit ) ;
  
  return( TriggerSignals ) ;
}


/*
** create the memory manager for vhi custom memory management
*/
struct MemoryManager *CreateMemoryManager( struct Context *MyContext )
{
  struct ExecBase *SysBase ;
  struct MemoryManager *NewMemoryManager ;
  
  NewMemoryManager = NULL ;
  
  if( ( NULL != MyContext ) && ( NULL != MyContext->c_Dispatcher )  )
  {  /* requirements seem ok */
    SysBase = MyContext->c_SysBase ;
    NewMemoryManager = AllocVec( sizeof( struct MemoryManager ), MEMF_ANY | MEMF_CLEAR ) ;
    if( NULL != NewMemoryManager )
    {  /* memory for memory manager context ok */
      NewMemoryManager->mm_Context = MyContext ;
      NewMemoryManager->mm_MsgPort = CreateMsgPort( ) ;
      if( NULL != NewMemoryManager->mm_MsgPort )
      {  /* message port ok */
        NewMemoryManager->mm_DispatcherNode.dn_Signals = ( 1UL << NewMemoryManager->mm_MsgPort->mp_SigBit ) ;
        NewMemoryManager->mm_DispatcherNode.dn_DoFunction = ( DISPATCHER_DOFUNCTION )DoMemoryManager ;
        NewMemoryManager->mm_DispatcherNode.dn_DoData = NewMemoryManager ;
        AddDispatcherNode( MyContext->c_Dispatcher, &NewMemoryManager->mm_DispatcherNode ) ;
      }
      else
      {  /* message port not ok */
        DeleteMemoryManager( NewMemoryManager ) ;
        NewMemoryManager = NULL ;
      }
    }
    else
    {  /* memory for memory manager context not ok */
    }
  }
  else
  {  /* requirements not ok */
  }

  return( NewMemoryManager ) ;
}


/*
** delete the memory manager
*/
void DeleteMemoryManager( struct MemoryManager *OldMemoryManager )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  
  if( NULL != OldMemoryManager )
  {  /* memory manager context needs to be freed */
    MyContext = OldMemoryManager->mm_Context ;
    SysBase = MyContext->c_SysBase ;
    if( NULL != OldMemoryManager->mm_MsgPort )
    {  /* message port needs to be deleted */
      RemDispatcherNode( MyContext->c_Dispatcher, &OldMemoryManager->mm_DispatcherNode ) ;
      DeleteMsgPort( OldMemoryManager->mm_MsgPort ) ;
    }
    FreeVec( OldMemoryManager ) ;
  }
}
