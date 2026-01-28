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
** SettingsWindowClass.h
*/


#ifndef _SETTINGSWINDOWCLASS_H
#define _SETTINGSWINDOWCLASS_H


#include "Magic.h"
#include "CompilerExtensions.h"
#include "Settings.h"
#include "VideoHardware.h"
#include "Log.h"
#include <intuition/classes.h>


/*
** custom methods of settings window class
*/
#define MUIM_SettingsWindowOpen ( MUIB_SETTINGSWINDOW + 0x01 )
#define MUIM_SettingsWindowClose ( MUIB_SETTINGSWINDOW + 0x02 )
#define MUIM_SettingsWindowHandleVhiDriverString ( MUIB_SETTINGSWINDOW + 0x03 )


/*
** custom attributes of settings window class
*/
#define MUIA_VhiDriver ( MUIB_SETTINGSWINDOW + 0x01 )
#define MUIA_VhiInput ( MUIB_SETTINGSWINDOW + 0x02 )
#define MUIA_VhiColorMode ( MUIB_SETTINGSWINDOW + 0x03 )
#define MUIA_VhiVideoFormat ( MUIB_SETTINGSWINDOW + 0x04 )


/*
** private data for the settings window class
*/
#define SWD_VHIINFO_LENGHT ( 512 )
#define SWD_SPECIFIER_LENGTH ( 48 )
struct SettingsWindowData
{
  struct Context *swd_Context ;
  APTR swd_WindowObject ;
  STRPTR swd_CategoryStrings[ NUM_CATEGORIES + 1 ] ;
  APTR swd_CategoryList ;
  APTR swd_CategoryListview ;
  APTR swd_VhiDriverString ;
  APTR swd_VhiDriverPopasl ;
  APTR swd_VhiInfoText ;
  BYTE swd_VhiInfo[ SWD_VHIINFO_LENGHT ] ;
  APTR swd_VhiInputCycle ;
  APTR swd_VhiInputCycleGroup ;
  APTR swd_VhiVideoFormatCycle ;
  APTR swd_VhiVideoFormatCycleGroup ;
  APTR swd_VhiColorCheckmark ;
  APTR swd_VhiX1Slider ;
  APTR swd_VhiY1Slider ;
  APTR swd_VhiX2Slider ;
  APTR swd_VhiY2Slider ;
  APTR swd_VhiWidthInteger ;
  APTR swd_VhiHeightInteger ;
  APTR swd_VideohardwareGroup ;
  struct VideoHardware *swd_VideoHardware ;
  APTR swd_StampEnableCheckmark ;
  APTR swd_StampTextString ;
  APTR swd_StampFontString ;
  APTR swd_StampXInteger ;
  APTR swd_StampYInteger ;
  APTR swd_DisplayEnableCheckmark ;
  APTR swd_DisplayIntervalInteger ;
  APTR swd_DisplayZoomInteger ;
  APTR swd_DisplayRatioCycle ;
  STRPTR swd_DisplayRatioCycleNames[ NUM_RATIOS + 1 ] ;
  APTR swd_SchedulerIntervalInteger ;
  APTR swd_CaptureEnableCheckmark ;
  APTR swd_CaptureDrawerString ;
  APTR swd_CaptureFileString ;
  APTR swd_CaptureFormatCycle ;
  STRPTR swd_CaptureFormatCycleNames[ NUM_FORMATS + 1 ] ;
  APTR swd_UploadEnableCheckmark ;
  APTR swd_UploadServerString ;
  APTR swd_UploadPassiveCheckmark ;
  APTR swd_UploadTimeoutInteger ;
  APTR swd_UploadUserString ;
  APTR swd_UploadPasswordString ;
  APTR swd_UploadDrawerString ;
  APTR swd_UploadFileString ;
  APTR swd_UploadFormatCycle ;
  STRPTR swd_UploadFormatCycleNames[ NUM_FORMATS + 1 ] ;
  APTR swd_LogLevelCycle ;
  STRPTR swd_LogLevelCycleNames[ NUM_LEVELS + 1 ] ;
  APTR swd_CategoryPages ;
  struct Hook swd_InsertSpecifierHook ;
  struct Hook swd_PopobjectWindowHook ;
  BYTE swd_YearSpecifier[ SWD_SPECIFIER_LENGTH ] ;
  BYTE swd_MonthSpecifier[ SWD_SPECIFIER_LENGTH ] ;
  BYTE swd_DaySpecifier[ SWD_SPECIFIER_LENGTH ] ;
  BYTE swd_HourSpecifier[ SWD_SPECIFIER_LENGTH ] ;
  BYTE swd_MinuteSpecifier[ SWD_SPECIFIER_LENGTH ] ;
  BYTE swd_SecondSpecifier[ SWD_SPECIFIER_LENGTH ] ;
  BYTE swd_CounterSpecifier[ SWD_SPECIFIER_LENGTH ] ;
} ;


/*
** the dispatcher of the settings window class
*/
IPTR SettingsWindowDispatcher( REG( a0, struct IClass *MyIClass ), REG( a2, Object *MyObject ), REG( a1, Msg MyMsg ) ) ;


#endif  /* !_SETTINGSWINDOWCLASS_H */
