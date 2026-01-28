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
** AboutWindow.c
*/


#include "AboutWindow.h"
#include "Magic.h"
#include "MainWindow.h"
#include "Localization.h"
#include "Log.h"
#include "Version.h"
#include "ab_stdio.h"
#include <proto/exec.h>
#include <proto/muimaster.h>
#include <libraries/iffparse.h>
#include <clib/alib_protos.h>


/*
** delete a about window
*/
void DeleteAboutWindow( struct AboutWindow *OldAboutWindow )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  //struct IntuitionBase *IntuitionBase ;
  struct Library *MUIMasterBase ;
 
  if( NULL != OldAboutWindow )
  {  /* about window context needs to be freed */
    MyContext = OldAboutWindow->aw_Context ;
    SysBase = MyContext->c_SysBase ;
    MUIMasterBase = MyContext->c_MUIMasterBase ;
    
    if( NULL != OldAboutWindow->aw_WindowObject )
    {  /* window object needs to be disposed */
      SetAttrs( OldAboutWindow->aw_WindowObject, MUIA_Window_Open, FALSE, TAG_END ) ;
      DoMethod( MyContext->c_MainWindow->mw_WindowObject, MUIM_RemoveAboutWindow, OldAboutWindow->aw_WindowObject ) ;
      DoMethod( MyContext->c_Magic->m_Application, OM_REMMEMBER, OldAboutWindow->aw_WindowObject ) ;
      MUI_DisposeObject( OldAboutWindow->aw_WindowObject ) ;
    }
    FreeVec( OldAboutWindow ) ;
  }
}


/*
** create a about window
*/
struct AboutWindow *CreateAboutWindow( struct Context *MyContext )
{
  struct ExecBase *SysBase ;
  struct Library *MUIMasterBase ;
  struct AboutWindow *NewAboutWindow ;
  APTR RootObject, OkButton, AboutText ;

  NewAboutWindow = NULL ;
  
  if( ( NULL != MyContext->c_MainWindow ) )
  {  /* requirements seem ok */
    SysBase = MyContext->c_SysBase ;
    MUIMasterBase = MyContext->c_MUIMasterBase ;
    
    NewAboutWindow = AllocVec( sizeof( struct AboutWindow ), MEMF_ANY | MEMF_CLEAR ) ;
    if( NULL != NewAboutWindow )
    {  /* memory for about window context ok */
      NewAboutWindow->aw_Context = MyContext ;
      
      snprintf( NewAboutWindow->aw_AboutText, AW_ABOUTTEXT_LENGTH, "%s\n%s\n\n%s\n\n%s",
        GetVersionString( ),
        GetLocalizedString( MyContext->c_Localization, STR_CAMIRA_DESCRIPTION ),
        GetCopyrightString( ),
        GetLocalizedString( MyContext->c_Localization, STR_CAMIRA_TRANSLATION_BY ) ) ;
      AboutText = MUI_NewObject( MUIC_Text,
        MUIA_Text_Contents, ( IPTR )NewAboutWindow->aw_AboutText,
        MUIA_Text_PreParse, ( IPTR )"\33c",
        MUIA_Text_SetMax, TRUE,
        MUIA_Frame, MUIV_Frame_Text,
        MUIA_Background, MUII_WindowBack,
        MUIA_Font, ( IPTR ) MUIV_Font_Normal,
        MUIA_Text_SetMin, TRUE,
        MUIA_CycleChain, FALSE,
        TAG_END ) ;
      OkButton = MagicButton( MyContext->c_Magic, 
        GetLocalizedString( MyContext->c_Localization, STR_ABOUTWINDOW_OK ),
        *GetLocalizedString( MyContext->c_Localization, STR_ABOUTWINDOW_OK_KEY ),
        GetLocalizedString( MyContext->c_Localization, STR_ABOUTWINDOW_OK_HELP ) ) ;
      RootObject = MUI_NewObject( MUIC_Group,
        MUIA_Group_Horiz, FALSE,
        MUIA_Background, MUII_RequesterBack,
        MUIA_Group_Child, ( IPTR )AboutText,
        MUIA_Group_Child, ( IPTR )MUI_NewObject( MUIC_Group,
          MUIA_Group_Horiz, TRUE,
          MUIA_Group_Child, ( IPTR )( MagicSpace( MyContext->c_Magic ) ),
          MUIA_Group_Child, ( IPTR )( OkButton ),
          MUIA_Group_Child, ( IPTR )( MagicSpace( MyContext->c_Magic ) ),
          TAG_END ),
        TAG_END ) ;
      NewAboutWindow->aw_WindowObject = MUI_NewObject( MUIC_Window,
        MUIA_Window_ID, ( IPTR )MAKE_ID( 'A', 'B', 'O', 'U' ),
        MUIA_Window_Title, ( IPTR )GetLocalizedString( MyContext->c_Localization, STR_ABOUTWINDOW_TITLE ),
        MUIA_Window_Activate, TRUE, 
        MUIA_Window_SizeGadget, FALSE,
        MUIA_Window_SizeRight, FALSE,
        MUIA_Window_DepthGadget, TRUE,
        MUIA_Window_CloseGadget, TRUE,
        MUIA_Window_DragBar, TRUE,
        MUIA_Window_RootObject, ( IPTR )RootObject,
        TAG_END ) ;
      if( NULL != NewAboutWindow->aw_WindowObject )
      {  /* window object ok */
        DoMethod( MyContext->c_Magic->m_Application, OM_ADDMEMBER, NewAboutWindow->aw_WindowObject ) ;
        DoMethod( MyContext->c_MainWindow->mw_WindowObject, MUIM_AddAboutWindow, NewAboutWindow->aw_WindowObject ) ;

        DoMethod( NewAboutWindow->aw_WindowObject, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, 
                  NewAboutWindow->aw_WindowObject, 3, MUIM_Set, MUIA_Window_Open, FALSE ) ;
        DoMethod( OkButton, MUIM_Notify, MUIA_Pressed, FALSE, 
                  NewAboutWindow->aw_WindowObject, 3, MUIM_Set, MUIA_Window_Open, FALSE ) ;
      }
      else
      {  /* window object not ok */
        LogText( MyContext->c_Log, WARNING_LEVEL, "about window object not ok" ) ;
      }
    }
    else
    {  /* memory for about window context not ok */
      LogText( MyContext->c_Log, WARNING_LEVEL, "about window not ok" ) ;
    }
  }
  else
  {  /* requirements not ok */
  }

  return( NewAboutWindow ) ;
}
