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
** MainWindowClass.h
*/


#ifndef _MAINWINDOWCLASS_H
#define _MAINWINDOWCLASS_H


#include "Magic.h"
#include "CompilerExtensions.h"
#include <intuition/classes.h>


/*
** custom methods of main window class
*/
#define MUIM_AddScrollBorders ( MUIB_MAINWINDOW + 0x01 )
#define MUIM_RemoveScrollBorders ( MUIB_MAINWINDOW + 0x02 )
#define MUIM_AddSettingsWindow ( MUIB_MAINWINDOW + 0x03 )
#define MUIM_RemoveSettingsWindow ( MUIB_MAINWINDOW + 0x04 )
#define MUIM_AddDisplayArea ( MUIB_MAINWINDOW + 0x05 )
#define MUIM_RemoveDisplayArea ( MUIB_MAINWINDOW + 0x06 )
#define MUIM_AddProgressBar ( MUIB_MAINWINDOW + 0x07 )
#define MUIM_RemoveProgressBar ( MUIB_MAINWINDOW + 0x08 )
#define MUIM_AddAboutWindow ( MUIB_MAINWINDOW + 0x09 )
#define MUIM_RemoveAboutWindow ( MUIB_MAINWINDOW + 0x0A )
#define MUIM_AddLogWindow ( MUIB_MAINWINDOW + 0x0B )
#define MUIM_RemoveLogWindow ( MUIB_MAINWINDOW + 0x0C )


/*
** custom attributes of main window class
*/


/*
** private data for the main window class
*/
struct MainWindowData
{
  struct Context *mwd_Context ;
  APTR mwd_RootObject ;
  APTR mwd_DisplayArea ;
  APTR mwd_ProgressBar ;
  APTR mwd_ReleaseButton ;
  APTR mwd_ReleaseGroup ;
  APTR mwd_MenuItemProjectAbout ;
  APTR mwd_MenuItemProjectAboutMUI ;
  APTR mwd_MenuItemViewLogWindow ;
  APTR mwd_MenuItemSettingsVideohardware ;
  APTR mwd_MenuItemSettingsStamp ;
  APTR mwd_MenuItemSettingsDisplay ;
  APTR mwd_MenuItemSettingsScheduler ;
  APTR mwd_MenuItemSettingsCapture ;
  APTR mwd_MenuItemSettingsUpload ;
  APTR mwd_MenuItemSettingsLog ;
  APTR mwd_MenuItemSettingsMUI ;
} ;


/*
** 
*/
IPTR MainWindowDispatcher( REG( a0, struct IClass *MyIClass ), REG( a2, Object *MyObject ), REG( a1, Msg MyMsg ) ) ;


#endif  /* !_MAINWINDOWCLASS_H */
