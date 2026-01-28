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
** SettingsWindow.h
*/


#ifndef _SETTINGSWINDOW_H
#define _SETTINGSWINDOW_H


#include "SettingsWindowClass.h"


/*
** settings window context
*/
struct SettingsWindow
{
  struct Context *sw_Context ;
  APTR sw_WindowObject ;
} ;


/*
** functions of this module
*/
struct SettingsWindow *CreateSettingsWindow( struct Context *MyContext ) ;
void DeleteSettingsWindow( struct SettingsWindow *OldSettingsWindow ) ;


#endif  /* !_SETTINGSWINDOW_H */
