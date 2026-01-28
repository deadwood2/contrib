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
** LogWindowClass.h
*/


#ifndef _LOGWINDOWCLASS_H
#define _LOGWINDOWCLASS_H


#include "Magic.h"
#include "CompilerExtensions.h"
#include <utility/hooks.h>


/*
** custom methods of log window class
*/
#define MUIM_AddLogEntry ( MUIB_LOGWINDOW + 0x01 )
#define MUIM_FlushLog ( MUIB_LOGWINDOW + 0x02 )
#define MUIM_SaveLog ( MUIB_LOGWINDOW + 0x03 )
#define MUIM_ClearLogGui ( MUIB_LOGWINDOW + 0x04 )


/*
** private data for the log window class
*/
struct LogWindowData
{
  struct Context *lwd_Context ;
  struct Hook lwd_ListDisplayHook ;  /* map log enties to list entries */
  APTR lwd_LogList ;  /* list to collect log entries */
  APTR lwd_LogFileString ;  /* to store the path where to save the log */
} ;


/*
** functions of this module
*/
IPTR LogWindowDispatcher( REG( a0, struct IClass *MyIClass ), REG( a2, Object *MyObject ), REG( a1, Msg MyMsg ) ) ;


#endif  /* !_LOGWINDOWCLASS_H */
