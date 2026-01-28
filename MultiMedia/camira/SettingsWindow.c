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
** SettingsWindow.c
*/


#include "SettingsWindow.h"
#include "MainWindow.h"
#include "Localization.h"
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <libraries/iffparse.h>
#include <libraries/asl.h>
#include <clib/alib_protos.h>
#include <stdio.h>


/*
** create a settings window
*/
struct SettingsWindow *CreateSettingsWindow( struct Context *MyContext )
{
  struct ExecBase *SysBase ;
  struct Library *MUIMasterBase ;
  struct IntuitionBase *IntuitionBase ;
  struct SettingsWindow *NewSettingsWindow ;

  NewSettingsWindow = NULL ;
  
  if( ( NULL != MyContext->c_MainWindow ) && ( MyContext->c_Magic->m_SettingsWindowClass ) )
  {  /* requirements seem ok */
    SysBase = MyContext->c_SysBase ;
    IntuitionBase = MyContext->c_IntuitionBase ;
    MUIMasterBase = MyContext->c_MUIMasterBase ;
    NewSettingsWindow = AllocVec( sizeof( struct SettingsWindow ), MEMF_ANY | MEMF_CLEAR ) ;
    if( NULL != NewSettingsWindow )
    {  /* memory for settings window context ok */
      NewSettingsWindow->sw_Context = MyContext ;
      NewSettingsWindow->sw_WindowObject = NewObject( MyContext->c_Magic->m_SettingsWindowClass->mcc_Class, NULL,
        MUIA_Window_ID, ( IPTR )MAKE_ID( 'S', 'E', 'T', 'T' ),
        MUIA_Window_Title, ( IPTR )( GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_TITLE ) ),
        MUIA_Window_Activate, TRUE, 
        MUIA_Window_SizeGadget, TRUE,
        MUIA_Window_SizeRight, FALSE,
        MUIA_Window_DepthGadget, TRUE,
        MUIA_Window_CloseGadget, TRUE,
        MUIA_Window_DragBar, TRUE,
        TAG_END ) ;
      if( NULL != NewSettingsWindow->sw_WindowObject )
      {  /* settings window object ok */
        DoMethod( MyContext->c_Magic->m_Application, OM_ADDMEMBER, NewSettingsWindow->sw_WindowObject ) ;
        DoMethod( MyContext->c_MainWindow->mw_WindowObject, MUIM_AddSettingsWindow, NewSettingsWindow->sw_WindowObject ) ;
      }
      else
      {  /* settings window object not ok */
        LogText( MyContext->c_Log, ERROR_LEVEL, "settings window object not ok" ) ;
        DeleteSettingsWindow( NewSettingsWindow ) ;
        NewSettingsWindow = NULL ;
      }
    }
    else
    {  /* memory for settings window context not ok */
      LogText( MyContext->c_Log, ERROR_LEVEL, "memory for settings window context not ok" ) ;
    }
  }
  else
  {  /* requirements not ok */
  }

  return( NewSettingsWindow ) ;
}


/*
** delete a settings window
*/
void DeleteSettingsWindow( struct SettingsWindow *OldSettingsWindow )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct IntuitionBase *IntuitionBase ;
  struct Library *MUIMasterBase ;

  if( NULL != OldSettingsWindow )
  {  /* settings window context needs to be freed */
    MyContext = OldSettingsWindow->sw_Context ;
    SysBase = MyContext->c_SysBase ;
    IntuitionBase = MyContext->c_IntuitionBase ;
    MUIMasterBase = MyContext->c_MUIMasterBase ;
    if( NULL != OldSettingsWindow->sw_WindowObject )
    {  /* window object needs to be disposed */
      SetAttrs( OldSettingsWindow->sw_WindowObject, MUIA_Window_Open, FALSE, TAG_END ) ;
      DoMethod( MyContext->c_MainWindow->mw_WindowObject, MUIM_RemoveSettingsWindow, OldSettingsWindow->sw_WindowObject ) ;
      DoMethod( MyContext->c_Magic->m_Application, OM_REMMEMBER, OldSettingsWindow->sw_WindowObject ) ;
      MUI_DisposeObject( OldSettingsWindow->sw_WindowObject ) ;
    }
    FreeVec( OldSettingsWindow ) ;
  }
}
