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
** Magic.c
*/


#include "Magic.h"
#include "MainWindow.h"
#include "SettingsWindow.h"
#include "LogWindow.h"
#include "Version.h"
#include "Localization.h"
#include "ab_stdio.h"
#include <proto/exec.h>
#include <proto/icon.h>
#include <proto/muimaster.h>
#include <intuition/classusr.h>
#include <clib/alib_protos.h>


/*
** create all custom mui classes
*/
static void CreateCustomClasses( struct Magic *MyMagic )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct Library *MUIMasterBase ;

  MyContext = MyMagic->m_Context ;
  SysBase = MyContext->c_SysBase ;
  MUIMasterBase = MyContext->c_MUIMasterBase ;

  MyMagic->m_MainWindowClass = MUI_CreateCustomClass( NULL, MUIC_Window, NULL, sizeof( struct MainWindowData ), MainWindowDispatcher ) ;
  if( NULL != MyMagic->m_MainWindowClass )
  {  /* custom main window class ok */
    MyMagic->m_MainWindowClass->mcc_Class->cl_UserData = ( IPTR )MyContext ;
  }

  MyMagic->m_DisplayAreaClass = MUI_CreateCustomClass( NULL, MUIC_Group, NULL, sizeof( struct DisplayAreaData ), DisplayAreaDispatcher ) ;
  if( NULL != MyMagic->m_DisplayAreaClass )
  {  /* custom display area class ok */
    MyMagic->m_DisplayAreaClass->mcc_Class->cl_UserData = ( IPTR )MyContext ;
  }

  MyMagic->m_SettingsWindowClass = MUI_CreateCustomClass( NULL, MUIC_Window, NULL, sizeof( struct SettingsWindowData ), SettingsWindowDispatcher ) ;
  if( NULL != MyMagic->m_SettingsWindowClass )
  {  /* custom settings window class ok */
    MyMagic->m_SettingsWindowClass->mcc_Class->cl_UserData = ( IPTR )MyContext ;
  }

  MyMagic->m_LogWindowClass = MUI_CreateCustomClass( NULL, MUIC_Window, NULL, sizeof( struct LogWindowData ), LogWindowDispatcher ) ;
  if( NULL != MyMagic->m_LogWindowClass )
  {  /* custom log window class ok */
    MyMagic->m_LogWindowClass->mcc_Class->cl_UserData = ( IPTR )MyContext ;
  }
}


/*
** delete all custom mui classes
*/
static void DeleteCustomClasses( struct Magic *MyMagic )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct Library *MUIMasterBase ;

  MyContext = MyMagic->m_Context ;
  SysBase = MyContext->c_SysBase ;
  MUIMasterBase = MyContext->c_MUIMasterBase ;

  if( NULL != MyMagic->m_LogWindowClass )
  {
    MUI_DeleteCustomClass( MyMagic->m_LogWindowClass ) ;
    MyMagic->m_LogWindowClass = NULL ;
  }
  
  if( NULL != MyMagic->m_SettingsWindowClass )
  {
    MUI_DeleteCustomClass( MyMagic->m_SettingsWindowClass ) ;
    MyMagic->m_SettingsWindowClass = NULL ;
  }
  
  if( NULL != MyMagic->m_DisplayAreaClass )
  {
    MUI_DeleteCustomClass( MyMagic->m_DisplayAreaClass ) ;
    MyMagic->m_DisplayAreaClass = NULL ;
  }
  
  if( NULL != MyMagic->m_MainWindowClass )
  {
    MUI_DeleteCustomClass( MyMagic->m_MainWindowClass ) ;
    MyMagic->m_MainWindowClass = NULL ;
  }
}


/*
** processed magic events
*/
static IPTR DoMagic( struct Magic *MyMagic, ULONG TriggerSignals )
{
  if( NULL != MyMagic )
  {  /* there seems to be a magic context */
    while( MUIV_Application_ReturnID_Quit != DoMethod( MyMagic->m_Application, MUIM_Application_NewInput, &TriggerSignals ) )
    {  /* as long as application does not quit */
      if( 0 != TriggerSignals )
      {  /* now mui needs to wait for more events, so exit */
        break ;
      }
    }
  }
  else
  {  /* there is no magic context */
  }

  return( TriggerSignals ) ;
}


/*
** create all mui application related stuff
*/
struct Magic *CreateMagic( struct Context *MyContext )
{
  struct ExecBase *SysBase ;
  struct Library *MUIMasterBase ;
  struct IntuitionBase *IntuitionBase ;
  struct Magic *NewMagic ;

  NewMagic = NULL ;
  
  if( ( MyContext->c_MUIMasterBase ) && ( MyContext->c_IntuitionBase ) )
  {  /* requirements seem ok */
    SysBase = MyContext->c_SysBase ;
    IntuitionBase = MyContext->c_IntuitionBase ;
    MUIMasterBase = MyContext->c_MUIMasterBase ;
    NewMagic = AllocVec( sizeof( struct Magic ), MEMF_ANY | MEMF_CLEAR ) ;
    if( NULL != NewMagic )
    {  /* memory for magic context ok */
      NewMagic->m_Context = MyContext ;
      CreateCustomClasses( NewMagic ) ;
      NewMagic->m_Application = MUI_NewObject( MUIC_Application,
        MUIA_Application_Title, ( IPTR )"cAMIra",
        MUIA_Application_Version, ( IPTR )GetVersionTagString( ),
        MUIA_Application_Copyright, ( IPTR )GetCopyrightString( ),
        MUIA_Application_Author, ( IPTR )GetAuthorString( ),
        MUIA_Application_Description, ( IPTR )GetLocalizedString( MyContext->c_Localization, STR_CAMIRA_DESCRIPTION ),
        MUIA_Application_Base, ( IPTR )"CAMIRA",
        MUIA_Application_DiskObject, ( IPTR )MyContext->c_DiskObject,
        MUIA_Application_SingleTask, FALSE,
        TAG_END ) ;
      if( NULL != NewMagic->m_Application )
      {  /* magic application ok */
        NewMagic->m_DispatcherNode.dn_Signals = DoMagic( NewMagic, 0 ) ;
        NewMagic->m_DispatcherNode.dn_DoFunction = ( DISPATCHER_DOFUNCTION )DoMagic ;
        NewMagic->m_DispatcherNode.dn_DoData = NewMagic ;
        AddDispatcherNode( MyContext->c_Dispatcher, &NewMagic->m_DispatcherNode ) ;
      }
      else
      {  /* magic application not ok */
        LogText( MyContext->c_Log, WARNING_LEVEL, "magic application not ok" ) ; 
        DeleteMagic( NewMagic ) ;
        NewMagic = NULL ;
      }
    }
    else
    {  /* magic context not ok */
      LogText( MyContext->c_Log, WARNING_LEVEL, "magic context not ok" ) ; 
    }
  }
  else
  {  /* requirements not ok */
  }

  return( NewMagic ) ;
}


/*
** delete the magic
*/
void DeleteMagic( struct Magic *OldMagic )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct Library *MUIMasterBase ;

  if( NULL != OldMagic )
  {  /* magic context needs to be freed */
    MyContext = OldMagic->m_Context ;
    SysBase = MyContext->c_SysBase ;
    MUIMasterBase = MyContext->c_MUIMasterBase ;
    if( NULL != OldMagic->m_Application )
    {  /* magic application needs to be disposed */
      RemDispatcherNode( MyContext->c_Dispatcher, &OldMagic->m_DispatcherNode ) ;
      MUI_DisposeObject( OldMagic->m_Application );
    }
    DeleteCustomClasses( OldMagic ) ;
    FreeVec( OldMagic ) ;
  }
}


/*
** helper function to create consistent space
*/
APTR MagicSpace( struct Magic *MyMagic )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct Library *MUIMasterBase ;
  APTR Space ;

  MyContext = MyMagic->m_Context ;
  SysBase = MyContext->c_SysBase ;
  MUIMasterBase = MyContext->c_MUIMasterBase ;

  Space = MUI_NewObject( MUIC_Rectangle,
    TAG_END ) ;
    
  return( Space ) ;
}


/*
** helper function to create consistent buttons
*/
APTR MagicButton( struct Magic *MyMagic, STRPTR Name, ULONG Key, STRPTR Help )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct Library *MUIMasterBase ;
  APTR Button ;

  MyContext = MyMagic->m_Context ;
  SysBase = MyContext->c_SysBase ;
  MUIMasterBase = MyContext->c_MUIMasterBase ;

  Button = MUI_NewObject( MUIC_Text,
    MUIA_Text_PreParse, ( IPTR )"\33c",
    MUIA_Text_Contents, ( IPTR )Name,
    MUIA_Text_HiChar, ( IPTR )Key,
    MUIA_ControlChar, ( IPTR )Key,
    MUIA_ShortHelp, ( IPTR )Help,
    MUIA_Frame, MUIV_Frame_Button,
    MUIA_Background, MUII_ButtonBack,
    MUIA_Font, ( IPTR ) MUIV_Font_Button,
    MUIA_InputMode, MUIV_InputMode_RelVerify,
    MUIA_CycleChain, TRUE,
    TAG_END ) ;
    
  return( Button ) ;
}


/*
** helper function to create consistent toggle buttons
*/
APTR MagicToggleButton( struct Magic *MyMagic, STRPTR Name, ULONG Key, STRPTR Help )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct Library *MUIMasterBase ;
  APTR Button ;

  MyContext = MyMagic->m_Context ;
  SysBase = MyContext->c_SysBase ;
  MUIMasterBase = MyContext->c_MUIMasterBase ;

  Button = MUI_NewObject( MUIC_Text,
    MUIA_Text_PreParse, ( IPTR )"\33c",
    MUIA_Text_Contents, ( IPTR )Name,
    MUIA_Text_HiChar, ( IPTR )Key,
    MUIA_ControlChar, ( IPTR )Key,
    MUIA_ShortHelp, ( IPTR )Help,
    MUIA_Frame, MUIV_Frame_Button,
    MUIA_Background, MUII_ButtonBack,
    MUIA_Font, ( IPTR ) MUIV_Font_Button,
    MUIA_InputMode, MUIV_InputMode_Toggle,
    MUIA_CycleChain, TRUE,
    TAG_END ) ;
    
  return( Button ) ;
}


/*
** helper function to create consistent labels
*/
APTR MagicLabel( struct Magic *MyMagic, STRPTR Name, ULONG Key )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct Library *MUIMasterBase ;
  APTR NewLabel ;

  MyContext = MyMagic->m_Context ;
  SysBase = MyContext->c_SysBase ;
  MUIMasterBase = MyContext->c_MUIMasterBase ;

  NewLabel = MUI_NewObject( MUIC_Text,
    MUIA_Text_PreParse, ( IPTR )"\33r",
    MUIA_Text_Contents, ( IPTR )Name,
    MUIA_Text_HiChar, ( IPTR )Key,
    MUIA_Frame, MUIV_Frame_None,
    MUIA_Font, ( IPTR ) MUIV_Font_Normal,
    MUIA_InputMode, MUIV_InputMode_None,
    MUIA_HorizWeight, 0,
    TAG_END ) ;

  return( NewLabel ) ;
}


/*
** helper function to create consistent checkmarks
*/
APTR MagicCheckmark( struct Magic *MyMagic, ULONG Key, STRPTR Help )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct Library *MUIMasterBase ;
  APTR NewCheckmark ;

  MyContext = MyMagic->m_Context ;
  SysBase = MyContext->c_SysBase ;
  MUIMasterBase = MyContext->c_MUIMasterBase ;

  NewCheckmark = MUI_NewObject( MUIC_Image,
    MUIA_ControlChar, ( IPTR )Key,
    MUIA_ShortHelp, ( IPTR )Help,
    MUIA_Frame, MUIV_Frame_Button,
    MUIA_Background, MUII_ButtonBack,
    MUIA_InputMode, MUIV_InputMode_Toggle,
    MUIA_ShowSelState, FALSE,
    MUIA_Image_Spec, MUII_CheckMark,
    MUIA_Image_FreeVert, TRUE,
    MUIA_Image_FreeHoriz, FALSE,
    MUIA_CycleChain, TRUE,
    TAG_END ) ;

  return( NewCheckmark ) ;
}


/*
** helper function to create consistent string input fields
*/
APTR MagicString( struct Magic *MyMagic, ULONG Key, STRPTR Help )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct Library *MUIMasterBase ;
  APTR NewString ;

  MyContext = MyMagic->m_Context ;
  SysBase = MyContext->c_SysBase ;
  MUIMasterBase = MyContext->c_MUIMasterBase ;

  NewString = MUI_NewObject( MUIC_String,
    MUIA_ControlChar, ( IPTR )Key,
    MUIA_ShortHelp, ( IPTR )Help,
    MUIA_Frame, MUIV_Frame_String,
    MUIA_Font, ( IPTR ) MUIV_Font_Normal,
    MUIA_CycleChain, TRUE,
    TAG_END ) ;

  return( NewString ) ;
}


/*
** helper function to create consistent secret string input fields
*/
APTR MagicSecretString( struct Magic *MyMagic, ULONG Key, STRPTR Help )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct Library *MUIMasterBase ;
  APTR NewSecretString ;

  MyContext = MyMagic->m_Context ;
  SysBase = MyContext->c_SysBase ;
  MUIMasterBase = MyContext->c_MUIMasterBase ;

  NewSecretString = MUI_NewObject( MUIC_String,
    MUIA_String_Secret, TRUE,
    MUIA_ControlChar, ( IPTR )Key,
    MUIA_ShortHelp, ( IPTR )Help,
    MUIA_Frame, MUIV_Frame_String,
    MUIA_Font, ( IPTR ) MUIV_Font_Normal,
    MUIA_CycleChain, TRUE,
    TAG_END ) ;

  return( NewSecretString ) ;
}


/*
** helper function to create consistent integer input fields
*/
APTR MagicInteger( struct Magic *MyMagic, ULONG Key, STRPTR Help )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct Library *MUIMasterBase ;
  APTR Integer ;

  MyContext = MyMagic->m_Context ;
  SysBase = MyContext->c_SysBase ;
  MUIMasterBase = MyContext->c_MUIMasterBase ;

  Integer = MUI_NewObject( MUIC_String,
    MUIA_String_Accept, ( IPTR )"0123456879",
    MUIA_ControlChar, ( IPTR )Key,
    MUIA_ShortHelp, ( IPTR )Help,
    MUIA_Frame, MUIV_Frame_String,
    MUIA_Font, ( IPTR ) MUIV_Font_Normal,
    MUIA_CycleChain, TRUE,
    TAG_END ) ;

  return( Integer ) ;
}


/*
** helper function to create consistent text fields
*/
APTR MagicText( struct Magic *MyMagic, STRPTR Contents )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct Library *MUIMasterBase ;
  APTR _Text ;

  MyContext = MyMagic->m_Context ;
  SysBase = MyContext->c_SysBase ;
  MUIMasterBase = MyContext->c_MUIMasterBase ;

  _Text = MUI_NewObject( MUIC_Text,
    MUIA_Text_Contents, ( IPTR )Contents,
    MUIA_Text_PreParse, ( IPTR )"\33c",
    MUIA_Text_SetMax, FALSE,
    MUIA_Frame, MUIV_Frame_Text,
    MUIA_Background, MUII_TextBack,
    MUIA_Font, ( IPTR ) MUIV_Font_Normal,
    MUIA_Text_SetMin, FALSE,
    MUIA_CycleChain, FALSE,
    TAG_END ) ;

  return( _Text ) ;
}


/*
** helper function to create consistent image buttons
*/
APTR MagicImage( struct Magic *MyMagic, ULONG Spec, ULONG Key )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct Library *MUIMasterBase ;
  APTR NewImage ;

  MyContext = MyMagic->m_Context ;
  SysBase = MyContext->c_SysBase ;
  MUIMasterBase = MyContext->c_MUIMasterBase ;

  NewImage = MUI_NewObject( MUIC_Image,
    MUIA_ControlChar, ( IPTR )Key,
    MUIA_Frame, MUIV_Frame_Button,
    MUIA_Background, MUII_ButtonBack,
    MUIA_InputMode, MUIV_InputMode_RelVerify,
    MUIA_Image_Spec, ( IPTR )Spec,
    MUIA_Image_FreeVert, TRUE,
    MUIA_Image_FreeHoriz, FALSE,
    MUIA_CycleChain, TRUE,
    TAG_END ) ;

  return( NewImage ) ;
}


/*
** helper function to create consistent cycle gadgets
*/
APTR MagicCycle( struct Magic *MyMagic, STRPTR *Names, ULONG Key, STRPTR Help )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct Library *MUIMasterBase ;
  APTR NewCycle ;

  MyContext = MyMagic->m_Context ;
  SysBase = MyContext->c_SysBase ;
  MUIMasterBase = MyContext->c_MUIMasterBase ;

  NewCycle = MUI_NewObject( MUIC_Cycle,
    MUIA_Cycle_Entries, ( IPTR )Names,
    MUIA_ControlChar, ( IPTR )Key,
    MUIA_ShortHelp, ( IPTR )Help,
    MUIA_Disabled, NULL == Names ? TRUE : FALSE, 
    MUIA_CycleChain, TRUE,
    TAG_END ) ;

  return( NewCycle ) ;
}


/*
** helper function to create consistent slider
*/
APTR MagicSlider( struct Magic *MyMagic, ULONG Key, STRPTR Help )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct Library *MUIMasterBase ;
  APTR NewSlider ;

  MyContext = MyMagic->m_Context ;
  SysBase = MyContext->c_SysBase ;
  MUIMasterBase = MyContext->c_MUIMasterBase ;

  NewSlider = MUI_NewObject( MUIC_Slider,
    MUIA_ControlChar, ( IPTR )Key,
    MUIA_ShortHelp, ( IPTR )Help,
    MUIA_CycleChain, TRUE,
    TAG_END ) ;

  return( NewSlider ) ;
}


/*
** insert some text on current cursor location of string object
*/
void MagicStringInsert( struct Magic *MyMagic, APTR MyString, STRPTR MyText )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct Library *MUIMasterBase ;
  STRPTR Contents ;
  LONG BufferPos, BenchPos, Increment ;
  
  MyContext = MyMagic->m_Context ;
  SysBase = MyContext->c_SysBase ;
  MUIMasterBase = MyContext->c_MUIMasterBase ;
  
  GetAttr( MUIA_String_Contents, MyString, ( ULONG *)&Contents ) ;
  GetAttr( MUIA_String_BufferPos, MyString, ( ULONG * )&BufferPos ) ;
  if( M_STRINGBENCH_LENGTH > ( BufferPos + 1 ) )
  {
    snprintf( &MyMagic->m_StringBench[ 0 ], ( BufferPos + 1 ), "%s%n", &Contents[ 0 ], &Increment ) ;
    BenchPos = Increment ;
    snprintf( &MyMagic->m_StringBench[ BenchPos ], ( M_STRINGBENCH_LENGTH - BenchPos - 1 ), "%s%n", MyText, &Increment ) ;
    BenchPos += Increment ;
    snprintf( &MyMagic->m_StringBench[ BenchPos ], ( M_STRINGBENCH_LENGTH - BenchPos - 1 ), "%s", &Contents[ ( BufferPos ) ] ) ;
    SetAttrs( MyString, MUIA_String_Contents, MyMagic->m_StringBench, TAG_END ) ;
    SetAttrs( MyString, MUIA_String_BufferPos, ( BufferPos + Increment ) , TAG_END ) ;
  }
}

