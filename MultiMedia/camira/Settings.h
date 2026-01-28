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
** Settings.h
*/


#ifndef _SETTINGS_H
#define _SETTINGS_H


#include "Context.h"
#include <workbench/startup.h>
#include <workbench/workbench.h>


/*
** identifier for main setting categories
*/
enum SettingCategories
{
  VIDEOHARDWARE_CATEGORY,
  STAMP_CATEGORY,
  DISPLAY_CATEGORY,
  SCHEDULER_CATEGORY,
  CAPTURE_CATEGORY,
  UPLOAD_CATEGORY,
  LOG_CATEGORY,
  NUM_CATEGORIES  /* number of categories */
} ;


/*
** the available setting index values
*/
enum SettingIndex
{
  Nogui,
  VhiDriver,
  VhiInput,
  VhiVideoformat,
  VhiColormode,
  VhiX1,
  VhiY1,
  VhiX2,
  VhiY2,
  VhiWidth,
  VhiHeight,
  StampEnable,
  StampText,
  StampFont,
  StampFontsize,
  StampX,
  StampY,
  DisplayEnable,
  DisplayInterval,
  DisplayZoom,
  DisplayRatio,
  SchedulerInterval,
  CaptureEnable,
  CaptureDrawer,
  CaptureFile,
  CaptureFormat,
  UploadEnable,
  UploadServer,
  UploadPassive,
  UploadTimeout,
  UploadUser,
  UploadPassword,
  UploadDrawer,
  UploadFile,
  UploadFormat,
  LogLevel,
  JpegQuality,
  JpegProgressive,
  PngCompression,
  NUM_SETTINGS  /* number of settings */
} ;


/*
** possible preview ratio settings in free zoom mode
*/
enum DisplayRatio
{
  FREE_RATIO,
  KEEP_RATIO,
  NUM_RATIOS
} ;


/*
** possible capture or upload formats
*/
enum ExportFormat
{
  JPG_FORMAT,
  PNG_FORMAT,
  NUM_FORMATS
} ;


/*
** context for expat parser
*/
#define EXPAT_OPTION_LENGTH ( 64 )
#define EXPAT_VALUE_LENGTH ( 128 )
struct ExpatContext
{
  LONG ec_Depth ;  /* expat parser depth */
  LONG ec_EndCount ;  /* expat parser option end counter */
  char ec_Option[ EXPAT_OPTION_LENGTH ] ;  /* expat parser option name */
  char ec_Value[ EXPAT_VALUE_LENGTH ] ;  /* expat parser option value */
} ;


/*
** this defines a setting
*/
struct SettingDescriptor
{
  ULONG sd_Index ;  /* a index number */
  STRPTR sd_Name ;  /* a name string */
  ULONG sd_Type ;  /* the type imformation */
} ;


/*
** settings context
*/
#define S_FILEBUFFER_LENGTH ( 1024 )
#define S_VALUESTRING_LENGTH ( 128 )
struct Settings
{
  struct Context *s_Context ;  /* pointer to the context */
  SIPTR *s_Current ;  /* current setting values */
  SIPTR s_Changed ;  /* copy of setting values */
  const struct SettingDescriptor *s_Descriptors ;  /* description of settings */
  BPTR s_File ;
  UBYTE s_FileBuffer[ S_FILEBUFFER_LENGTH ] ;
  LONG s_FileBufferPosition ;
  char s_ValueString[ S_VALUESTRING_LENGTH ] ;
  struct ExpatContext *s_ExpatContext ;
} ;


/*
** settings functions
*/
struct Settings *CreateSettings( struct Context *MyContext ) ;
void DeleteSettings( struct Settings *MySettings ) ;
void SetSetting( struct Settings *MySettings, ULONG Index, SIPTR Value ) ;
SIPTR GetSetting( struct Settings *MySettings, ULONG Index ) ;
struct DiskObject *ParseParameters( struct Settings *MySettings, int argc, char *argv[] ) ;
struct DiskObject *ParseToolTypes( struct Settings *MySettings, struct WBStartup *MyWBStartup ) ;
void SetDefaultSettings( struct Settings *MySettings ) ;
void LoadSettings( struct Settings *MySettings ) ;
void SaveSettings( struct Settings *MySettings ) ;
void UseSettings( struct Settings *MySettings ) ;
void PrintSettings( struct Settings *MySettings ) ;


#endif  /* !_SETTINGS_H */
