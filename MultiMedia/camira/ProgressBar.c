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
** ProgressBar.c
*/


#include "ProgressBar.h"
#include "Magic.h"
#include "MainWindow.h"
#include "Log.h"
#include "Localization.h"
#include <proto/exec.h>
#include <proto/muimaster.h>
#include <clib/alib_protos.h>


/*
** prepare gauge output with certain data
*/
static void ConfigureProgressBar( struct ProgressBar *MyProgressBar, struct ProgressBarNode *MyNode )
{
  struct Context *MyContext ;
  struct Library *MUIMasterBase ;
  struct IntuitionBase *IntuitionBase ;

  if( NULL != MyNode )
  {  /* configure according to the node */
    MyContext = MyProgressBar->pb_Context ;
    IntuitionBase = MyContext->c_IntuitionBase ;
    MUIMasterBase = MyContext->c_MUIMasterBase ;
    
    SetAttrs( MyProgressBar->pb_ProgressGauge,
      MUIA_Disabled, FALSE,
      MUIA_Gauge_InfoText, MyNode->pbn_InfoString,
      MUIA_Gauge_Max, MyNode->pbn_Limit,
      MUIA_Gauge_Current, MyNode->pbn_LastCurrent,
      TAG_END ) ;

    if( DoMethod( MyProgressBar->pb_ProgressButtonGroup, MUIM_Group_InitChange ) )
    {  /* remove button object */
      DoMethod( MyProgressBar->pb_ProgressButtonGroup, OM_REMMEMBER, MyProgressBar->pb_ProgressButton ) ;
      MUI_DisposeObject( MyProgressBar->pb_ProgressButton ) ;
      if( ( MUIV_InputMode_Toggle == MyNode->pbn_ButtonMode ) )
      {  /* create the button with toggle mode */
        MyProgressBar->pb_ProgressButton = MagicToggleButton( MyContext->c_Magic,
          GetLocalizedString( MyContext->c_Localization, STR_PROGRESS_TOGGLE ),
          *GetLocalizedString( MyContext->c_Localization, STR_PROGRESS_TOGGLE_KEY ),
          GetLocalizedString( MyContext->c_Localization, STR_PROGRESS_TOGGLE_HELP ) ) ;
        SetAttrs( MyProgressBar->pb_ProgressButton,
          MUIA_Selected, MyNode->pbn_ButtonState,
          TAG_END ) ;
        DoMethod( MyProgressBar->pb_ProgressButton, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
                  MyProgressBar->pb_ProgressButton, 3, MUIM_WriteLong, MUIV_TriggerValue, &MyNode->pbn_ButtonState ) ;
      }
      else  // if( ( MUIV_InputMode_RelVerify == MyNode->pbn_ButtonMode ) )
      {  /* create the button with relverify mode */
        MyProgressBar->pb_ProgressButton = MagicButton( MyContext->c_Magic,
          GetLocalizedString( MyContext->c_Localization, STR_PROGRESS_CANCEL ),
          *GetLocalizedString( MyContext->c_Localization, STR_PROGRESS_CANCEL_KEY ),
          GetLocalizedString( MyContext->c_Localization, STR_PROGRESS_CANCEL_HELP ) ) ;
        DoMethod( MyProgressBar->pb_ProgressButton, MUIM_Notify, MUIA_Pressed, FALSE,
                  MyProgressBar->pb_ProgressButton, 3, MUIM_WriteLong, 0, &MyNode->pbn_ButtonState ) ;
      }
      DoMethod( MyProgressBar->pb_ProgressButtonGroup, OM_ADDMEMBER, MyProgressBar->pb_ProgressButton ) ;
      DoMethod( MyProgressBar->pb_ProgressButtonGroup, MUIM_Group_ExitChange ) ;
    }
  }
  else
  {  /* disable */
    SetAttrs( MyProgressBar->pb_ProgressGauge,
      MUIA_Disabled, TRUE,
      TAG_END ) ;
    SetAttrs( MyProgressBar->pb_ProgressButton,
      MUIA_Disabled, TRUE,
      TAG_END ) ;
  }
}


/*
** update gauge value
*/
static void UpdateProgressBar( struct ProgressBar *MyProgressBar, ULONG Current )
{
  struct Context *MyContext ;
  struct IntuitionBase *IntuitionBase ;

  MyContext = MyProgressBar->pb_Context ;
  IntuitionBase = MyContext->c_IntuitionBase ;
    
  SetAttrs( MyProgressBar->pb_ProgressGauge,
    MUIA_Gauge_Current, Current,
    TAG_END ) ;
}


#if 0
/*
** enable cancel button
*/
void EnableCancel( struct MainWindowContext *MyMainWindow )
{
  struct Context *MyContext ;
  struct IntuitionBase *IntuitionBase ;

  if( NULL != MyMainWindow )
  {  /* requirements ok */
    MyContext = MyMainWindow->mwc_Context ;
    IntuitionBase = MyContext->c_IntuitionBase ;
    SetAttrs( MyMainWindow->mwc_CancelButton, MUIA_Disabled, FALSE,
                                              TAG_END ) ;
    //printf( "enabled cancel button\n" ) ;
  }
  else
  {  /* requirements not ok */
  }
}


/*
** disable cancel button
*/
void DisableCancel( struct MainWindowContext *MyMainWindow )
{
  struct Context *MyContext ;
  struct IntuitionBase *IntuitionBase ;

  if( NULL != MyMainWindow )
  {  /* requirements ok */
    MyContext = MyMainWindow->mwc_Context ;
    IntuitionBase = MyContext->c_IntuitionBase ;
    SetAttrs( MyMainWindow->mwc_CancelButton, MUIA_Disabled, TRUE,
                                              TAG_END ) ;
    //printf( "disabled cancel button\n" ) ;
  }
  else
  {  /* requirements not ok */
  }
}
#endif





/*
** create progress bar context 
*/
struct ProgressBar *CreateProgressBar( struct Context *MyContext )
{
  struct ExecBase *SysBase ;
  struct Library *MUIMasterBase ;
  struct IntuitionBase *IntuitionBase ;
  struct ProgressBar *NewProgressBar ;

  NewProgressBar = NULL ;
  
  if( ( NULL != MyContext->c_MainWindow ) )
  {  /* requirements seem ok */
    SysBase = MyContext->c_SysBase ;
    IntuitionBase = MyContext->c_IntuitionBase ;
    MUIMasterBase = MyContext->c_MUIMasterBase ;
    NewProgressBar = AllocVec( sizeof( struct ProgressBar ), MEMF_ANY | MEMF_CLEAR ) ;
    if( NULL != NewProgressBar )
    {  /* memory for gauge context ok */
      NewProgressBar->pb_Context = MyContext ;
      NewProgressBar->pb_ProgressGauge = MUI_NewObject( MUIC_Gauge,
        MUIA_Gauge_Horiz, TRUE,
        MUIA_Frame, MUIV_Frame_Gauge,
        TAG_END ) ;
      NewProgressBar->pb_ProgressButton = MagicButton( MyContext->c_Magic, " ", 0, NULL ),
      NewProgressBar->pb_ProgressButtonGroup = MUI_NewObject( MUIC_Group,
        MUIA_HorizWeight, 0,
        MUIA_Group_Child, ( IPTR )( NewProgressBar->pb_ProgressButton ),
        TAG_END ) ;
      NewProgressBar->pb_BarObject = MUI_NewObject( MUIC_Group,
        MUIA_Group_Columns, 2,
        MUIA_Group_Child, ( IPTR )( NewProgressBar->pb_ProgressGauge ),
        MUIA_Group_Child, ( IPTR )( NewProgressBar->pb_ProgressButtonGroup ),
        TAG_END ) ;
      if( NULL != NewProgressBar->pb_BarObject )
      {  /* progress bar object ok */
        NewProgressBar->pb_AddedToMainWindow = DoMethod( MyContext->c_MainWindow->mw_WindowObject, MUIM_AddProgressBar, NewProgressBar->pb_BarObject ) ;
        if( TRUE == NewProgressBar->pb_AddedToMainWindow )
        {  /* added to main window */
          ConfigureProgressBar( NewProgressBar, NULL ) ;
          NewList( &NewProgressBar->pb_NodesList ) ;
        }
        else
        {  /* could not add progress bar to main window */
          LogText( MyContext->c_Log, WARNING_LEVEL, "could not add progress bar to main window" ) ;
          DeleteProgressBar( NewProgressBar ) ;
          NewProgressBar = NULL ;
        }
      }
      else
      {  /* progress bar object not ok */
        LogText( MyContext->c_Log, WARNING_LEVEL, "progress bar object not ok" ) ;
        DeleteProgressBar( NewProgressBar ) ;
        NewProgressBar = NULL ;
      }
    }
    else
    {  /* progress bar context not ok */
      LogText( MyContext->c_Log, WARNING_LEVEL, "progress bar context not ok" ) ;
    }
  }
  else
  {  /* requirements not ok */
  }

  return( NewProgressBar ) ;
}


/*
** delete progress bar context
*/
void DeleteProgressBar( struct ProgressBar *OldProgressBar )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct IntuitionBase *IntuitionBase ;
  struct Library *MUIMasterBase ;
  
  if( NULL != OldProgressBar )
  {  /* progress bar context needs to be freed */
    MyContext = OldProgressBar->pb_Context ;
    SysBase = MyContext->c_SysBase ;
    IntuitionBase = MyContext->c_IntuitionBase ;
    MUIMasterBase = MyContext->c_MUIMasterBase ;
    if( NULL != OldProgressBar->pb_BarObject )
    {  /* progress bar object needs to be disposed */
      if( TRUE == OldProgressBar->pb_AddedToMainWindow )
      {  /* progress bar needs to be removed from main window */
        DoMethod( MyContext->c_MainWindow->mw_WindowObject, MUIM_RemoveProgressBar, OldProgressBar->pb_BarObject ) ;
      }
      MUI_DisposeObject( OldProgressBar->pb_BarObject ) ;
    }
    FreeVec( OldProgressBar ) ;
    //printf( "progress bar context freed\n" ) ;
  }
}


/*
** update progress of a certain action
*/
void ReportProgress( struct ProgressBar *MyProgressBar, struct ProgressBarNode *MyNode, ULONG Current )
{
  struct Context *MyContext ;
  struct Node *WalkNode ;
  
  if( ( NULL != MyProgressBar ) && ( NULL != MyNode ) )
  {  /* requirements ok */
    MyContext = MyProgressBar->pb_Context ;
    
    WalkNode = MyProgressBar->pb_NodesList.lh_Head ;
    while( WalkNode->ln_Succ )
    {  /* loop all current list entries */
      if( WalkNode == &MyNode->pbn_Node )
      {  /* update this node */
        break ;
      }
      WalkNode = WalkNode->ln_Succ ;
    }
    if( NULL != WalkNode->ln_Succ )
    {  /* node is in list */
      if( MyNode->pbn_LastCurrent != Current )
      {  /* new current for this progress bar node */
        if( WalkNode == MyProgressBar->pb_NodesList.lh_Head )
        {  /* update head of list */
          UpdateProgressBar( MyProgressBar, Current ) ;
        }
        else
        {  /* no update for head of list */
        }
        MyNode->pbn_LastCurrent = Current ;
      }
      else
      {  /* no new current for this guage node */
      }
    }
    else
    {  /* node not in the list */
    }
  }
  else
  {  /* requirements not ok */
  }
  
  return ;
}


/*
** add a progress bar node to the list
*/
void AddProgressBarNode( struct ProgressBar *MyProgressBar, struct ProgressBarNode *AddNode, ULONG Initial )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct Node *WalkNode ;

  if( ( NULL != MyProgressBar ) && ( NULL != AddNode ) )
  {  /* requirements ok */
    MyContext = MyProgressBar->pb_Context ;
    SysBase = MyContext->c_SysBase ;
    
    WalkNode = MyProgressBar->pb_NodesList.lh_Head ;
    while( WalkNode->ln_Succ )
    {  /* loop all current list entries */
      if( WalkNode->ln_Pri <= AddNode->pbn_Node.ln_Pri )
      {  /* higher or same prio, so add infront of this */
        break ;
      }
      WalkNode = WalkNode->ln_Succ ;
    }
    AddNode->pbn_LastCurrent = Initial ;
    if( WalkNode == MyProgressBar->pb_NodesList.lh_Head )
    {  /* insert at head of list */
      AddHead( &MyProgressBar->pb_NodesList, &AddNode->pbn_Node ) ;
      ConfigureProgressBar( MyProgressBar, AddNode ) ;
    }
    else if( NULL == WalkNode->ln_Succ )
    {  /* insert at tail of list */
      AddTail( &MyProgressBar->pb_NodesList, &AddNode->pbn_Node ) ;
    }
    else
    {  /* insert somewhere in the list */
      Insert( &MyProgressBar->pb_NodesList, &AddNode->pbn_Node, WalkNode->ln_Pred ) ;
    }
  }
  else
  {  /* requirements not ok */
  }
}


/*
** remove a gauge from the list
*/
void RemProgressBarNode( struct ProgressBar *MyProgressBar, struct ProgressBarNode *RemNode )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct Node *WalkNode ;
  struct ProgressBarNode *CurrentNode ;

  if( ( NULL != MyProgressBar ) && ( NULL != RemNode ) )
  {  /* requirements ok */
    MyContext = MyProgressBar->pb_Context ;
    SysBase = MyContext->c_SysBase ;
    
    WalkNode = MyProgressBar->pb_NodesList.lh_Head ;
    while( WalkNode->ln_Succ )
    {  /* loop all current list entries */
      if( WalkNode == &RemNode->pbn_Node )
      {  /* remove this node */
        break ;
      }
      WalkNode = WalkNode->ln_Succ ;
    }
    if( WalkNode == MyProgressBar->pb_NodesList.lh_Head )
    {  /* remove head of list */
      RemHead( &MyProgressBar->pb_NodesList ) ;
      if( IsListEmpty( &MyProgressBar->pb_NodesList ) )
      {  /* no more nodes, cancel can be disabled */
        ConfigureProgressBar( MyProgressBar, NULL ) ;
      }
      else
      {  /* display next remaining node */
        CurrentNode = ( struct ProgressBarNode * )MyProgressBar->pb_NodesList.lh_Head ;
        ConfigureProgressBar( MyProgressBar, CurrentNode ) ;
      }
    }
    else if( NULL == WalkNode->ln_Succ )
    {  /* node not in the list */
    }
    else
    {  /* remove somewhere in the list */
      Remove( WalkNode ) ;
    }
  }
  else
  {  /* requirements not ok */
  }
}


