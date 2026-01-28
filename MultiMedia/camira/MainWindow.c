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
** MainWindow.c
*/


#include "MainWindow.h"
#include "Magic.h"
#include "Settings.h"
#include "Localization.h"
#include "Log.h"
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <libraries/iffparse.h>
#include <clib/alib_protos.h>


/*
** open the main window
*/
void OpenMainWindow( struct MainWindow *MyMainWindow )
{
  struct Context *MyContext ;
  struct IntuitionBase *IntuitionBase ;

  if( NULL != MyMainWindow )
  {  /* requirements ok */
    MyContext = MyMainWindow->mw_Context ;
    IntuitionBase = MyContext->c_IntuitionBase ;
   
    SetAttrs( MyMainWindow->mw_WindowObject,
      MUIA_Window_Open, TRUE,
      TAG_END ) ;
  }
  else
  {  /* requirements not ok */
  }
}


/*
** close a main window
*/
void CloseMainWindow( struct MainWindow *MyMainWindow )
{
  struct Context *MyContext ;
  struct IntuitionBase *IntuitionBase ;

  if( NULL != MyMainWindow )
  {  /* requirements ok */
    MyContext = MyMainWindow->mw_Context ;
    IntuitionBase = MyContext->c_IntuitionBase ;
   
    SetAttrs( MyMainWindow->mw_WindowObject,
      MUIA_Window_Open, FALSE,
      TAG_END ) ;
  }
  else
  {  /* requirements not ok */
  }
}


/*
** create a main window
*/
struct MainWindow *CreateMainWindow( struct Context *MyContext )
{
  struct ExecBase *SysBase ;
  struct Library *MUIMasterBase ;
  struct IntuitionBase *IntuitionBase ;
  struct MainWindow *NewMainWindow ;
  LONG CurrentNogui ;

  NewMainWindow = NULL ;
  
  if( ( NULL != MyContext->c_Magic ) && ( NULL != MyContext->c_Magic->m_Application ) && ( NULL != MyContext->c_Magic->m_MainWindowClass ) )
  {  /* requirements seem ok */
    SysBase = MyContext->c_SysBase ;
    IntuitionBase = MyContext->c_IntuitionBase ;
    MUIMasterBase = MyContext->c_MUIMasterBase ;
    
    CurrentNogui = GetSetting( MyContext->c_Settings, Nogui ) ;
    if( !( CurrentNogui ) )
    {  /* gui allowed */
      NewMainWindow = AllocVec( sizeof( struct MainWindow ), MEMF_ANY | MEMF_CLEAR ) ;
      if( NULL != NewMainWindow )
      {  /* main window context ok */
        NewMainWindow->mw_Context = MyContext ;
        NewMainWindow->mw_WindowObject = NewObject( MyContext->c_Magic->m_MainWindowClass->mcc_Class, NULL,
          MUIA_Window_ID, ( IPTR )MAKE_ID( 'M', 'A', 'I', 'N' ),
          MUIA_Window_Title, ( IPTR )( GetLocalizedString( MyContext->c_Localization, STR_MAINWINDOW_TITLE ) ),
          MUIA_Window_Activate, TRUE, 
          MUIA_Window_SizeGadget, TRUE,
          MUIA_Window_SizeRight, FALSE,
          MUIA_Window_DepthGadget, TRUE,
          MUIA_Window_CloseGadget, TRUE,
          MUIA_Window_DragBar, TRUE,
          TAG_END ) ;
        if( NULL != NewMainWindow->mw_WindowObject )
        {  /* window object ok */
          DoMethod( MyContext->c_Magic->m_Application, OM_ADDMEMBER, NewMainWindow->mw_WindowObject ) ;
        }
        else
        {  /* main window object not ok */
          LogText( MyContext->c_Log, ERROR_LEVEL, "main window object not ok" ) ;
          DeleteMainWindow( NewMainWindow ) ;
          NewMainWindow = NULL ;
        }
      }
      else
      {  /* main window context not ok */
        LogText( MyContext->c_Log, ERROR_LEVEL, "main window context not ok" ) ;
      }
    }
    else
    {  /* gui not allowed */
    }
  }
  else
  {  /* requirements not ok */
  }

  return( NewMainWindow ) ;
}


/*
** delete a main window
*/
void DeleteMainWindow( struct MainWindow *OldMainWindow )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct IntuitionBase *IntuitionBase ;
  struct Library *MUIMasterBase ;
 
  if( NULL != OldMainWindow )
  {  /* main window context needs to be freed */
    MyContext = OldMainWindow->mw_Context ;
    SysBase = MyContext->c_SysBase ;
    IntuitionBase = MyContext->c_IntuitionBase ;
    MUIMasterBase = MyContext->c_MUIMasterBase ;
    if( NULL != OldMainWindow->mw_WindowObject )
    {  /* window object needs to be disposed */
      DoMethod( OldMainWindow->mw_WindowObject, MUIM_Window_Snapshot, 1 ) ;
      DoMethod( MyContext->c_Magic->m_Application, OM_REMMEMBER, OldMainWindow->mw_WindowObject ) ;
      MUI_DisposeObject( OldMainWindow->mw_WindowObject ) ;
    }
    FreeVec( OldMainWindow ) ;
    //Log( MyContext->c_Log, DEBUG_LEVEL, "main window context freed" ) ;
  }
}
