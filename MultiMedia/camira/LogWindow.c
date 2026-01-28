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
** LogWindow.c
*/


#include "LogWindow.h"
#include "MainWindow.h"
#include "Localization.h"
#include "Log.h"
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <libraries/iffparse.h>
#include <clib/alib_protos.h>


/*
** delete a log window
*/
void DeleteLogWindow( struct LogWindow *OldLogWindow )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  //struct IntuitionBase *IntuitionBase ;
  struct Library *MUIMasterBase ;
 
  if( NULL != OldLogWindow )
  {  /* log window context needs to be freed */
    MyContext = OldLogWindow->lw_Context ;
    SysBase = MyContext->c_SysBase ;
    //IntuitionBase = MyContext->c_IntuitionBase ;
    MUIMasterBase = MyContext->c_MUIMasterBase ;
    
    if( NULL != OldLogWindow->lw_WindowObject )
    {  /* window object needs to be disposed */
      DisableLogGui( MyContext->c_Log ) ;
      SetAttrs( OldLogWindow->lw_WindowObject, MUIA_Window_Open, FALSE, TAG_END ) ;
      DoMethod( MyContext->c_MainWindow->mw_WindowObject, MUIM_RemoveLogWindow, OldLogWindow->lw_WindowObject ) ;
      DoMethod( MyContext->c_Magic->m_Application, OM_REMMEMBER, OldLogWindow->lw_WindowObject ) ;
      MUI_DisposeObject( OldLogWindow->lw_WindowObject ) ;
    }
    FreeVec( OldLogWindow ) ;
  }
}


/*
** create a log window
*/
struct LogWindow *CreateLogWindow( struct Context *MyContext )
{
  struct ExecBase *SysBase ;
  struct Library *MUIMasterBase ;
  struct LogWindow *NewLogWindow ;

  NewLogWindow = NULL ;
  
  if( ( NULL != MyContext->c_MainWindow ) )
  {  /* requirements seem ok */
    SysBase = MyContext->c_SysBase ;
    MUIMasterBase = MyContext->c_MUIMasterBase ;
    
    NewLogWindow = AllocVec( sizeof( struct LogWindow ), MEMF_ANY | MEMF_CLEAR ) ;
    if( NULL != NewLogWindow )
    {  /* log window context ok */
      NewLogWindow->lw_Context = MyContext ;
      NewLogWindow->lw_WindowObject = NewObject( MyContext->c_Magic->m_LogWindowClass->mcc_Class, NULL,
        MUIA_Window_ID, ( IPTR )MAKE_ID( 'L', 'O', 'G', ' ' ),
        MUIA_Window_Title, ( IPTR )GetLocalizedString( MyContext->c_Localization, STR_LOGWINDOW_TITLE ),
        MUIA_Window_Activate, TRUE, 
        MUIA_Window_SizeGadget, TRUE,
        MUIA_Window_SizeRight, FALSE,
        MUIA_Window_DepthGadget, TRUE,
        MUIA_Window_CloseGadget, TRUE,
        MUIA_Window_DragBar, TRUE,
        TAG_END ) ;
      if( NULL != NewLogWindow->lw_WindowObject )
      {  /* window object ok */
        DoMethod( MyContext->c_Magic->m_Application, OM_ADDMEMBER, NewLogWindow->lw_WindowObject ) ;
        DoMethod( MyContext->c_MainWindow->mw_WindowObject, MUIM_AddLogWindow, NewLogWindow->lw_WindowObject ) ;
        EnableLogGui( MyContext->c_Log, NewLogWindow->lw_WindowObject ) ;
      }
      else
      {  /* window object not ok */
        LogText( MyContext->c_Log, ERROR_LEVEL, "log window object not ok" ) ;
        DeleteLogWindow( NewLogWindow ) ;
        DisableLogGui( MyContext->c_Log ) ;
        NewLogWindow = NULL ;
      }
    }
    else
    {  /* log window context not ok */
      LogText( MyContext->c_Log, ERROR_LEVEL, "log window context not ok" ) ;
      DisableLogGui( MyContext->c_Log ) ;
    }
  }
  else
  {  /* requirements not ok */
    DisableLogGui( MyContext->c_Log ) ;
  }

  return( NewLogWindow ) ;
}
