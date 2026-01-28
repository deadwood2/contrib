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
** DisplayArea.c
*/


#include "DisplayArea.h"
#include "Magic.h"
#include "MainWindow.h"
#include "Settings.h"
#include "VideoHardware.h"
#include "Log.h"
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <clib/alib_protos.h>


/*
** trigger the display request
*/
static void DoDisplayTimer( struct DisplayArea *MyDisplayArea )
{
  struct Context *MyContext ;
  LONG Error ;

  MyContext = MyDisplayArea->da_Context ;
  
  do
  {
    MyContext->c_DisplayRequest = 1 ;
    Error = AddTimerNode( MyContext->c_Timer, &MyDisplayArea->da_TimerNode ) ;
  }
  while( Error ) ;
}


/*
** start the timer that triggers the display request
*/
static void StartDisplayTimer( struct DisplayArea *MyDisplayArea )
{
  struct Context *MyContext ;

  MyContext = MyDisplayArea->da_Context ;

  MyDisplayArea->da_TimerNode.tn_DeltaTime.tv_secs = MyDisplayArea->da_CurrentInterval / 1000 ;
  MyDisplayArea->da_TimerNode.tn_DeltaTime.tv_micro = ( MyDisplayArea->da_CurrentInterval - MyDisplayArea->da_TimerNode.tn_DeltaTime.tv_secs * 1000 ) * 1000 ;
  MyDisplayArea->da_TimerNode.tn_DoFunction = ( void ( * )( APTR ) )DoDisplayTimer ;
  MyDisplayArea->da_TimerNode.tn_DoData = MyDisplayArea ;
  MyDisplayArea->da_TimerNode.tn_AbsoluteTime.tv_secs = 0 ;  /* delta is added to current time */
  AddTimerNode( MyContext->c_Timer, &MyDisplayArea->da_TimerNode ) ;
}


/*
** stop the timer that triggers the display request
*/
static void StopDisplayTimer( struct DisplayArea *MyDisplayArea )
{
  struct Context *MyContext ;
  
  MyContext = MyDisplayArea->da_Context ;

  RemTimerNode( MyContext->c_Timer, &MyDisplayArea->da_TimerNode ) ;
}


/*
** delete a display area
*/
void DeleteDisplayArea( struct DisplayArea *OldDisplayArea )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct IntuitionBase *IntuitionBase ;
  struct Library *MUIMasterBase ;
 
  if( NULL != OldDisplayArea )
  {  /* display area context needs to be freed */
    MyContext = OldDisplayArea->da_Context ;
    SysBase = MyContext->c_SysBase ;
    IntuitionBase = MyContext->c_IntuitionBase ;
    MUIMasterBase = MyContext->c_MUIMasterBase ;
    if( NULL != OldDisplayArea->da_AreaObject )
    {  /* display area object needs to be disposed */
      if( TRUE == OldDisplayArea->da_AddedToMainWindow )
      {  /* display area needs to be removed from main window */
        StopDisplayTimer( OldDisplayArea ) ;
        DisplayImage( OldDisplayArea, NULL ) ;
        DoMethod( MyContext->c_MainWindow->mw_WindowObject, MUIM_RemoveDisplayArea, OldDisplayArea->da_AreaObject ) ;
      }
      MUI_DisposeObject( OldDisplayArea->da_AreaObject ) ;
    }
    FreeVec( OldDisplayArea ) ;
  }
}


/*
** configure the display area according to settings
*/
void ConfigureDisplayArea( struct DisplayArea *MyDisplayArea )
{
  struct Context *MyContext ;
  struct IntuitionBase *IntuitionBase ;
  LONG SettingValue ;
  ULONG NewDisplayEnable ;
  ULONG NewDisplayInterval ;
  if( ( NULL != MyDisplayArea ) )
  {  /* requirements ok */
    MyContext = MyDisplayArea->da_Context ;
    IntuitionBase = MyContext->c_IntuitionBase ;
    
    SettingValue = GetSetting( MyContext->c_Settings, DisplayZoom ) ;
    SetAttrs( MyDisplayArea->da_AreaObject,
      MUIA_DisplayZoom, SettingValue,
      TAG_END ) ;
    SettingValue = GetSetting( MyContext->c_Settings, DisplayRatio ) ;
    SetAttrs( MyDisplayArea->da_AreaObject,
      MUIA_DisplayRatio, SettingValue, 
      TAG_END ) ;
  
    SettingValue = GetSetting( MyContext->c_Settings, DisplayEnable ) ;
    NewDisplayEnable = SettingValue ;
    SettingValue = GetSetting( MyContext->c_Settings, DisplayInterval ) ;
    NewDisplayInterval = SettingValue ;
    
    if( 0 > CheckVideoHardware( MyContext->c_VideoHardware ) )
    {  /* no video hardware, so also no display update */
      NewDisplayEnable = 0 ;
    }
    
    if( !( NewDisplayEnable ) || 
       ( MyDisplayArea->da_CurrentInterval != NewDisplayInterval ) )
    {  /* not enabled or a new rate */
      if( 0 != MyDisplayArea->da_CurrentInterval )
      {  /* we have to stop the timer */
        LogText( MyContext->c_Log, DEBUG_LEVEL, "stopping display timer" ) ;
        StopDisplayTimer( MyDisplayArea ) ;
        MyDisplayArea->da_CurrentInterval = 0 ;
        MyContext->c_DisplayRequest = 0 ;
      }
    }

    if( ( NewDisplayEnable ) &&
        ( 0 != NewDisplayInterval ) )
    {  /* display enabled and a valid interval */
      if( 0 == MyDisplayArea->da_CurrentInterval )
      {  /* we have to start the timer */
        MyDisplayArea->da_CurrentInterval = NewDisplayInterval ;
        LogText( MyContext->c_Log, DEBUG_LEVEL, "starting display timer with %ldms interval", MyDisplayArea->da_CurrentInterval ) ;
        StartDisplayTimer( MyDisplayArea ) ;
      }
    }
    else
    {  /* display remains disabled */
      DisplayImage( MyDisplayArea, NULL ) ;
    }
  }
  else
  {  /* requirements not ok */
  }
}


/*
** create a display area
*/
struct DisplayArea *CreateDisplayArea( struct Context *MyContext )
{
  struct ExecBase *SysBase ;
  struct Library *MUIMasterBase ;
  struct IntuitionBase *IntuitionBase ;
  struct DisplayArea *NewDisplayArea ;

  NewDisplayArea = NULL ;

  if( ( NULL != MyContext->c_MainWindow ) && ( NULL != MyContext->c_Magic->m_DisplayAreaClass ) )
  {  /* requirements seem ok */
    SysBase = MyContext->c_SysBase ;
    IntuitionBase = MyContext->c_IntuitionBase ;
    MUIMasterBase = MyContext->c_MUIMasterBase ;
    NewDisplayArea = AllocVec( sizeof( struct DisplayArea ), MEMF_ANY | MEMF_CLEAR ) ;
    if( NULL != NewDisplayArea )
    {  /* memory for display area context ok */
      NewDisplayArea->da_Context = MyContext ;
      NewDisplayArea->da_AreaObject = NewObject( MyContext->c_Magic->m_DisplayAreaClass->mcc_Class, NULL,
        MUIA_Group_Horiz, TRUE,
        MUIA_Group_Spacing, 0,  /* required to make props "invisible" */
        TAG_END ) ;
      if( NULL != NewDisplayArea->da_AreaObject )
      {  /* display area object ok */
        NewDisplayArea->da_AddedToMainWindow = DoMethod( MyContext->c_MainWindow->mw_WindowObject, MUIM_AddDisplayArea, NewDisplayArea->da_AreaObject ) ;
        if( TRUE == NewDisplayArea->da_AddedToMainWindow )
        {  /* added to main window */
        }
        else
        {  /* could not add display area to main window */
          LogText( MyContext->c_Log, WARNING_LEVEL, "could not add display area to main window" ) ;
          DeleteDisplayArea( NewDisplayArea ) ;
          NewDisplayArea = NULL ;
        }
      }
      else
      {  /* display area object not ok */
        LogText( MyContext->c_Log, WARNING_LEVEL, "display area object not ok" ) ;
        DeleteDisplayArea( NewDisplayArea ) ;
        NewDisplayArea = NULL ;
      }
    }
    else
    {  /* display area context not ok */
      LogText( MyContext->c_Log, WARNING_LEVEL, "display area context not ok" ) ;
    }
  }
  else
  {  /* requirements not ok */
  }

  return( NewDisplayArea ) ;
}


/*
** show an image in the display area
*/
void DisplayImage( struct DisplayArea *MyDisplayArea, struct ImageBox *MyImageBox )
{
  struct Context *MyContext ;
  struct ImageBox *PreviousImageBox ;

  if( NULL != MyDisplayArea )
  {  /* requirements ok */
    MyContext = MyDisplayArea->da_Context ;
    PreviousImageBox = NULL ;

    if( NULL != MyImageBox )
    {  /* display an image from this box */
      MyImageBox->ib_RgbImage = CommandImageProcessor( MyContext->c_ImageProcessor, IMAGE_COMMAND_CREATE_RGB24, MyImageBox ) ;
      if( NULL != MyImageBox->ib_RgbImage )
      {  /* there is a rgb image in the box */
        PreviousImageBox = MyDisplayArea->da_CurrentImageBox ;
        MyDisplayArea->da_CurrentImageBox = MyImageBox ;
        SetAttrs( MyDisplayArea->da_AreaObject,
          MUIA_DisplayImage, MyDisplayArea->da_CurrentImageBox->ib_RgbImage,
          TAG_END ) ;
        MyDisplayArea->da_CurrentImageBox->ib_UseCounter++ ;
      }
      else
      {  /* do nothing if there is no rgb image in the box */
      }
    }
    else
    {  /* clear the display area */
      PreviousImageBox = MyDisplayArea->da_CurrentImageBox ;
      MyDisplayArea->da_CurrentImageBox = NULL ;
      SetAttrs( MyDisplayArea->da_AreaObject,
        MUIA_DisplayImage, NULL,
        TAG_END ) ;
    }
    
    if( NULL != PreviousImageBox )
    {  /* delete the previous display image */
      PreviousImageBox->ib_UseCounter-- ;
      DeleteImageBox( MyContext, PreviousImageBox ) ;
    }
  }
  else
  {  /* requirements not ok */
  }
}


/*
** report, if this image box is still in use by the display area
*/
LONG CheckDisplayArea( struct DisplayArea *MyDisplayArea )
{
  LONG Result ;
  
  Result = 0 ;
  
  if( NULL != MyDisplayArea )
  {  /* display area ok */
    if( NULL != MyDisplayArea->da_CurrentImageBox )
    {  /* we are currently busy with some image box */
      Result = 1 ;
    }
    else
    {  /* we don't have this box in use */
    }
  }
  else
  {  /* display area not ok */
    Result = -1 ;
  }
  
  return( Result ) ;
}


/*
** get currently displayed image box
*/
struct ImageBox *GetDisplayImage( struct DisplayArea *MyDisplayArea )
{
  struct ImageBox *Result ;
  
  Result = NULL ;
  
  if( 0 < CheckDisplayArea( MyDisplayArea ) )
  {  /* there is a current image box */
    Result = MyDisplayArea->da_CurrentImageBox ;
  }
  
  return( Result ) ;
}
