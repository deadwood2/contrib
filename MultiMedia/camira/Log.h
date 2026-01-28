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
** Log.h
*/


#ifndef _LOG_H
#define _LOG_H


#include "Context.h"


/*
** possible log levels
*/
enum LogLevel
{
  OFF_LEVEL,
  FAILURE_LEVEL,
  ERROR_LEVEL,
  WARNING_LEVEL,
  INFO_LEVEL,
  DEBUG_LEVEL,
  NUM_LEVELS
} ;


/*
** strings belonging to a log entry
*/
#define LE_TIMESTRING_LENGTH ( 16 )
#define LE_LEVELSTRING_LENGTH ( 16 )
#define LE_MESSAGESTRING_LENGTH ( 128 )
struct LogEntry
{
  struct Node le_Node ;  /* to be linked in a list */
  BYTE le_TimeString[ LE_TIMESTRING_LENGTH ] ;
  BYTE le_LevelString[ LE_LEVELSTRING_LENGTH ] ;
  BYTE le_MessageString[ LE_MESSAGESTRING_LENGTH ] ;
} ;


/*
** log context
*/
#define L_FILEBUFFER_LENGTH ( 1024 )
struct Log
{
  struct Context *l_Context ;  /* pointer to the global context */
  LONG l_CurrentLogLevel ;  /* current log level from settings */
  APTR l_GuiObject ;  /* supplied by log window */
  struct SignalSemaphore l_Semaphore ;  /* guard */
  struct List l_LoggedList ;  /* list of logged entries, like in the gui */
  struct List l_PendingList ;  /* list of still pending entries */
  BOOL l_LogToGui ;  /* if we can log to the gui */
  BOOL l_LogLocked ;  /* set after 3rd party process tried to log something */
  BPTR l_File ;
  UBYTE l_FileBuffer[ L_FILEBUFFER_LENGTH ] ;
  LONG l_FileBufferPosition ;
} ;


/*
** functions of this module
*/
struct Log *CreateLog( struct Context *MyContext ) ;
void DeleteLog( struct Log *OldLog ) ;
void ConfigureLog( struct Log *MyLog ) ;
void LogText( struct Log *MyLog, LONG MyLevel, STRPTR MyFormat, ... ) ;
void UnlockLog( struct Log *MyLog ) ;
void ClearLog( struct Log *MyLog ) ;
LONG SaveLog( struct Log *MyLog, STRPTR MyPath ) ;
void EnableLogGui( struct Log *MyLog, APTR MyGuiObject ) ;
void DisableLogGui( struct Log *MyLog ) ;

#endif  /* !_LOG_H */
