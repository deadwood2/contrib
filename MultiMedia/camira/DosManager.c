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
** DosManager.c
*/


#include "DosManager.h"
#include "Log.h"
#include <proto/exec.h>
#include <proto/dos.h>
#include <clib/alib_protos.h>


#if 1


/*
** provide a text error for the given error code
*/
STRPTR DosManagerFault( struct DosManager *MyDosManager, LONG MyCode )
{
  struct Context *MyContext ;
  struct DosLibrary *DOSBase ;
  STRPTR Result ;

  if( NULL != MyDosManager )
  {  /* requirements ok */
    MyContext = MyDosManager->dm_Context ;
    DOSBase = MyContext->c_DOSBase ;
    
    Fault( MyCode, NULL, MyDosManager->dm_ErrorText, DOSMANAGER_ERRORTEXT_LENGTH ) ;
    
    Result = MyDosManager->dm_ErrorText ;
  }
  else
  {  /* requirements not ok */
    Result = NULL ;
  }

  return( Result ) ;
}


/*
** transform a C string to a B string
*/
static LONG MakeBStr( char *CStr, char *BStr, LONG BStrLength )
{
  LONG Length ;
  
  Length = 0 ;
  while( ( BStrLength > Length ) && ( '\0' != CStr[ Length ] ) )
  {
    Length = Length + 1 ;
    BStr[ Length ] = CStr[ Length - 1 ] ;
  }
  BStr[ 0 ] = Length ;
  
  return( Length ) ;
}


/*
** dispatcher callback function for the dos manager
*/
static ULONG DoDosManager( struct DosManager *MyDosManager, ULONG TriggerSignals )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct StandardPacket *FinishedPacket ;
  struct Node *WalkNode ;
  struct DosNode *MyDosNode ;

  if( NULL != MyDosManager )
  {  /* requirements ok */
    MyContext = MyDosManager->dm_Context ;
    SysBase = MyContext->c_SysBase ;

    while( ( FinishedPacket = ( ( struct StandardPacket * )GetMsg( MyDosManager->dm_MsgPort ) ) ) )
    {  /* a dos packet arrived */
      WalkNode = MyDosManager->dm_List.lh_Head ;
      while( WalkNode->ln_Succ )
      {  /* loop all current list entries */
        MyDosNode = ( struct DosNode * )( WalkNode ) ;
        if( &FinishedPacket->sp_Pkt == MyDosNode->dn_DosPacket ) 
        {  /* the packet belongs to this node */
          break ;
        }
        WalkNode = WalkNode->ln_Succ ;
      }
      if( NULL == WalkNode->ln_Succ )
      {  /* node not in the list */
      }
      else
      {  /* found related node */
        Remove( &MyDosNode->dn_Node ) ;
        if( NULL != MyDosNode->dn_DoFunction )
        {  /* call do function */
          ( MyDosNode->dn_DoFunction )( MyDosNode->dn_DoData ) ;
        }
      }
    }
  }
  else
  {  /* requirements not ok */
  }

  TriggerSignals = ( 1UL << MyDosManager->dm_MsgPort->mp_SigBit ) ;
  
  return( TriggerSignals ) ;
}


/*
** delete the dos manager
*/
void DeleteDosManager( struct DosManager *OldDosManager )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;

  if( NULL != OldDosManager )
  {  /* dos manager context needs to be freed */
    MyContext = OldDosManager->dm_Context ;
    SysBase = MyContext->c_SysBase ;
    
    if( NULL != OldDosManager->dm_MsgPort )
    {  /* message port needs to be closed */
      RemDispatcherNode( MyContext->c_Dispatcher, &OldDosManager->dm_DispatcherNode ) ;
      DeleteMsgPort( OldDosManager->dm_MsgPort ) ;
    }
    FreeVec( OldDosManager ) ;
  }
}


/*
** create the dos manager
*/
struct DosManager *CreateDosManager( struct Context *MyContext )
{
  struct ExecBase *SysBase ;
  struct DosManager *NewDosManager ;
  
  NewDosManager = NULL ;
  
  if( ( NULL != MyContext ) && ( NULL != MyContext->c_Dispatcher )  )
  {  /* requirements seem ok */
    SysBase = MyContext->c_SysBase ;
    
    NewDosManager = AllocVec( sizeof( struct DosManager ), MEMF_ANY | MEMF_CLEAR ) ;
    if( NULL != NewDosManager )
    {  /* memory for dos manager ok */
      NewDosManager->dm_Context = MyContext ;
      NewList( &NewDosManager->dm_List ) ;
      NewDosManager->dm_MsgPort = CreateMsgPort( ) ;
      if( NULL != NewDosManager->dm_MsgPort )
      {  /* message port ok */
        NewDosManager->dm_DispatcherNode.dn_Signals = ( 1UL << NewDosManager->dm_MsgPort->mp_SigBit ) ;
        NewDosManager->dm_DispatcherNode.dn_DoFunction = ( DISPATCHER_DOFUNCTION )DoDosManager ;
        NewDosManager->dm_DispatcherNode.dn_DoData = NewDosManager ;
        AddDispatcherNode( MyContext->c_Dispatcher, &NewDosManager->dm_DispatcherNode ) ;
      }
      else
      {  /* message port not ok */
        LogText( MyContext->c_Log, ERROR_LEVEL, "dos manager message port not ok" ) ;
        DeleteDosManager( NewDosManager ) ;
        NewDosManager = NULL ;
      }
    }
    else
    {  /* dos manager not ok */
      LogText( MyContext->c_Log, ERROR_LEVEL, "dos manager not ok" ) ;
    }
  }
  else
  {  /* requirements not ok */
  }

  return( NewDosManager ) ;
}


/*
** delete common stuff and then the dos manager node
*/
void DeleteDosNode( struct DosManager *MyDosManager, struct DosNode *OldDosNode )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct DosLibrary *DOSBase ;

  if( NULL != MyDosManager )
  {  /* requirements ok */
    MyContext = MyDosManager->dm_Context ;
    SysBase = MyContext->c_SysBase ;
    DOSBase = MyContext->c_DOSBase ;

    if( NULL != OldDosNode )
    {  /* dos manager node memory needs to be freed */
      if( NULL != OldDosNode->dn_DosPacket )
      {  /* dos packet needs to be freed */
        if( NULL != OldDosNode->dn_DevProc )
        {  /* file system process needs to be released */ 
          if( NULL != OldDosNode->dn_CurrentDirectoryLock ) ;
          {  /* directory lock needs to be unlocked */
            UnLock( OldDosNode->dn_CurrentDirectoryLock ) ;
          }  
          FreeDeviceProc( OldDosNode->dn_DevProc ) ;
        }
        FreeDosObject( DOS_STDPKT, OldDosNode->dn_DosPacket ) ;
      }
      FreeVec( OldDosNode ) ;
    }
  }
}


/*
** create a dos manager node and prepare common stuff
*/
struct DosNode *CreateDosNode( struct DosManager *MyDosManager, STRPTR MyPath, LONG *MyError )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct DosLibrary *DOSBase ;
  struct DosNode *NewDosNode ;

  NewDosNode = NULL ;
  
  if( NULL != MyDosManager )
  {  /* requirements ok */
    MyContext = MyDosManager->dm_Context ;
    SysBase = MyContext->c_SysBase ;
    DOSBase = MyContext->c_DOSBase ;

    NewDosNode = AllocVec( sizeof( struct DosNode ), MEMF_ANY | MEMF_CLEAR ) ;
    if( NULL != NewDosNode )
    {  /* new dos manager node ok */
      NewDosNode->dn_BStr = ( BYTE * )( ( ( ( IPTR )NewDosNode->dn_CStr ) + 3 ) & ( ~3UL ) ) ;
      NewDosNode->dn_DosPacket = AllocDosObject( DOS_STDPKT, NULL ) ;
      if( NULL != NewDosNode->dn_DosPacket )
      {  /* dos packet ok */
        NewDosNode->dn_DevProc = GetDeviceProc( MyPath, NULL ) ;
        if( ( NULL != NewDosNode->dn_DevProc ) && ( NULL != NewDosNode->dn_DevProc->dvp_Port ) )
        {  /* file system process found */ 
          NewDosNode->dn_CurrentDirectoryLock = DupLock( NewDosNode->dn_DevProc->dvp_Lock ) ;
        }
        else
        {  /* file system process not found */ 
          DeleteDosNode( MyDosManager, NewDosNode ) ;
          NewDosNode = NULL ;
          *MyError = IoErr( ) ;
        }
      }
      else
      {  /* dos packet not ok */
        DeleteDosNode( MyDosManager, NewDosNode ) ;
        NewDosNode = NULL ;
        *MyError = IoErr( ) ;
      }
    }
    else
    {  /* new dos manager node not ok */
      *MyError = ERROR_NO_FREE_STORE ;
    }
  }
  else
  {  /* requirements not ok */
  }
  
  return( NewDosNode ) ;
}


/*
** check if dos node is still active
*/
LONG CheckDosNode( struct DosManager *MyDosManager, struct DosNode *MyDosNode )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct DosLibrary *DOSBase ;
  struct Node *WalkNode ;
  LONG Result ;

  if( ( NULL != MyDosManager ) && ( NULL != MyDosNode ) )
  {  /* requirements ok */
    MyContext = MyDosManager->dm_Context ;
    SysBase = MyContext->c_SysBase ;
    DOSBase = MyContext->c_DOSBase ;
    
    WalkNode = MyDosManager->dm_List.lh_Head ;
    while( WalkNode->ln_Succ )
    {  /* loop all current list entries */
      if( WalkNode == ( struct Node * )MyDosNode )
      {  /* this node is still active */
        break ;
      }
      WalkNode = WalkNode->ln_Succ ;
    }
    if( NULL == WalkNode->ln_Succ )
    {  /* node not in the list */
      Result = 0 ;
    }
    else
    {  /* node still in the list */
      Result = 1 ;
    }
  }
  else
  {  /* requirements not ok */
    Result = -1 ;
  }

  return( Result ) ;
}


/*
** start a dos node
*/
void SendDosNode( struct DosManager *MyDosManager, struct DosNode *MyDosNode )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct DosLibrary *DOSBase ;

  if( 0 == CheckDosNode( MyDosManager, MyDosNode ) )
  {  /* node not busy */
    MyContext = MyDosManager->dm_Context ;
    SysBase = MyContext->c_SysBase ;
    DOSBase = MyContext->c_DOSBase ;

    AddTail( &MyDosManager->dm_List, ( struct Node * )MyDosNode ) ;
    SendPkt( MyDosNode->dn_DosPacket, MyDosNode->dn_DevProc->dvp_Port, MyDosManager->dm_MsgPort ) ;
  }
  else
  {  /* node still busy */
  }
}


/*
** abort a dos node
*/
void AbortDosNode( struct DosManager *MyDosManager, struct DosNode *MyDosNode )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct DosLibrary *DOSBase ;

  if( 1 == CheckDosNode( MyDosManager, MyDosNode ) )
  {  /* node busy */
    MyContext = MyDosManager->dm_Context ;
    SysBase = MyContext->c_SysBase ;
    DOSBase = MyContext->c_DOSBase ;
    
    AbortPkt( MyDosNode->dn_DevProc->dvp_Port, MyDosNode->dn_DosPacket ) ;
  }
}


/*
** change the current directory of a dos node
*/
BPTR CurrentDirDosNode( struct DosManager *MyDosManager, struct DosNode *MyDosNode, BPTR MyLock )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct DosLibrary *DOSBase ;
  BPTR Result ;

  Result = 0 ;
  if( 0 <= CheckDosNode( MyDosManager, MyDosNode ) )
  {  /* node busy or not */
    MyContext = MyDosManager->dm_Context ;
    SysBase = MyContext->c_SysBase ;
    DOSBase = MyContext->c_DOSBase ;
    
    Result = MyDosNode->dn_CurrentDirectoryLock ;
    if( NULL != MyLock )
    {  /* use the lock */
      MyDosNode->dn_CurrentDirectoryLock = MyLock ;
    }
    else
    {  /* get the default lock */
      if( NULL != MyDosNode->dn_DevProc->dvp_Lock )
      {  /* default directory lock is probably an assign */
        MyDosNode->dn_CurrentDirectoryLock = DupLock( MyDosNode->dn_DevProc->dvp_Lock ) ;
      }
      else
      {  /* default directory lock is root of handler */
        MyDosNode->dn_CurrentDirectoryLock = 0 ;
      }
    }
  }
  
  return( Result ) ;
}


/*
** prepare dos node for Lock()
*/
LONG BeforeLock( struct DosManager *MyDosManager, struct DosNode *MyDosNode, STRPTR MyName, LONG MyMode )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct DosLibrary *DOSBase ;
  LONG Error ;
  
  Error = TRUE ;
  if( ( NULL != MyDosManager ) && ( NULL != MyDosNode ) && ( NULL != MyName ) )
  {  /* requirements ok */
    MyContext = MyDosManager->dm_Context ;
    SysBase = MyContext->c_SysBase ;
    DOSBase = MyContext->c_DOSBase ;

    MyDosNode->dn_DosPacket->dp_Type = ACTION_LOCATE_OBJECT ;
    MyDosNode->dn_DosPacket->dp_Arg1 = ( SIPTR ) MyDosNode->dn_CurrentDirectoryLock ;
    MakeBStr( MyName, MyDosNode->dn_BStr, DOSNODE_BCSTR_LENGTH ) ;
    MyDosNode->dn_DosPacket->dp_Arg2 = ( SIPTR ) MKBADDR( MyDosNode->dn_BStr ) ;
    MyDosNode->dn_DosPacket->dp_Arg3 = MyMode ;
    /* all done */
    Error = FALSE ;
  }
  else
  {  /* requirements not ok */
  }

  return( Error ) ;  
}


/*
** follow up dos node after Lock()
*/
BPTR AfterLock( struct DosManager *MyDosManager, struct DosNode *MyDosNode, LONG *MyError )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct DosLibrary *DOSBase ;
  BPTR Result ;

  if( ( NULL != MyDosManager ) && ( NULL != MyDosNode ) )
  {  /* requirements ok */
    MyContext = MyDosManager->dm_Context ;
    SysBase = MyContext->c_SysBase ;
    DOSBase = MyContext->c_DOSBase ;

    if( DOSFALSE != MyDosNode->dn_DosPacket->dp_Res1 )
    {  /* lock ok */
      Result = (BPTR) MyDosNode->dn_DosPacket->dp_Res1 ;
    }
    else
    {  /* lock not ok */
      Result = 0 ;
      *MyError = MyDosNode->dn_DosPacket->dp_Res2 ;
    }
  }
  else
  {  /* requirements not ok */
  }
  
  return( Result ) ;
}


/*
** prepare dos node for UnLock()
*/
LONG BeforeUnLock( struct DosManager *MyDosManager, struct DosNode *MyDosNode, BPTR MyLock )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct DosLibrary *DOSBase ;
  LONG Error ;
  
  Error = TRUE ;
  if( ( NULL != MyDosManager ) && ( NULL != MyDosNode ) )
  {  /* requirements ok */
    MyContext = MyDosManager->dm_Context ;
    SysBase = MyContext->c_SysBase ;
    DOSBase = MyContext->c_DOSBase ;

    MyDosNode->dn_DosPacket->dp_Type = ACTION_FREE_LOCK ;
    MyDosNode->dn_DosPacket->dp_Arg1 = ( SIPTR) MyLock ;
    /* all done */
    Error = FALSE ;
  }
  else
  {  /* requirements not ok */
  }

  return( Error ) ;  
}


/*
** follow up dos node after UnLock()
*/
BOOL AfterUnLock( struct DosManager *MyDosManager, struct DosNode *MyDosNode, LONG *MyError )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct DosLibrary *DOSBase ;
  BOOL Result ;

  if( ( NULL != MyDosManager ) && ( NULL != MyDosNode ) )
  {  /* requirements ok */
    MyContext = MyDosManager->dm_Context ;
    SysBase = MyContext->c_SysBase ;
    DOSBase = MyContext->c_DOSBase ;

    if( DOSFALSE != MyDosNode->dn_DosPacket->dp_Res1 )
    {  /* unlock ok */
      Result = TRUE ;
    }
    else
    {  /* unlock not ok */
      Result = FALSE ;
      *MyError = MyDosNode->dn_DosPacket->dp_Res2 ;
    }
  }
  else
  {  /* requirements not ok */
  }
  
  return( Result ) ;
}


/*
** prepare dos node for CreateDir()
*/
LONG BeforeCreateDir( struct DosManager *MyDosManager, struct DosNode *MyDosNode, STRPTR MyName )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct DosLibrary *DOSBase ;
  LONG Error ;
  
  Error = TRUE ;
  if( ( NULL != MyDosManager ) && ( NULL != MyDosNode ) && ( NULL != MyName ) )
  {  /* requirements ok */
    MyContext = MyDosManager->dm_Context ;
    SysBase = MyContext->c_SysBase ;
    DOSBase = MyContext->c_DOSBase ;

    MyDosNode->dn_DosPacket->dp_Type = ACTION_CREATE_DIR ;
    MyDosNode->dn_DosPacket->dp_Arg1 = ( SIPTR ) MyDosNode->dn_CurrentDirectoryLock ;
    MakeBStr( MyName, MyDosNode->dn_BStr, DOSNODE_BCSTR_LENGTH ) ;
    MyDosNode->dn_DosPacket->dp_Arg2 = ( SIPTR ) MKBADDR( MyDosNode->dn_BStr ) ;
    /* all done */
    Error = FALSE ;
  }
  else
  {  /* requirements not ok */
  }

  return( Error ) ;  
}


/*
** follow up dos node after CreateDir()
*/
BPTR AfterCreateDir( struct DosManager *MyDosManager, struct DosNode *MyDosNode, LONG *MyError )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct DosLibrary *DOSBase ;
  BPTR Result ;

  if( ( NULL != MyDosManager ) && ( NULL != MyDosNode ) )
  {  /* requirements ok */
    MyContext = MyDosManager->dm_Context ;
    SysBase = MyContext->c_SysBase ;
    DOSBase = MyContext->c_DOSBase ;

    if( DOSFALSE != MyDosNode->dn_DosPacket->dp_Res1 )
    {  /* create dir ok */
      Result = (BPTR) MyDosNode->dn_DosPacket->dp_Res1 ;
    }
    else
    {  /* create dir not ok */
      Result = 0 ;
      *MyError = MyDosNode->dn_DosPacket->dp_Res2 ;
    }
  }
  else
  {  /* requirements not ok */
  }
  
  return( Result ) ;
}


/*
** prepare dos node for Open()
*/
LONG BeforeOpen( struct DosManager *MyDosManager, struct DosNode *MyDosNode, STRPTR MyName, LONG MyMode )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct DosLibrary *DOSBase ;
  struct FileHandle *MyFileHandle ;
  LONG Error ;
  
  Error = TRUE ;
  if( ( NULL != MyDosManager ) && ( NULL != MyDosNode ) && ( NULL != MyName ) )
  {  /* requirements ok */
    MyContext = MyDosManager->dm_Context ;
    SysBase = MyContext->c_SysBase ;
    DOSBase = MyContext->c_DOSBase ;

    MyFileHandle = AllocDosObject( DOS_FILEHANDLE, NULL ) ;
    if( NULL != MyFileHandle )
    {  /* filehandle ok */
      MyFileHandle->fh_Pos = -1 ;
      MyFileHandle->fh_End = -1 ;
      switch( MyMode )
      {
        case MODE_READWRITE:
          MyDosNode->dn_DosPacket->dp_Type = ACTION_FINDUPDATE ;
          break ;
        case MODE_OLDFILE:
          MyDosNode->dn_DosPacket->dp_Type = ACTION_FINDINPUT ;
          break ;
        case MODE_NEWFILE:
          MyDosNode->dn_DosPacket->dp_Type = ACTION_FINDOUTPUT ;
          break ;
      }
      MyDosNode->dn_DosPacket->dp_Arg1 = ( SIPTR ) MKBADDR( MyFileHandle ) ;
      MyDosNode->dn_DosPacket->dp_Arg2 = ( SIPTR ) MyDosNode->dn_CurrentDirectoryLock ;
      MakeBStr( MyName, MyDosNode->dn_BStr, DOSNODE_BCSTR_LENGTH ) ;
      MyDosNode->dn_DosPacket->dp_Arg3 = ( SIPTR ) MKBADDR( MyDosNode->dn_BStr ) ;
      /* all done */
      Error = FALSE ;
    }
    else
    {  /* filehandle not ok */
      MyDosNode->dn_DosPacket->dp_Res1 = 0;
      MyDosNode->dn_DosPacket->dp_Res2 = IoErr( ) ;
    }
  }
  else
  {  /* requirements not ok */
  }
  
  return( Error ) ;
}


/*
** follow up dos node after Open()
*/
BPTR AfterOpen( struct DosManager *MyDosManager, struct DosNode *MyDosNode, LONG *MyError )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct DosLibrary *DOSBase ;
  BPTR Result ;

  if( ( NULL != MyDosManager ) && ( NULL != MyDosNode ) )
  {  /* requirements ok */
    MyContext = MyDosManager->dm_Context ;
    SysBase = MyContext->c_SysBase ;
    DOSBase = MyContext->c_DOSBase ;

    if( DOSFALSE != MyDosNode->dn_DosPacket->dp_Res1 )
    {  /* open ok */
      Result = (BPTR) MyDosNode->dn_DosPacket->dp_Arg1 ;
    }
    else
    {  /* open not ok */
      FreeDosObject( DOS_FILEHANDLE, BADDR( MyDosNode->dn_DosPacket->dp_Arg1 ) ) ;
      Result = 0 ;
      *MyError = MyDosNode->dn_DosPacket->dp_Res2 ;
    }
  }
  else
  {  /* requirements not ok */
  }
  
  return( Result ) ;
}


/*
** prepare dos node for Close()
*/
LONG BeforeClose( struct DosManager *MyDosManager, struct DosNode *MyDosNode, BPTR MyFile )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct DosLibrary *DOSBase ;
  struct FileHandle *MyFileHandle ;
  LONG Error ;
  
  Error = TRUE ;
  if( ( NULL != MyDosManager ) && ( NULL != MyDosNode ) && ( NULL != MyFile ) )
  {  /* requirements ok */
    MyContext = MyDosManager->dm_Context ;
    SysBase = MyContext->c_SysBase ;
    DOSBase = MyContext->c_DOSBase ;

    MyFileHandle = BADDR( MyFile ) ;
    MyDosNode->dn_DosPacket->dp_Type = ACTION_END ;
    MyDosNode->dn_DosPacket->dp_Arg1 = MyFileHandle->fh_Arg1 ;
    FreeDosObject( DOS_FILEHANDLE, MyFileHandle ) ;
    /* all done */
    Error = FALSE ;
  }
  else
  {  /* requirements not ok */
  }

  return( Error ) ;  
}


/*
** follow up dos node after Close()
*/
BOOL AfterClose( struct DosManager *MyDosManager, struct DosNode *MyDosNode, LONG *MyError )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct DosLibrary *DOSBase ;
  BOOL Result ;

  if( ( NULL != MyDosManager ) && ( NULL != MyDosNode ) )
  {  /* requirements ok */
    MyContext = MyDosManager->dm_Context ;
    SysBase = MyContext->c_SysBase ;
    DOSBase = MyContext->c_DOSBase ;

    if( DOSFALSE != MyDosNode->dn_DosPacket->dp_Res1 )
    {  /* close ok */
      Result = TRUE ;
    }
    else
    {  /* close not ok */
      Result = FALSE ;
      *MyError = MyDosNode->dn_DosPacket->dp_Res2 ;
    }
  }
  else
  {  /* requirements not ok */
  }
  
  return( Result ) ;
}


/*
** prepare dos node for Read()
*/
LONG BeforeRead( struct DosManager *MyDosManager, struct DosNode *MyDosNode, BPTR MyFile, void *MyBuffer, LONG MyLength )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct DosLibrary *DOSBase ;
  struct FileHandle *MyFileHandle ;
  LONG Error ;
  
  Error = TRUE ;
  if( ( NULL != MyDosManager ) && ( NULL != MyDosNode ) && ( NULL != MyFile ) && ( 0 < MyLength ) )
  {  /* requirements ok */
    MyContext = MyDosManager->dm_Context ;
    SysBase = MyContext->c_SysBase ;
    DOSBase = MyContext->c_DOSBase ;

    MyFileHandle = ( struct FileHandle * )BADDR( MyFile ) ;
    MyDosNode->dn_DosPacket->dp_Type = ACTION_READ ;
    MyDosNode->dn_DosPacket->dp_Arg1 = MyFileHandle->fh_Arg1 ;
    MyDosNode->dn_DosPacket->dp_Arg2 = ( IPTR )MyBuffer ;
    MyDosNode->dn_DosPacket->dp_Arg3 = MyLength ;
    /* all done */
    Error = FALSE ;
  }
  else
  {  /* requirements not ok */
  }
  
  return( Error ) ;
}


/*
** follow up dos node after Read()
*/
LONG AfterRead( struct DosManager *MyDosManager, struct DosNode *MyDosNode, LONG *MyError ) 
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct DosLibrary *DOSBase ;
  LONG Result ;

  Result = 0 ;
  if( ( NULL != MyDosManager ) && ( NULL != MyDosNode ) )
  {  /* requirements ok */
    MyContext = MyDosManager->dm_Context ;
    SysBase = MyContext->c_SysBase ;
    DOSBase = MyContext->c_DOSBase ;

    if( MyDosNode->dn_DosPacket->dp_Res1 == MyDosNode->dn_DosPacket->dp_Arg3 )
    {  /* read ok */
      Result = MyDosNode->dn_DosPacket->dp_Res1 ;
    }
    else
    {  /* read not ok */
      Result = MyDosNode->dn_DosPacket->dp_Res1 ;
      *MyError = MyDosNode->dn_DosPacket->dp_Res2 ;
    }
  }
  else
  {  /* requirements not ok */
  }
  
  return( Result ) ;
}


/*
** prepare dos node for Write()
*/
LONG BeforeWrite( struct DosManager *MyDosManager, struct DosNode *MyDosNode, BPTR MyFile, void *MyBuffer, LONG MyLength )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct DosLibrary *DOSBase ;
  struct FileHandle *MyFileHandle ;
  LONG Error ;
  
  Error = TRUE ;
  if( ( NULL != MyDosManager ) && ( NULL != MyDosNode ) && ( NULL != MyFile ) && ( 0 < MyLength ) )
  {  /* requirements ok */
    MyContext = MyDosManager->dm_Context ;
    SysBase = MyContext->c_SysBase ;
    DOSBase = MyContext->c_DOSBase ;

    MyFileHandle = ( struct FileHandle * )BADDR( MyFile ) ;
    MyDosNode->dn_DosPacket->dp_Type = ACTION_WRITE ;
    MyDosNode->dn_DosPacket->dp_Arg1 = MyFileHandle->fh_Arg1 ;
    MyDosNode->dn_DosPacket->dp_Arg2 = ( IPTR )MyBuffer ;
    MyDosNode->dn_DosPacket->dp_Arg3 = MyLength ;
    /* all done */
    Error = FALSE ;
  }
  else
  {  /* requirements not ok */
  }
  
  return( Error ) ;
}


/*
** follow up dos node after Write()
*/
LONG AfterWrite( struct DosManager *MyDosManager, struct DosNode *MyDosNode, LONG *MyError )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct DosLibrary *DOSBase ;
  LONG Result ;

  Result = 0 ;
  if( ( NULL != MyDosManager ) && ( NULL != MyDosNode ) )
  {  /* requirements ok */
    MyContext = MyDosManager->dm_Context ;
    SysBase = MyContext->c_SysBase ;
    DOSBase = MyContext->c_DOSBase ;

    if( MyDosNode->dn_DosPacket->dp_Res1 == MyDosNode->dn_DosPacket->dp_Arg3 )
    {  /* write ok */
      Result = MyDosNode->dn_DosPacket->dp_Res1 ;
    }
    else
    {  /* write not ok */
      Result = MyDosNode->dn_DosPacket->dp_Res1 ;
      *MyError = MyDosNode->dn_DosPacket->dp_Res2 ;
    }
  }
  else
  {  /* requirements not ok */
  }
  
  return( Result ) ;
}
















#else







#define STATICBSTR 1

#if STATICBSTR
/*
** transform a C string to a B string
*/
static LONG MakeBStr( char *CStr, char *BStr, LONG BStrLength )
{
  LONG Length ;
  
  Length = 0 ;
  while( ( BStrLength > Length ) && ( '\0' != CStr[ Length ] ) )
  {
    Length = Length + 1 ;
    BStr[ Length ] = CStr[ Length - 1 ] ;
  }
  BStr[ 0 ] = Length ;
  
  return( Length ) ;
}


#else


/*
** create a BSTR from a normal C string
*/
static char *CreateBStr( struct DosManagerContext *MyDosManager, char *CStr )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  ULONG Length ;
  char *BStr ;

  BStr = NULL ;

  if( ( NULL != MyDosManager ) && ( NULL != CStr ) )
  {  /* requirements ok */
    MyContext = MyDosManager->dmc_Context ;
    SysBase = MyContext->c_SysBase ;

    Length = strlen( CStr ) ;
    if( Length  <= 255 )
    {  /* cstr fits in bstr */
      BStr = AllocVec( ( Length + 1 ), MEMF_PUBLIC ) ;
      if( NULL != BStr )
      {  /* bstr ok */
        CopyMem( CStr, &BStr[ 1 ], Length ) ;
        BStr[ 0 ] = Length ;
      }
      else
      {  /* bstr not ok */
      }
    }
    else
    {  /* cstr is too long */
    }
  }
  else
  {  /* requirements not ok */
  }

  return( BStr ) ;
}


/*
** delete a BSTR
*/
static void DeleteBStr( struct DosManagerContext *MyDosManager, char *BStr )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  
  if( ( NULL != MyDosManager ) && ( NULL != BStr ) )
  {  /* requirements ok */
    MyContext = MyDosManager->dmc_Context ;
    SysBase = MyContext->c_SysBase ;
    FreeVec( BStr ) ;
  }
}
#endif


/*
** dispatcher callback function for the dos manager
*/
static ULONG DoDosManager( struct DosManagerContext *MyDosManager, ULONG TriggerSignals )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct StandardPacket *FinishedPacket ;
  struct Node *WalkNode ;
  struct DosManagerNode *CurrentNode ;

  if( NULL != MyDosManager )
  {  /* requirements ok */
    MyContext = MyDosManager->dmc_Context ;
    SysBase = MyContext->c_SysBase ;

    while( ( FinishedPacket = ( ( struct StandardPacket * )GetMsg( MyDosManager->dmc_MsgPort ) ) ) )
    {  /* a dos packet arrived */
      //print_f( "dos packet finished: 0x%p\n", FinishedPacket ) ;
      WalkNode = MyDosManager->dmc_List.lh_Head ;
      while( WalkNode->ln_Succ )
      {  /* loop all current list entries */
        //print_f( "walking dos manager node: 0x%p\n", WalkNode ) ;
        if( &FinishedPacket->sp_Pkt == ( ( struct DosManagerNode * )( WalkNode ) )->dmn_DosPacket ) 
        {  /* the packet belongs to this node */
          break ;
        }
        WalkNode = WalkNode->ln_Succ ;
      }
      if( NULL == WalkNode->ln_Succ )
      {  /* node not in the list */
      }
      else
      {  /* found related node */
        CurrentNode = ( struct DosManagerNode * )( WalkNode ) ;
        Remove( &CurrentNode->dmn_Node ) ;
        switch( CurrentNode->dmn_Action )
        {  /* prepare results */
          case OPENNEW_ACTION:
#if STATICBSTR
#else
            DeleteBStr( MyDosManager, BADDR( CurrentNode->dmn_DosPacket->dp_Arg3 ) ) ;
#endif
            if( DOSTRUE == CurrentNode->dmn_DosPacket->dp_Res1 )
            {  /* open new file ok */
              CurrentNode->dmn_Result = ( ULONG )MKBADDR( CurrentNode->dmn_FileHandle ) ;
              CurrentNode->dmn_Error = 0 ;
            }
            else
            {  /* open new file not ok */
              CurrentNode->dmn_Result = 0 ;
              CurrentNode->dmn_Error = CurrentNode->dmn_DosPacket->dp_Res2 ;
            }
            break ;
          case WRITE_ACTION:
            //print_f( "write result: %d of %d\n", CurrentNode->dmn_DosPacket->dp_Res1, CurrentNode->dmn_BufferSize ) ;
            if( CurrentNode->dmn_DosPacket->dp_Res1 == CurrentNode->dmn_BufferSize )
            {  /* write to the file ok */
              CurrentNode->dmn_Result = CurrentNode->dmn_DosPacket->dp_Res1 ;
              CurrentNode->dmn_Error = 0 ;
            }
            else
            {  /* write to the file not ok */
              CurrentNode->dmn_Result = 0 ;
              CurrentNode->dmn_Error = CurrentNode->dmn_DosPacket->dp_Res2 ;
            }
            break ;
          case CLOSE_ACTION:
            if( DOSFALSE != CurrentNode->dmn_DosPacket->dp_Res1 )
            {  /* close file ok */
              CurrentNode->dmn_Result = 0 ;
              CurrentNode->dmn_Error = 0 ;
            }
            else
            {  /* close file not ok */
              CurrentNode->dmn_Result = 0 ;
              CurrentNode->dmn_Error = CurrentNode->dmn_DosPacket->dp_Res1 ;
            }
            break ;
          default:  /* unknown action */
            break ;
        }
        if( NULL != CurrentNode->dmn_DoFunction )
        {  /* call do function */
          ( CurrentNode->dmn_DoFunction )( CurrentNode->dmn_DoData ) ;
        }
      }
    }
  }
  else
  {  /* requirements not ok */
  }

  TriggerSignals = ( 1UL << MyDosManager->dmc_MsgPort->mp_SigBit ) ;
  
  return( TriggerSignals ) ;
}


/*
** delete the dos manager
*/
void DeleteDosManager( struct DosManagerContext *OldDosManager )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;

  if( NULL != OldDosManager )
  {  /* dos manager context needs to be freed */
    MyContext = OldDosManager->dmc_Context ;
    SysBase = MyContext->c_SysBase ;
    
    if( NULL != OldDosManager->dmc_MsgPort )
    {  /* message port needs to be closed */
      RemDispatcherNode( MyContext->c_Dispatcher, &OldDosManager->dmc_DispatcherNode ) ;
      DeleteMsgPort( OldDosManager->dmc_MsgPort ) ;
    }
    FreeVec( OldDosManager ) ;
  }
}


/*
** create the dos manager
*/
struct DosManagerContext *CreateDosManager( struct Context *MyContext )
{
  struct ExecBase *SysBase ;
  struct DosManagerContext *NewDosManager ;
  
  NewDosManager = NULL ;
  
  if( ( NULL != MyContext ) && ( NULL != MyContext->c_Dispatcher )  )
  {  /* requirements seem ok */
    SysBase = MyContext->c_SysBase ;
    
    NewDosManager = AllocVec( sizeof( struct DosManagerContext ), MEMF_ANY | MEMF_CLEAR ) ;
    if( NULL != NewDosManager )
    {  /* memory for dos manager context ok */
      NewDosManager->dmc_Context = MyContext ;
      NewList( &NewDosManager->dmc_List ) ;

      NewDosManager->dmc_MsgPort = CreateMsgPort( ) ;
      if( NULL != NewDosManager->dmc_MsgPort )
      {  /* message port ok */
        NewDosManager->dmc_DispatcherNode.dn_Signals = ( 1UL << NewDosManager->dmc_MsgPort->mp_SigBit ) ;
        NewDosManager->dmc_DispatcherNode.dn_DoFunction = ( DISPATCHER_DOFUNCTION )DoDosManager ;
        NewDosManager->dmc_DispatcherNode.dn_DoData = NewDosManager ;
        AddDispatcherNode( MyContext->c_Dispatcher, &NewDosManager->dmc_DispatcherNode ) ;
      }
      else
      {  /* message port not ok */
        DeleteDosManager( NewDosManager ) ;
        NewDosManager = NULL ;
      }
    }
    else
    {  /* memory for dos manager context not ok */
    }
  }
  else
  {  /* requirements not ok */
  }

  return( NewDosManager ) ;
}


/*
** delete common stuff and then the dos manager node
*/
void DeleteDosManagerNode( struct DosManagerContext *MyDosManager, struct DosManagerNode *OldDosManagerNode )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct DosLibrary *DOSBase ;

  if( NULL != MyDosManager )
  {  /* requirements ok */
    MyContext = MyDosManager->dmc_Context ;
    SysBase = MyContext->c_SysBase ;
    DOSBase = MyContext->c_DOSBase ;

    if( NULL != OldDosManagerNode )
    {  /* dos manager node memory needs to be freed */
      if( NULL != OldDosManagerNode->dmn_DirectoryLock ) ;
      {  /* directory lock needs to be unlocked */
        if( NULL != OldDosManagerNode->dmn_DevProc )
        {  /* file system process needs to be released */ 
          if( NULL != OldDosManagerNode->dmn_DosPacket )
          {  /* dos packet needs to be freed */
            if( NULL != OldDosManagerNode->dmn_FileHandle )
            {  /* file handle needs to be freed */
              FreeDosObject( DOS_FILEHANDLE, OldDosManagerNode->dmn_FileHandle ) ;
            }
            FreeDosObject( DOS_STDPKT, OldDosManagerNode->dmn_DosPacket ) ;
          }
          FreeDeviceProc( OldDosManagerNode->dmn_DevProc ) ;
        }
        UnLock( OldDosManagerNode->dmn_DirectoryLock ) ;
      }  
      FreeVec( OldDosManagerNode ) ;
    }
  }
  else
  {  /* requirements not ok */
  }
}


/*
** create a dos manager node and prepare common stuff
*/
struct DosManagerNode *CreateDosManagerNode( struct DosManagerContext *MyDosManager, STRPTR MyPath, LONG *MyError )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct DosLibrary *DOSBase ;
  struct DosManagerNode *NewDosManagerNode ;

  NewDosManagerNode = NULL ;
  
  if( NULL != MyDosManager )
  {  /* requirements ok */
    MyContext = MyDosManager->dmc_Context ;
    SysBase = MyContext->c_SysBase ;
    DOSBase = MyContext->c_DOSBase ;

    NewDosManagerNode = AllocVec( sizeof( struct DosManagerNode ), MEMF_ANY | MEMF_CLEAR ) ;
    if( NULL != NewDosManagerNode )
    {  /* new dos manager node ok */
      NewDosManagerNode->dmn_BStr = ( BYTE * )( ( ( ( IPTR )NewDosManagerNode->dmn_CStr ) + 3 ) & ( ~3UL ) ) ;
      //print_f( "bstr address: 0x%p, cstr address: 0x%p\n", NewDosManagerNode->dmn_BStr, NewDosManagerNode->dmn_CStr ) ;
      NewDosManagerNode->dmn_DirectoryLock = Lock( MyPath, ACCESS_READ ) ;
      if( NULL != NewDosManagerNode->dmn_DirectoryLock )
      {  /* directory lock ok */
        NameFromLock( NewDosManagerNode->dmn_DirectoryLock, MyDosManager->dmc_PathExpander, PATHEXPANDER_LENGTH ) ;
        //print_f( "locked path: %s\n", MyDosManager->dmc_PathExpander ) ;
        NewDosManagerNode->dmn_DevProc = GetDeviceProc( MyDosManager->dmc_PathExpander, NULL ) ;
        if( NULL != NewDosManagerNode->dmn_DevProc )
        {  /* file system process found */ 
          NewDosManagerNode->dmn_DosPacket = AllocDosObject( DOS_STDPKT, NULL ) ;
          if( NULL != NewDosManagerNode->dmn_DosPacket )
          {  /* dos packet ok */
            NewDosManagerNode->dmn_FileHandle = AllocDosObject( DOS_FILEHANDLE, NULL ) ;
            if( NULL != NewDosManagerNode->dmn_FileHandle )
            {  /* file handle ok */
            }
            else
            {  /* file handle not ok */
              DeleteDosManagerNode( MyDosManager, NewDosManagerNode ) ;
              *MyError = IoErr( ) ;
              NewDosManagerNode = NULL ;
            }
          }
          else
          {  /* dos packet not ok */
            DeleteDosManagerNode( MyDosManager, NewDosManagerNode ) ;
            *MyError = IoErr( ) ;
            NewDosManagerNode = NULL ;
          }
        }
        else
        {  /* file system process not found */ 
          DeleteDosManagerNode( MyDosManager, NewDosManagerNode ) ;
          *MyError = IoErr( ) ;
          NewDosManagerNode = NULL ;
        }
      }
      else
      {  /* directory lock not ok */
        DeleteDosManagerNode( MyDosManager, NewDosManagerNode ) ;
        *MyError = IoErr( ) ;
        NewDosManagerNode = NULL ;
      }
    }
    else
    {  /* new dos manager node not ok */
      *MyError = -2 ;
    }
  }
  else
  {  /* requirements not ok */
    *MyError = -1 ;
  }
  
  return( NewDosManagerNode ) ;
}


/*
** start a dos action
*/
LONG AddDosManagerNode( struct DosManagerContext *MyDosManager, struct DosManagerNode *AddNode )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct DosLibrary *DOSBase ;
  ULONG Error ;
  
  Error = TRUE ;
  
  if( ( NULL != MyDosManager ) && ( NULL != AddNode ) )
  {
    MyContext = MyDosManager->dmc_Context ;
    SysBase = MyContext->c_SysBase ;
    DOSBase = MyContext->c_DOSBase ;

    switch( AddNode->dmn_Action )
    {  /* prepare the dos packet depending of the requested action */
      case OPENNEW_ACTION:  /* open a new file */
        AddNode->dmn_FileHandle->fh_Pos = -1 ;
        AddNode->dmn_FileHandle->fh_End = -1 ;
        AddNode->dmn_DosPacket->dp_Type = ACTION_FINDOUTPUT ;
        AddNode->dmn_DosPacket->dp_Arg1 = MKBADDR( AddNode->dmn_FileHandle ) ;
        AddNode->dmn_DosPacket->dp_Arg2 = AddNode->dmn_DirectoryLock ;
#if STATICBSTR
        MakeBStr( AddNode->dmn_Path, ( BYTE * )AddNode->dmn_BStr, BCSTR_LENGTH ) ;
        AddNode->dmn_DosPacket->dp_Arg3 = MKBADDR( AddNode->dmn_BStr ) ;
        Error = FALSE ;  /* packet can be send */
#else
        AddNode->dmn_DosPacket->dp_Arg3 = MKBADDR( CreateBStr( MyDosManager, AddNode->dmn_Path ) ) ;
        if( NULL != AddNode->dmn_DosPacket->dp_Arg3 )
        {  /* bstr ok */
          Error = FALSE ;  /* packet can be send */
        }
        else
        {  /* bstr not ok */
          /* packet can't be send */
        }
#endif
        break ;
      case WRITE_ACTION:  /* write to the open file of this node */
        AddNode->dmn_DosPacket->dp_Type = ACTION_WRITE ;
        AddNode->dmn_DosPacket->dp_Arg1 = AddNode->dmn_FileHandle->fh_Arg1 ;
        AddNode->dmn_DosPacket->dp_Arg2 = ( IPTR )AddNode->dmn_Buffer ;
        AddNode->dmn_DosPacket->dp_Arg3 = AddNode->dmn_BufferSize ;
        Error = FALSE ;  /* packet can be send */
        break ;
      case CLOSE_ACTION:  /* close the open file of this node */
        AddNode->dmn_DosPacket->dp_Type = ACTION_END ;
        AddNode->dmn_DosPacket->dp_Arg1 = AddNode->dmn_FileHandle->fh_Arg1 ;
        Error = FALSE ;  /* packet can be send */
        break ;
      default:  /* unsupported action */
        Error = TRUE ;
        break ;
    }
    
    if( !( Error ) )
    {  /* send the packet */
      //print_f( "adding dos manager node: 0x%p\n", AddNode ) ;
      AddTail( &MyDosManager->dmc_List, &AddNode->dmn_Node ) ;
      //print_f( "sending dos packet: 0x%p\n", AddNode->dmn_DosPacket ) ;
      SendPkt( AddNode->dmn_DosPacket, AddNode->dmn_DevProc->dvp_Port, MyDosManager->dmc_MsgPort ) ;
    }
    else
    {  /* can't send the packet */
    }
  }
  
  return( Error ) ;
}


/*
** stop a dos action
*/
void RemDosManagerNode( struct DosManagerContext *MyDosManager, struct DosManagerNode *RemNode )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct DosLibrary *DOSBase ;
  struct Node *WalkNode ;

  if( ( NULL != MyDosManager ) && ( NULL != RemNode ) )
  {
    MyContext = MyDosManager->dmc_Context ;
    SysBase = MyContext->c_SysBase ;
    DOSBase = MyContext->c_DOSBase ;
    
    WalkNode = MyDosManager->dmc_List.lh_Head ;
    while( WalkNode->ln_Succ )
    {  /* loop all current list entries */
      if( WalkNode == ( struct Node * )RemNode )
      {  /* this is the node to remove */
        break ;
      }
      WalkNode = WalkNode->ln_Succ ;
    }
    if( NULL == WalkNode->ln_Succ )
    {  /* node not in the list */
    }
    else
    {  /* remove it from somewhere */
      /* found no better way up to now */
      Remove( ( struct Node * )RemNode ) ;
      AbortPkt( RemNode->dmn_DevProc->dvp_Port, RemNode->dmn_DosPacket ) ;
      WaitPort( MyDosManager->dmc_MsgPort ) ;
      GetMsg( MyDosManager->dmc_MsgPort ) ;
    }
  }
}


/*
** abort a dos manager node
*/
void AbortDosManagerNode( struct DosManagerContext *MyDosManager, struct DosManagerNode *RemNode )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct DosLibrary *DOSBase ;
  struct Node *WalkNode ;

  if( ( NULL != MyDosManager ) && ( NULL != RemNode ) )
  {
    MyContext = MyDosManager->dmc_Context ;
    SysBase = MyContext->c_SysBase ;
    DOSBase = MyContext->c_DOSBase ;
    
    WalkNode = MyDosManager->dmc_List.lh_Head ;
    while( WalkNode->ln_Succ )
    {  /* loop all current list entries */
      if( WalkNode == ( struct Node * )RemNode )
      {  /* this is the node to remove */
        break ;
      }
      WalkNode = WalkNode->ln_Succ ;
    }
    if( NULL == WalkNode->ln_Succ )
    {  /* node not in the list */
    }
    else
    {  /* abort it from somewhere */
      AbortPkt( RemNode->dmn_DevProc->dvp_Port, RemNode->dmn_DosPacket ) ;
    }
  }
}


/*
** provide a text error for the given error code
*/
STRPTR DosManagerFault( struct DosManagerContext *MyDosManager, LONG MyCode )
{
  struct Context *MyContext ;
  struct DosLibrary *DOSBase ;
  STRPTR Result ;

  if( NULL != MyDosManager )
  {  /* requirements ok */
    MyContext = MyDosManager->dmc_Context ;
    DOSBase = MyContext->c_DOSBase ;
    
    Fault( MyCode, NULL, MyDosManager->dmc_ErrorText, ERRORTEXT_LENGTH ) ;
    
    Result = MyDosManager->dmc_ErrorText ;
  }
  else
  {  /* requirements not ok */
    Result = NULL ;
  }

  return( Result ) ;
}
#endif
