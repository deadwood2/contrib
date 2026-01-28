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
** Timer.c
*/


#include "Timer.h"
#include "Log.h"
#include <proto/exec.h>
#include <proto/utility.h>
#include <clib/alib_protos.h>
#include <proto/timer.h>


/*
** start ticking with head of the list of timer client nodes
*/
static void StartTicking( struct Timer *MyTimer )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct TimerNode *CurrentNode ;

  if( ( NULL != MyTimer ) && ( !( IsListEmpty( &MyTimer->t_TimerList ) ) ) )
  {  /* requirements ok */
    MyContext = MyTimer->t_Context ;
    SysBase = MyContext->c_SysBase ;
    CurrentNode = ( struct TimerNode * )MyTimer->t_TimerList.lh_Head ;
    /* setup io request */
    MyTimer->t_TimeRequest->tr_time = CurrentNode->tn_AbsoluteTime ;
    MyTimer->t_TimeRequest->tr_node.io_Command = TR_ADDREQUEST ;
    SendIO( ( struct IORequest * )MyTimer->t_TimeRequest ) ;
  }
  else
  {  /* requirements not ok */
  }
}


/*
** abort the current ticking timer
*/
static void StopTicking( struct Timer *MyTimer )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct TimerNode *CurrentNode ;

  if( ( NULL != MyTimer ) && ( !( IsListEmpty( &MyTimer->t_TimerList ) ) ) )
  {  /* requirements ok */
    MyContext = MyTimer->t_Context ;
    SysBase = MyContext->c_SysBase ;
    CurrentNode = ( struct TimerNode * )MyTimer->t_TimerList.lh_Head ;
    AbortIO( ( struct IORequest * )MyTimer->t_TimeRequest ) ;
    WaitIO( ( struct IORequest * )MyTimer->t_TimeRequest ) ;
  }
  else
  {  /* there are no timers */
  }
}


/*
** callback function of the timeouter (name conflict with amiga.lib)
*/
static ULONG _DoTimer( struct Timer *MyTimer, ULONG TriggerSignals )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct TimerNode *CurrentNode ;
  
  if( NULL != MyTimer )
  {  /* requirements ok */
    MyContext = MyTimer->t_Context ;
    SysBase = MyContext->c_SysBase ;

    while( MyTimer->t_TimeRequest == ( struct timerequest * )GetMsg( MyTimer->t_MsgPort ) )
    {  /* our timerequest returned, so current timeouter timed out */
      CurrentNode = ( struct TimerNode * )RemHead( &MyTimer->t_TimerList ) ;
      StartTicking( MyTimer ) ;
      if( NULL != CurrentNode->tn_DoFunction )
      {  /* call the do function of this timeouter node */
        ( CurrentNode->tn_DoFunction )( CurrentNode->tn_DoData ) ;
      }
    }
  }
  else
  {  /* requirements not ok */
  }
  
  TriggerSignals = ( 1UL << MyTimer->t_MsgPort->mp_SigBit ) ;
  
  return( TriggerSignals ) ;
}


/*
** create the timeouter
*/
struct Timer *CreateTimer( struct Context *MyContext )
{
  struct ExecBase *SysBase ;
  struct Timer *NewTimer ;
  
  NewTimer = NULL ;
  
  if( ( NULL != MyContext ) && ( NULL != MyContext->c_Dispatcher )  )
  {  /* requirements seem ok */
    SysBase = MyContext->c_SysBase ;
    NewTimer = AllocVec( sizeof( struct Timer ), MEMF_ANY | MEMF_CLEAR ) ;
    if( NULL != NewTimer )
    {  /* memory for timeouter context ok */
      NewTimer->t_Context = MyContext ;
      NewList( &NewTimer->t_TimerList ) ;

      NewTimer->t_MsgPort = CreateMsgPort( ) ;
      if( NULL != NewTimer->t_MsgPort )
      {  /* message port ok */
        NewTimer->t_TimeRequest = ( struct timerequest * )CreateIORequest( NewTimer->t_MsgPort , sizeof ( struct timerequest ) ) ;
        if( NULL != NewTimer->t_TimeRequest )
        {  /* time io request ok */
          OpenDevice( TIMERNAME, UNIT_WAITUNTIL, ( struct IORequest * )NewTimer->t_TimeRequest, 0 ) ;
          if( 0 == NewTimer->t_TimeRequest->tr_node.io_Error )
          {  /* timer device ok */
            NewTimer->t_TimerBase = NewTimer->t_TimeRequest->tr_node.io_Device ;
            NewTimer->t_DispatcherNode.dn_Signals = ( 1UL << NewTimer->t_MsgPort->mp_SigBit ) ;
            NewTimer->t_DispatcherNode.dn_DoFunction = ( DISPATCHER_DOFUNCTION )_DoTimer ;
            NewTimer->t_DispatcherNode.dn_DoData = NewTimer ;
            AddDispatcherNode( MyContext->c_Dispatcher, &NewTimer->t_DispatcherNode ) ;
          }
          else
          {  /* timer device not ok */
            LogText( MyContext->c_Log, ERROR_LEVEL, "timer device not ok" ) ;
            DeleteTimer( NewTimer ) ;
            NewTimer = NULL ;
          }
        }
        else
        {  /* time io request not ok */
          LogText( MyContext->c_Log, ERROR_LEVEL, "time io request not ok" ) ;
          DeleteTimer( NewTimer ) ;
          NewTimer = NULL ;
        }
      }
      else
      {  /* message port not ok */
        LogText( MyContext->c_Log, ERROR_LEVEL, "timer message port not ok" ) ;
        DeleteTimer( NewTimer ) ;
        NewTimer = NULL ;
      }
    }
    else
    {  /* memory for timeouter context not ok */
      LogText( MyContext->c_Log, ERROR_LEVEL, "timer context not ok" ) ;
    }
  }
  else
  {  /* requirements not ok */
  }

  return( NewTimer ) ;
}


/*
** free all resources used by timerouter
*/
void DeleteTimer( struct Timer *OldTimer )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;

  if( NULL != OldTimer )
  {  /* timeouter context needs to be freed */
    MyContext = OldTimer->t_Context ;
    SysBase = MyContext->c_SysBase ;
    if( NULL != OldTimer->t_MsgPort )
    {  /* message port needs to be closed */
      if( NULL != OldTimer->t_TimeRequest )
      {  /* time io requeset needs to be deleted */
        if( NULL != OldTimer->t_TimerBase )
        {  /* device needs to be closed */
          RemDispatcherNode( MyContext->c_Dispatcher, &OldTimer->t_DispatcherNode ) ;
          CloseDevice( ( struct IORequest * )OldTimer->t_TimeRequest ) ;
        }
        DeleteIORequest( ( struct IORequest * )OldTimer->t_TimeRequest ) ;
      }
      DeleteMsgPort( OldTimer->t_MsgPort ) ;
    }
    FreeVec( OldTimer ) ;
  }
}


/*
** start a timeouter node by adding to list of active timers
*/
LONG AddTimerNode( struct Timer *MyTimer, struct TimerNode *StartNode )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct Device *TimerBase ;
  struct Node *WalkNode ;
  struct timeval CurrentTimeval ;
  ULONG Error ;
  
  Error = TRUE ;
  
  if( ( NULL != MyTimer ) && ( NULL != StartNode ) )
  {  /* requirements ok */
    MyContext = MyTimer->t_Context ;
    SysBase = MyContext->c_SysBase ;
    TimerBase = MyTimer->t_TimerBase ;

    GetSysTime( &CurrentTimeval ) ;
    if( 0 == StartNode->tn_AbsoluteTime.tv_secs )
    {  /* update start time if requested by putting 0 in secs value */
      StartNode->tn_AbsoluteTime = CurrentTimeval ;
    }
    AddTime( &StartNode->tn_AbsoluteTime, &StartNode->tn_DeltaTime ) ;
    if( 1 == CmpTime( &CurrentTimeval, &StartNode->tn_AbsoluteTime ) )
    {  /* new node has more time than current time */
      WalkNode = MyTimer->t_TimerList.lh_Head ;
      while( WalkNode->ln_Succ )
      {  /* loop all current list entries */
        if( 1 == CmpTime( &StartNode->tn_AbsoluteTime, &( ( struct TimerNode * )WalkNode )->tn_AbsoluteTime ) )
        {  /* this one takes longer than the new one */
          break ;
        }
        WalkNode = WalkNode->ln_Succ ;
      }
      if( WalkNode == MyTimer->t_TimerList.lh_Head )
      {  /* inset at head of list */
        StopTicking( MyTimer ) ;
        AddHead( &MyTimer->t_TimerList, &StartNode->tn_Node ) ;
        StartTicking( MyTimer ) ;
      }
      else if( NULL == WalkNode->ln_Succ )
      {  /* insert at tail of list */
        AddTail( &MyTimer->t_TimerList, &StartNode->tn_Node ) ;
      }
      else
      {  /* insert somewhere in the list */
        Insert( &MyTimer->t_TimerList, &StartNode->tn_Node, WalkNode->ln_Pred ) ;
      }
      
      Error = FALSE ;
    }
    else
    {
    }
  }
  else
  {  /* requirements not ok */
  }
  
  return( Error ) ;
}


/*
** remove a timer client node from the list of active timers
*/
void RemTimerNode( struct Timer *MyTimer, struct TimerNode *CancelNode )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct Node *WalkNode ;

  if( ( NULL != MyTimer ) && ( NULL != CancelNode ) )
  {  /* requirements ok */
    MyContext = MyTimer->t_Context ;
    SysBase = MyContext->c_SysBase ;

    WalkNode = MyTimer->t_TimerList.lh_Head ;
    while( WalkNode->ln_Succ )
    {  /* loop all current list entries */
      if( WalkNode == ( struct Node * )CancelNode )
      {  /* this is the node to remove */
         break ;
      }
      WalkNode = WalkNode->ln_Succ ;
    }
    if( WalkNode == MyTimer->t_TimerList.lh_Head )
    {  /* remove the head of the list */
      StopTicking( MyTimer ) ;
      RemHead( &MyTimer->t_TimerList ) ;
      StartTicking( MyTimer ) ;
    }
    else if( NULL == WalkNode->ln_Succ )
    {  /* node not in the list */
    }
    else
    {  /* remove any other node */
      Remove( WalkNode ) ;
    }
  }
}


/*
** get utility clock struct filled
*/
void GetClock( struct Timer *MyTimer, struct ClockData *MyClockData )
{
  struct Context *MyContext ;
  struct Device *TimerBase ;
  struct Library *UtilityBase ;
  struct timeval CurrentTimeval ;

  if( ( NULL != MyTimer ) && ( NULL != MyClockData ) )
  {  /* requirements ok */
    MyContext = MyTimer->t_Context ;
    UtilityBase = MyContext->c_UtilityBase ;
    TimerBase = MyTimer->t_TimerBase ;

    GetSysTime( &CurrentTimeval ) ;
    //printf( "secs: %ld, mirco: %ld\n", CurrentTimeVal.tv_secs, CurrentTimeVal.tv_micro ) ;
    Amiga2Date( CurrentTimeval.tv_secs, MyClockData ) ;
    //printf( "%d-%d-%d %d:%d:%d\n", MyClockData->year, MyClockData->month, MyClockData->mday, MyClockData->hour, MyClockData->min, MyClockData->sec ) ;
  }
  else
  {  /* requirements not ok */
    if( NULL != MyClockData )
    {  /* clear the data */
      MyClockData->year = MyClockData->month = MyClockData->mday = MyClockData->hour = MyClockData->min = MyClockData->sec = 0 ;
    }
  }
}


#if 0
/*
** create a date/time string based on current time and the given template
*/
LONG GetCurrentClockString( struct Timer *MyTimer, STRPTR MyTemplate, STRPTR MyString, ULONG MyLength )
{
  LONG StringWalker, TemplateWalker ;
  struct ClockData MyClockData ;
  
  StringWalker = 0 ;
  if( ( NULL != MyTimer ) && ( NULL != MyTemplate ) && ( NULL != MyString ) && ( 0 < MyLength ) )
  {
    TemplateWalker = 0 ;
    GetClock( MyTimer, &MyClockData ) ;
    
    while( '\0' != MyTemplate[ TemplateWalker ] )
    {  /* walk through the template */
      if( '%' != MyTemplate[ TemplateWalker ] )
      {  /* no specifier */
        MyString[ StringWalker ] = MyTemplate[ TemplateWalker ] ;
        TemplateWalker++ ;
        StringWalker++ ;
      }
      else
      {  /* a specifier */
        TemplateWalker++ ;
        switch( MyTemplate[ TemplateWalker ] )
        {  /* time/data placeholder taken from strftime() */
          case 'Y':  /* year with 4 digits */
            StringWalker += sprintf( &MyString[ StringWalker ], "%04d", MyClockData.year ) ;
            break ;
          case 'm':  /* month, 2 digits */
            StringWalker += sprintf( &MyString[ StringWalker ], "%02d", MyClockData.month ) ;
            break ;
          case 'd':  /* day, 2 digits */
            StringWalker += sprintf( &MyString[ StringWalker ], "%02d", MyClockData.mday ) ;
            break ;
          case 'H':  /* hour, 24h format, 2 digits */
            StringWalker += sprintf( &MyString[ StringWalker ], "%02d", MyClockData.hour ) ;
            break ;
          case 'M':  /* minutes, 2 digits */
            StringWalker += sprintf( &MyString[ StringWalker ], "%02d", MyClockData.min ) ;
            break ;
          case 'S':  /* seconds, 2 digits */
            StringWalker += sprintf( &MyString[ StringWalker ], "%02d", MyClockData.sec ) ;
            break ;
          default:  /* unhandled specifier */
            StringWalker += sprintf( &MyString[ StringWalker ]/*, MyLength*/, "%%%c", MyTemplate[ TemplateWalker ] ) ;
            break ;
        }
        TemplateWalker++ ;
      }
    }
    MyString[ StringWalker ] = '\0' ;
  }
  else
  {  /* requirements not ok */
  }

  return( StringWalker ) ;
}
#endif
