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
** MainWindow.h
*/


#ifndef _MAINWINDOW_H
#define _MAINWINDOW_H


#include "MainWindowClass.h"
#include "DisplayAreaClass.h"
#include <vhi/vhi.h>


/*
** main window context
*/
struct MainWindow
{
  struct Context *mw_Context ;
  struct MUI_CustomClass *mw_MainWindowClass ;
  struct MUI_CustomClass *mw_RootGroupClass ;
  struct MUI_CustomClass *mw_PreviewGroupClass ;
  struct MUI_EventHandlerNode mw_EventHandlerNode ;
  APTR mw_WindowObject ;
  APTR mw_RootGroup ;
  APTR mw_PreviewGroup ;
  APTR mw_PreviewArea ;
  APTR mw_ControlGroup ;
  APTR mw_GrabButton ;
  APTR mw_GaugeGroup ;
  APTR mw_Gauge ;
  APTR mw_CancelButton ;
  APTR mw_HorizontalProp ;
  APTR mw_VerticalProp ;
  APTR mw_MenuStrip ;
  APTR mw_MenuProject ;
  APTR mw_MenuItemProjectAboutcAMIra ;
  APTR mw_MenuItemProjectAboutMUI ;
  APTR mw_MenuItemProjectQuit ;
  APTR mw_MenuSettings ;
  APTR mw_MenuItemSettingsVideohardware ;
  APTR mw_MenuItemSettingsImage ;
  APTR mw_MenuItemSettingsDisplay ;
  APTR mw_MenuItemSettingsCapture ;
  APTR mw_MenuItemSettingsUpload ;
  APTR mw_MenuItemSettingsMagic ;
  ULONG mw_CurrentZoom ;
  ULONG mw_CurrentImageWidth ;
  ULONG mw_CurrentImageHeight ;
  ULONG mw_CurrentAreaWidth ;
  ULONG mw_CurrentAreaHeight ;
  ULONG mw_CurrentHorizontalOffset ;
  ULONG mw_CurrentVerticalOffset ;
  ULONG mw_CurrentRatio ;
  struct vhi_image *mw_DefaultImage ;
  struct vhi_image *mw_CurrentImage ;
} ;


/*
** functions of this module
*/
struct MainWindow *CreateMainWindow( struct Context *MyContext ) ;
void DeleteMainWindow( struct MainWindow *OldMainWindow ) ;
void OpenMainWindow( struct MainWindow *MyMainWindow ) ;
void CloseMainWindow( struct MainWindow *MyMainWindow ) ;
//void DisplayImage( struct MainWindow *MyMainWindow, struct vhi_image *MyImage ) ;
void ConfigureGauge( struct MainWindow *MyMainWindow, STRPTR InfoText, ULONG Total, ULONG Initial ) ;
void UpdateGauge( struct MainWindow *MyMainWindow, ULONG Current ) ;
void EnableCancel( struct MainWindow *MyMainWindow ) ;
void DisableCancel( struct MainWindow *MyMainWindow ) ;


#endif  /* !_MAINWINDOW_H */
