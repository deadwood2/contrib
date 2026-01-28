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
** MainWindowClass.c
*/


#include "MainWindowClass.h"
#include "DisplayAreaClass.h"
#include "SettingsWindowClass.h"
#include "Localization.h"
#include <proto/muimaster.h>
#include <libraries/mui.h>
#include <libraries/gadtools.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <clib/alib_protos.h>
#include <stdio.h>
//#include <vhi/vhi.h>


/*
** connect a log window to main window
*/
static IPTR MainWindowAddLogWindow( struct IClass *MyIClass, Object *MyObject, Object *MyLogWindowObject )
{
  struct MainWindowData *MyData ;
  struct IntuitionBase *IntuitionBase ;

  MyData = INST_DATA( MyIClass, MyObject ) ;
  IntuitionBase = MyData->mwd_Context->c_IntuitionBase ;
  
  DoMethod( MyData->mwd_MenuItemViewLogWindow, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
            MyLogWindowObject, 3, MUIM_Set, MUIA_Window_Open, TRUE ) ;
  SetAttrs( MyData->mwd_MenuItemViewLogWindow,
            MUIA_Menuitem_Enabled, TRUE,
            TAG_END ) ;

  return( 0 ) ;
}


/*
** disconnect a Log window from main window
*/
static IPTR MainWindowRemoveLogWindow( struct IClass *MyIClass, Object *MyObject, Object *MyLogWindowObject )
{
  struct MainWindowData *MyData ;
  struct IntuitionBase *IntuitionBase ;

  MyData = INST_DATA( MyIClass, MyObject ) ;
  IntuitionBase = MyData->mwd_Context->c_IntuitionBase ;
  
  SetAttrs( MyData->mwd_MenuItemViewLogWindow,
            MUIA_Menuitem_Enabled, FALSE,
            TAG_END ) ;
  DoMethod( MyData->mwd_MenuItemViewLogWindow, MUIM_KillNotify, MUIA_Menuitem_Trigger ) ;

  return( 0 ) ;
}


/*
** connect a about window to main window
*/
static IPTR MainWindowAddAboutWindow( struct IClass *MyIClass, Object *MyObject, Object *MyAboutWindowObject )
{
  struct MainWindowData *MyData ;
  struct IntuitionBase *IntuitionBase ;

  MyData = INST_DATA( MyIClass, MyObject ) ;
  IntuitionBase = MyData->mwd_Context->c_IntuitionBase ;
  
  DoMethod( MyData->mwd_MenuItemProjectAbout, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
            MyAboutWindowObject, 3, MUIM_Set, MUIA_Window_Open, TRUE ) ;
  SetAttrs( MyData->mwd_MenuItemProjectAbout,
            MUIA_Menuitem_Enabled, TRUE,
            TAG_END ) ;

  return( 0 ) ;
}


/*
** disconnect a about window from main window
*/
static IPTR MainWindowRemoveAboutWindow( struct IClass *MyIClass, Object *MyObject, Object *MyAboutWindowObject )
{
  struct MainWindowData *MyData ;
  struct IntuitionBase *IntuitionBase ;

  MyData = INST_DATA( MyIClass, MyObject ) ;
  IntuitionBase = MyData->mwd_Context->c_IntuitionBase ;
  
  SetAttrs( MyData->mwd_MenuItemProjectAbout,
            MUIA_Menuitem_Enabled, FALSE,
            TAG_END ) ;
  DoMethod( MyData->mwd_MenuItemProjectAbout, MUIM_KillNotify, MUIA_Menuitem_Trigger ) ;

  return( 0 ) ;
}


/*
** connect a settings window to main window
*/
static IPTR MainWindowAddSettingsWindow( struct IClass *MyIClass, Object *MyObject, Object *MySettingsWindowObject )
{
  struct MainWindowData *MyData ;
  struct IntuitionBase *IntuitionBase ;

  MyData = INST_DATA( MyIClass, MyObject ) ;
  IntuitionBase = MyData->mwd_Context->c_IntuitionBase ;
  
  DoMethod( MyData->mwd_MenuItemSettingsVideohardware, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
            MySettingsWindowObject, 2, MUIM_SettingsWindowOpen, VIDEOHARDWARE_CATEGORY ) ;
  SetAttrs( MyData->mwd_MenuItemSettingsVideohardware,
            MUIA_Menuitem_Enabled, TRUE,
            TAG_END ) ;
  DoMethod( MyData->mwd_MenuItemSettingsStamp, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
            MySettingsWindowObject, 2, MUIM_SettingsWindowOpen, STAMP_CATEGORY ) ;
  SetAttrs( MyData->mwd_MenuItemSettingsStamp,
            MUIA_Menuitem_Enabled, TRUE,
            TAG_END ) ;
  DoMethod( MyData->mwd_MenuItemSettingsDisplay, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
            MySettingsWindowObject, 2, MUIM_SettingsWindowOpen, DISPLAY_CATEGORY ) ;
  SetAttrs( MyData->mwd_MenuItemSettingsDisplay,
            MUIA_Menuitem_Enabled, TRUE,
            TAG_END ) ;
  DoMethod( MyData->mwd_MenuItemSettingsScheduler, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
            MySettingsWindowObject, 2, MUIM_SettingsWindowOpen, SCHEDULER_CATEGORY ) ;
  SetAttrs( MyData->mwd_MenuItemSettingsScheduler,
            MUIA_Menuitem_Enabled, TRUE,
            TAG_END ) ;
  DoMethod( MyData->mwd_MenuItemSettingsCapture, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
            MySettingsWindowObject, 2, MUIM_SettingsWindowOpen, CAPTURE_CATEGORY ) ;
  SetAttrs( MyData->mwd_MenuItemSettingsCapture,
            MUIA_Menuitem_Enabled, TRUE,
            TAG_END ) ;
  DoMethod( MyData->mwd_MenuItemSettingsUpload, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
            MySettingsWindowObject, 2, MUIM_SettingsWindowOpen, UPLOAD_CATEGORY ) ;
  SetAttrs( MyData->mwd_MenuItemSettingsUpload,
            MUIA_Menuitem_Enabled, TRUE,
            TAG_END ) ;
  DoMethod( MyData->mwd_MenuItemSettingsLog, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
            MySettingsWindowObject, 2, MUIM_SettingsWindowOpen, LOG_CATEGORY ) ;
  SetAttrs( MyData->mwd_MenuItemSettingsLog,
            MUIA_Menuitem_Enabled, TRUE,
            TAG_END ) ;

  return( 0 ) ;
}


/*
** disconnect a settings window from main window
*/
static IPTR MainWindowRemoveSettingsWindow( struct IClass *MyIClass, Object *MyObject, Object *MySettingsWindowObject )
{
  struct MainWindowData *MyData ;
  struct IntuitionBase *IntuitionBase ;

  MyData = INST_DATA( MyIClass, MyObject ) ;
  IntuitionBase = MyData->mwd_Context->c_IntuitionBase ;
  
  SetAttrs( MyData->mwd_MenuItemSettingsVideohardware,
            MUIA_Menuitem_Enabled, FALSE,
            TAG_END ) ;
  DoMethod( MyData->mwd_MenuItemSettingsVideohardware, MUIM_KillNotify, MUIA_Menuitem_Trigger ) ;
  SetAttrs( MyData->mwd_MenuItemSettingsStamp,
            MUIA_Menuitem_Enabled, FALSE,
            TAG_END ) ;
  DoMethod( MyData->mwd_MenuItemSettingsStamp, MUIM_KillNotify, MUIA_Menuitem_Trigger ) ;
  SetAttrs( MyData->mwd_MenuItemSettingsDisplay,
            MUIA_Menuitem_Enabled, FALSE,
            TAG_END ) ;
  DoMethod( MyData->mwd_MenuItemSettingsDisplay, MUIM_KillNotify, MUIA_Menuitem_Trigger ) ;
  SetAttrs( MyData->mwd_MenuItemSettingsScheduler,
            MUIA_Menuitem_Enabled, FALSE,
            TAG_END ) ;
  DoMethod( MyData->mwd_MenuItemSettingsScheduler, MUIM_KillNotify, MUIA_Menuitem_Trigger ) ;
  SetAttrs( MyData->mwd_MenuItemSettingsCapture,
            MUIA_Menuitem_Enabled, FALSE,
            TAG_END ) ;
  DoMethod( MyData->mwd_MenuItemSettingsCapture, MUIM_KillNotify, MUIA_Menuitem_Trigger ) ;
  SetAttrs( MyData->mwd_MenuItemSettingsUpload,
            MUIA_Menuitem_Enabled, FALSE,
            TAG_END ) ;
  DoMethod( MyData->mwd_MenuItemSettingsUpload, MUIM_KillNotify, MUIA_Menuitem_Trigger ) ;
  SetAttrs( MyData->mwd_MenuItemSettingsLog,
            MUIA_Menuitem_Enabled, FALSE,
            TAG_END ) ;
  DoMethod( MyData->mwd_MenuItemSettingsLog, MUIM_KillNotify, MUIA_Menuitem_Trigger ) ;

  return( 0 ) ;
}


/*
** add display area to main window
*/
static IPTR MainWindowAddDisplayArea( struct IClass *MyIClass, Object *MyObject, Object *MyDisplayAreaObject )
{
  struct MainWindowData *MyData ;
  struct IntuitionBase *IntuitionBase ;
  IPTR Result ;
  
  MyData = INST_DATA( MyIClass, MyObject ) ;
  IntuitionBase = MyData->mwd_Context->c_IntuitionBase ;
  Result = FALSE ;
  
  if( NULL != MyDisplayAreaObject )
  {  /* requirements ok */
    if( DoMethod( MyData->mwd_RootObject, MUIM_Group_InitChange ) )
    {  /* add the display area */
      MyData->mwd_DisplayArea = MyDisplayAreaObject ;
      DoMethod( MyData->mwd_RootObject, OM_ADDMEMBER, MyData->mwd_DisplayArea ) ;
      DoMethod( MyData->mwd_RootObject, MUIM_Group_Sort, MyData->mwd_DisplayArea, MyData->mwd_ProgressBar, MyData->mwd_ReleaseGroup, NULL ) ;
      DoMethod( MyData->mwd_RootObject, MUIM_Group_ExitChange ) ;
      SetAttrs( MyDisplayAreaObject,
        MUIA_DisplayWindowObject, ( IPTR )MyObject, 
        TAG_END ) ;
        
      Result = TRUE ;
    }
    else
    {  /* can't modify root object */
    }
  }
  else
  {  /* requirements not ok */
  }

  return( Result ) ;
}


/*
** remove display area from main window
*/
static IPTR MainWindowRemoveDisplayArea( struct IClass *MyIClass, Object *MyObject, Object *MyDisplayAreaObject )
{
  struct MainWindowData *MyData ;
  struct IntuitionBase *IntuitionBase ;
  IPTR Result ;

  MyData = INST_DATA( MyIClass, MyObject ) ;
  IntuitionBase = MyData->mwd_Context->c_IntuitionBase ;
  Result = FALSE ;

  if( ( NULL != MyDisplayAreaObject ) && ( MyDisplayAreaObject == MyData->mwd_DisplayArea ) )
  {  /* requirements ok */
    if( DoMethod( MyData->mwd_RootObject, MUIM_Group_InitChange ) )
    {  /* add the display area */
      DoMethod( MyData->mwd_RootObject, OM_REMMEMBER, MyData->mwd_DisplayArea ) ;
      MyData->mwd_DisplayArea = NULL ;
      DoMethod( MyData->mwd_RootObject, MUIM_Group_ExitChange ) ;
      SetAttrs( MyDisplayAreaObject,
        MUIA_DisplayWindowObject, ( IPTR )NULL, 
        TAG_END ) ;

      Result = TRUE ;
    }
    else
    {  /* can't modify root object */
    }
  }
  else
  {  /* requirements not ok */
  }

  return( Result ) ;
}


/*
** add progress bar to main window
*/
static IPTR MainWindowAddProgressBar( struct IClass *MyIClass, Object *MyObject, Object *MyProgressBarObject )
{
  struct MainWindowData *MyData ;
  struct IntuitionBase *IntuitionBase ;
  IPTR Result ;
  
  MyData = INST_DATA( MyIClass, MyObject ) ;
  IntuitionBase = MyData->mwd_Context->c_IntuitionBase ;
  Result = FALSE ;
  
  if( NULL != MyProgressBarObject )
  {  /* requirements ok */
    if( DoMethod( MyData->mwd_RootObject, MUIM_Group_InitChange ) )
    {  /* add the progress bar */
      MyData->mwd_ProgressBar = MyProgressBarObject ;
      DoMethod( MyData->mwd_RootObject, OM_ADDMEMBER, MyData->mwd_ProgressBar ) ;
      DoMethod( MyData->mwd_RootObject, MUIM_Group_Sort, MyData->mwd_DisplayArea, MyData->mwd_ProgressBar, MyData->mwd_ReleaseGroup, NULL ) ;
      DoMethod( MyData->mwd_RootObject, MUIM_Group_ExitChange ) ;
        
      Result = TRUE ;
    }
    else
    {  /* can't modify root object */
    }
  }
  else
  {  /* requirements not ok */
  }

  return( Result ) ;
}


/*
** remove progress bar from main window
*/
static IPTR MainWindowRemoveProgressBar( struct IClass *MyIClass, Object *MyObject, Object *MyProgressBarObject )
{
  struct MainWindowData *MyData ;
  struct IntuitionBase *IntuitionBase ;
  IPTR Result ;

  MyData = INST_DATA( MyIClass, MyObject ) ;
  IntuitionBase = MyData->mwd_Context->c_IntuitionBase ;
  Result = FALSE ;

  if( ( NULL != MyProgressBarObject ) && ( MyProgressBarObject == MyData->mwd_ProgressBar ) )
  {  /* requirements ok */
    if( DoMethod( MyData->mwd_RootObject, MUIM_Group_InitChange ) )
    {  /* add the progress bar */
      DoMethod( MyData->mwd_RootObject, OM_REMMEMBER, MyData->mwd_ProgressBar ) ;
      MyData->mwd_ProgressBar = NULL ;
      DoMethod( MyData->mwd_RootObject, MUIM_Group_ExitChange ) ;

      Result = TRUE ;
    }
    else
    {  /* can't modify root object */
    }
  }
  else
  {  /* requirements not ok */
  }

  return( Result ) ;
}


/*
** add scroll borders to the main window
*/
static IPTR MainWindowAddScrollBorders( struct IClass *MyIClass, Object *MyObject, Msg MyMsg )
{
  struct MainWindowData *MyData ;
  struct IntuitionBase *IntuitionBase ;
  IPTR WindowOpen ;

  MyData = INST_DATA( MyIClass, MyObject ) ;
  IntuitionBase = MyData->mwd_Context->c_IntuitionBase ;

  GetAttr( MUIA_Window_Open, MyObject, &WindowOpen ) ;
  if( TRUE == WindowOpen )
  {  /* as we change window borders, close the window */
    SetAttrs( MyObject, 
      MUIA_Window_Open, FALSE,
      TAG_END ) ;
  }
  SetAttrs( MyObject,
    MUIA_Window_UseBottomBorderScroller, TRUE,
    MUIA_Window_UseRightBorderScroller, TRUE,
    TAG_END ) ;
  if( TRUE == WindowOpen )
  {  /* window was open, so reopen it again */
    SetAttrs( MyObject,
      MUIA_Window_Open, TRUE,
      TAG_END ) ;
  }

  return( 0 ) ;  
}


/*
** remove scroll borders to the main window
*/
static IPTR MainWindowRemoveScrollBorders( struct IClass *MyIClass, Object *MyObject, Msg MyMsg )
{
  struct MainWindowData *MyData ;
  struct IntuitionBase *IntuitionBase ;
  IPTR WindowOpen ;

  MyData = INST_DATA( MyIClass, MyObject ) ;
  IntuitionBase = MyData->mwd_Context->c_IntuitionBase ;
  
  GetAttr( MUIA_Window_Open, MyObject, &WindowOpen ) ;
  if( TRUE == WindowOpen )
  {  /* as we change window borders, close the window */
    SetAttrs( MyObject,
      MUIA_Window_Open, FALSE,
      TAG_END ) ;
  }
  SetAttrs( MyObject,
    MUIA_Window_UseBottomBorderScroller, FALSE,
    MUIA_Window_UseRightBorderScroller, FALSE,
    TAG_END ) ;
  if( TRUE == WindowOpen )
  {  /* window was open, so reopen it again */
    SetAttrs( MyObject, 
      MUIA_Window_Open, TRUE,
      TAG_END ) ;
  }

  return( 0 ) ;  
}


/*
** NEW method for the main window class
*/
static IPTR MainWindowNew( struct IClass *MyIClass, Object *MyObject, struct opSet *MyMsg )
{
  struct Context *MyContext ;
  struct Library *MUIMasterBase ;
  struct IntuitionBase *IntuitionBase ;
  struct MainWindowData *MyData ;
  struct TagItem AttrListExtension[ 3 ] ;
  APTR RootObject ;
  APTR MenuItemProjectQuit ;
  APTR MenuProject ;
  APTR MenuView ;
  APTR MenuSettings ;
  APTR MenuStrip ;
  IPTR Result ;

  MyContext = ( struct Context * )MyIClass->cl_UserData ;
  MUIMasterBase = MyContext->c_MUIMasterBase ;
  IntuitionBase = MyContext->c_IntuitionBase ;

  /* root object required initially */
  RootObject = MUI_NewObject( MUIC_Group,
    MUIA_Group_Horiz, FALSE,
    TAG_END ) ;
  /* menu strip required initially */
  MenuStrip = MUI_NewObject( MUIC_Menustrip,
    TAG_END ) ;

  /* extend the attributes list */
  AttrListExtension[ 0 ].ti_Tag = MUIA_Window_RootObject ;
  AttrListExtension[ 0 ].ti_Data = ( IPTR )RootObject ;
  AttrListExtension[ 1 ].ti_Tag = MUIA_Window_Menustrip ;
  AttrListExtension[ 1 ].ti_Data = ( IPTR )MenuStrip ;
  AttrListExtension[ 2 ].ti_Tag = TAG_MORE ;
  AttrListExtension[ 2 ].ti_Data = ( IPTR )MyMsg->ops_AttrList ;
  MyMsg->ops_AttrList = AttrListExtension ;
  /* call super method to create the window object */
  Result = DoSuperMethodA( MyIClass, MyObject, ( Msg )MyMsg ) ;
  if( 0 != Result )
  {  /* window object created */
    MyObject = ( Object * )Result ;
    MyData = INST_DATA( MyIClass, MyObject ) ;
    MyData->mwd_Context = MyContext ;
    MyData->mwd_RootObject = RootObject ;
      
    MyData->mwd_MenuItemProjectAbout = MUI_NewObject( MUIC_Menuitem,
      MUIA_Menuitem_Title, ( IPTR )( GetLocalizedString( MyContext->c_Localization, STR_MAINMENU_ABOUT ) ),
      MUIA_Menuitem_Shortcut, ( IPTR )( GetLocalizedString( MyContext->c_Localization, STR_MAINMENU_ABOUT_KEY ) ),
      MUIA_Menuitem_Enabled, FALSE,
      TAG_END ) ;
    MyData->mwd_MenuItemProjectAboutMUI = MUI_NewObject( MUIC_Menuitem,
      MUIA_Menuitem_Title, ( IPTR )( GetLocalizedString( MyContext->c_Localization, STR_MAINMENU_ABOUTMUI ) ),
      MUIA_Menuitem_Enabled, TRUE,
      TAG_END ) ;
    MenuItemProjectQuit = MUI_NewObject( MUIC_Menuitem,
      MUIA_Menuitem_Title, ( IPTR )( GetLocalizedString( MyContext->c_Localization, STR_MAINMENU_QUIT ) ),
      MUIA_Menuitem_Shortcut, ( IPTR )( GetLocalizedString( MyContext->c_Localization, STR_MAINMENU_QUIT_KEY ) ),
      MUIA_Menuitem_Enabled, TRUE,
      TAG_END ) ;
    MenuProject = MUI_NewObject( MUIC_Menu,
      MUIA_Menu_Title, ( IPTR )( GetLocalizedString( MyContext->c_Localization, STR_MAINMENU_PROJECT ) ),
      MUIA_Family_Child, ( IPTR )MyData->mwd_MenuItemProjectAbout,
      MUIA_Family_Child, ( IPTR )MyData->mwd_MenuItemProjectAboutMUI,
      MUIA_Family_Child, ( IPTR )MenuItemProjectQuit,
      TAG_END ) ;
    DoMethod( MenuStrip, OM_ADDMEMBER, MenuProject ) ;
    
    MyData->mwd_MenuItemViewLogWindow = MUI_NewObject( MUIC_Menuitem,
      MUIA_Menuitem_Title, ( IPTR )( GetLocalizedString( MyContext->c_Localization, STR_MAINMENU_LOGWINDOW ) ),
      MUIA_Menuitem_Shortcut, ( IPTR )( GetLocalizedString( MyContext->c_Localization, STR_MAINMENU_LOGWINDOW_KEY ) ),
      MUIA_Menuitem_Enabled, FALSE,
      TAG_END ) ;
    MenuView = MUI_NewObject( MUIC_Menu,
      MUIA_Menu_Title, ( IPTR )( GetLocalizedString( MyContext->c_Localization, STR_MAINMENU_VIEW ) ),
      MUIA_Family_Child, ( IPTR )MyData->mwd_MenuItemViewLogWindow,
      TAG_END ) ;
    DoMethod( MenuStrip, OM_ADDMEMBER, MenuView ) ;

    MyData->mwd_MenuItemSettingsVideohardware = MUI_NewObject( MUIC_Menuitem,
      MUIA_Menuitem_Title, ( IPTR )( GetLocalizedString( MyContext->c_Localization, STR_MAINMENU_SOURCE ) ),
      MUIA_Menuitem_Shortcut, ( IPTR )( GetLocalizedString( MyContext->c_Localization, STR_MAINMENU_SOURCE_KEY ) ),
      MUIA_Menuitem_Enabled, FALSE,
      TAG_END ) ;
    MyData->mwd_MenuItemSettingsStamp = MUI_NewObject( MUIC_Menuitem,
      MUIA_Menuitem_Title, ( IPTR )( GetLocalizedString( MyContext->c_Localization, STR_MAINMENU_STAMP ) ),
      MUIA_Menuitem_Shortcut, ( IPTR )( GetLocalizedString( MyContext->c_Localization, STR_MAINMENU_STAMP_KEY ) ),
      MUIA_Menuitem_Enabled, FALSE,
      TAG_END ) ;
    MyData->mwd_MenuItemSettingsDisplay = MUI_NewObject( MUIC_Menuitem,
      MUIA_Menuitem_Title, ( IPTR )( GetLocalizedString( MyContext->c_Localization, STR_MAINMENU_DISPLAY ) ),
      MUIA_Menuitem_Shortcut, ( IPTR )( GetLocalizedString( MyContext->c_Localization, STR_MAINMENU_DISPLAY_KEY ) ),
      MUIA_Menuitem_Enabled, FALSE,
      TAG_END ) ;
    MyData->mwd_MenuItemSettingsScheduler = MUI_NewObject( MUIC_Menuitem,
      MUIA_Menuitem_Title, ( IPTR )( GetLocalizedString( MyContext->c_Localization, STR_MAINMENU_SCHEDULER ) ),
      MUIA_Menuitem_Shortcut, ( IPTR )( GetLocalizedString( MyContext->c_Localization, STR_MAINMENU_SCHEDULER_KEY ) ),
      MUIA_Menuitem_Enabled, FALSE,
      TAG_END ) ;
    MyData->mwd_MenuItemSettingsCapture = MUI_NewObject( MUIC_Menuitem,
      MUIA_Menuitem_Title, ( IPTR )( GetLocalizedString( MyContext->c_Localization, STR_MAINMENU_CAPTURE ) ),
      MUIA_Menuitem_Shortcut, ( IPTR )( GetLocalizedString( MyContext->c_Localization, STR_MAINMENU_CAPTURE_KEY ) ),
      MUIA_Menuitem_Enabled, FALSE,
      TAG_END ) ;
    MyData->mwd_MenuItemSettingsUpload = MUI_NewObject( MUIC_Menuitem,
      MUIA_Menuitem_Title, ( IPTR )( GetLocalizedString( MyContext->c_Localization, STR_MAINMENU_UPLOAD ) ),
      MUIA_Menuitem_Shortcut, ( IPTR )( GetLocalizedString( MyContext->c_Localization, STR_MAINMENU_UPLOAD_KEY ) ),
      MUIA_Menuitem_Enabled, FALSE,
      TAG_END ) ;
    MyData->mwd_MenuItemSettingsLog = MUI_NewObject( MUIC_Menuitem,
      MUIA_Menuitem_Title, ( IPTR )( GetLocalizedString( MyContext->c_Localization, STR_MAINMENU_LOG ) ),
      MUIA_Menuitem_Shortcut, ( IPTR )( GetLocalizedString( MyContext->c_Localization, STR_MAINMENU_LOG_KEY ) ),
      MUIA_Menuitem_Enabled, FALSE,
      TAG_END ) ;
    MyData->mwd_MenuItemSettingsMUI = MUI_NewObject( MUIC_Menuitem,
      MUIA_Menuitem_Title, ( IPTR )( GetLocalizedString( MyContext->c_Localization, STR_MAINMENU_MUI ) ),
      MUIA_Menuitem_Enabled, TRUE,
      TAG_END ) ;
    MenuSettings = MUI_NewObject( MUIC_Menu,
      MUIA_Menu_Title, ( IPTR )GetLocalizedString( MyContext->c_Localization, STR_MAINMENU_SETTINGS ),
      MUIA_Family_Child, ( IPTR )MyData->mwd_MenuItemSettingsVideohardware,
      MUIA_Family_Child, ( IPTR )MyData->mwd_MenuItemSettingsStamp,
      MUIA_Family_Child, ( IPTR )MyData->mwd_MenuItemSettingsDisplay,
      MUIA_Family_Child, ( IPTR )MyData->mwd_MenuItemSettingsScheduler,
      MUIA_Family_Child, ( IPTR )MyData->mwd_MenuItemSettingsCapture,
      MUIA_Family_Child, ( IPTR )MyData->mwd_MenuItemSettingsUpload,
      MUIA_Family_Child, ( IPTR )MyData->mwd_MenuItemSettingsLog,
      MUIA_Family_Child, ( IPTR )MyData->mwd_MenuItemSettingsMUI,
      TAG_END ) ;
    DoMethod( MenuStrip, OM_ADDMEMBER, MenuSettings ) ;

    MyData->mwd_ReleaseButton = MagicButton( MyContext->c_Magic,
      GetLocalizedString( MyContext->c_Localization, STR_MAINWINDOW_RELEASE ),
      *GetLocalizedString( MyContext->c_Localization, STR_MAINWINDOW_RELEASE_KEY ),
      GetLocalizedString( MyContext->c_Localization, STR_MAINWINDOW_RELEASE_HELP ) ) ;
    MyData->mwd_ReleaseGroup = MUI_NewObject( MUIC_Group,
      MUIA_Group_Columns, 1,
      MUIA_Group_Child, ( IPTR )( MyData->mwd_ReleaseButton ),
      TAG_END ) ;
    DoMethod( RootObject, OM_ADDMEMBER, MyData->mwd_ReleaseGroup ) ;
    
    if( ( NULL != MyData->mwd_ReleaseGroup ) )
    {  /* window is complete */
      //DoMethod( MyContext->c_Magic->mc_Application, OM_ADDMEMBER, MyObject ) ;

      DoMethod( MyObject, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
                MyObject, 3, MUIM_WriteLong, 1, &MyContext->c_ShutdownRequest  ) ;
      DoMethod( MyData->mwd_MenuItemProjectAboutMUI, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
                MUIV_Notify_Application /* MyContext->c_Magic->mc_Application */, 2, MUIM_Application_AboutMUI, MyObject ) ;          
      DoMethod( MenuItemProjectQuit, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
                MenuItemProjectQuit, 3, MUIM_WriteLong, 1, &MyContext->c_ShutdownRequest ) ;
      DoMethod( MyData->mwd_MenuItemSettingsMUI, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
                MUIV_Notify_Application /* MyContext->c_Magic->mc_Application */, 2, MUIM_Application_OpenConfigWindow, 0 ) ;
      DoMethod( MyData->mwd_ReleaseButton, MUIM_Notify, MUIA_Pressed, FALSE,
                MenuItemProjectQuit, 3, MUIM_WriteLong, 1, &MyContext->c_ReleaseRequest ) ;
      //printf( "main window object ok\n" ) ;
    }
    else
    {  /* window is not complete */
      CoerceMethod( MyIClass, MyObject, OM_DISPOSE ) ;
      Result = 0 ;
    }
  }
  else
  {  /* window object could not be created */
  }
  
  return( Result ) ; 
}


/*
** dispose main window
*/
static IPTR MainWindowDispose( struct IClass *MyIClass, Object *MyObject, Msg MyMsg )
{
  struct MainWindowData *MyData ;
  IPTR Result ;

  MyData = INST_DATA( MyIClass, MyObject ) ;
      
  DoMethod( MyData->mwd_MenuItemSettingsMUI, MUIM_KillNotify, MUIA_Menuitem_Trigger ) ;
  DoMethod( MyData->mwd_MenuItemProjectAboutMUI, MUIM_KillNotify, MUIA_Menuitem_Trigger ) ;
  
  Result = DoSuperMethodA( MyIClass, MyObject, MyMsg ) ;
  
  return( Result ) ;
}


/*
** dispatcher for our custom application class
*/
IPTR MainWindowDispatcher( REG( a0, struct IClass *MyIClass ), REG( a2, Object *MyObject ), REG( a1, Msg MyMsg ) )
{
  IPTR Result ;
  
  Result = 0 ;
  
  switch( MyMsg->MethodID )
  {
    case OM_NEW:
      //printf( "MainWindowNew\n" ) ;
      Result = MainWindowNew( MyIClass, MyObject, ( struct opSet * )MyMsg ) ;
      break ;
    case OM_DISPOSE:
      //printf( "MainWindowDispose\n" ) ;
      Result = MainWindowDispose( MyIClass, MyObject, MyMsg ) ;
      break ;
    case MUIM_AddScrollBorders:
      //printf( "MainWindowAddScrollBorders\n" ) ;
      Result = MainWindowAddScrollBorders( MyIClass, MyObject, MyMsg ) ;
      break ;
    case MUIM_RemoveScrollBorders:
      //printf( "MainWindowRemoveScrollBorders\n" ) ;
      Result = MainWindowRemoveScrollBorders( MyIClass, MyObject, MyMsg ) ;
      break ;
    case MUIM_AddDisplayArea:
      //printf( "MainWindowAddDisplayArea\n" ) ;
      Result = MainWindowAddDisplayArea( MyIClass, MyObject, ( Object * )( ( struct ParamMsg * )MyMsg )->MsgParam ) ;
      break ;
    case MUIM_RemoveDisplayArea:
      //printf( "MainWindowRemoveDisplayArea\n" ) ;
      Result = MainWindowRemoveDisplayArea( MyIClass, MyObject, ( Object * )( ( struct ParamMsg * )MyMsg )->MsgParam ) ;
      break ;
    case MUIM_AddProgressBar:
      //printf( "MainWindowAddProgressBar\n" ) ;
      Result = MainWindowAddProgressBar( MyIClass, MyObject, ( Object * )( ( struct ParamMsg * )MyMsg )->MsgParam ) ;
      break ;
    case MUIM_RemoveProgressBar:
      //printf( "MainWindowRemoveProgressBar\n" ) ;
      Result = MainWindowRemoveProgressBar( MyIClass, MyObject, ( Object * )( ( struct ParamMsg * )MyMsg )->MsgParam ) ;
      break ;
    case MUIM_AddSettingsWindow:
      //printf( "MainWindowAddSettingsWindow\n" ) ;
      Result = MainWindowAddSettingsWindow( MyIClass, MyObject, ( Object * )( ( struct ParamMsg * )MyMsg )->MsgParam ) ;
      break ;
    case MUIM_RemoveSettingsWindow:
      //printf( "MainWindowRemoveSettingsWindow\n" ) ;
      Result = MainWindowRemoveSettingsWindow( MyIClass, MyObject, ( Object * )( ( struct ParamMsg * )MyMsg )->MsgParam ) ;
      break ;
    case MUIM_AddAboutWindow:
      //printf( "MainWindowAddAboutWindow\n" ) ;
      Result = MainWindowAddAboutWindow( MyIClass, MyObject, ( Object * )( ( struct ParamMsg * )MyMsg )->MsgParam ) ;
      break ;
    case MUIM_RemoveAboutWindow:
      //printf( "MainWindowRemoveAboutWindow\n" ) ;
      Result = MainWindowRemoveAboutWindow( MyIClass, MyObject, ( Object * )( ( struct ParamMsg * )MyMsg )->MsgParam ) ;
      break ;
    case MUIM_AddLogWindow:
      //printf( "MainWindowAddLogWindow\n" ) ;
      Result = MainWindowAddLogWindow( MyIClass, MyObject, ( Object * )( ( struct ParamMsg * )MyMsg )->MsgParam ) ;
      break ;
    case MUIM_RemoveLogWindow:
      //printf( "MainWindowRemoveLogWindow\n" ) ;
      Result = MainWindowRemoveLogWindow( MyIClass, MyObject, ( Object * )( ( struct ParamMsg * )MyMsg )->MsgParam ) ;
      break ;
    default:
      Result = DoSuperMethodA( MyIClass, MyObject, MyMsg ) ;
      break ;
  }

  return( Result ) ;
}
