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
** LogWindowClass.c
*/


#include "LogWindowClass.h"
#include "Localization.h"
#include "Log.h"
#include <proto/muimaster.h>
#include <clib/alib_protos.h>
#include <libraries/asl.h>


/*
** add a log entry
*/
static IPTR LogWindowAddLogEntry( struct IClass *MyIClass, Object *MyObject, struct LogEntry *MyLogEntry ) 
{
  struct LogWindowData *MyData ;
  struct Context *MyContext ;
  LONG MyEntries, MyVisible, MyFirst, MyJump ;
  IPTR Result ;

  MyData = INST_DATA( MyIClass, MyObject ) ;
  MyContext = MyData->lwd_Context ;
  
  GetAttr( MUIA_List_Visible, MyData->lwd_LogList, &MyVisible ) ;
  MyJump = 0 ;
  if( -1 != MyVisible )
  {  /* list is visible */
    GetAttr( MUIA_List_Entries, MyData->lwd_LogList, &MyEntries ) ;
    GetAttr( MUIA_List_First, MyData->lwd_LogList, &MyFirst ) ;
    if( ( MyEntries - MyFirst ) == MyVisible )
    {  /* keep the last visible */
      MyJump = MyEntries + 1 ;
    }
  }
  
  DoMethod( MyData->lwd_LogList, MUIM_List_InsertSingle, MyLogEntry, MUIV_List_Insert_Bottom ) ;
  
  Result = 0 ;  /* MUIM_List_InsertSingle does not document any return value */

  if( MyJump )
  {  /* stick at the bottom of the list */
    DoMethod( MyData->lwd_LogList, MUIM_List_Jump, MyJump ) ;
  }
  
  return( Result ) ;
}


/*
** flush button was pressed
*/
static IPTR LogWindowFlushLog( struct IClass *MyIClass, Object *MyObject ) 
{
  struct LogWindowData *MyData ;
  struct Context *MyContext ;
  IPTR Result ;

  MyData = INST_DATA( MyIClass, MyObject ) ;
  MyContext = MyData->lwd_Context ;
  
  ClearLog( MyContext->c_Log ) ; 
  Result = 0 ;
  
  return( Result ) ;
}


/*
** clear the log gui
*/
static IPTR LogWindowClearLog( struct IClass *MyIClass, Object *MyObject ) 
{
  struct LogWindowData *MyData ;
  IPTR Result ;

  MyData = INST_DATA( MyIClass, MyObject ) ;
  
  Result = DoMethod( MyData->lwd_LogList, MUIM_List_Clear ) ;
  
  return( Result ) ;
}


/*
** save button was pressed
*/
static IPTR LogWindowSaveLog( struct IClass *MyIClass, Object *MyObject, STRPTR MyLogFile ) 
{
  struct LogWindowData *MyData ;
  struct Context *MyContext ;
  IPTR Result ;

  MyData = INST_DATA( MyIClass, MyObject ) ;
  MyContext = MyData->lwd_Context ;
  
  Result = SaveLog( MyContext->c_Log, MyLogFile ) ; 
  
  return( Result ) ;
}


/*
** gets called when list entry gets displayed
*/
static LONG REGFUNC ListDisplayHook( REG( a0, struct Hook *MyHook ),
                                     REG( a2, STRPTR *MyArray ),
                                     REG( a1, struct LogEntry *MyEntry ) )
{
  struct Context *MyContext ;
  struct LogWindowData *MyData ;
  struct ExecBase *SysBase ;

  MyData = MyHook->h_Data ;
  MyContext = MyData->lwd_Context ;
  SysBase = MyContext->c_SysBase ;
  
  MyArray[ 0 ] = MyEntry->le_TimeString ;
  MyArray[ 1 ] = MyEntry->le_LevelString ;
  MyArray[ 2 ] = MyEntry->le_MessageString ;
  
  return( 0 ) ;
}


/*
** NEW method for the log window class
*/
static IPTR LogWindowNew( struct IClass *MyIClass, Object *MyObject, struct opSet *MyMsg )
{
  struct Context *MyContext ;
  struct Library *MUIMasterBase ;
  struct LogWindowData *MyData ;
  struct TagItem AttrListExtension[ 2 ] ;
  APTR RootObject ;
  APTR LogListview ;
  APTR CloseButton, SaveButton, SavePopasl, FlushButton, ButtonGroup ;
  SIPTR Result ;

  MyContext = ( struct Context * )MyIClass->cl_UserData ;
  MUIMasterBase = MyContext->c_MUIMasterBase ;

  RootObject = MUI_NewObject( MUIC_Group,
    MUIA_Group_Horiz, FALSE,
    TAG_END ) ;
  AttrListExtension[ 0 ].ti_Tag = MUIA_Window_RootObject ;
  AttrListExtension[ 0 ].ti_Data = ( IPTR )RootObject ;
  AttrListExtension[ 1 ].ti_Tag = TAG_MORE ;
  AttrListExtension[ 1 ].ti_Data = ( IPTR )MyMsg->ops_AttrList ;
  MyMsg->ops_AttrList = AttrListExtension ;
  Result = DoSuperMethodA( MyIClass, MyObject, ( Msg )MyMsg ) ;
  if( 0 != Result )
  {  /* window object created */
    MyObject = ( Object * )Result ;
    MyData = INST_DATA( MyIClass, MyObject ) ;
    MyData->lwd_Context = MyContext ;
    
    MyData->lwd_ListDisplayHook.h_Entry = ( HOOKFUNC )ListDisplayHook ;
    MyData->lwd_ListDisplayHook.h_Data = MyData ;
    MyData->lwd_LogList = MUI_NewObject( MUIC_List,
      MUIA_List_Format, ( IPTR )( "BAR,BAR," ),
      MUIA_List_DisplayHook, ( IPTR )( &MyData->lwd_ListDisplayHook ),
      TAG_END ) ;
    LogListview = MUI_NewObject( MUIC_Listview,
      MUIA_Listview_List, ( IPTR )( MyData->lwd_LogList ),
      MUIA_Listview_Input, FALSE,
      MUIA_Frame, MUIV_Frame_ReadList,
      TAG_END ) ;
    DoMethod( RootObject, OM_ADDMEMBER, LogListview ) ;

    CloseButton = MagicButton( MyContext->c_Magic, 
      GetLocalizedString( MyContext->c_Localization, STR_LOGWINDOW_CLOSE ),
      *GetLocalizedString( MyContext->c_Localization, STR_LOGWINDOW_CLOSE_KEY ),
      GetLocalizedString( MyContext->c_Localization, STR_LOGWINDOW_CLOSE_HELP ) ) ;
    FlushButton = MagicButton( MyContext->c_Magic, 
      GetLocalizedString( MyContext->c_Localization, STR_LOGWINDOW_FLUSH ),
      *GetLocalizedString( MyContext->c_Localization, STR_LOGWINDOW_FLUSH_KEY ),
      GetLocalizedString( MyContext->c_Localization, STR_LOGWINDOW_FLUSH_HELP ) ) ;

    MyData->lwd_LogFileString = MUI_NewObject( MUIC_String,
      MUIA_String_Contents, ( IPTR )"cAMIra.log",
      MUIA_ShowMe, FALSE,
      TAG_END ) ;
    SaveButton = MagicButton( MyContext->c_Magic, 
      GetLocalizedString( MyContext->c_Localization, STR_LOGWINDOW_SAVE ),
      *GetLocalizedString( MyContext->c_Localization, STR_LOGWINDOW_SAVE_KEY ),
      GetLocalizedString( MyContext->c_Localization, STR_LOGWINDOW_SAVE_HELP ) ) ;
    SavePopasl = MUI_NewObject( MUIC_Popasl,
      MUIA_Popstring_String, ( IPTR )( MyData->lwd_LogFileString ),
      MUIA_Popstring_Button, ( IPTR )( SaveButton ) ,
      MUIA_Popasl_Type, ASL_FileRequest,
      ASLFR_DoSaveMode, TRUE,
      MUIA_CycleChain, TRUE,
      TAG_END ) ;

    ButtonGroup = MUI_NewObject( MUIC_Group,
      MUIA_Group_Horiz, TRUE,
      MUIA_Group_Child, ( IPTR )( MagicSpace( MyContext->c_Magic ) ),
      MUIA_Group_Child, ( IPTR )( SavePopasl ),
      MUIA_Group_Child, ( IPTR )( MagicSpace( MyContext->c_Magic ) ),
      MUIA_Group_Child, ( IPTR )( FlushButton ),
      MUIA_Group_Child, ( IPTR )( MagicSpace( MyContext->c_Magic ) ),
      MUIA_Group_Child, ( IPTR )( CloseButton ),
      MUIA_Group_Child, ( IPTR )( MagicSpace( MyContext->c_Magic ) ),
      TAG_END ) ;
    DoMethod( RootObject, OM_ADDMEMBER, ButtonGroup ) ;
    
    if( ( NULL != LogListview ) && ( NULL != ButtonGroup ) )
    {  /* window is complete */
      DoMethod( FlushButton, MUIM_Notify, MUIA_Pressed, FALSE,
                MyObject, 1, MUIM_FlushLog ) ;
      /* http://tech.dir.groups.yahoo.com/group/amiga-jabberwocky/message/2792 */
      DoMethod( MyData->lwd_LogFileString, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
                MyObject, 2, MUIM_SaveLog, MUIV_TriggerValue ) ;
      DoMethod( CloseButton, MUIM_Notify, MUIA_Pressed, FALSE, 
                MyObject, 3, MUIM_Set, MUIA_Window_Open, FALSE ) ;
      DoMethod( MyObject, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, 
                MyObject, 3, MUIM_Set, MUIA_Window_Open, FALSE ) ;
    }
    else
    {  /* window is not complete */
      CoerceMethod( MyIClass, MyObject, OM_DISPOSE ) ;
      Result = 0 ;
    }
  }
  else
  {  /* window object could not be created */
    DisableLogGui( MyContext->c_Log ) ;
  }
  
  return( Result ) ; 
}


/*
** dispose log window
*/
static IPTR LogWindowDispose( struct IClass *MyIClass, Object *MyObject, Msg MyMsg )
{
  struct LogWindowData *MyData ;
  struct Context *MyContext ;
  IPTR Result ;

  MyData = INST_DATA( MyIClass, MyObject ) ;
  MyContext = MyData->lwd_Context ;
  
  Result = DoSuperMethodA( MyIClass, MyObject, MyMsg ) ;
  
  return( Result ) ;
}


/*
** dispatcher for our custom log list class
*/
IPTR LogWindowDispatcher( REG( a0, struct IClass *MyIClass ), REG( a2, Object *MyObject ), REG( a1, Msg MyMsg ) )
{
  IPTR Result ;
  
  Result = 0 ;
  
  switch( MyMsg->MethodID )
  {
    case OM_NEW:
      Result = LogWindowNew( MyIClass, MyObject, ( struct opSet * )MyMsg ) ;
      break ;
    case OM_DISPOSE:
      Result = LogWindowDispose( MyIClass, MyObject, MyMsg ) ;
      break ;
    case MUIM_AddLogEntry:
      Result = LogWindowAddLogEntry( MyIClass, MyObject, ( struct LogEntry * )( ( ( struct ParamMsg * )MyMsg )->MsgParam ) ) ;
      break ;
    case MUIM_FlushLog:
      Result = LogWindowFlushLog( MyIClass, MyObject ) ;
      break ;
    case MUIM_SaveLog:
      Result = LogWindowSaveLog( MyIClass, MyObject, ( STRPTR )( ( ( struct ParamMsg * )MyMsg )->MsgParam ) ) ;
      break ;
    case MUIM_ClearLogGui:
      Result = LogWindowClearLog( MyIClass, MyObject ) ;
      break ;
    default:
      Result = DoSuperMethodA( MyIClass, MyObject, MyMsg ) ;
      break ;
  }

  return( Result ) ;
}
