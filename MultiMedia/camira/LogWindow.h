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
** LogWindow.h
*/


#ifndef _LOGWINDOW_H
#define _LOGWINDOW_H


#include "LogWindowClass.h"
#include <utility/hooks.h>


/*
** log window context
*/
struct LogWindow
{
  struct Context *lw_Context ;
  APTR lw_WindowObject ;
} ;


/*
** functions of this module
*/
struct LogWindow *CreateLogWindow( struct Context *MyContext ) ;
void DeleteLogWindow( struct LogWindow *OldLogWindow ) ;


#endif  /* !_LOGWINDOW_H */
