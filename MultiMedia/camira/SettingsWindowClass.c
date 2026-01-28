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
** SettingsWindowClass.c
*/


#include "SettingsWindowClass.h"
#include "Localization.h"
#include "cAMIraLib.h"
#include "Log.h"
#include "ab_stdio.h"
#include "ab_stdlib.h"
#include <libraries/iffparse.h>
#include <libraries/asl.h>
#include <proto/utility.h>
#include <proto/dos.h>
#include <proto/muimaster.h>
#include <clib/alib_protos.h>

#include <string.h>


/*
** gets called when popup window was created
*/
static void REGFUNC PopobjectWindowHook( REG( a0, struct Hook *MyHook ),
                                         REG( a2, APTR MyObject ),
                                         REG( a1, APTR MyWindow ) )
{
  SetAttrs( MyWindow, MUIA_Window_DefaultObject, MyObject ) ;
}


/*
** gets called when specifier popup closed
*/
static LONG REGFUNC InsertSpecifierHook( REG( a0, struct Hook *MyHook ),
                                         REG( a2, APTR MyListview ),
                                         REG( a1, APTR MyString ) )
{
  struct Context *MyContext ;
  struct SettingsWindowData *MyData ;
  struct ExecBase *SysBase ;
  STRPTR MyEntry ;

  MyData = MyHook->h_Data ;
  MyContext = MyData->swd_Context ;
  SysBase = MyContext->c_SysBase ;
  
  DoMethod( MyListview, MUIM_List_GetEntry, MUIV_List_GetEntry_Active, &MyEntry );
  if( ( NULL != MyEntry ) && ( ':' == MyEntry[ 2 ] ) )
  {  /* there is an active entry */
    MyEntry[ 2 ] = '\0' ;
    MagicStringInsert( MyContext->c_Magic, MyString, MyEntry ) ;
    MyEntry[ 2 ] = ':' ;
    SetAttrs( MyListview, MUIA_List_Active, MUIV_List_Active_Off, TAG_END ) ;
  }
  
  return( TRUE ) ;
}


/*
** create some objects
*/
static APTR CreateCategoryListview( struct SettingsWindowData *MyData )
{
  struct Context *MyContext ;
  struct Library *MUIMasterBase ;
  APTR CategoryListview ;

  MyContext = MyData->swd_Context ;
  MUIMasterBase = MyContext->c_MUIMasterBase ;

  MyData->swd_CategoryStrings[ VIDEOHARDWARE_CATEGORY ] = GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_SOURCE ) ;
  MyData->swd_CategoryStrings[ STAMP_CATEGORY ] = GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_STAMP ) ;
  MyData->swd_CategoryStrings[ DISPLAY_CATEGORY ] = GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_DISPLAY ) ;
  MyData->swd_CategoryStrings[ SCHEDULER_CATEGORY ] = GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_SCHEDULER ) ;
  MyData->swd_CategoryStrings[ CAPTURE_CATEGORY ] = GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_CAPTURE ) ;
  MyData->swd_CategoryStrings[ UPLOAD_CATEGORY ] = GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_UPLOAD ) ;
  MyData->swd_CategoryStrings[ LOG_CATEGORY ] = GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_LOG ) ;
  MyData->swd_CategoryStrings[ NUM_CATEGORIES ] = NULL ;

  MyData->swd_CategoryList = MUI_NewObject( MUIC_List,
    MUIA_List_SourceArray, ( IPTR )( MyData->swd_CategoryStrings ),
    MUIA_List_Active, ( IPTR )MUIV_List_Active_Top,
    MUIA_List_AdjustWidth, TRUE,
    MUIA_Frame, MUIV_Frame_InputList,
    MUIA_Background, MUII_ListBack,
    MUIA_Font, ( IPTR )MUIV_Font_List,
    MUIA_CycleChain, FALSE,
    TAG_END ) ;
  CategoryListview = MUI_NewObject( MUIC_Listview,
    MUIA_Listview_List, ( IPTR )( MyData->swd_CategoryList ),
    MUIA_Listview_Input, TRUE,
    MUIA_ShortHelp, ( IPTR )GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_CATEGORYLIST_HELP ),
    MUIA_CycleChain, TRUE,
    TAG_END ) ;
  
  return( CategoryListview ) ;
}


/*
** create some objects
*/
static APTR CreateVideohardwarePage( struct SettingsWindowData *MyData )
{
  struct Context *MyContext ;
  struct Library *MUIMasterBase ;
  APTR VhiDriverLabel ;
  APTR VhiDriverButton ;
  APTR VhiInfoLabel ;
  APTR VhiInputLabel ;
  APTR VhiColorLabel, VhiColorCheckmarkGroup ;
  APTR VhiVideoFormatLabel ;
  APTR VhiX1Label ;
  APTR VhiY1Label ;
  APTR VhiX2Label ;
  APTR VhiY2Label ;
  APTR VhiWidthLabel ;
  APTR VhiHeightLabel ;
  APTR VideohardwarePage ;

  MyContext = MyData->swd_Context ;
  MUIMasterBase = MyContext->c_MUIMasterBase ;

  VhiDriverLabel = MagicLabel( MyContext->c_Magic,
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIDRIVER ),
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIDRIVER_KEY ) ) ;
  MyData->swd_VhiDriverString = MagicString( MyContext->c_Magic,
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIDRIVER_KEY ),
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIDRIVER_HELP ) ) ;
  VhiDriverButton = MagicImage( MyContext->c_Magic, MUII_PopFile, 0 ) ;
  MyData->swd_VhiDriverPopasl = MUI_NewObject( MUIC_Popasl,
    MUIA_Popstring_String, ( IPTR )( MyData->swd_VhiDriverString ),
    MUIA_Popstring_Button, ( IPTR )( VhiDriverButton ) ,
    MUIA_Popasl_Type, ASL_FileRequest,
    MUIA_CycleChain, FALSE,
    TAG_END ) ;

  VhiInfoLabel = MagicLabel( MyContext->c_Magic,
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIINFO ),
    0 ) ;
  snprintf( MyData->swd_VhiInfo, SWD_VHIINFO_LENGHT,
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIINFO_EMPTY ) ) ;
  MyData->swd_VhiInfoText = MagicText( MyContext->c_Magic,
    MyData->swd_VhiInfo ) ;

  VhiInputLabel = MagicLabel( MyContext->c_Magic,
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIINPUT ),
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIINPUT_KEY ) ) ;
  MyData->swd_VhiInputCycleGroup = MUI_NewObject( MUIC_Group,
    TAG_END ) ;

  VhiVideoFormatLabel = MagicLabel( MyContext->c_Magic,
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIVIDEOFORMAT ),
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIVIDEOFORMAT_KEY ) ) ;
  MyData->swd_VhiVideoFormatCycleGroup = MUI_NewObject( MUIC_Group,
    TAG_END ) ;

  VhiColorLabel = MagicLabel( MyContext->c_Magic,
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHICOLOR ),
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHICOLOR_KEY ) ) ;
  MyData->swd_VhiColorCheckmark = MagicCheckmark( MyContext->c_Magic,
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHICOLOR_KEY ),
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHICOLOR_HELP ) ) ;
  VhiColorCheckmarkGroup = MUI_NewObject( MUIC_Group,
    MUIA_Group_Horiz, TRUE,
    MUIA_Group_Child, ( IPTR )( MyData->swd_VhiColorCheckmark ),
    MUIA_Group_Child, ( IPTR )( MagicSpace( MyContext->c_Magic ) ),
    TAG_END ) ;

  VhiX1Label = MagicLabel( MyContext->c_Magic,
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIX1 ),
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIX1_KEY ) ) ;
  MyData->swd_VhiX1Slider = MagicSlider( MyContext->c_Magic,
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIX1_KEY ),
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIX1_HELP ) ) ;
  VhiY1Label = MagicLabel( MyContext->c_Magic,
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIY1 ),
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIY1_KEY ) ) ;
  MyData->swd_VhiY1Slider = MagicSlider( MyContext->c_Magic,
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIY1_KEY ),
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIY1_HELP ) ) ;
  VhiX2Label = MagicLabel( MyContext->c_Magic,
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIX2 ),
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIX2_KEY ) ) ;
  MyData->swd_VhiX2Slider = MagicSlider( MyContext->c_Magic,
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIX2_KEY ),
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIX2_HELP ) ) ;
  VhiY2Label = MagicLabel( MyContext->c_Magic,
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIY2 ),
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIY2_KEY ) ) ;
  MyData->swd_VhiY2Slider = MagicSlider( MyContext->c_Magic,
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIY2_KEY ),
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIY2_HELP ) ) ;

  VhiWidthLabel = MagicLabel( MyContext->c_Magic,
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIWIDTH ),
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIWIDTH_KEY ) ) ;
  MyData->swd_VhiWidthInteger = MagicInteger( MyContext->c_Magic,
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIWIDTH_KEY ),
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIWIDTH_HELP ) ) ;
  VhiHeightLabel = MagicLabel( MyContext->c_Magic,
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIHEIGHT ),
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIHEIGHT_KEY ) ) ;
  MyData->swd_VhiHeightInteger = MagicInteger( MyContext->c_Magic,
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIHEIGHT_KEY ),
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIHEIGHT_HELP ) ) ;

  MyData->swd_VideohardwareGroup = MUI_NewObject( MUIC_Group,
    MUIA_Group_Columns, 2,
    MUIA_Group_Child, ( IPTR )( VhiDriverLabel ),
    MUIA_Group_Child, ( IPTR )( MyData->swd_VhiDriverPopasl ),
    MUIA_Group_Child, ( IPTR )( VhiInfoLabel ),
    MUIA_Group_Child, ( IPTR )( MyData->swd_VhiInfoText ),
    MUIA_Group_Child, ( IPTR )( VhiInputLabel ),
    MUIA_Group_Child, ( IPTR )( MyData->swd_VhiInputCycleGroup ),
    MUIA_Group_Child, ( IPTR )( VhiVideoFormatLabel ),
    MUIA_Group_Child, ( IPTR )( MyData->swd_VhiVideoFormatCycleGroup ),
    MUIA_Group_Child, ( IPTR )( VhiColorLabel ),
    MUIA_Group_Child, ( IPTR )( VhiColorCheckmarkGroup ),
    MUIA_Group_Child, ( IPTR )( VhiX1Label ),
    MUIA_Group_Child, ( IPTR )( MyData->swd_VhiX1Slider ),
    MUIA_Group_Child, ( IPTR )( VhiY1Label ),
    MUIA_Group_Child, ( IPTR )( MyData->swd_VhiY1Slider ),
    MUIA_Group_Child, ( IPTR )( VhiX2Label ),
    MUIA_Group_Child, ( IPTR )( MyData->swd_VhiX2Slider ),
    MUIA_Group_Child, ( IPTR )( VhiY2Label ),
    MUIA_Group_Child, ( IPTR )( MyData->swd_VhiY2Slider ),
    MUIA_Group_Child, ( IPTR )( VhiWidthLabel ),
    MUIA_Group_Child, ( IPTR )( MyData->swd_VhiWidthInteger ),
    MUIA_Group_Child, ( IPTR )( VhiHeightLabel ),
    MUIA_Group_Child, ( IPTR )( MyData->swd_VhiHeightInteger ),
    TAG_END ) ;

  VideohardwarePage = MUI_NewObject( MUIC_Group,
    MUIA_Group_Child, ( IPTR )( MagicSpace( MyContext->c_Magic ) ),
    MUIA_Group_Child, ( IPTR )( MyData->swd_VideohardwareGroup ),
    MUIA_Group_Child, ( IPTR )( MagicSpace( MyContext->c_Magic ) ),
    MUIA_Group_Horiz, FALSE,
    TAG_END ) ;

  DoMethod( MyData->swd_VhiDriverString, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
    MyData->swd_WindowObject, 3, MUIM_Set, MUIA_VhiDriver, MUIV_TriggerValue ) ;
  DoMethod( MyData->swd_VhiColorCheckmark, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
    MyData->swd_WindowObject, 3, MUIM_Set, MUIA_VhiColorMode, MUIV_TriggerValue ) ;
  DoMethod( MyData->swd_VhiX1Slider, MUIM_Notify, MUIA_Slider_Level, MUIV_EveryTime,
    MyData->swd_VhiX2Slider, 3, MUIM_Set, MUIA_Slider_Min, MUIV_TriggerValue ) ;
  DoMethod( MyData->swd_VhiY1Slider, MUIM_Notify, MUIA_Slider_Level, MUIV_EveryTime,
    MyData->swd_VhiY2Slider, 3, MUIM_Set, MUIA_Slider_Min, MUIV_TriggerValue ) ;
  DoMethod( MyData->swd_VhiX2Slider, MUIM_Notify, MUIA_Slider_Level, MUIV_EveryTime,
    MyData->swd_VhiX1Slider, 3, MUIM_Set, MUIA_Slider_Max, MUIV_TriggerValue ) ;
  DoMethod( MyData->swd_VhiY2Slider, MUIM_Notify, MUIA_Slider_Level, MUIV_EveryTime,
    MyData->swd_VhiY1Slider, 3, MUIM_Set, MUIA_Slider_Max, MUIV_TriggerValue ) ;

  return( VideohardwarePage ) ;
}


/*
** update cropping and scaling elements on videohardware page
*/
static void UpdateCroppingAndScaling( struct SettingsWindowData *MyData )
{
  struct Context *MyContext ;
  BOOL CroppingDisabled, ScalingDisabled ;
  LONG X1SliderLevel, Y1SliderLevel ;
  LONG X2SliderLevel, Y2SliderLevel ;
  LONG WidthInteger, HeightInteger ;
  ULONG UpdateRequired ;
  LONG SettingValue ;

  MyContext = MyData->swd_Context ;
  CroppingDisabled = ScalingDisabled = TRUE ;

  if( 0 <= CheckVideoHardware( MyData->swd_VideoHardware ) )
  {  /* video hardware awailable */
    UpdateRequired = 0 ;
    GetAttr( MUIA_Selected, MyData->swd_VhiColorCheckmark, &SettingValue ) ;
    UpdateRequired += SelectVhiColorMode( MyData->swd_VideoHardware, ( IPTR )SettingValue ) ;
    GetAttr( MUIA_Cycle_Active, MyData->swd_VhiInputCycle, &SettingValue ) ;
    UpdateRequired += SelectVhiInput( MyData->swd_VideoHardware, ( IPTR )SettingValue ) ;
    GetAttr( MUIA_Cycle_Active, MyData->swd_VhiVideoFormatCycle, &SettingValue ) ;
    UpdateRequired += SelectVhiVideoFormat( MyData->swd_VideoHardware, ( IPTR )SettingValue ) ;
    if( UpdateRequired )
    {  /* dynamic values, like dimensions, can be different now */
      DynamicQueryVideoHardware( MyData->swd_VideoHardware ) ;
    }

    if( !( MyData->swd_VideoHardware->vh_MaximumSize.fixed ) )
    {  /* cropping possible */
      CroppingDisabled = FALSE ;
      X1SliderLevel = GetSetting( MyContext->c_Settings, VhiX1 ) ;
      Y1SliderLevel = GetSetting( MyContext->c_Settings, VhiY1 ) ;
      X2SliderLevel = GetSetting( MyContext->c_Settings, VhiX2 ) ;
      Y2SliderLevel = GetSetting( MyContext->c_Settings, VhiY2 ) ;
      if( 0 > X1SliderLevel )
      {  /* use defualt value */
        X1SliderLevel = 0 ;
      }
      if( 0 > Y1SliderLevel )
      {  /* use defualt value */
        Y1SliderLevel = 0 ;
      }
      if( 0 > X2SliderLevel )
      {  /* use defualt value */
        X2SliderLevel = ( MyData->swd_VideoHardware->vh_MaximumSize.max_width - 1 )  ;
      }
      if( 0 > Y2SliderLevel )
      {  /* use defualt value */
        Y1SliderLevel = ( MyData->swd_VideoHardware->vh_MaximumSize.max_height - 1 ) ;
      }
    }
    else
    {  /* cropping not possible */
      X1SliderLevel = 0 ;
      Y1SliderLevel = 0 ;
      X2SliderLevel = ( MyData->swd_VideoHardware->vh_MaximumSize.max_width - 1 ) ;
      Y2SliderLevel = ( MyData->swd_VideoHardware->vh_MaximumSize.max_height - 1 ) ;
    }
    SetAttrs( MyData->swd_VhiX1Slider,
      MUIA_Slider_Min, 0,
      MUIA_Slider_Max, ( MyData->swd_VideoHardware->vh_MaximumSize.max_width - 1 ),
      MUIA_Slider_Level, X1SliderLevel,
      TAG_END ) ;
    SetAttrs( MyData->swd_VhiY1Slider,
      MUIA_Slider_Min, 0,
      MUIA_Slider_Max, ( MyData->swd_VideoHardware->vh_MaximumSize.max_height - 1 ),
      MUIA_Slider_Level, Y1SliderLevel,
      TAG_END ) ;
    SetAttrs( MyData->swd_VhiX2Slider,
      MUIA_Slider_Min, 0,
      MUIA_Slider_Max, ( MyData->swd_VideoHardware->vh_MaximumSize.max_width - 1 ),
      MUIA_Slider_Level, X2SliderLevel,
      TAG_END ) ;
    SetAttrs( MyData->swd_VhiY2Slider,
      MUIA_Slider_Min, 0,
      MUIA_Slider_Max, ( MyData->swd_VideoHardware->vh_MaximumSize.max_height - 1 ),
      MUIA_Slider_Level, Y2SliderLevel,
      TAG_END ) ;

    if( ( MyData->swd_VideoHardware->vh_MaximumSize.scalable ) )
    {  /* scaling possible, use setting values */
      ScalingDisabled = FALSE ;
      WidthInteger = GetSetting( MyContext->c_Settings, VhiWidth ) ;
      HeightInteger = GetSetting( MyContext->c_Settings, VhiHeight ) ;
    }
    else
    {  /* no scaling, use default values */
      WidthInteger = 0 ;
      HeightInteger = 0 ;
    }
    SetAttrs( MyData->swd_VhiWidthInteger,
      MUIA_String_Integer, WidthInteger,
      TAG_END ) ;
    SetAttrs( MyData->swd_VhiHeightInteger,
      MUIA_String_Integer, HeightInteger,
      TAG_END ) ;
  }
  SetAttrs( MyData->swd_VhiX1Slider,
    MUIA_Disabled, CroppingDisabled,
    TAG_END ) ;
  SetAttrs( MyData->swd_VhiY1Slider,
    MUIA_Disabled, CroppingDisabled,
    TAG_END ) ;
  SetAttrs( MyData->swd_VhiX2Slider,
    MUIA_Disabled, CroppingDisabled,
    TAG_END ) ;
  SetAttrs( MyData->swd_VhiY2Slider,
    MUIA_Disabled, CroppingDisabled,
    TAG_END ) ;
  SetAttrs( MyData->swd_VhiWidthInteger,
    MUIA_Disabled, ScalingDisabled,
    TAG_END ) ;
  SetAttrs( MyData->swd_VhiHeightInteger,
    MUIA_Disabled, ScalingDisabled,
    TAG_END ) ;
}


/*
** adjust video hardware page according to vhi driver string value
*/
static void VhiDriverUpdate( struct SettingsWindowData *MyData, STRPTR NewVhiDriver )
{
  struct Context *MyContext ;
  struct Library *MUIMasterBase ;
  struct DosLibrary *DOSBase ;
  ULONG ColorCheckmarkDisabled ;

  MyContext = MyData->swd_Context ;
  MUIMasterBase = MyContext->c_MUIMasterBase ;
  DOSBase = MyContext->c_DOSBase ;

  LogText( MyContext->c_Log, DEBUG_LEVEL, "vhi driver update: %s", NewVhiDriver ) ;
  if( NULL != MyData->swd_VideoHardware )
  {  /* check, if we still change settings this driver */
    if( 0 != strcmp( FilePart( MyData->swd_VideoHardware->vh_CurrentVhiDriver ), FilePart( NewVhiDriver ) ) )
    {  /* no, we no longer change settings this driver */
      if( MyData->swd_VideoHardware == MyContext->c_VideoHardware )
      {  /* we no longer change settings for the current driver */
        MyData->swd_VideoHardware = NULL ;
      }
      else
      {  /* we no longer change settings for this temporary driver */
        LogText( MyContext->c_Log, DEBUG_LEVEL, "close temp vhi driver: %s", MyData->swd_VideoHardware->vh_CurrentVhiDriver ) ;
        DeleteVideoHardware( MyData->swd_VideoHardware ) ;
        MyData->swd_VideoHardware = NULL ;
      }
    }
    else
    {  /* yes, we still change settings for this driver */
    }
  }

  if( NULL == MyData->swd_VideoHardware )
  {  /* check, for what driver we want to change settings now */
    if( ( NULL != MyContext->c_VideoHardware ) &&
        ( NULL != MyContext->c_VideoHardware->vh_VhiHandle ) &&
        ( 0 == strcmp( FilePart( MyContext->c_VideoHardware->vh_CurrentVhiDriver ), FilePart( NewVhiDriver ) ) ) )
    {  /* we now change settings for the current driver */
      MyData->swd_VideoHardware = MyContext->c_VideoHardware ;
    }
    else
    {  /* we use some other temporary driver */
      MyData->swd_VideoHardware = CreateVideoHardware( MyContext ) ;
      if( NULL != MyData->swd_VideoHardware )
      {  /* video hardware context ok */
        strcpy( MyData->swd_VideoHardware->vh_CurrentVhiDriver, NewVhiDriver ) ;
        LogText( MyContext->c_Log, DEBUG_LEVEL, "open temp vhi driver: %s", MyData->swd_VideoHardware->vh_CurrentVhiDriver ) ;
        OpenVideoHardware( MyData->swd_VideoHardware ) ;
        if( NULL != MyData->swd_VideoHardware->vh_VhiHandle )
        {  /* we could open the temporary driver */
          DynamicQueryVideoHardware( MyData->swd_VideoHardware ) ;
        }
        else
        {  /* we could not open the temporary driver */
          DeleteVideoHardware( MyData->swd_VideoHardware ) ;
          MyData->swd_VideoHardware = NULL ;
        }
      }
    }
  }

  if( ( NULL != MyData->swd_VhiInputCycle ) && DoMethod( MyData->swd_VhiInputCycleGroup, MUIM_Group_InitChange ) )
  {  /* remove the old input cycle */
    DoMethod( MyData->swd_VhiInputCycle, MUIM_KillNotify, MUIA_Cycle_Active ) ;
    DoMethod( MyData->swd_VhiInputCycleGroup, OM_REMMEMBER, MyData->swd_VhiInputCycle ) ;
    MUI_DisposeObject( MyData->swd_VhiInputCycle ) ;
    MyData->swd_VhiInputCycle = NULL ;
    DoMethod( MyData->swd_VhiInputCycleGroup, MUIM_Group_ExitChange ) ;
  }

  if( ( NULL != MyData->swd_VhiVideoFormatCycle ) && DoMethod( MyData->swd_VhiVideoFormatCycleGroup, MUIM_Group_InitChange ) )
  {  /* remove the old video format cycle */
    DoMethod( MyData->swd_VhiVideoFormatCycle, MUIM_KillNotify, MUIA_Cycle_Active ) ;
    DoMethod( MyData->swd_VhiVideoFormatCycleGroup, OM_REMMEMBER, MyData->swd_VhiVideoFormatCycle ) ;
    MUI_DisposeObject( MyData->swd_VhiVideoFormatCycle ) ;
    MyData->swd_VhiVideoFormatCycle = NULL ;
    DoMethod( MyData->swd_VhiVideoFormatCycleGroup, MUIM_Group_ExitChange ) ;
  }

  if( ( NULL != MyData->swd_VideoHardware ) )
  {
    /* create the input cycle with input names */
    MyData->swd_VhiInputCycle = MagicCycle( MyContext->c_Magic,
      MyData->swd_VideoHardware->vh_NameOfInput,
      *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIINPUT_KEY ),
      GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIINPUT_HELP ) ) ;
    SetAttrs( MyData->swd_VhiInputCycle,
      MUIA_Cycle_Active, MyData->swd_VideoHardware->vh_CurrentVhiInput,
      TAG_END ) ;
    /* fill in the driver information string */
    snprintf( MyData->swd_VhiInfo, SWD_VHIINFO_LENGHT,
      GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIINFO_TEMPLATE ),
      MyData->swd_VideoHardware->vh_CardName,
      MyData->swd_VideoHardware->vh_CardVersion,
      MyData->swd_VideoHardware->vh_CardRevision,
      MyData->swd_VideoHardware->vh_CardManufacturer,
      FilePart( MyData->swd_VideoHardware->vh_CurrentVhiDriver ),
      MyData->swd_VideoHardware->vh_CardDriverVersion,
      MyData->swd_VideoHardware->vh_CardDriverRevision,
      MyData->swd_VideoHardware->vh_CardDriverAuthor ) ;
    /* enable color mode checkmark */
    SetAttrs( MyData->swd_VhiColorCheckmark,
      MUIA_NoNotify, TRUE,
      MUIA_Selected, MyData->swd_VideoHardware->vh_CurrentVhiColorMode,
      MUIA_Disabled, ( ( VHI_MODE_COLOR | VHI_MODE_GRAYSCALE ) != ( ( VHI_MODE_COLOR | VHI_MODE_GRAYSCALE ) & MyData->swd_VideoHardware->vh_SupportedColorModes ) ),
      TAG_END ) ;
    /* create the video format cycle with video format names */
    MyData->swd_VhiVideoFormatCycle = MagicCycle( MyContext->c_Magic,
      MyData->swd_VideoHardware->vh_NameOfVideoFormat,
      *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIVIDEOFORMAT_KEY ),
      GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIVIDEOFORMAT_HELP ) ) ;
    SetAttrs( MyData->swd_VhiVideoFormatCycle,
      MUIA_Cycle_Active, MyData->swd_VideoHardware->vh_CurrentVhiVideoFormat,
      TAG_END ) ;
  }
  else
  {
    /* create a disabled input cycle */
    MyData->swd_VhiInputCycle = MagicCycle( MyContext->c_Magic,
      NULL,
      *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIINPUT_KEY ),
      GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIINPUT_HELP ) ) ;
    /* fill in an empty information string */
    snprintf( MyData->swd_VhiInfo, SWD_VHIINFO_LENGHT,
      GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIINFO_EMPTY ) ) ;
    /* disable color mode checkmark */
    SetAttrs( MyData->swd_VhiColorCheckmark,
      MUIA_Disabled, ColorCheckmarkDisabled,
      TAG_END ) ;
    /* create a disabled video format cycle */
    MyData->swd_VhiVideoFormatCycle = MagicCycle( MyContext->c_Magic,
      NULL,
      *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIVIDEOFORMAT_KEY ),
      GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_VHIVIDEOFORMAT_HELP ) ) ;
  }

  if( ( NULL != MyData->swd_VhiInputCycle ) && DoMethod( MyData->swd_VhiInputCycleGroup, MUIM_Group_InitChange ) )
  {  /* we can change the input cycle object */
    DoMethod( MyData->swd_VhiInputCycleGroup, OM_ADDMEMBER, MyData->swd_VhiInputCycle ) ;
    DoMethod( MyData->swd_VhiInputCycle, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
      MyData->swd_WindowObject, 3, MUIM_Set, MUIA_VhiInput, MUIV_TriggerValue ) ;
    DoMethod( MyData->swd_VhiInputCycleGroup, MUIM_Group_ExitChange ) ;
  }

  if( ( NULL != MyData->swd_VhiInputCycle ) && DoMethod( MyData->swd_VhiVideoFormatCycleGroup, MUIM_Group_InitChange ) )
  {  /* we can change the video format cycle object */
    DoMethod( MyData->swd_VhiVideoFormatCycleGroup, OM_ADDMEMBER, MyData->swd_VhiVideoFormatCycle ) ;
    DoMethod( MyData->swd_VhiVideoFormatCycle, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
      MyData->swd_WindowObject, 3, MUIM_Set, MUIA_VhiVideoFormat, MUIV_TriggerValue ) ;
    DoMethod( MyData->swd_VhiVideoFormatCycleGroup, MUIM_Group_ExitChange ) ;
  }

  /* show new information string */
  SetAttrs( MyData->swd_VhiInfoText,
    MUIA_Text_Contents, MyData->swd_VhiInfo,
    TAG_END ) ;

  UpdateCroppingAndScaling( MyData ) ;
}



/*
** adjust video hardware page according to vhi input selection
*/
static void VhiInputUpdate( struct SettingsWindowData *MyData, ULONG NewVhiInput )
{
  struct Context *MyContext ;

  MyContext = MyData->swd_Context ;

  LogText( MyContext->c_Log, DEBUG_LEVEL, "vhi input update: %ld", NewVhiInput ) ;
  UpdateCroppingAndScaling( MyData ) ;
  if( MyData->swd_VideoHardware == MyContext->c_VideoHardware )
  {  /* if we configure the current video hardware, restore current settings */
    ConfigureVideoHardware( MyContext->c_VideoHardware ) ;
  }
}


/*
** adjust video hardware page according to vhi color mode selection
*/
static void VhiColorModeUpdate( struct SettingsWindowData *MyData,  ULONG NewVhiColorMode )
{
  struct Context *MyContext ;

  MyContext = MyData->swd_Context ;

  LogText( MyContext->c_Log, DEBUG_LEVEL, "vhi color mode update: %ld", NewVhiColorMode ) ;
  UpdateCroppingAndScaling( MyData ) ;
  if( MyData->swd_VideoHardware == MyContext->c_VideoHardware )
  {  /* if we configure the current video hardware, restore current settings */
    ConfigureVideoHardware( MyContext->c_VideoHardware ) ;
  }
}


/*
** adjust video hardware page according to vhi video format selection
*/
static void VhiVideoFormatUpdate( struct SettingsWindowData *MyData, ULONG NewVhiVideoFormat )
{
  struct Context *MyContext ;

  MyContext = MyData->swd_Context ;

  LogText( MyContext->c_Log, DEBUG_LEVEL, "vhi video format update: %ld", NewVhiVideoFormat ) ;
  UpdateCroppingAndScaling( MyData ) ;
  if( MyData->swd_VideoHardware == MyContext->c_VideoHardware )
  {  /* if we configure the current video hardware, restore current settings */
    ConfigureVideoHardware( MyContext->c_VideoHardware ) ;
  }
}



/*
** fill videohardware page with setting values
*/
static void FillVideohardwarePageValues( struct SettingsWindowData *MyData )
{
  struct Context *MyContext ;
  struct Library *MUIMasterBase ;
  IPTR SettingValue ;

  MyContext = MyData->swd_Context ;
  MUIMasterBase = MyContext->c_MUIMasterBase ;

  SettingValue = GetSetting( MyContext->c_Settings, VhiDriver ) ;
  SetAttrs( MyData->swd_VhiDriverString,
    MUIA_NoNotify, TRUE,
    MUIA_String_Contents, SettingValue,
    TAG_END ) ;
  VhiDriverUpdate( MyData, ( STRPTR )SettingValue ) ;
}


/*
** use videohardware page setting values
*/
static void UseVideohardwarePageValues( struct SettingsWindowData *MyData )
{
  struct Context *MyContext ;
  LONG SettingValue ;

  MyContext = MyData->swd_Context ;

  GetAttr( MUIA_String_Contents, MyData->swd_VhiDriverString, &SettingValue ) ;
  SetSetting( MyContext->c_Settings, VhiDriver, SettingValue ) ;
  GetAttr( MUIA_Cycle_Active, MyData->swd_VhiInputCycle, &SettingValue ) ;
  SetSetting( MyContext->c_Settings, VhiInput, SettingValue ) ;
  GetAttr( MUIA_Cycle_Active, MyData->swd_VhiVideoFormatCycle, &SettingValue ) ;
  SetSetting( MyContext->c_Settings, VhiVideoformat, SettingValue ) ;
  GetAttr( MUIA_Selected, MyData->swd_VhiColorCheckmark, &SettingValue ) ;
  SetSetting( MyContext->c_Settings, VhiColormode, SettingValue ) ;
  GetAttr( MUIA_Slider_Level, MyData->swd_VhiX1Slider, &SettingValue ) ;
  SetSetting( MyContext->c_Settings, VhiX1, SettingValue ) ;
  GetAttr( MUIA_Slider_Level, MyData->swd_VhiY1Slider, &SettingValue ) ;
  SetSetting( MyContext->c_Settings, VhiY1, SettingValue ) ;
  GetAttr( MUIA_Slider_Level, MyData->swd_VhiX2Slider, &SettingValue ) ;
  SetSetting( MyContext->c_Settings, VhiX2, SettingValue ) ;
  GetAttr( MUIA_Slider_Level, MyData->swd_VhiY2Slider, &SettingValue ) ;
  SetSetting( MyContext->c_Settings, VhiY2, SettingValue ) ;
  GetAttr( MUIA_Slider_Level, MyData->swd_VhiWidthInteger, &SettingValue ) ;
  SetSetting( MyContext->c_Settings, VhiWidth, SettingValue ) ;
  GetAttr( MUIA_Slider_Level, MyData->swd_VhiHeightInteger, &SettingValue ) ;
  SetSetting( MyContext->c_Settings, VhiHeight, SettingValue ) ;
}


/*
** create some objects for the stamp page
*/
static APTR CreateStampPage( struct SettingsWindowData *MyData )
{
  struct Context *MyContext ;
  struct Library *MUIMasterBase ;
  APTR StampEnableLabel, StampEnableCheckmarkGroup ;
  APTR StampTextLabel, StampTextButton, StampTextList, StampTextListview, StampTextPopobject ;
  APTR StampFontLabel, StampFontButton, StampFontPopasl ;
  APTR StampXLabel ;
  APTR StampYLabel ;
  APTR StampGroup, StampPage ;

  MyContext = MyData->swd_Context ;
  MUIMasterBase = MyContext->c_MUIMasterBase ;

  StampEnableLabel = MagicLabel( MyContext->c_Magic,
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_STAMPENABLE ),
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_STAMPENABLE_KEY ) ) ;
  MyData->swd_StampEnableCheckmark = MagicCheckmark( MyContext->c_Magic,
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_STAMPENABLE_KEY ),
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_STAMPENABLE_HELP ) ) ;
  StampEnableCheckmarkGroup = MUI_NewObject( MUIC_Group,
    MUIA_Group_Horiz, TRUE,
    MUIA_Group_Child, ( IPTR )( MyData->swd_StampEnableCheckmark ),
    MUIA_Group_Child, ( IPTR )( MagicSpace( MyContext->c_Magic ) ),
    TAG_END ) ;
  StampTextLabel = MagicLabel( MyContext->c_Magic,
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_STAMPTEXT ),
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_STAMPTEXT_KEY ) ) ;
  MyData->swd_StampTextString = MagicString( MyContext->c_Magic,
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_STAMPTEXT_KEY ),
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_STAMPTEXT_HELP ) ) ;
  StampTextButton = MagicImage( MyContext->c_Magic, MUII_PopUp, 0 ) ;
  StampTextList = MUI_NewObject( MUIC_List,
    MUIA_Frame, MUIV_Frame_InputList,
    MUIA_List_AdjustHeight, TRUE,
    TAG_END ) ;
  DoMethod( StampTextList, MUIM_List_InsertSingle, MyData->swd_YearSpecifier, MUIV_List_Insert_Bottom ) ;
  DoMethod( StampTextList, MUIM_List_InsertSingle, MyData->swd_MonthSpecifier, MUIV_List_Insert_Bottom ) ;
  DoMethod( StampTextList, MUIM_List_InsertSingle, MyData->swd_DaySpecifier, MUIV_List_Insert_Bottom ) ;
  DoMethod( StampTextList, MUIM_List_InsertSingle, MyData->swd_HourSpecifier, MUIV_List_Insert_Bottom ) ;
  DoMethod( StampTextList, MUIM_List_InsertSingle, MyData->swd_MinuteSpecifier, MUIV_List_Insert_Bottom ) ;
  DoMethod( StampTextList, MUIM_List_InsertSingle, MyData->swd_SecondSpecifier, MUIV_List_Insert_Bottom ) ;
  StampTextListview = MUI_NewObject( MUIC_Listview,
    MUIA_Listview_List, ( IPTR )( StampTextList ),
    MUIA_Listview_Input, TRUE,
    MUIA_Listview_ScrollerPos, MUIV_Listview_ScrollerPos_None,
    TAG_END ) ;
  StampTextPopobject = MUI_NewObject( MUIC_Popobject,
    MUIA_Popstring_String, ( IPTR )( MyData->swd_StampTextString ),
    MUIA_Popstring_Button, ( IPTR )( StampTextButton ) ,
    MUIA_Popobject_ObjStrHook, ( IPTR )( &MyData->swd_InsertSpecifierHook ),
    MUIA_Popobject_WindowHook, ( IPTR )( &MyData->swd_PopobjectWindowHook ),
    MUIA_Popobject_Object, ( IPTR )StampTextListview,
    MUIA_CycleChain, FALSE,
    TAG_END ) ;
  DoMethod( StampTextListview, MUIM_Notify, MUIA_Listview_DoubleClick, TRUE, StampTextPopobject, 2, MUIM_Popstring_Close, TRUE ) ;
  StampFontLabel = MagicLabel( MyContext->c_Magic,
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_STAMPFONT ),
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_STAMPFONT_KEY ) ) ;
  MyData->swd_StampFontString = MagicString( MyContext->c_Magic,
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_STAMPFONT_KEY ),
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_STAMPFONT_HELP ) ) ;
  StampFontButton = MagicImage( MyContext->c_Magic, MUII_PopUp, 0 ) ;
  StampFontPopasl = MUI_NewObject( MUIC_Popasl,
    MUIA_Popstring_String, ( IPTR )( MyData->swd_StampFontString ),
    MUIA_Popstring_Button, ( IPTR )( StampFontButton ) ,
    MUIA_Popasl_Type, ASL_FontRequest,
    MUIA_CycleChain, FALSE,
    TAG_END ) ;
  StampXLabel = MagicLabel( MyContext->c_Magic,
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_STAMPX ),
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_STAMPX_KEY ) ) ;
  MyData->swd_StampXInteger = MagicInteger( MyContext->c_Magic,
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_STAMPX_KEY ),
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_STAMPX_HELP ) ) ;
  StampYLabel = MagicLabel( MyContext->c_Magic,
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_STAMPY ),
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_STAMPY_KEY ) ) ;
  MyData->swd_StampYInteger = MagicInteger( MyContext->c_Magic,
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_STAMPY_KEY ),
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_STAMPY_HELP ) ) ;
  StampGroup = MUI_NewObject( MUIC_Group,
    MUIA_Group_Columns, 2,
    MUIA_Group_Child, ( IPTR )( StampEnableLabel ),
    MUIA_Group_Child, ( IPTR )( StampEnableCheckmarkGroup ),
    MUIA_Group_Child, ( IPTR )( StampTextLabel ),
    MUIA_Group_Child, ( IPTR )( StampTextPopobject ),
    MUIA_Group_Child, ( IPTR )( StampFontLabel ),
    MUIA_Group_Child, ( IPTR )( StampFontPopasl ),
    MUIA_Group_Child, ( IPTR )( StampXLabel ),
    MUIA_Group_Child, ( IPTR )( MyData->swd_StampXInteger ),
    MUIA_Group_Child, ( IPTR )( StampYLabel ),
    MUIA_Group_Child, ( IPTR )( MyData->swd_StampYInteger ),
    TAG_END ) ;

  StampPage = MUI_NewObject( MUIC_Group,
    MUIA_Group_Horiz, FALSE,
    MUIA_Group_Child, ( IPTR )( MagicSpace( MyContext->c_Magic ) ),
    MUIA_Group_Child, ( IPTR )( StampGroup ),
    MUIA_Group_Child, ( IPTR )( MagicSpace( MyContext->c_Magic ) ),
    TAG_END ) ;

  return( StampPage ) ;
}


/*
** fill stamp page with setting values
*/
static void FillStampPageValues( struct SettingsWindowData *MyData )
{
  struct Context *MyContext ;
  SIPTR SettingValue ;
  LONG DotIndex ;
  UBYTE FontNameSize[ 36 ] ;

  MyContext = MyData->swd_Context ;

  SettingValue = GetSetting( MyContext->c_Settings, StampEnable ) ;
  SetAttrs( MyData->swd_StampEnableCheckmark, 
    MUIA_Selected, SettingValue,
    TAG_END ) ;
  SettingValue = GetSetting( MyContext->c_Settings, StampText ) ;
  SetAttrs( MyData->swd_StampTextString, 
    MUIA_String_Contents, SettingValue,
    TAG_END ) ;
  snprintf( FontNameSize, 36, "%s", ( STRPTR )GetSetting( MyContext->c_Settings, StampFont ) ) ;
  DotIndex = 0 ;
  while( '\0' != FontNameSize[ DotIndex ] )
  {  /* look in the dot of ".font" */
    if( '.' == FontNameSize[ DotIndex ] )
    {  /* found the dot, append size */
      snprintf( &FontNameSize[ DotIndex ], 36 - DotIndex, "/%ld", ( LONG )GetSetting( MyContext->c_Settings, StampFontsize ) ) ;
    }
    DotIndex++ ;
  }
  SetAttrs( MyData->swd_StampFontString, 
    MUIA_String_Contents, ( IPTR )FontNameSize,
    TAG_END ) ;
  SettingValue = GetSetting( MyContext->c_Settings, StampX ) ;
  SetAttrs( MyData->swd_StampXInteger, 
    MUIA_String_Integer, SettingValue,
    TAG_END ) ;
  SettingValue = GetSetting( MyContext->c_Settings, StampY ) ;
  SetAttrs( MyData->swd_StampYInteger, 
    MUIA_String_Integer, SettingValue,
    TAG_END ) ;
}


/*
** use stamp page setting values
*/
static void UseStampPageValues( struct SettingsWindowData *MyData )
{
  struct Context *MyContext ;
  SIPTR SettingValue ;
  LONG SlashIndex ;
  UBYTE FontNameSize[ 36 ] ;

  MyContext = MyData->swd_Context ;

  GetAttr( MUIA_Selected, MyData->swd_StampEnableCheckmark, &SettingValue ) ;
  SetSetting( MyContext->c_Settings, StampEnable, SettingValue ) ;

  GetAttr( MUIA_String_Contents, MyData->swd_StampTextString, &SettingValue ) ;
  SetSetting( MyContext->c_Settings, StampText, SettingValue ) ;

  GetAttr( MUIA_String_Contents, MyData->swd_StampFontString, &SettingValue ) ;
  snprintf( FontNameSize, 36, "%s", ( STRPTR )SettingValue ) ;
  SlashIndex = 0 ;
  while( '\0' != FontNameSize[ SlashIndex ] )
  {  /* look for the slash of the mui font string */
    if( '/' == FontNameSize[ SlashIndex ] )
    {  /* found the slash, set settings */
      SetSetting( MyContext->c_Settings, StampFontsize, atol( &FontNameSize[ SlashIndex + 1 ] ) ) ;
      snprintf( &FontNameSize[ SlashIndex ], 36 - SlashIndex, ".font" ) ;
      SetSetting( MyContext->c_Settings, StampFont, ( IPTR )FontNameSize ) ;
    }
    SlashIndex++ ;
  }
  GetAttr( MUIA_String_Integer, MyData->swd_StampXInteger, &SettingValue ) ;
  SetSetting( MyContext->c_Settings, StampX, SettingValue ) ;
  GetAttr( MUIA_String_Integer, MyData->swd_StampYInteger, &SettingValue ) ;
  SetSetting( MyContext->c_Settings, StampY, SettingValue ) ;
}


/*
** create some objects
*/
static APTR CreateDisplayPage( struct SettingsWindowData *MyData )
{
  struct Context *MyContext ;
  struct Library *MUIMasterBase ;
  APTR DisplayEnableLable, DisplayEnableCheckmarkGroup ;
  APTR DisplayIntervalLable ;
  APTR DisplayZoomLable ;
  APTR DisplayRatioLable ;
  APTR DisplayGroup, DisplayPage ;

  MyContext = MyData->swd_Context ;
  MUIMasterBase = MyContext->c_MUIMasterBase ;

  DisplayEnableLable = MagicLabel( MyContext->c_Magic,
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_DISPLAYENABLE ),
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_DISPLAYENABLE_KEY ) ) ;
  MyData->swd_DisplayEnableCheckmark = MagicCheckmark( MyContext->c_Magic,
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_DISPLAYENABLE_KEY ),
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_DISPLAYENABLE_HELP ) ) ;
  DisplayEnableCheckmarkGroup = MUI_NewObject( MUIC_Group,
    MUIA_Group_Horiz, TRUE,
    MUIA_Group_Child, ( IPTR )( MyData->swd_DisplayEnableCheckmark ),
    MUIA_Group_Child, ( IPTR )( MagicSpace( MyContext->c_Magic ) ),
    TAG_END ) ;

  DisplayIntervalLable = MagicLabel( MyContext->c_Magic,
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_DISPLAYINTERVAL ),
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_DISPLAYINTERVAL_KEY ) ) ;
  MyData->swd_DisplayIntervalInteger = MagicInteger( MyContext->c_Magic,
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_DISPLAYINTERVAL_KEY ),
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_DISPLAYINTERVAL_HELP ) ) ;

  DisplayZoomLable = MagicLabel( MyContext->c_Magic,
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_DISPLAYZOOM ),
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_DISPLAYZOOM_KEY ) ) ;
  MyData->swd_DisplayZoomInteger = MagicInteger( MyContext->c_Magic,
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_DISPLAYZOOM_KEY ),
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_DISPLAYZOOM_HELP ) ) ;

  DisplayRatioLable = MagicLabel( MyContext->c_Magic,
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_DISPLAYRATIO ),
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_DISPLAYRATIO_KEY ) ) ;
  MyData->swd_DisplayRatioCycleNames[ FREE_RATIO ] = 
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_DISPLAYRATIOFREE ) ;
  MyData->swd_DisplayRatioCycleNames[ KEEP_RATIO ] =
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_DISPLAYRATIOKEEP ) ;
  MyData->swd_DisplayRatioCycleNames[ NUM_RATIOS ] = NULL ;
  MyData->swd_DisplayRatioCycle = MagicCycle( MyContext->c_Magic,
    MyData->swd_DisplayRatioCycleNames,
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_DISPLAYRATIO_KEY ),
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_DISPLAYRATIO_HELP ) ) ;

  DisplayGroup = MUI_NewObject( MUIC_Group,
    MUIA_Group_Columns, 2,
    MUIA_Group_Child, ( IPTR )( DisplayEnableLable ),
    MUIA_Group_Child, ( IPTR )( DisplayEnableCheckmarkGroup ),
    MUIA_Group_Child, ( IPTR )( DisplayIntervalLable ),
    MUIA_Group_Child, ( IPTR )( MyData->swd_DisplayIntervalInteger ),
    MUIA_Group_Child, ( IPTR )( DisplayZoomLable ),
    MUIA_Group_Child, ( IPTR )( MyData->swd_DisplayZoomInteger ),
    MUIA_Group_Child, ( IPTR )( DisplayRatioLable ),
    MUIA_Group_Child, ( IPTR )( MyData->swd_DisplayRatioCycle ),
    TAG_END ) ;

  DisplayPage = MUI_NewObject( MUIC_Group,
    MUIA_Group_Horiz, FALSE,
    MUIA_Group_Child, ( IPTR )( MagicSpace( MyContext->c_Magic ) ),
    MUIA_Group_Child, ( IPTR )( DisplayGroup ),
    MUIA_Group_Child, ( IPTR )( MagicSpace( MyContext->c_Magic ) ),
    TAG_END ) ;

  return( DisplayPage ) ;
}


/*
** fill display page with setting values 
*/
static void FillDisplayPageValues( struct SettingsWindowData *MyData )
{
  struct Context *MyContext ;
  SIPTR SettingValue ;

  MyContext = MyData->swd_Context ;

  SettingValue = GetSetting( MyContext->c_Settings, DisplayEnable ) ;
  SetAttrs( MyData->swd_DisplayEnableCheckmark, 
    MUIA_Selected, SettingValue,
    TAG_END ) ;
  SettingValue = GetSetting( MyContext->c_Settings, DisplayInterval ) ;
  SetAttrs( MyData->swd_DisplayIntervalInteger, 
    MUIA_String_Integer, SettingValue,
    TAG_END ) ;
  SettingValue = GetSetting( MyContext->c_Settings, DisplayZoom ) ;
  SetAttrs( MyData->swd_DisplayZoomInteger, 
    MUIA_String_Integer, SettingValue,
    TAG_END ) ;
  SettingValue = GetSetting( MyContext->c_Settings, DisplayRatio ) ;
  SetAttrs( MyData->swd_DisplayRatioCycle, 
    MUIA_Cycle_Active, SettingValue,
    TAG_END ) ;
}


/*
** use display page setting values 
*/
static void UseDisplayPageValues( struct SettingsWindowData *MyData )
{
  struct Context *MyContext ;
  LONG SettingValue ;

  MyContext = MyData->swd_Context ;

  GetAttr( MUIA_Selected, MyData->swd_DisplayEnableCheckmark, &SettingValue ) ;
  SetSetting( MyContext->c_Settings, DisplayEnable, SettingValue ) ;
  GetAttr( MUIA_String_Integer, MyData->swd_DisplayIntervalInteger, &SettingValue ) ;
  SetSetting( MyContext->c_Settings, DisplayInterval, SettingValue ) ;
  GetAttr( MUIA_String_Integer, MyData->swd_DisplayZoomInteger, &SettingValue ) ;
  SetSetting( MyContext->c_Settings, DisplayZoom, SettingValue ) ;
  GetAttr( MUIA_Cycle_Active, MyData->swd_DisplayRatioCycle, &SettingValue ) ;
  SetSetting( MyContext->c_Settings, DisplayRatio, SettingValue ) ;
}


/*
** create scheduler page objects
*/
static APTR CreateSchedulerPage( struct SettingsWindowData *MyData )
{
  struct Context *MyContext ;
  struct Library *MUIMasterBase ;
  APTR SchedulerIntervalLabel ;
  APTR SchedulerGroup, SchedulerPage ;

  MyContext = MyData->swd_Context ;
  MUIMasterBase = MyContext->c_MUIMasterBase ;

  SchedulerIntervalLabel = MagicLabel( MyContext->c_Magic,
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_SCHEDULERINTERVAL ),
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_SCHEDULERINTERVAL_KEY ) ) ;
  MyData->swd_SchedulerIntervalInteger = MagicInteger( MyContext->c_Magic,
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_SCHEDULERINTERVAL_KEY ),
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_SCHEDULERINTERVAL_HELP ) ) ;

  SchedulerGroup = MUI_NewObject( MUIC_Group,
    MUIA_Group_Columns, 2,
    MUIA_Group_Child, ( IPTR )( SchedulerIntervalLabel ),
    MUIA_Group_Child, ( IPTR )( MyData->swd_SchedulerIntervalInteger ),
    TAG_END ) ;

  SchedulerPage = MUI_NewObject( MUIC_Group,
    MUIA_Group_Horiz, FALSE,
    MUIA_Group_Child, ( IPTR )( MagicSpace( MyContext->c_Magic ) ),
    MUIA_Group_Child, ( IPTR )( SchedulerGroup ),
    MUIA_Group_Child, ( IPTR )( MagicSpace( MyContext->c_Magic ) ),
    TAG_END ) ;

  return( SchedulerPage ) ;
}


/*
** fill scheduler page with setting values 
*/
static void FillSchedulerPageValues( struct SettingsWindowData *MyData )
{
  struct Context *MyContext ;
  SIPTR SettingValue ;

  MyContext = MyData->swd_Context ;

  SettingValue = GetSetting( MyContext->c_Settings, SchedulerInterval ) ;
  SetAttrs( MyData->swd_SchedulerIntervalInteger, 
    MUIA_String_Integer, SettingValue,
    TAG_END ) ;
}


/*
** use scheduler page setting values 
*/
static void UseSchedulerPageValues( struct SettingsWindowData *MyData )
{
  struct Context *MyContext ;
  LONG SettingValue ;

  MyContext = MyData->swd_Context ;

  GetAttr( MUIA_String_Integer, MyData->swd_SchedulerIntervalInteger, &SettingValue ) ;
  SetSetting( MyContext->c_Settings, SchedulerInterval, SettingValue ) ;
}


/*
** create capture page objects
*/
static APTR CreateCapturePage( struct SettingsWindowData *MyData )
{
  struct Context *MyContext ;
  struct Library *MUIMasterBase ;
  APTR CaptureEnableLabel, CaptureEnableCheckmarkGroup ;
  APTR CaptureDrawerLabel, CaptureDrawerButton, CaptureDrawerPopasl ;
  APTR CaptureFileLabel, CaptureFileButton, CaptureFileList, CaptureFileListview, CaptureFilePopobject ;
  APTR CaptureFormatLabel ;
  APTR CaptureGroup, CapturePage ;

  MyContext = MyData->swd_Context ;
  MUIMasterBase = MyContext->c_MUIMasterBase ;

  CaptureEnableLabel = MagicLabel( MyContext->c_Magic,
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_CAPTUREENABLE ),
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_CAPTUREENABLE_KEY ) ) ;
  MyData->swd_CaptureEnableCheckmark = MagicCheckmark( MyContext->c_Magic,
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_CAPTUREENABLE_KEY ),
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_CAPTUREENABLE_HELP ) ) ;
  CaptureEnableCheckmarkGroup = MUI_NewObject( MUIC_Group,
    MUIA_Group_Horiz, TRUE,
    MUIA_Group_Child, ( IPTR )( MyData->swd_CaptureEnableCheckmark ),
    MUIA_Group_Child, ( IPTR )( MagicSpace( MyContext->c_Magic ) ),
    TAG_END ) ;
  CaptureDrawerLabel = MagicLabel( MyContext->c_Magic,
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_CAPTUREDRAWER ),
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_CAPTUREDRAWER_KEY ) ) ;
  CaptureDrawerButton = MagicImage( MyContext->c_Magic, MUII_PopDrawer, 0 ) ;
  MyData->swd_CaptureDrawerString = MagicString( MyContext->c_Magic,
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_CAPTUREDRAWER_KEY ),
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_CAPTUREDRAWER_HELP ) ) ;
  CaptureDrawerPopasl = MUI_NewObject( MUIC_Popasl,
    MUIA_Popstring_String, ( IPTR )( MyData->swd_CaptureDrawerString ),
    MUIA_Popstring_Button, ( IPTR )( CaptureDrawerButton ) ,
    MUIA_Popasl_Type, ASL_FileRequest,
    MUIA_CycleChain, FALSE,
    ASLFR_DrawersOnly, TRUE,
    TAG_END ) ;
  CaptureFileLabel = MagicLabel( MyContext->c_Magic,
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_CAPTUREFILE ),
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_CAPTUREFILE_KEY ) ) ;
  MyData->swd_CaptureFileString = MagicString( MyContext->c_Magic,
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_CAPTUREFILE_KEY ),
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_CAPTUREFILE_HELP ) ) ;
  CaptureFileButton = MagicImage( MyContext->c_Magic, MUII_PopUp, 0 ) ;
  CaptureFileList = MUI_NewObject( MUIC_List,
    MUIA_Frame, MUIV_Frame_InputList,
    MUIA_List_AdjustHeight, TRUE,
    TAG_END ) ;
  DoMethod( CaptureFileList, MUIM_List_InsertSingle, MyData->swd_YearSpecifier, MUIV_List_Insert_Bottom ) ;
  DoMethod( CaptureFileList, MUIM_List_InsertSingle, MyData->swd_MonthSpecifier, MUIV_List_Insert_Bottom ) ;
  DoMethod( CaptureFileList, MUIM_List_InsertSingle, MyData->swd_DaySpecifier, MUIV_List_Insert_Bottom ) ;
  DoMethod( CaptureFileList, MUIM_List_InsertSingle, MyData->swd_HourSpecifier, MUIV_List_Insert_Bottom ) ;
  DoMethod( CaptureFileList, MUIM_List_InsertSingle, MyData->swd_MinuteSpecifier, MUIV_List_Insert_Bottom ) ;
  DoMethod( CaptureFileList, MUIM_List_InsertSingle, MyData->swd_SecondSpecifier, MUIV_List_Insert_Bottom ) ;
  DoMethod( CaptureFileList, MUIM_List_InsertSingle, MyData->swd_CounterSpecifier, MUIV_List_Insert_Bottom ) ;
  CaptureFileListview = MUI_NewObject( MUIC_Listview,
    MUIA_Listview_List, ( IPTR )( CaptureFileList ),
    MUIA_Listview_Input, TRUE,
    MUIA_Listview_ScrollerPos, MUIV_Listview_ScrollerPos_None,
    TAG_END ) ;
  CaptureFilePopobject = MUI_NewObject( MUIC_Popobject,
    MUIA_Popstring_String, ( IPTR )( MyData->swd_CaptureFileString ),
    MUIA_Popstring_Button, ( IPTR )( CaptureFileButton ) ,
    MUIA_Popobject_ObjStrHook, ( IPTR )( &MyData->swd_InsertSpecifierHook ),
    MUIA_Popobject_WindowHook, ( IPTR )( &MyData->swd_PopobjectWindowHook ),
    MUIA_Popobject_Object, ( IPTR )CaptureFileListview,
    MUIA_CycleChain, FALSE,
    TAG_END ) ;
  DoMethod( CaptureFileListview, MUIM_Notify, MUIA_Listview_DoubleClick, TRUE, CaptureFilePopobject, 2, MUIM_Popstring_Close, TRUE ) ;

  CaptureFormatLabel = MagicLabel( MyContext->c_Magic,
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_CAPTUREFORMAT ),
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_CAPTUREFORMAT_KEY ) ) ;
  MyData->swd_CaptureFormatCycleNames[ JPG_FORMAT ] = GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_FORMAT_JPG ) ;
  MyData->swd_CaptureFormatCycleNames[ PNG_FORMAT ] = GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_FORMAT_PNG ) ;
  MyData->swd_CaptureFormatCycleNames[ NUM_FORMATS ] = NULL ;
  MyData->swd_CaptureFormatCycle = MagicCycle( MyContext->c_Magic,
    MyData->swd_CaptureFormatCycleNames,
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_CAPTUREFORMAT_KEY ),
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_CAPTUREFORMAT_HELP ) ) ;

  CaptureGroup = MUI_NewObject( MUIC_Group,
    MUIA_Group_Columns, 2,
    MUIA_Group_Child, ( IPTR )( CaptureEnableLabel ),
    MUIA_Group_Child, ( IPTR )( CaptureEnableCheckmarkGroup ),
    MUIA_Group_Child, ( IPTR )( CaptureDrawerLabel ),
    MUIA_Group_Child, ( IPTR )( CaptureDrawerPopasl ),
    MUIA_Group_Child, ( IPTR )( CaptureFileLabel ),
    MUIA_Group_Child, ( IPTR )( CaptureFilePopobject ),
    MUIA_Group_Child, ( IPTR )( CaptureFormatLabel ),
    MUIA_Group_Child, ( IPTR )( MyData->swd_CaptureFormatCycle ),
    TAG_END ) ;

  CapturePage = MUI_NewObject( MUIC_Group,
    MUIA_Group_Horiz, FALSE,
    MUIA_Group_Child, ( IPTR )( MagicSpace( MyContext->c_Magic ) ),
    MUIA_Group_Child, ( IPTR )( CaptureGroup ),
    MUIA_Group_Child, ( IPTR )( MagicSpace( MyContext->c_Magic ) ),
    TAG_END ) ;

  return( CapturePage ) ;
}


/*
** fill capture page with setting values 
*/
static void FillCapturePageValues( struct SettingsWindowData *MyData )
{
  struct Context *MyContext ;
  SIPTR SettingValue ;

  MyContext = MyData->swd_Context ;

  SettingValue = GetSetting( MyContext->c_Settings, CaptureEnable ) ;
  SetAttrs( MyData->swd_CaptureEnableCheckmark, 
    MUIA_Selected, SettingValue,
    TAG_END ) ;
  SettingValue = GetSetting( MyContext->c_Settings, CaptureDrawer ) ;
  SetAttrs( MyData->swd_CaptureDrawerString, 
    MUIA_String_Contents, SettingValue,
    TAG_END ) ;
  SettingValue = GetSetting( MyContext->c_Settings, CaptureFile ) ;
  SetAttrs( MyData->swd_CaptureFileString, 
    MUIA_String_Contents, SettingValue,
    TAG_END ) ;
  SettingValue = GetSetting( MyContext->c_Settings, CaptureFormat ) ;
  SetAttrs( MyData->swd_CaptureFormatCycle, 
    MUIA_Cycle_Active, SettingValue,
    TAG_END ) ;
}


/*
** use capture page setting values 
*/
static void UseCapturePageValues( struct SettingsWindowData *MyData )
{
  struct Context *MyContext ;
  LONG SettingValue ;

  MyContext = MyData->swd_Context ;

  GetAttr( MUIA_Selected, MyData->swd_CaptureEnableCheckmark, &SettingValue ) ;
  SetSetting( MyContext->c_Settings, CaptureEnable, SettingValue ) ;
  GetAttr( MUIA_String_Contents, MyData->swd_CaptureDrawerString, &SettingValue ) ;
  SetSetting( MyContext->c_Settings, CaptureDrawer, SettingValue ) ;
  GetAttr( MUIA_String_Contents, MyData->swd_CaptureFileString, &SettingValue ) ;
  SetSetting( MyContext->c_Settings, CaptureFile, SettingValue ) ;
  GetAttr( MUIA_Cycle_Active, MyData->swd_CaptureFormatCycle, &SettingValue ) ;
  SetSetting( MyContext->c_Settings, CaptureFormat, SettingValue ) ;
}


/*
** create some objects
*/
static APTR CreateUploadPage( struct SettingsWindowData *MyData )
{
  struct Context *MyContext ;
  struct Library *MUIMasterBase ;
  APTR UploadEnableLabel, UploadEnableCheckmarkGroup ;
  APTR UploadServerLabel ;
  APTR UploadPassiveLabel, UploadPassiveCheckmarkGroup ;
  APTR UploadTimeoutLabel ;
  APTR UploadUserLabel ;
  APTR UploadPasswordLabel ;
  APTR UploadDrawerLabel ;
  APTR UploadFileLabel ;
  APTR UploadFormatLabel ;
  APTR UploadGroup, UploadPage ;

  MyContext = MyData->swd_Context ;
  MUIMasterBase = MyContext->c_MUIMasterBase ;

  UploadEnableLabel = MagicLabel( MyContext->c_Magic,
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_UPLOADENABLE ),
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_UPLOADENABLE_KEY ) ) ;
  MyData->swd_UploadEnableCheckmark = MagicCheckmark( MyContext->c_Magic,
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_UPLOADENABLE_KEY ),
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_UPLOADENABLE_HELP ) ) ;
  UploadEnableCheckmarkGroup = MUI_NewObject( MUIC_Group,
    MUIA_Group_Horiz, TRUE,
    MUIA_Group_Child, ( IPTR )( MyData->swd_UploadEnableCheckmark ),
    MUIA_Group_Child, ( IPTR )( MagicSpace( MyContext->c_Magic ) ),
    TAG_END ) ;
  UploadServerLabel = MagicLabel( MyContext->c_Magic,
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_UPLOADSERVER ),
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_UPLOADSERVER_KEY ) ) ;
  MyData->swd_UploadServerString = MagicString( MyContext->c_Magic,
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_UPLOADSERVER_KEY ),
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_UPLOADSERVER_HELP ) ) ;
  UploadPassiveLabel = MagicLabel( MyContext->c_Magic,
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_UPLOADPASSIVE ),
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_UPLOADPASSIVE_KEY ) ) ;
  MyData->swd_UploadPassiveCheckmark = MagicCheckmark( MyContext->c_Magic,
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_UPLOADPASSIVE_KEY ),
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_UPLOADPASSIVE_HELP ) ) ;
  UploadPassiveCheckmarkGroup = MUI_NewObject( MUIC_Group,
    MUIA_Group_Horiz, TRUE,
    MUIA_Group_Child, ( IPTR )( MyData->swd_UploadPassiveCheckmark ),
    MUIA_Group_Child, ( IPTR )( MagicSpace( MyContext->c_Magic ) ),
    TAG_END ) ;
  UploadTimeoutLabel = MagicLabel( MyContext->c_Magic,
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_UPLOADTIMEOUT ),
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_UPLOADTIMEOUT_KEY ) ) ;
  MyData->swd_UploadTimeoutInteger = MagicInteger( MyContext->c_Magic,
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_UPLOADTIMEOUT_KEY ),
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_UPLOADTIMEOUT_HELP ) ) ;
  UploadUserLabel = MagicLabel( MyContext->c_Magic,
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_UPLOADUSER ),
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_UPLOADUSER_KEY ) ) ;
  MyData->swd_UploadUserString = MagicString( MyContext->c_Magic,
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_UPLOADUSER_KEY ),
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_UPLOADUSER_HELP ) ) ;
  UploadPasswordLabel = MagicLabel( MyContext->c_Magic,
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_UPLOADPASSWORD ),
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_UPLOADPASSWORD_KEY ) ) ;
  MyData->swd_UploadPasswordString = MagicSecretString( MyContext->c_Magic,
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_UPLOADPASSWORD_KEY ),
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_UPLOADPASSWORD_HELP ) ) ;
  UploadDrawerLabel = MagicLabel( MyContext->c_Magic,
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_UPLOADDRAWER ),
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_UPLOADDRAWER_KEY ) ) ;
  MyData->swd_UploadDrawerString = MagicString( MyContext->c_Magic,
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_UPLOADDRAWER_KEY ),
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_UPLOADDRAWER_HELP ) ) ;
  UploadFileLabel = MagicLabel( MyContext->c_Magic,
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_UPLOADFILE ),
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_UPLOADFILE_KEY ) ) ;
  MyData->swd_UploadFileString = MagicString( MyContext->c_Magic,
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_UPLOADFILE_KEY ),
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_UPLOADFILE_HELP ) ) ;
  UploadFormatLabel = MagicLabel( MyContext->c_Magic,
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_UPLOADFORMAT ),
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_UPLOADFORMAT_KEY ) ) ;
  MyData->swd_UploadFormatCycleNames[ JPG_FORMAT ] = GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_FORMAT_JPG ) ;
  MyData->swd_UploadFormatCycleNames[ PNG_FORMAT ] = GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_FORMAT_PNG ) ;
  MyData->swd_UploadFormatCycleNames[ NUM_FORMATS ] = NULL ;
  MyData->swd_UploadFormatCycle = MagicCycle( MyContext->c_Magic,
    MyData->swd_UploadFormatCycleNames,
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_UPLOADFORMAT_KEY ),
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_UPLOADFORMAT_HELP ) ) ;

  UploadGroup = MUI_NewObject( MUIC_Group,
    MUIA_Group_Columns, 2,
    MUIA_Group_Child, ( IPTR )( UploadEnableLabel ),
    MUIA_Group_Child, ( IPTR )( UploadEnableCheckmarkGroup ),
    MUIA_Group_Child, ( IPTR )( UploadServerLabel ),
    MUIA_Group_Child, ( IPTR )( MyData->swd_UploadServerString ),
    MUIA_Group_Child, ( IPTR )( UploadPassiveLabel ),
    MUIA_Group_Child, ( IPTR )( UploadPassiveCheckmarkGroup ),
    MUIA_Group_Child, ( IPTR )( UploadTimeoutLabel ),
    MUIA_Group_Child, ( IPTR )( MyData->swd_UploadTimeoutInteger ),
    MUIA_Group_Child, ( IPTR )( UploadUserLabel ),
    MUIA_Group_Child, ( IPTR )( MyData->swd_UploadUserString ),
    MUIA_Group_Child, ( IPTR )( UploadPasswordLabel ),
    MUIA_Group_Child, ( IPTR )( MyData->swd_UploadPasswordString ),
    MUIA_Group_Child, ( IPTR )( UploadDrawerLabel ),
    MUIA_Group_Child, ( IPTR )( MyData->swd_UploadDrawerString ),
    MUIA_Group_Child, ( IPTR )( UploadFileLabel ),
    MUIA_Group_Child, ( IPTR )( MyData->swd_UploadFileString ),
    MUIA_Group_Child, ( IPTR )( UploadFormatLabel ),
    MUIA_Group_Child, ( IPTR )( MyData->swd_UploadFormatCycle ),
    TAG_END ) ;

  UploadPage = MUI_NewObject( MUIC_Group,
    MUIA_Group_Horiz, FALSE,
    MUIA_Group_Child, ( IPTR )( MagicSpace( MyContext->c_Magic ) ),
    MUIA_Group_Child, ( IPTR )( UploadGroup ),
    MUIA_Group_Child, ( IPTR )( MagicSpace( MyContext->c_Magic ) ),
    TAG_END ) ;

  return( UploadPage ) ;
}


/*
** fill upload page with setting values 
*/
static void FillUploadPageValues( struct SettingsWindowData *MyData )
{
  struct Context *MyContext ;
  SIPTR SettingValue ;

  MyContext = MyData->swd_Context ;

  SettingValue = GetSetting( MyContext->c_Settings, UploadEnable ) ;
  SetAttrs( MyData->swd_UploadEnableCheckmark, 
    MUIA_Selected, SettingValue,
    TAG_END ) ;
  SettingValue = GetSetting( MyContext->c_Settings, UploadServer ) ;
  SetAttrs( MyData->swd_UploadServerString, 
    MUIA_String_Contents, SettingValue,
    TAG_END ) ;
  SettingValue = GetSetting( MyContext->c_Settings, UploadPassive ) ;
  SetAttrs( MyData->swd_UploadPassiveCheckmark, 
    MUIA_Selected, SettingValue,
    TAG_END ) ;
  SettingValue = GetSetting( MyContext->c_Settings, UploadTimeout ) ;
  SetAttrs( MyData->swd_UploadTimeoutInteger, 
    MUIA_String_Integer, SettingValue,
    TAG_END ) ;
  SettingValue = GetSetting( MyContext->c_Settings, UploadUser ) ;
  SetAttrs( MyData->swd_UploadUserString, 
    MUIA_String_Contents, SettingValue,
    TAG_END ) ;
  SettingValue = GetSetting( MyContext->c_Settings, UploadPassword ) ;
  SetAttrs( MyData->swd_UploadPasswordString, 
    MUIA_String_Contents, SettingValue,
    TAG_END ) ;
  SettingValue = GetSetting( MyContext->c_Settings, UploadDrawer ) ;
  SetAttrs( MyData->swd_UploadDrawerString, 
    MUIA_String_Contents, SettingValue,
    TAG_END ) ;
  SettingValue = GetSetting( MyContext->c_Settings, UploadFile ) ;
  SetAttrs( MyData->swd_UploadFileString, 
    MUIA_String_Contents, SettingValue,
    TAG_END ) ;
  SettingValue = GetSetting( MyContext->c_Settings, UploadFormat ) ;
  SetAttrs( MyData->swd_UploadFormatCycle, 
    MUIA_Cycle_Active, SettingValue,
    TAG_END ) ;
}


/*
** use upload page setting values 
*/
static void UseUploadPageValues( struct SettingsWindowData *MyData )
{
  struct Context *MyContext ;
  LONG SettingValue ;

  MyContext = MyData->swd_Context ;

  GetAttr( MUIA_Selected, MyData->swd_UploadEnableCheckmark, &SettingValue ) ;
  SetSetting( MyContext->c_Settings, UploadEnable, SettingValue ) ;
  GetAttr( MUIA_String_Contents, MyData->swd_UploadServerString, &SettingValue ) ;
  SetSetting( MyContext->c_Settings, UploadServer, SettingValue ) ;
  GetAttr( MUIA_Selected, MyData->swd_UploadPassiveCheckmark, &SettingValue ) ;
  SetSetting( MyContext->c_Settings, UploadPassive, SettingValue ) ;
  GetAttr( MUIA_String_Integer, MyData->swd_UploadTimeoutInteger, &SettingValue ) ;
  SetSetting( MyContext->c_Settings, UploadTimeout, SettingValue ) ;
  GetAttr( MUIA_String_Contents, MyData->swd_UploadUserString, &SettingValue ) ;
  SetSetting( MyContext->c_Settings, UploadUser, SettingValue ) ;
  GetAttr( MUIA_String_Contents, MyData->swd_UploadPasswordString, &SettingValue ) ;
  SetSetting( MyContext->c_Settings, UploadPassword, SettingValue ) ;
  GetAttr( MUIA_String_Contents, MyData->swd_UploadDrawerString, &SettingValue ) ;
  SetSetting( MyContext->c_Settings, UploadDrawer, SettingValue ) ;
  GetAttr( MUIA_String_Contents, MyData->swd_UploadFileString, &SettingValue ) ;
  SetSetting( MyContext->c_Settings, UploadFile, SettingValue ) ;
  GetAttr( MUIA_Cycle_Active, MyData->swd_UploadFormatCycle, &SettingValue ) ;
  SetSetting( MyContext->c_Settings, UploadFormat, SettingValue ) ;
}


/*
** create log page objects
*/
static APTR CreateLogPage( struct SettingsWindowData *MyData )
{
  struct Context *MyContext ;
  struct Library *MUIMasterBase ;
  APTR LogLevelLabel ;
  APTR LogGroup, LogPage ;

  MyContext = MyData->swd_Context ;
  MUIMasterBase = MyContext->c_MUIMasterBase ;

  LogLevelLabel = MagicLabel( MyContext->c_Magic,
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_LOGLEVEL ),
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_LOGLEVEL_KEY ) ) ;
  MyData->swd_LogLevelCycleNames[ OFF_LEVEL ] = GetLocalizedString( MyContext->c_Localization, STR_LOGLEVEL_OFF ) ;
  MyData->swd_LogLevelCycleNames[ FAILURE_LEVEL ] = GetLocalizedString( MyContext->c_Localization, STR_LOGLEVEL_FAILURE ) ;
  MyData->swd_LogLevelCycleNames[ ERROR_LEVEL ] = GetLocalizedString( MyContext->c_Localization, STR_LOGLEVEL_ERROR ) ;
  MyData->swd_LogLevelCycleNames[ WARNING_LEVEL ] = GetLocalizedString( MyContext->c_Localization, STR_LOGLEVEL_WARNING ) ;
  MyData->swd_LogLevelCycleNames[ INFO_LEVEL ] = GetLocalizedString( MyContext->c_Localization, STR_LOGLEVEL_INFO ) ;
  MyData->swd_LogLevelCycleNames[ DEBUG_LEVEL ] = GetLocalizedString( MyContext->c_Localization, STR_LOGLEVEL_DEBUG ) ;
  MyData->swd_LogLevelCycleNames[ NUM_LEVELS ] = NULL ;
  MyData->swd_LogLevelCycle = MagicCycle( MyContext->c_Magic,
    MyData->swd_LogLevelCycleNames,
    *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_LOGLEVEL_KEY ),
    GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_LOGLEVEL_HELP ) ) ;

  LogGroup = MUI_NewObject( MUIC_Group,
    MUIA_Group_Columns, 2,
    MUIA_Group_Child, ( IPTR )( LogLevelLabel ),
    MUIA_Group_Child, ( IPTR )( MyData->swd_LogLevelCycle ),
    TAG_END ) ;

  LogPage = MUI_NewObject( MUIC_Group,
    MUIA_Group_Horiz, FALSE,
    MUIA_Group_Child, ( IPTR )( MagicSpace( MyContext->c_Magic ) ),
    MUIA_Group_Child, ( IPTR )( LogGroup ),
    MUIA_Group_Child, ( IPTR )( MagicSpace( MyContext->c_Magic ) ),
    TAG_END ) ;

  return( LogPage ) ;
}


/*
** fill log page with setting values 
*/
static void FillLogPageValues( struct SettingsWindowData *MyData )
{
  struct Context *MyContext ;
  SIPTR SettingValue ;

  MyContext = MyData->swd_Context ;

  SettingValue = GetSetting( MyContext->c_Settings, LogLevel ) ;
  SetAttrs( MyData->swd_LogLevelCycle, 
    MUIA_Cycle_Active, SettingValue,
    TAG_END ) ;
}


/*
** use log page setting values 
*/
static void UseLogPageValues( struct SettingsWindowData *MyData )
{
  struct Context *MyContext ;
  LONG SettingValue ;
  
  MyContext = MyData->swd_Context ;

  GetAttr( MUIA_Cycle_Active, MyData->swd_LogLevelCycle, &SettingValue ) ;
  SetSetting( MyContext->c_Settings, LogLevel, SettingValue ) ;
}


/*
** NEW method for the settings window class
*/
static IPTR SettingsWindowNew( struct IClass *MyIClass, Object *MyObject, struct opSet *MyMsg )
{
  struct Context *MyContext ;
  struct Library *MUIMasterBase ;
  struct SettingsWindowData *MyData ;
  struct TagItem AttrListExtension[ 2 ] ;
  APTR RootObject ;
  APTR CategoryListview, CategoryGroup ;
  APTR OkButton, CancelButton, OkCancelGroup ;
  IPTR Result ;

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
    MyData->swd_Context = MyContext ;
    MyData->swd_WindowObject = MyObject ;

    snprintf( MyData->swd_YearSpecifier, SWD_SPECIFIER_LENGTH, "%%%c: %s",
      EXPAND_SPECIFIER_YEAR, GetLocalizedString( MyContext->c_Localization, STR_SPECIFIER_YEAR_HELP ) ) ;
    snprintf( MyData->swd_MonthSpecifier, SWD_SPECIFIER_LENGTH, "%%%c: %s",
      EXPAND_SPECIFIER_MONTH, GetLocalizedString( MyContext->c_Localization, STR_SPECIFIER_MONTH_HELP ) ) ;
    snprintf( MyData->swd_DaySpecifier, SWD_SPECIFIER_LENGTH, "%%%c: %s",
      EXPAND_SPECIFIER_DAY, GetLocalizedString( MyContext->c_Localization, STR_SPECIFIER_DAY_HELP ) ) ;
    snprintf( MyData->swd_HourSpecifier, SWD_SPECIFIER_LENGTH, "%%%c: %s",
      EXPAND_SPECIFIER_HOUR, GetLocalizedString( MyContext->c_Localization, STR_SPECIFIER_HOUR_HELP ) ) ;
    snprintf( MyData->swd_MinuteSpecifier, SWD_SPECIFIER_LENGTH, "%%%c: %s",
      EXPAND_SPECIFIER_MINUTE, GetLocalizedString( MyContext->c_Localization, STR_SPECIFIER_MINUTE_HELP ) ) ;
    snprintf( MyData->swd_SecondSpecifier, SWD_SPECIFIER_LENGTH, "%%%c: %s",
      EXPAND_SPECIFIER_SECOND, GetLocalizedString( MyContext->c_Localization, STR_SPECIFIER_SECOND_HELP ) ) ;
    snprintf( MyData->swd_CounterSpecifier, SWD_SPECIFIER_LENGTH, "%%%c: %s",
      EXPAND_SPECIFIER_COUNTER, GetLocalizedString( MyContext->c_Localization, STR_SPECIFIER_COUNTER_HELP ) ) ;

    MyData->swd_InsertSpecifierHook.h_Entry = ( HOOKFUNC )InsertSpecifierHook ;
    MyData->swd_InsertSpecifierHook.h_Data = MyData ;
    MyData->swd_PopobjectWindowHook.h_Entry = ( HOOKFUNC )PopobjectWindowHook ;
    MyData->swd_PopobjectWindowHook.h_Data = MyData ;

    MyData->swd_CategoryPages = MUI_NewObject( MUIC_Group,
      MUIA_Group_PageMode, TRUE,
      MUIA_Group_Child, ( IPTR )( CreateVideohardwarePage( MyData ) ),
      MUIA_Group_Child, ( IPTR )( CreateStampPage( MyData ) ),
      MUIA_Group_Child, ( IPTR )( CreateDisplayPage( MyData ) ),
      MUIA_Group_Child, ( IPTR )( CreateSchedulerPage( MyData ) ),
      MUIA_Group_Child, ( IPTR )( CreateCapturePage( MyData ) ),
      MUIA_Group_Child, ( IPTR )( CreateUploadPage( MyData ) ),
      MUIA_Group_Child, ( IPTR )( CreateLogPage( MyData ) ),
      MUIA_Group_ActivePage, MUIV_Group_ActivePage_First,
      TAG_END ) ;
    CategoryListview = CreateCategoryListview( MyData ) ;
    CategoryGroup = MUI_NewObject( MUIC_Group,
      MUIA_Group_Horiz, TRUE,
      MUIA_Group_Child, ( IPTR )( CategoryListview ),
      MUIA_Group_Child, ( IPTR )( MyData->swd_CategoryPages ),
      TAG_END ) ;
    DoMethod( RootObject, OM_ADDMEMBER, CategoryGroup ) ;

    OkButton = MagicButton( MyContext->c_Magic,
      GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_OK ),
      *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_OK_KEY ),
      GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_OK_HELP ) ) ;
    CancelButton = MagicButton( MyContext->c_Magic,
      GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_CANCEL ),
      *GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_CANCEL_KEY ),
      GetLocalizedString( MyContext->c_Localization, STR_SETTINGSWINDOW_CANCEL_HELP ) ) ;
    OkCancelGroup = MUI_NewObject( MUIC_Group,
      MUIA_Group_Columns, 5,
      MUIA_Group_Child, ( IPTR )( MagicSpace( MyContext->c_Magic ) ),
      MUIA_Group_Child, ( IPTR )( OkButton ),
      MUIA_Group_Child, ( IPTR )( MagicSpace( MyContext->c_Magic ) ),
      MUIA_Group_Child, ( IPTR )( CancelButton ),
      MUIA_Group_Child, ( IPTR )( MagicSpace( MyContext->c_Magic ) ),
      TAG_END ) ;
    DoMethod( RootObject, OM_ADDMEMBER, OkCancelGroup ) ;

    if( ( NULL != OkCancelGroup ) && ( NULL != CategoryGroup ) )
    {  /* window is complete */
      SetAttrs( MyObject, MUIA_Window_DefaultObject, CategoryListview ) ;
      DoMethod( OkButton, MUIM_Notify, MUIA_Pressed, FALSE,
        MyObject, 2, MUIM_SettingsWindowClose, 1 ) ;
      DoMethod( CancelButton, MUIM_Notify, MUIA_Pressed, FALSE,
        MyObject, 2, MUIM_SettingsWindowClose, 0 ) ;
      DoMethod( MyData->swd_CategoryList, MUIM_Notify, MUIA_List_Active, MUIV_EveryTime,
        MyData->swd_CategoryPages, 3, MUIM_Set, MUIA_Group_ActivePage, MUIV_TriggerValue ) ;
      DoMethod( MyObject, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
        MyObject, 2, MUIM_SettingsWindowClose, 0 ) ;
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
** open method for the settings window class
*/
static IPTR SettingsWindowOpen( struct IClass *MyIClass, Object *MyObject, ULONG MyCategory )
{
  struct Context *MyContext ;
  struct SettingsWindowData *MyData ;

  MyData = INST_DATA( MyIClass, MyObject ) ;
  MyContext = MyData->swd_Context ;

  SetAttrs( MyData->swd_CategoryList,
    MUIA_List_Active, ( IPTR )( MyCategory ),
    TAG_END ) ;
  FillVideohardwarePageValues( MyData ) ;
  FillStampPageValues( MyData ) ;
  FillDisplayPageValues( MyData ) ;
  FillSchedulerPageValues( MyData ) ;
  FillCapturePageValues( MyData ) ;
  FillUploadPageValues( MyData ) ;
  FillLogPageValues( MyData ) ;

  SetAttrs( MyObject, 
    MUIA_Window_Open, TRUE, 
    TAG_END ) ;

  return( 1 ) ;
}


/*
** clsse method for the settings window class
*/
static IPTR SettingsWindowClose( struct IClass *MyIClass, Object *MyObject, ULONG TakeOver )
{
  struct Context *MyContext ;
  struct SettingsWindowData *MyData ;

  MyData = INST_DATA( MyIClass, MyObject ) ;
  MyContext = MyData->swd_Context ;

  SetAttrs( MyObject,
    MUIA_Window_Open, FALSE,
    TAG_END ) ;

  if( TakeOver )
  {
    UseVideohardwarePageValues( MyData ) ;
    UseStampPageValues( MyData ) ;
    UseDisplayPageValues( MyData ) ;
    UseSchedulerPageValues( MyData ) ;
    UseCapturePageValues( MyData ) ;
    UseUploadPageValues( MyData ) ;
    UseLogPageValues( MyData ) ;
    MyContext->c_ConfigureRequest = 1 ;
  }

  if( NULL != MyData->swd_VideoHardware )
  {  /* there is a temporary video hardware ... */
    if( MyData->swd_VideoHardware != MyContext->c_VideoHardware )
    {  /* ... and it is not the current, so delte it */
      LogText( MyContext->c_Log, DEBUG_LEVEL, "close temp vhi driver: %s", MyData->swd_VideoHardware->vh_CurrentVhiDriver ) ;
      DeleteVideoHardware( MyData->swd_VideoHardware ) ;
    }
    MyData->swd_VideoHardware = NULL ;
  }

  return( 1 ) ;
}


/*
** set settings window attributes
*/
static IPTR SettingsWindowSet( struct IClass *MyIClass, Object *MyObject, struct opSet *MyMsg )
{
  struct SettingsWindowData *MyData ;
  struct Context *MyContext ;
  struct Library *MUIMasterBase ;
  struct IntuitionBase *IntuitionBase ;
  struct Library *UtilityBase ;
  struct TagItem *Attribute, *Attributes ;
  ULONG TagCount = 0 ;

  MyData = INST_DATA( MyIClass, MyObject ) ;
  MyContext = MyData->swd_Context ;
  if (NULL != MyContext) {
    MUIMasterBase = MyContext->c_MUIMasterBase ;
    IntuitionBase = MyContext->c_IntuitionBase ;
    UtilityBase = MyContext->c_UtilityBase ;
      
    Attributes = MyMsg->ops_AttrList;

    while( NULL != ( Attribute = NextTagItem( &Attributes ) ) )
    {
      switch( Attribute->ti_Tag )
      {
        case MUIA_VhiDriver:
          VhiDriverUpdate( MyData, ( STRPTR )Attribute->ti_Data ) ;
          TagCount++;
          break ;
        case MUIA_VhiInput:
          VhiInputUpdate( MyData, ( IPTR )Attribute->ti_Data ) ;
          TagCount++;
          break ;
        case MUIA_VhiColorMode:
          VhiColorModeUpdate( MyData, ( IPTR )Attribute->ti_Data ) ;
          TagCount++;
          break ;
        case MUIA_VhiVideoFormat:
          VhiVideoFormatUpdate( MyData, ( IPTR )Attribute->ti_Data ) ;
          TagCount++;
          break ;
        default:  /* some attribute we don't handle here */
          break ;
      }
    }
    TagCount += DoSuperMethodA( MyIClass, MyObject, ( Msg )MyMsg ) ;
  }

  return( TagCount ) ;
}


/*
** dispatcher for the settings window class
*/
IPTR SettingsWindowDispatcher( REG( a0, struct IClass *MyIClass ), REG( a2, Object *MyObject ), REG( a1, Msg MyMsg ) )
{
  IPTR Result ;

  Result = 0 ;

  switch( MyMsg->MethodID )
  {
    case OM_NEW:
      Result = SettingsWindowNew( MyIClass, MyObject, ( struct opSet * )MyMsg ) ;
      break ;
    case OM_SET:
      Result = SettingsWindowSet( MyIClass, MyObject, ( struct opSet * )MyMsg ) ;
      break ;
    case MUIM_SettingsWindowOpen:
      Result = SettingsWindowOpen( MyIClass, MyObject, ( ( struct SWinMsg * )MyMsg )->Catagory ) ;
      break ;
    case MUIM_SettingsWindowClose:
      Result = SettingsWindowClose( MyIClass, MyObject, ( ( struct SWinMsg * )MyMsg )->Catagory ) ;
      break ;
    default:
      Result = DoSuperMethodA( MyIClass, MyObject, MyMsg ) ;
      break ;
  }

  return( Result ) ;
}
