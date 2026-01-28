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
** Scheduler.c
*/


#include "Scheduler.h"
#include "Settings.h"
#include "VideoHardware.h"
#include "Log.h"
#include <proto/exec.h>
#include <clib/alib_protos.h>
#include <stdio.h>


/*
** count down and trigger the configured requests
*/
static void DoSchedulerTimer( struct Scheduler *MyScheduler )
{
  struct Context *MyContext ;
  LONG Error ;
  
  MyContext = MyScheduler->s_Context ;

  do
  {  
    if( MyScheduler->s_ProgressBarNode.pbn_ButtonState )
    {  /* only if progress button is enabling us */
      MyScheduler->s_Counter-- ;
      ReportProgress( MyContext->c_ProgressBar, &MyScheduler->s_ProgressBarNode, MyScheduler->s_Counter ) ;
      if( 0 == MyScheduler->s_Counter )
      {  /* capture timeout finished */
        MyContext->c_CaptureRequest = MyScheduler->s_CurrentCaptureEnable ;
        MyContext->c_UploadRequest = MyScheduler->s_CurrentUploadEnable ;
        MyScheduler->s_Counter = MyScheduler->s_CurrentInterval ;
      }
    }
    Error = AddTimerNode( MyContext->c_Timer, &MyScheduler->s_TimerNode ) ;
  }
  while( Error ) ;
}


/*
** start the timer that triggers the display request
*/
static void StartSchedulerTimer( struct Scheduler *MyScheduler )
{
  struct Context *MyContext ;
  
  MyContext = MyScheduler->s_Context ;

  MyScheduler->s_TimerNode.tn_DeltaTime.tv_secs = 1 ;
  MyScheduler->s_TimerNode.tn_DeltaTime.tv_micro = 0 ;
  MyScheduler->s_TimerNode.tn_AbsoluteTime.tv_secs = 0 ;  /* delta is added to current time */
  MyScheduler->s_TimerNode.tn_DoFunction = ( void ( * )( APTR ) )DoSchedulerTimer ;
  MyScheduler->s_TimerNode.tn_DoData = MyScheduler ;
  AddTimerNode( MyContext->c_Timer, &MyScheduler->s_TimerNode ) ;
}


/*
** stop the timer that triggers the display request
*/
static void StopSchedulerTimer( struct Scheduler *MyScheduler )
{
  struct Context *MyContext ;
  
  MyContext = MyScheduler->s_Context ;

  RemTimerNode( MyContext->c_Timer, &MyScheduler->s_TimerNode ) ;
}


/*
** create the display job
*/
struct Scheduler *CreateScheduler( struct Context *MyContext )
{
  struct ExecBase *SysBase ;
  struct Scheduler *NewScheduler ;

  NewScheduler = NULL ;
  
  if( ( NULL != MyContext ) && ( NULL != MyContext->c_Timer ) )
  {  /* requirements seem ok */
    SysBase = MyContext->c_SysBase ;
    NewScheduler = AllocVec( sizeof( struct Scheduler ), MEMF_ANY | MEMF_CLEAR ) ;
    if( NULL != NewScheduler )
    {  /* memory for capture area context ok */
      NewScheduler->s_Context = MyContext ;

      NewScheduler->s_ProgressBarNode.pbn_Node.ln_Pri = 0 ;  /* lowest priorety */
      NewScheduler->s_ProgressBarNode.pbn_InfoString = "%ld" ;
      NewScheduler->s_ProgressBarNode.pbn_ButtonMode = MUIV_InputMode_Toggle ;
      NewScheduler->s_ProgressBarNode.pbn_ButtonState = 1 ;
    }
    else
    {  /* memory for capture area context not ok */
    }
  }
  else
  {  /* requirements not ok */
  }

  return( NewScheduler ) ;
}


/*
** uninit displaying of images
*/
void DeleteScheduler( struct Scheduler *OldScheduler )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
 
  if( NULL != OldScheduler )
  {  /* capture job context needs to be freed */
    MyContext = OldScheduler->s_Context ;
    SysBase = MyContext->c_SysBase ;
    StopSchedulerTimer( OldScheduler ) ;
    FreeVec( OldScheduler ) ;
  }
}


/*
** configure displaying of images according to setings
*/
void ConfigureScheduler( struct Scheduler *MyScheduler )
{
  struct Context *MyContext ;
  LONG SettingValue ;
  ULONG NewInterval, NewCaptureEnable, NewUploadEnable ;

  if( ( NULL != MyScheduler ) )
  {  /* requirements ok */
    MyContext = MyScheduler->s_Context ;
    
    SettingValue = GetSetting( MyContext->c_Settings, SchedulerInterval ) ;
    NewInterval = ( ULONG )SettingValue ;
    SettingValue = GetSetting( MyContext->c_Settings, CaptureEnable ) ;
    NewCaptureEnable = ( ULONG )SettingValue ;
    SettingValue = GetSetting( MyContext->c_Settings, UploadEnable ) ;
    NewUploadEnable = ( ULONG )SettingValue ;

    if( 0 > CheckVideoHardware( MyContext->c_VideoHardware ) )
    {  /* no video hardware, so also no display update */
      NewInterval = 0 ;
    }

    if( ( !( NewCaptureEnable || NewUploadEnable ) ) ||
        ( 0 == NewInterval ) ||
        ( MyScheduler->s_CurrentInterval != NewInterval ) )
    {  /* capture and upload disabled, scheduler disabled or interval changed */
      if( 0 != MyScheduler->s_CurrentInterval )
      {  /* stop timer and remove the progress bar node */
        LogText( MyContext->c_Log, DEBUG_LEVEL, "stopping scheduler" ) ;
        StopSchedulerTimer( MyScheduler ) ;
        RemProgressBarNode( MyContext->c_ProgressBar, &MyScheduler->s_ProgressBarNode ) ;
        MyScheduler->s_CurrentInterval = 0 ;
      }
    }
    
    if( ( NewCaptureEnable || NewUploadEnable ) &&
        ( 0 != NewInterval ) )
    {  /* capture or upload enabled and scheduler also enabled */
      if( 0 == MyScheduler->s_CurrentInterval )
      {  /* start timer and add the progress bar node */
        MyScheduler->s_CurrentInterval = MyScheduler->s_Counter = NewInterval ;
        MyScheduler->s_ProgressBarNode.pbn_Limit = MyScheduler->s_CurrentInterval ;
        AddProgressBarNode( MyContext->c_ProgressBar, &MyScheduler->s_ProgressBarNode, MyScheduler->s_CurrentInterval ) ;
        LogText( MyContext->c_Log, DEBUG_LEVEL, "starting scheduler" ) ;
        StartSchedulerTimer( MyScheduler ) ;
      }
      MyScheduler->s_CurrentCaptureEnable = NewCaptureEnable ;
      MyScheduler->s_CurrentUploadEnable = NewUploadEnable ;
    }
  }
  else
  {  /* requirements not ok */
  }
}
