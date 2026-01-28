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
** Log.c
*/


#include "Log.h"
#include "LogWindow.h"
#include "Settings.h"
#include "Timer.h"
#include "Localization.h"
#include "cAMIraLib.h"
#include "ab_stdio.h"
#include <proto/exec.h>
#include <proto/dos.h>

#include <exec/rawfmt.h>

#include <clib/alib_protos.h>

#include <string.h>
#include <stdarg.h>


/*
** debug output function in vprintf style
*/
static void LogToSerial(const char *MyFormat, va_list MyValues)
{
    /* Keep the local SysBase trick if you are in an environment without startup code */
    struct ExecBase *SysBase = *((struct ExecBase **)4UL);

    /* Do not consume the caller's va_list object directly */
    va_list ap;
    va_copy(ap, MyValues);

    /* va_list-aware formatter -> serial */
    VNewRawDoFmt((CONST_STRPTR)MyFormat, RAWFMTFUNC_SERIAL, NULL, ap);

    va_end(ap);
}

static void LogToSerialF(const char *MyFormat, ...)
{
    va_list ap;
    va_start(ap, MyFormat);
    LogToSerial(MyFormat, ap);
    va_end(ap);
}


/*
**  show log entry in gui
*/
static LONG LogToGui( struct Log *MyLog, struct LogEntry *MyLogEntry )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  LONG Result ;

  MyContext = MyLog->l_Context ;
  SysBase = MyContext->c_SysBase ;
  Result = -1 ;
  
  if( NULL != MyLog->l_GuiObject )
  {  /* requirements ok */
    if( MyContext->c_Process == ( struct Process * )FindTask( NULL ) )
    {  /* our main process is ok to use the log gui */
      Result = DoMethod( MyLog->l_GuiObject, MUIM_AddLogEntry, MyLogEntry ) ;
    }
    else
    {  /* other process is not ok to use the log gui */
    }
  }
  else
  {  /* requirements not ok */
  }
  
  return( Result ) ;
}


/*
** show log entry on the console
*/
static LONG LogToConsole(struct Log *MyLog, struct LogEntry *MyLogEntry)
{
    struct Context    *MyContext;
    struct DosLibrary *DOSBase;
    LONG Result;

    /* VPrintf expects RAWARG-style argument vector, not a va_list */
    IPTR argv[3];

    MyContext = MyLog->l_Context;
    DOSBase   = MyContext->c_DOSBase;
    Result    = -1;

    argv[0] = (IPTR)MyLogEntry->le_TimeString;
    argv[1] = (IPTR)MyLogEntry->le_LevelString;
    argv[2] = (IPTR)MyLogEntry->le_MessageString;

    if ((DOSBase != NULL) && (Output() != (BPTR)0) &&
        (VPrintf("%s, %s, %s\n", (RAWARG)argv) >= 0))
    {
        Result = 0;
    }
    else
    {
        /* Required: fallback uses LogToSerial (via the wrapper) */
        LogToSerialF("%s, %s, %s\n",
                     MyLogEntry->le_TimeString,
                     MyLogEntry->le_LevelString,
                     MyLogEntry->le_MessageString);
        Result = 0;
    }

    return Result;
}


/*
** show log entry somewhere
*/
static void OutputLogEntry( struct Log *MyLog, struct LogEntry *MyLogEntry )
{
  struct ExecBase *SysBase ;
  struct Context *MyContext ;
  LONG Error ;
  
  if( ( NULL != MyLogEntry ) )
  {  /* requirements ok */
    MyContext = MyLog->l_Context ;
    SysBase = MyContext->c_SysBase ;

    ObtainSemaphore( &MyLog->l_Semaphore ) ;
    
    if( TRUE == MyLog->l_LogToGui )
    {  /* show log entry in the gui */
      if( ( MyContext->c_Process == ( ( struct Process * )FindTask( NULL ) ) ) && ( !( MyLog->l_LogLocked ) ) )
      {  /* our main process is ok to use the log gui */
        Error = LogToGui( MyLog, MyLogEntry ) ;
      }
      else
      {  /* other process is not ok to use the log gui */
        Error = MyLog->l_LogLocked = MyContext->c_LogRequest = 1 ;
      }
      if( Error )
      {  /* error while adding to log gui */
        AddTail( &MyLog->l_PendingList, ( struct Node * )MyLogEntry ) ;
      }
      else
      {  /* no error while adding to log gui */
        AddTail( &MyLog->l_LoggedList, ( struct Node * )MyLogEntry ) ;
      }
    }
    else
    {  /* show log entry somewhere else */
      LogToConsole( MyLog, MyLogEntry ) ;
      FreeVec( MyLogEntry ) ;
    }

    ReleaseSemaphore( &MyLog->l_Semaphore ) ;
  }
  else
  {  /* requirements not ok */
  }
}


/*
** show pending entries somewhere
*/
static void OutputPendingLogEntries( struct Log *MyLog )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct LogEntry *MyLogEntry ;

  if( !( IsListEmpty( &MyLog->l_PendingList ) ) )
  {  /* requirements ok */
    MyContext = MyLog->l_Context ;
    SysBase = MyContext->c_SysBase ;

    while( ( MyLogEntry = ( struct LogEntry * )RemHead( &MyLog->l_PendingList ) ) )
    {  /* pending log entries need to be shown */
      OutputLogEntry( MyLog, MyLogEntry ) ;
    }
  }
  else
  {  /* requirements not ok */
  }
}


/*
** unlock log again after other process tried to log something
*/
void UnlockLog( struct Log *MyLog )
{
  if( NULL != MyLog )
  {  /* requirements ok */
    MyLog->l_LogLocked = 0 ;
    OutputPendingLogEntries( MyLog ) ;
  }
  else
  {  /* requirements not ok */
  }
}


/*
** log some formatted message
*/
void LogText( struct Log *MyLog, LONG MyLevel, STRPTR MyFormat, ... )
{
  struct ExecBase *SysBase ;
  struct Context *MyContext ;
  va_list MyValues ;
  BOOL Logged ;
  struct LogEntry *MyLogEntry ;
  struct ClockData MyClockData ;
  struct ExpandData MyExpandData ;

  Logged = FALSE ;
  va_start( MyValues, MyFormat ) ;

  if( ( NULL != MyLog ) )
  {  /* requirements ok */
    MyContext = MyLog->l_Context ;
    SysBase = MyContext->c_SysBase ;

    if( MyLog->l_CurrentLogLevel >= MyLevel )
    {  /* log level ok */
      MyLogEntry = AllocVec( sizeof( struct LogEntry ), MEMF_ANY | MEMF_CLEAR ) ;
      if( NULL != MyLogEntry )
      {  /* log entry ok */
        GetClock( MyContext->c_Timer, &MyClockData ) ;
        MyExpandData.ed_ClockData = &MyClockData ;
        ExpandString( MyLogEntry->le_TimeString, LE_TIMESTRING_LENGTH, "%H:%M:%S", &MyExpandData ) ;
        switch( MyLevel )
        {
          case FAILURE_LEVEL:
            snprintf( MyLogEntry->le_LevelString, LE_LEVELSTRING_LENGTH, "%s", 
              GetLocalizedString( MyContext->c_Localization, STR_LOGLEVEL_FAILURE ) ) ;
            break ;
          case ERROR_LEVEL:
            snprintf( MyLogEntry->le_LevelString, LE_LEVELSTRING_LENGTH, "%s", 
              GetLocalizedString( MyContext->c_Localization, STR_LOGLEVEL_ERROR ) ) ;
            break ;
          case WARNING_LEVEL:
            snprintf( MyLogEntry->le_LevelString, LE_LEVELSTRING_LENGTH, "%s", 
              GetLocalizedString( MyContext->c_Localization, STR_LOGLEVEL_WARNING ) ) ;
            break ;
          case INFO_LEVEL:
            snprintf( MyLogEntry->le_LevelString, LE_LEVELSTRING_LENGTH, "%s",
            GetLocalizedString( MyContext->c_Localization, STR_LOGLEVEL_INFO ) ) ;
            break ;
          case DEBUG_LEVEL:
            snprintf( MyLogEntry->le_LevelString, LE_LEVELSTRING_LENGTH, "%s",
              GetLocalizedString( MyContext->c_Localization, STR_LOGLEVEL_DEBUG ) ) ;
            break ;
          default:
            snprintf( MyLogEntry->le_LevelString, LE_LEVELSTRING_LENGTH, "%s", "???" ) ;
            break ;
        }
        vsnprintf( MyLogEntry->le_MessageString, LE_MESSAGESTRING_LENGTH, MyFormat, MyValues ) ;
        OutputLogEntry( MyLog, MyLogEntry ) ;
        Logged = TRUE ;
      }
      else
      {  /* log entry not ok */
      }
    }
    else
    {  /* log level too low */
      Logged = TRUE ;
    }
  }
  else
  {  /* requirements not ok */
  }

  if( FALSE == Logged )
  {  /* last resort logging */
    LogToSerial( MyFormat, MyValues ) ;
    LogToSerial( "\n", NULL ) ;
  }
  va_end( MyValues ) ;
}


/*
** clear all log entries
*/
void ClearLog( struct Log *MyLog )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct LogEntry *OldLogEntry ;

  if( NULL != MyLog )
  {  /* requirements ok */
    MyContext = MyLog->l_Context ;
    SysBase = MyContext->c_SysBase ;

    if( !( IsListEmpty( &MyLog->l_LoggedList ) ) )
    {  /* logged entries need to be freed */
      if( NULL != MyLog->l_GuiObject )
      {  /* log gui needs to be cleared */
        DoMethod( MyLog->l_GuiObject, MUIM_ClearLogGui ) ;
      }
      while( ( OldLogEntry = ( struct LogEntry * )RemHead( &MyLog->l_LoggedList ) ) )
      {  /* logged entries need to be freed */
        FreeVec( OldLogEntry ) ;
      }
    }
  }
  else
  {  /* requirements not ok */
  }
}


/*
** transfer the buffer content to the file
*/
static void FlushFileBuffer( struct Log *MyLog )
{
  struct Context *MyContext ;
  struct DosLibrary *DOSBase ;

  MyContext = MyLog->l_Context ;
  DOSBase = MyContext->c_DOSBase ;
  
  Write( MyLog->l_File, MyLog->l_FileBuffer, MyLog->l_FileBufferPosition ) ;
  MyLog->l_FileBufferPosition = 0 ;
}


/*
** buffer the file data and call flush if required
*/
static void WriteFileBuffer( struct Log *MyLog, UBYTE *Data, ULONG Lenght )
{
  LONG Count ;
  
  for( Count = 0 ; Count < Lenght ; Count++ )
  {
    MyLog->l_FileBuffer[ MyLog->l_FileBufferPosition ] = Data[ Count ] ;
    MyLog->l_FileBufferPosition++ ;
    if( L_FILEBUFFER_LENGTH <= MyLog->l_FileBufferPosition )
    {
      FlushFileBuffer( MyLog ) ;
    }
  }
}


/*
** save all log entries to a file
*/
LONG SaveLog( struct Log *MyLog, STRPTR MyPath )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct DosLibrary *DOSBase ;
  struct Node *WalkNode ;
  struct LogEntry *MyLogEntry ;

  if( ( NULL != MyLog ) && ( NULL != MyLog->l_Context->c_DOSBase ) )
  {  /* requirements ok */
    MyContext = MyLog->l_Context ;
    SysBase = MyContext->c_SysBase ;
    DOSBase = MyContext->c_DOSBase ;
    MyLog->l_File = Open( MyPath, MODE_NEWFILE ) ;
    if( NULL != MyLog->l_File )
    {  /* log file ok */
      MyLog->l_FileBufferPosition = 0 ;
      WalkNode = MyLog->l_LoggedList.lh_Head ;
      while( WalkNode->ln_Succ )
      {  /* loop all current list entries */
        MyLogEntry = ( struct LogEntry * )WalkNode ;
        WriteFileBuffer( MyLog, MyLogEntry->le_TimeString, strlen( MyLogEntry->le_TimeString ) ) ;
        WriteFileBuffer( MyLog, ", ", 2 ) ;
        WriteFileBuffer( MyLog, MyLogEntry->le_LevelString, strlen( MyLogEntry->le_LevelString ) ) ;
        WriteFileBuffer( MyLog, ", ", 2 ) ;
        WriteFileBuffer( MyLog, MyLogEntry->le_MessageString, strlen( MyLogEntry->le_MessageString ) ) ;
        WriteFileBuffer( MyLog, "\n", 1 ) ;
        WalkNode = WalkNode->ln_Succ ;
      }
      FlushFileBuffer( MyLog ) ;
      Close( MyLog->l_File ) ;
    }
    else
    {  /* log file not ok */
      LogText( MyLog, ERROR_LEVEL, "log file not ok" ) ;
    }
  }
  else
  {  /* requirements not ok */
  }

  return( 0 ) ;
}


/*
** delete log
*/
void DeleteLog( struct Log *MyLog )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  
  if( ( NULL != MyLog ) )
  {  /* log context needs to be freed */
    MyContext = MyLog->l_Context ;
    SysBase = MyContext->c_SysBase ;
    
    DisableLogGui( MyLog ) ;
    ClearLog( MyLog ) ;
    FreeVec( MyLog ) ;
  }
}


/*
** prepare/configure the log
*/
void ConfigureLog( struct Log *MyLog )
{
  struct Context *MyContext ;
  LONG SettingValue ;
  LONG NewLogLevel ;
  LONG NewNoGUI ;
  
  if( NULL != MyLog )
  {  /* requirements ok */
    MyContext = MyLog->l_Context ;
    
    SettingValue = GetSetting( MyContext->c_Settings, LogLevel ) ;
    NewLogLevel = ( LONG )SettingValue ;
    SettingValue = GetSetting( MyContext->c_Settings, Nogui ) ;
    NewNoGUI = ( LONG )SettingValue ;

    if( MyLog->l_CurrentLogLevel != NewLogLevel )
    {  /* level changed */
      MyLog->l_CurrentLogLevel = NewLogLevel ;
    }
    if( FALSE != NewNoGUI )
    {  /* gui mode disabled */
      DisableLogGui( MyLog ) ;
    }
  }
  else
  {  /* requirements not ok */
  }
}


/*
** create the log
*/
struct Log *CreateLog( struct Context *MyContext )
{
  struct ExecBase *SysBase ;
  struct Log *NewLog ;

  NewLog = NULL ;
  
  if( ( NULL != MyContext ) )
  {  /* requirements seem ok */
    SysBase = MyContext->c_SysBase ;
    NewLog = AllocVec( sizeof( struct Log ), MEMF_ANY | MEMF_CLEAR ) ;
    if( NULL != NewLog )
    {  /* memory for log context ok */
      NewLog->l_Context = MyContext ;
      NewList( &NewLog->l_LoggedList ) ;
      NewList( &NewLog->l_PendingList ) ;
      InitSemaphore( &NewLog->l_Semaphore ) ;
      NewLog->l_CurrentLogLevel = WARNING_LEVEL ;
      NewLog->l_LogToGui = TRUE ;
      NewLog->l_GuiObject = NULL ;
    }
    else
    {  /* memory for log context not ok */
    }
  }
  else
  {  /* requirements not ok */
  }

  return( NewLog ) ;
}


/*
** enable gui logging
*/
void EnableLogGui( struct Log *MyLog, APTR MyGuiObject )
{
  if( NULL != MyLog )
  {  /* requirements seem ok */
    MyLog->l_GuiObject = MyGuiObject ;
    MyLog->l_LogToGui = TRUE ;
    OutputPendingLogEntries( MyLog ) ;
  }
  else
  {  /* requirements not ok */
  }
}


/*
** disable gui logging
*/
void DisableLogGui( struct Log *MyLog )
{
  if( NULL != MyLog )
  {  /* requirements seem ok */
    MyLog->l_GuiObject = NULL ;
    MyLog->l_LogToGui = FALSE ;
    OutputPendingLogEntries( MyLog ) ;
  }
  else
  {  /* requirements not ok */
  }
}

