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
** Settings.c
*/


#include "Settings.h"
#include "Log.h"
#include "ab_stdio.h"
#include "ab_stdlib.h"
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/icon.h>
#include <stddef.h>
#include <proto/expat.h>
#include <vhi/vhi.h>
#include <string.h>
#include <ctype.h>


/*
** with this we use the index name as name string
*/
#define MAKE_SETTING_DESCRIPTOR( _Index, _Flags ) { _Index, #_Index, _Flags }


/*
** possible types
*/
#define SETTING_TYPE_STRING ( 0 )
#define SETTING_TYPE_NUMBER ( 1 )
#define SETTING_TYPE_SWITCH ( 2 )


/*
** the cAMIra setting description
*/
static const struct SettingDescriptor Descriptors[ NUM_SETTINGS ] =
{
  MAKE_SETTING_DESCRIPTOR( Nogui, SETTING_TYPE_SWITCH ),
  MAKE_SETTING_DESCRIPTOR( VhiDriver, SETTING_TYPE_STRING ),
  MAKE_SETTING_DESCRIPTOR( VhiInput, SETTING_TYPE_NUMBER ) ,
  MAKE_SETTING_DESCRIPTOR( VhiVideoformat, SETTING_TYPE_NUMBER ) ,
  MAKE_SETTING_DESCRIPTOR( VhiColormode, SETTING_TYPE_NUMBER ) ,
  MAKE_SETTING_DESCRIPTOR( VhiX1, SETTING_TYPE_NUMBER ) ,
  MAKE_SETTING_DESCRIPTOR( VhiY1, SETTING_TYPE_NUMBER ) ,
  MAKE_SETTING_DESCRIPTOR( VhiX2, SETTING_TYPE_NUMBER ) ,
  MAKE_SETTING_DESCRIPTOR( VhiY2, SETTING_TYPE_NUMBER ) ,
  MAKE_SETTING_DESCRIPTOR( VhiWidth, SETTING_TYPE_NUMBER ) ,
  MAKE_SETTING_DESCRIPTOR( VhiHeight, SETTING_TYPE_NUMBER ) ,
  MAKE_SETTING_DESCRIPTOR( StampEnable, SETTING_TYPE_NUMBER ) ,
  MAKE_SETTING_DESCRIPTOR( StampText, SETTING_TYPE_STRING ) ,
  MAKE_SETTING_DESCRIPTOR( StampFont, SETTING_TYPE_STRING ) ,
  MAKE_SETTING_DESCRIPTOR( StampFontsize, SETTING_TYPE_NUMBER ) ,
  MAKE_SETTING_DESCRIPTOR( StampX, SETTING_TYPE_NUMBER ) ,
  MAKE_SETTING_DESCRIPTOR( StampY, SETTING_TYPE_NUMBER ) ,
  MAKE_SETTING_DESCRIPTOR( DisplayEnable, SETTING_TYPE_NUMBER ) ,
  MAKE_SETTING_DESCRIPTOR( DisplayInterval, SETTING_TYPE_NUMBER ) ,
  MAKE_SETTING_DESCRIPTOR( DisplayZoom, SETTING_TYPE_NUMBER ) ,
  MAKE_SETTING_DESCRIPTOR( DisplayRatio, SETTING_TYPE_NUMBER ),
  MAKE_SETTING_DESCRIPTOR( SchedulerInterval, SETTING_TYPE_NUMBER ),
  MAKE_SETTING_DESCRIPTOR( CaptureEnable, SETTING_TYPE_NUMBER ),
  MAKE_SETTING_DESCRIPTOR( CaptureDrawer, SETTING_TYPE_STRING ) ,
  MAKE_SETTING_DESCRIPTOR( CaptureFile, SETTING_TYPE_STRING ) ,
  MAKE_SETTING_DESCRIPTOR( CaptureFormat, SETTING_TYPE_NUMBER ),
  MAKE_SETTING_DESCRIPTOR( UploadEnable, SETTING_TYPE_NUMBER ),
  MAKE_SETTING_DESCRIPTOR( UploadServer, SETTING_TYPE_STRING ),
  MAKE_SETTING_DESCRIPTOR( UploadPassive, SETTING_TYPE_NUMBER ),
  MAKE_SETTING_DESCRIPTOR( UploadTimeout, SETTING_TYPE_NUMBER ),
  MAKE_SETTING_DESCRIPTOR( UploadUser, SETTING_TYPE_STRING ),
  MAKE_SETTING_DESCRIPTOR( UploadPassword, SETTING_TYPE_STRING ),
  MAKE_SETTING_DESCRIPTOR( UploadDrawer, SETTING_TYPE_STRING ),
  MAKE_SETTING_DESCRIPTOR( UploadFile, SETTING_TYPE_STRING ),
  MAKE_SETTING_DESCRIPTOR( UploadFormat, SETTING_TYPE_NUMBER ),
  MAKE_SETTING_DESCRIPTOR( LogLevel, SETTING_TYPE_NUMBER ),
  MAKE_SETTING_DESCRIPTOR( JpegQuality, SETTING_TYPE_NUMBER ),
  MAKE_SETTING_DESCRIPTOR( JpegProgressive, SETTING_TYPE_NUMBER ),
  MAKE_SETTING_DESCRIPTOR( PngCompression, SETTING_TYPE_NUMBER ),
} ;


/*
** set a setting
*/
void SetSetting( struct Settings *MySettings, ULONG Index, SIPTR Value )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  STRPTR NewString ;

  if( ( NULL != MySettings ) && ( NUM_SETTINGS > Index ) )
  {  /* parameter ok */
    MyContext = MySettings->s_Context ;
    SysBase = MyContext->c_SysBase ;
    switch( MySettings->s_Descriptors[ Index ].sd_Type )
    {  /* action depends on type */
      case SETTING_TYPE_STRING:
        if( NULL != ( STRPTR )MySettings->s_Current[ Index ] )
        {  /* old string setting needs to be freed */
          FreeVec( ( APTR )MySettings->s_Current[ Index ] ) ;
          MySettings->s_Current[ Index ] = 0;
        }
        if( ( NULL != ( STRPTR )Value ) && ( '\0' !=  ( ( STRPTR )Value )[ 0 ] ) )
        {  /* set a new string value */
          NewString = AllocVec( ( strlen( ( STRPTR )Value ) + 1 ), MEMF_ANY ) ;
          if( NULL != NewString )
          {  /* new string setting can be stored */
            strcpy( NewString, ( STRPTR )Value ) ;
            MySettings->s_Current[ Index ] = ( SIPTR )NewString ;
          }
        }
        else
        {  /* no new string value */
          MySettings->s_Current[ Index ] = ( SIPTR )NULL ;
        }
        break ;
      case SETTING_TYPE_NUMBER:
        MySettings->s_Current[ Index ] = Value ;
        break ;
      case SETTING_TYPE_SWITCH:
        MySettings->s_Current[ Index ] = ( 1 && Value ) ;
        break ;
      default:  /* unknown type */
        break ;
    }
    MySettings->s_Changed = 1 ;
  }
  else
  {  /* parameter not ok */
  }
}


/*
** helper to set setting with strings (xml file or tooltype parsers use it)
*/
static void SetSettingWithStrings( struct Settings *MySettings, STRPTR Name, STRPTR Value )
{
  ULONG Count ;

  if( NULL != MySettings )
  {  /* requirements seem ok */
    for( Count = 0 ; Count < NUM_SETTINGS ; Count++ )
    {  /* look for the name */
      if( 0 == stricmp( ( STRPTR )MySettings->s_Descriptors[ Count ].sd_Name, Name ) )
      {  /* found */
        break ;
      }
    }
    if( NUM_SETTINGS > Count )
    {  /* name matched a setting */
      switch( MySettings->s_Descriptors[ Count ].sd_Type )
      {
        case SETTING_TYPE_STRING:
          SetSetting( MySettings, Count, ( SIPTR )Value ) ;
          break ;
        case SETTING_TYPE_NUMBER:
          SetSetting( MySettings, Count, atol( Value ) ) ;
          break ;
        case SETTING_TYPE_SWITCH:
          SetSetting( MySettings, Count, ( 1 && atol( Value ) ) ) ;
          break ;
        default:  /* unknown type */
          break ;
      }
    }
    else
    {  /* unknown setting name */
    }
  }
  else
  {  /* requirements not ok */
  }
}


/*
** get a setting
*/
SIPTR GetSetting( struct Settings *MySettings, ULONG Index )
{
  SIPTR Value ;

  Value = 0 ;

  if( ( NULL != MySettings ) && ( NUM_SETTINGS > Index ) )
  {  /* parameter ok */
    Value = MySettings->s_Current[ Index ] ;
  }
  else
  {  /* parameter not ok */
  }

  return ( Value ) ;
}


/*
** get a setting as string (xml file writer does this)
*/
static void GetSettingAsString( struct Settings *MySettings, ULONG Index, STRPTR ResultString, LONG ResultLenght )
{
  if( ( NULL != MySettings ) && ( NUM_SETTINGS > Index ) )
  {  /* parameter ok */
    switch( MySettings->s_Descriptors[ Index ].sd_Type )
    {  /* what we do depends on type */
      case SETTING_TYPE_STRING:  /* here we print a string */
        if( 0 != MySettings->s_Current[ Index ] )
        {  /* there is a real string */
          snprintf( ResultString, ResultLenght, "%s", ( STRPTR )MySettings->s_Current[ Index ] ) ;
        }
        else
        {  /* no string stored */
          ResultString[ 0 ] = '\0' ;
        }
        break ;
      case SETTING_TYPE_NUMBER:  /* here we print a number */
        snprintf( ResultString, ResultLenght, "%ld", MySettings->s_Current[ Index ] ) ;
        break ;
      case SETTING_TYPE_SWITCH:  /* here we print 1 or 0 */
        snprintf( ResultString, ResultLenght, "%ld", MySettings->s_Current[ Index ] ) ;
        break ;
      default:  /* unknown type */
        ResultString[ 0 ] = '\0' ;
        break ;
     }
  }
  else
  {  /* parameter not ok */
  }
}


/*
** alloc space for setting values
*/
static SIPTR *AllocValues( struct Settings *MySettings )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  SIPTR *NewValues ;

  NewValues = NULL ;

  if( NULL != MySettings )
  {  /* requirements ok */
    MyContext = MySettings->s_Context ;
    SysBase = MyContext->c_SysBase ;
    NewValues = AllocVec( ( sizeof( SIPTR ) * NUM_SETTINGS ), ( MEMF_ANY | MEMF_CLEAR ) ) ;
  }
  else
  {  /* requirements not ok */
  }

  return( NewValues ) ;
}


/*
** free space for setting values and if string, also the string
*/
static void FreeValues( struct Settings *MySettings, SIPTR *OldValues )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  ULONG Count ;

  if( ( NULL != MySettings ) && ( NULL != OldValues ) )
  {  /* space for setting values needs to be freed */
    MyContext = MySettings->s_Context ;
    SysBase = MyContext->c_SysBase ;
    for( Count = 0 ; Count < NUM_SETTINGS ; Count++ )
    {  /* check values for strings */
      if( ( SETTING_TYPE_STRING == MySettings->s_Descriptors[ Count ].sd_Type ) && ( 0 != OldValues[ Count ] ) )
      {  /* string needs to be freed */
        FreeVec( ( APTR )OldValues[ Count ] ) ;
      }
    }
    FreeVec( OldValues ) ;
  }
}


/*
** create settings context
*/
struct Settings *CreateSettings( struct Context *MyContext )
{
  struct ExecBase *SysBase ;
  struct Settings *NewSettings ;
  ULONG Count ;

  NewSettings = NULL ;

  if( NULL != MyContext )
  {  /* requirements ok */
    SysBase = MyContext->c_SysBase ;
    for( Count = 0 ; Count < NUM_SETTINGS ; Count++ )
    {  /* check if index number are in order */
      if( Count != Descriptors[ Count ].sd_Index )
      {
        break ;
      }
    }
    if( ( NUM_SETTINGS == Count ) && ( NUM_SETTINGS == ( sizeof( Descriptors ) / sizeof( struct SettingDescriptor ) ) ) )
    {  /* setting descriptors properly ordered */
      NewSettings = AllocVec( sizeof( struct Settings ), MEMF_ANY | MEMF_CLEAR ) ;
      if( NULL != NewSettings )
      {  /* settings context memory ok */
        NewSettings->s_Context = MyContext ;
        NewSettings->s_Descriptors = Descriptors ;
        NewSettings->s_Current = AllocValues( NewSettings ) ;
        if( NULL != NewSettings->s_Current )
        {  /* space for current vlaues ok */
        }
        else
        {  /* space for current vlaues not ok */
          LogText( MyContext->c_Log, ERROR_LEVEL, "space for current vlaues not ok" ) ;
          DeleteSettings( NewSettings ) ;
          NewSettings = NULL ;
        }
      }
      else
      {  /* settings context memory not ok */
        LogText( MyContext->c_Log, ERROR_LEVEL, "settings context memory not ok" ) ;
      }
    }
    else
    {  /* setting descriptors not properly ordered */
      LogText( MyContext->c_Log, ERROR_LEVEL, "setting descriptors not properly ordered" ) ;
    }
  }
  else
  {  /* requirements not ok */
  }

  return( NewSettings ) ;
}


/*
** delete the settings
*/
void DeleteSettings( struct Settings *OldSettings )
{
  struct Context *MyContext ; 
  struct ExecBase *SysBase ;

  if( NULL != OldSettings )
  {  /* settings context needs to be freed */
    MyContext = OldSettings->s_Context ;
    SysBase = MyContext->c_SysBase ;
    if( NULL != OldSettings->s_Current )
    {  /* old current values need to be freed */
      FreeValues( OldSettings, OldSettings->s_Current ) ;
    }
    FreeVec( OldSettings ) ;
  }
}


/*
** parse command line
*/
struct DiskObject *ParseParameters( struct Settings *MySettings, int argc, char *argv[]  )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct DosLibrary *DOSBase ;
  struct Library *IconBase ;
  ULONG Count ;
  ULONG TemplateLength ;
  STRPTR Template ;
  SIPTR *Results ;
  struct RDArgs *MyRDArgs ;
  struct DiskObject *ToolDiskObject ;

  ToolDiskObject = NULL ;

  if( ( NULL != MySettings ) )
  {  /* requirements seem ok */
    MyContext = MySettings->s_Context ;
    SysBase = MyContext->c_SysBase ;
    DOSBase = MyContext->c_DOSBase ;
    IconBase = MyContext->c_IconBase ;

    ToolDiskObject = GetDiskObject( argv[ 0 ] ) ;

    TemplateLength = 0 ;
    for( Count = 0 ; Count < NUM_SETTINGS ; Count++ )
    {  /* get length of template string */
      TemplateLength += strlen( MySettings->s_Descriptors[ Count ].sd_Name ) ;
      switch( MySettings->s_Descriptors[ Count ].sd_Type )
      {
        case SETTING_TYPE_STRING:  /* here we add only /K */
          TemplateLength += 2 ;
          break ;
        case SETTING_TYPE_NUMBER:  /* here we add /K/N */
          TemplateLength += 4 ;
          break ;
        case SETTING_TYPE_SWITCH:  /* here we add /S */
          TemplateLength += 2 ;
          break ;
        default:  /* unknown type */
          break ;
      }
      TemplateLength += 1 ;  /* the comma char or final newline */
    }
    TemplateLength += 1 ;  /* final 0 */
    Template = AllocVec( TemplateLength, MEMF_ANY ) ;
    if( NULL != Template )
    {  /* memory for template string ok */
      Template[ 0 ] = '\0' ;
      for( Count = 0 ; Count < NUM_SETTINGS ; Count++ )
      {  /* construct the template string */
        strcat( Template, MySettings->s_Descriptors[ Count ].sd_Name ) ;
        switch( MySettings->s_Descriptors[ Count ].sd_Type )
        {
          case SETTING_TYPE_STRING:  /* here we add only /K */
            strcat( Template, "/K" ) ;
            break ;
          case SETTING_TYPE_NUMBER:  /* here we add /K/N */
            strcat( Template, "/K/N" ) ;
            break ;
          case SETTING_TYPE_SWITCH:  /* here we add /S */
            strcat( Template, "/S" ) ;
            break ;
          default:  /* unknown type */
            break ;
        }
        strcat( Template, "," ) ;
      }
      Template[ TemplateLength - 2 ] = '\n' ;
      Template[ TemplateLength - 1 ] = '\0' ;

      Results = AllocValues( MySettings ) ;
      if( NULL != Results )
      {  /* memory to store results ok */
        MyRDArgs = ReadArgs( Template, Results, NULL ) ;
        if( NULL != MyRDArgs )
        {  /* argument parsing ok */
          for( Count = 0 ; Count < NUM_SETTINGS ; Count++ )
          {  /* get results and store in our settings */
            switch( MySettings->s_Descriptors[ Count ].sd_Type )
            {
              case SETTING_TYPE_STRING:  /* result is a pointer to a string */
                if( 0 != Results[ Count ] )
                {  /* the parameter was given */
                  SetSetting( MySettings, Count, Results[ Count ] ) ;
                }
                break ;
              case SETTING_TYPE_NUMBER:  /* result is a pointer to the number */
                if( 0 != Results[ Count ] )
                {  /* the parameter was given */
                  SetSetting( MySettings, Count, *( ( SIPTR * )Results[ Count ] ) ) ;
                }
                break ;
              case SETTING_TYPE_SWITCH:  /* result is 0 for 0 or not 0 for 1 */
                SetSetting( MySettings, Count, 1 && Results[ Count ] ) ;
                break ;
              default:  /* unknown type */
                break ;
            }
          }
          FreeArgs( MyRDArgs ) ;
        }
        else
        {  /* argument parsing not ok */
        }
        FreeValues( MySettings, Results ) ;
      }
      else
      {  /* memory to store results not ok */
      }
      FreeVec( Template ) ;
    }
    else
    {  /* memory for template string not ok */
    }
    MySettings->s_Changed = 0 ;
  }
  else
  {  /* requirements not ok */
  }

  return( ToolDiskObject ) ;
}


/*
** get settings from icon tooltypes
*/
struct DiskObject *ParseToolTypes( struct Settings *MySettings, struct WBStartup *MyWBStartup )
{
  struct Context *MyContext ;
  struct DosLibrary *DOSBase ;
  struct Library *IconBase ;
  struct WBArg *MyWBArg ;
  BPTR OldCurrentDir ;
  ULONG ArgumentCount ;
  struct DiskObject *MyDiskObject, *ToolDiskObject ;
  ULONG SettingCount ;
  STRPTR ToolTypeValue ;

  ToolDiskObject = NULL ;

  if( ( NULL != MySettings ) && ( NULL != MyWBStartup ) )
  {  /* requirements seem ok */
    MyContext = MySettings->s_Context ;
    DOSBase = MyContext->c_DOSBase ;
    IconBase = MyContext->c_IconBase ;

    MyWBArg = MyWBStartup->sm_ArgList ;
    OldCurrentDir = CurrentDir( NULL ) ;
    for( ArgumentCount = 0 ; ArgumentCount < MyWBStartup->sm_NumArgs ; ArgumentCount++ )
    {  /* check all arguments */
      LogText( MyContext->c_Log, DEBUG_LEVEL, "wb argument %ld: %s", ArgumentCount, MyWBArg[ ArgumentCount ].wa_Name ) ;
      CurrentDir( MyWBArg[ ArgumentCount ].wa_Lock ) ;
      MyDiskObject = GetDiskObject( MyWBArg[ ArgumentCount ].wa_Name ) ;
      if( NULL != MyDiskObject )
      {  /* disk object ok */
        for( SettingCount = 0 ; SettingCount < NUM_SETTINGS ; SettingCount++ )
        {  /* check for tooltypes */
          ToolTypeValue = FindToolType( MyDiskObject->do_ToolTypes, MySettings->s_Descriptors[ SettingCount ].sd_Name ) ;
          if( NULL != ToolTypeValue )
          {  /* setting found in tool types */
            LogText( MyContext->c_Log, DEBUG_LEVEL, "found tooltype %s", MySettings->s_Descriptors[ SettingCount ].sd_Name ) ;
            switch( MySettings->s_Descriptors[ SettingCount ].sd_Type )
            {
              case SETTING_TYPE_STRING:  /*  */
                SetSetting( MySettings, SettingCount, ( SIPTR )ToolTypeValue ) ;
                break ;
              case SETTING_TYPE_NUMBER:  /*  */
                SetSetting( MySettings, SettingCount, atol( ToolTypeValue ) ) ;
                break ;
              case SETTING_TYPE_SWITCH:  /*  */
                SetSetting( MySettings, SettingCount, TRUE ) ;
                break ;
              default:  /* unknown type */
                break ;
            }
          }
        }
        if( 0 == ArgumentCount )
        {  /* first argument in the list is always the tool itself */
          ToolDiskObject = MyDiskObject ;
        }
        else
        {  /* some other icon */
          FreeDiskObject( MyDiskObject ) ;
        }
      }
    }
    CurrentDir( OldCurrentDir ) ;
    MySettings->s_Changed = 0 ;
  }
  else
  {  /* requirements not ok */
  }

  return( ToolDiskObject ) ;
}


/*
** set some defaults
*/
void SetDefaultSettings( struct Settings *MySettings )
{
  if( NULL != MySettings )
  {  /* requirements ok */
    SetSetting( MySettings, Nogui, FALSE ) ;
    SetSetting( MySettings, VhiDriver, ( SIPTR )"LIBS:vhi/camex.vhi" ) ;
    SetSetting( MySettings, VhiInput, 1 ) ;
    SetSetting( MySettings, VhiVideoformat, 0 ) ;
    SetSetting( MySettings, VhiColormode, TRUE ) ;
    SetSetting( MySettings, VhiX1, -1 ) ;
    SetSetting( MySettings, VhiY1, -1 ) ;
    SetSetting( MySettings, VhiX2, -1 ) ;
    SetSetting( MySettings, VhiY2, -1 ) ;
    SetSetting( MySettings, VhiWidth, 0 ) ;
    SetSetting( MySettings, VhiHeight, 0 ) ;
    SetSetting( MySettings, StampEnable, TRUE ) ;
    SetSetting( MySettings, StampText, ( SIPTR )"cAMIra %Y-%m-%d %H:%M:%S" ) ;
    SetSetting( MySettings, StampFont, ( SIPTR )"topaz.font" ) ;
    SetSetting( MySettings, StampFontsize, 8 ) ;
    SetSetting( MySettings, StampX, 10 ) ;
    SetSetting( MySettings, StampY, 10 ) ;
    SetSetting( MySettings, DisplayEnable, FALSE ) ;  /* in ms */
    SetSetting( MySettings, DisplayInterval, 1000 ) ;  /* in ms */
    SetSetting( MySettings, DisplayZoom, 0 ) ;  /* free zoom mode */
    SetSetting( MySettings, DisplayRatio, KEEP_RATIO ) ;
    SetSetting( MySettings, SchedulerInterval, 300 ) ;  /* in seconds */
    SetSetting( MySettings, CaptureEnable, FALSE ) ;
    SetSetting( MySettings, CaptureDrawer, ( SIPTR )"T:" ) ;
    SetSetting( MySettings, CaptureFile, ( SIPTR )"cAMIra_capture" ) ;
    SetSetting( MySettings, CaptureFormat, PNG_FORMAT ) ;
    SetSetting( MySettings, UploadEnable, FALSE ) ;
    SetSetting( MySettings, UploadPassive, TRUE ) ;
    SetSetting( MySettings, UploadServer, ( SIPTR )"ftpserver" ) ;
    SetSetting( MySettings, UploadTimeout, ( SIPTR )30 ) ;  /* in seconds */
    SetSetting( MySettings, UploadUser, ( SIPTR )"anonymous" ) ;
    SetSetting( MySettings, UploadPassword, ( SIPTR )"a@b.c" ) ;
    SetSetting( MySettings, UploadDrawer, ( SIPTR )"/" ) ;
    SetSetting( MySettings, UploadFile, ( SIPTR )"cAMIra_ftp" ) ;
    SetSetting( MySettings, UploadFormat, JPG_FORMAT ) ;
    SetSetting( MySettings, LogLevel, WARNING_LEVEL ) ;
    SetSetting( MySettings, JpegQuality, 90 ) ;
    SetSetting( MySettings, JpegProgressive, TRUE ) ;
    SetSetting( MySettings, PngCompression, 8 ) ;
    MySettings->s_Changed = 0 ;
  }
  else
  {  /* requirements not ok */
  }
}


/*
** expat parser start element handler
*/
static void StartElementHandler( void *UserData, const XML_Char *Element, const XML_Char **Attr )
{
  struct Settings *MySettings ;
  struct ExpatContext *MyExpat ;
  
  MySettings = UserData ;
  MyExpat = MySettings->s_ExpatContext ;

#if 0
  if( 0 < MyExpat->ec_Depth )
  {
    strcat( MyExpat->ec_Option, "_" ) ;
  }
#endif
  strcat( MyExpat->ec_Option, Element ) ;
  MyExpat->ec_Value[ 0 ] = '\0' ;
  MyExpat->ec_EndCount = 0 ;
  MyExpat->ec_Depth++;
}


/*
** expat parser end element handler
*/
static void EndElementHandler( void *UserData, const XML_Char *Element )
{
  struct Settings *MySettings ;
  struct ExpatContext *MyExpat ;
  
  MySettings = UserData ;
  MyExpat = MySettings->s_ExpatContext ;
  
  MyExpat->ec_Depth--;
  if( 0 == MyExpat->ec_EndCount )
  {  /* end element followed the start element, so end of constructed option */
    //printf( "option: %s = %s\n", MyExpat->ec_Option, MyExpat->ec_Value ) ;
    SetSettingWithStrings( MySettings, &MyExpat->ec_Option[ 6 ], MyExpat->ec_Value ) ;
  }
  MyExpat->ec_EndCount++ ;
#if 1
  *( strstr( MyExpat->ec_Option, Element ) ) = '\0' ;
#else
  *( strstr( MyExpat->ec_Option, Element ) - ( 0 < MyExpat->ec_Depth ) ) = '\0' ;
#endif
}


/*
** expat parser character data handler
*/
static void CharacterDataHandler( void *UserData, const XML_Char *Text, int Length )
{
  struct Settings *MySettings ;
  struct ExpatContext *MyExpat ;

  MySettings = UserData ;
  MyExpat = MySettings->s_ExpatContext ;

  strncat( MyExpat->ec_Value, Text, ( size_t )Length ) ;
}


/*
** read settings from a file
*/
static void ReadSettingsFile( struct Settings *MySettings, STRPTR Path )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct DosLibrary *DOSBase ;
  struct Library *ExpatBase ;
  XML_Parser ExpatParser ;

  MyContext = MySettings->s_Context ;
  SysBase = MyContext->c_SysBase ;
  DOSBase = MyContext->c_DOSBase ;
  ExpatBase = MyContext->c_ExpatBase ;

  if( ( NULL != ExpatBase ) && ( NULL != DOSBase ) )
  {  /* requirements ok */
    MySettings->s_ExpatContext = AllocVec( sizeof( struct ExpatContext ), ( MEMF_ANY ) ) ;
    if( NULL != MySettings->s_ExpatContext )
    {  /* expat context ok */
      MySettings->s_ExpatContext->ec_Depth = MySettings->s_ExpatContext->ec_Option[ 0 ] = 0 ;
      MySettings->s_File = Open( Path, MODE_OLDFILE ) ;
      if( NULL != MySettings->s_File )
      {  /* settings file ok */
        ExpatParser = XML_ParserCreate( NULL ) ;
        if( NULL != ExpatParser )
        {  /* expat parser ok */
          XML_SetUserData( ExpatParser, MySettings ) ;
          XML_SetElementHandler( ExpatParser, StartElementHandler, EndElementHandler ) ;
          XML_SetCharacterDataHandler( ExpatParser, CharacterDataHandler ) ;
          while( ( MySettings->s_FileBufferPosition = Read( MySettings->s_File, MySettings->s_FileBuffer, S_FILEBUFFER_LENGTH ) ) )
          {  /* until end of file */
            if( 0 == XML_Parse( ExpatParser, MySettings->s_FileBuffer, MySettings->s_FileBufferPosition, 0 ) )
            {  /* a parse error ocured */
              LogText( MyContext->c_Log, WARNING_LEVEL, "xml parse error, line: %d, reason: %s\n", XML_GetCurrentLineNumber( ExpatParser ), XML_ErrorString( XML_GetErrorCode( ExpatParser ) ) ) ;
              break ;
            }
            else
            {  /* parsing was ok */
            }
          }
          XML_Parse( ExpatParser, NULL, 0, 1 ) ;
          XML_ParserFree( ExpatParser ) ;
        }
        else
        {  /* expat parser not ok */
        }
        Close( MySettings->s_File ) ;
      }
      else
      {  /* settings file not ok */
        LogText( MyContext->c_Log, WARNING_LEVEL, "failed to open settings file: %s", Path ) ;
      }
      FreeVec( MySettings->s_ExpatContext ) ;
    }
    else
    {  /* expat context not ok */
      LogText( MyContext->c_Log, WARNING_LEVEL, "expat context not ok" ) ;
    }
  }
  else
  {  /* requirements not ok */
    LogText( MyContext->c_Log, WARNING_LEVEL, "requirements for reading settings file not ok" ) ;
  }
}


/*
**
*/
static void FlushFileBuffer( struct Settings *MySettings )
{
  struct Context *MyContext ;
  struct DosLibrary *DOSBase ;

  MyContext = MySettings->s_Context ;
  DOSBase = MyContext->c_DOSBase ;

  Write( MySettings->s_File, MySettings->s_FileBuffer, MySettings->s_FileBufferPosition ) ;
  MySettings->s_FileBufferPosition = 0 ;
}


/*
**
*/
static void WriteFileBuffer( struct Settings *MySettings, UBYTE *Data, LONG Lenght )
{
  LONG Count ;

  for( Count = 0 ; Count < Lenght ; Count++ )
  {
    MySettings->s_FileBuffer[ MySettings->s_FileBufferPosition ] = Data[ Count ] ;
    MySettings->s_FileBufferPosition++ ;
    if( S_FILEBUFFER_LENGTH <= MySettings->s_FileBufferPosition )
    {
      FlushFileBuffer( MySettings ) ;
    }
  }
}


/*
** write settings to a file
*/
static void WriteSettingsFile( struct Settings *MySettings, STRPTR Path )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct DosLibrary *DOSBase ;
  STRPTR CurrentName, PreviousName ;
  LONG MatchEndIndex, OldMatchEndIndex ;
  ULONG SettingIndex, Level, Count ;

  MyContext = MySettings->s_Context ;
  SysBase = MyContext->c_SysBase ;
  DOSBase = MyContext->c_DOSBase ;

  if( NULL != DOSBase )
  {  /* requirements ok */
    MySettings->s_File = Open( Path, MODE_NEWFILE ) ;
    if( NULL != MySettings->s_File )
    {  /* settings file ok */
      MySettings->s_FileBufferPosition = 0 ;
      WriteFileBuffer( MySettings, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n", 44 ) ; 
      WriteFileBuffer( MySettings, "<cAMIra>\n", 9 ) ;
      /* write settings to a xml file */
      SettingIndex = 0 ;  /* counts up to NUM_SETTINGS */
      Level = 0 ;  /* indent in written xml file structure */
      CurrentName = MySettings->s_Descriptors[ SettingIndex ].sd_Name ;
      PreviousName = NULL ;
      MatchEndIndex = 0 ;
      do
      {
        OldMatchEndIndex = MatchEndIndex ;
        if( ( NUM_SETTINGS > SettingIndex ) && ( ( 0 == MatchEndIndex ) || ( 0 == strncmp( CurrentName, PreviousName, ( size_t )( MatchEndIndex - 1 ) ) ) ) )
        {  /* print element or increase level */
          do
          {  /* as long as we find a \0 char */
            MatchEndIndex++ ;
            if( '\0' == CurrentName[ MatchEndIndex ] )
            {  /* found null char, so found elemet */
              Level++ ;
              break ;
            }
            else if( isupper( CurrentName[ MatchEndIndex ] ) )
            {  /* found upper case char, so inrease level */
              Level++ ;
              for( Count = 0 ; Count < ( 2 * Level ) ; Count++ ) WriteFileBuffer( MySettings, " ", 1 ) ;
              WriteFileBuffer( MySettings, "<", 1 ) ;
              WriteFileBuffer( MySettings, &CurrentName[ OldMatchEndIndex ], MatchEndIndex - OldMatchEndIndex ) ;
              WriteFileBuffer( MySettings, ">\n", 2 ) ;
              OldMatchEndIndex = MatchEndIndex ;
            }
          } while( 1 ) ;
          for( Count = 0 ; Count < ( 2 * Level ) ; Count++ ) WriteFileBuffer( MySettings, " ", 1 ) ;
          WriteFileBuffer( MySettings, "<", 1 ) ;
          WriteFileBuffer( MySettings, &CurrentName[ OldMatchEndIndex ], MatchEndIndex - OldMatchEndIndex ) ;
          WriteFileBuffer( MySettings, ">", 1 ) ;
          GetSettingAsString( MySettings, SettingIndex, MySettings->s_ValueString, S_VALUESTRING_LENGTH ) ;
          WriteFileBuffer( MySettings, MySettings->s_ValueString, ( SIPTR )strlen( MySettings->s_ValueString ) ) ;
          WriteFileBuffer( MySettings, "</", 2 ) ;
          WriteFileBuffer( MySettings, &CurrentName[ OldMatchEndIndex ], MatchEndIndex - OldMatchEndIndex ) ;
          WriteFileBuffer( MySettings, ">\n", 2 ) ;
          Level-- ;
          MatchEndIndex = OldMatchEndIndex ;
          SettingIndex++ ;
          PreviousName = CurrentName ;
          CurrentName = MySettings->s_Descriptors[ SettingIndex ].sd_Name ;
        }
        else
        {  /* decrease level */
          do
          {  /* look for the previous upper case char in previous name */
            MatchEndIndex-- ;
          }
          while( !( isupper( PreviousName[ MatchEndIndex ] ) ) ) ;
          for( Count = 0 ; Count < ( 2 * Level ) ; Count++ ) WriteFileBuffer( MySettings, " ", 1 ) ;
          WriteFileBuffer( MySettings, "</", 2 ) ;
          WriteFileBuffer( MySettings, &PreviousName[ MatchEndIndex ], OldMatchEndIndex - MatchEndIndex ) ;
          WriteFileBuffer( MySettings, ">\n", 2 ) ;
          Level-- ;
          OldMatchEndIndex = MatchEndIndex ;
        }
      } while( ( 0 < Level ) || ( NUM_SETTINGS > SettingIndex ) ) ;
      WriteFileBuffer( MySettings, "</cAMIra>\n", 10 ) ;
      FlushFileBuffer( MySettings ) ;
      Close( MySettings->s_File ) ;
    }
    else
    {  /* settings file not ok */
    }
  }
  else
  {  /* requirements not ok */
  }
}


/*
** load settings
*/
void LoadSettings( struct Settings *MySettings )
{
  if( NULL != MySettings )
  {  /* requirements ok */
    ReadSettingsFile( MySettings, "ENV:cAMIra.xml" ) ;
    MySettings->s_Changed = 0 ;
  }
  else
  {  /* requirements not ok */
  }
}


/*
** save settings
*/
void SaveSettings( struct Settings *MySettings )
{
  if( NULL != MySettings )
  {  /* requirements ok */
    if( MySettings->s_Changed )
    {  /* settinges changed since last save */
      WriteSettingsFile( MySettings, "ENVARC:cAMIra.xml" ) ;
      MySettings->s_Changed = 0 ;
    }
    else
    {  /* settinges did not change since last save */
    }
  }
  else
  {  /* requirements not ok */
  }
}


/*
** use settings
*/
void UseSettings( struct Settings *MySettings )
{
  if( NULL != MySettings )
  {  /* requirements ok */
    WriteSettingsFile( MySettings, "ENV:cAMIra.xml" ) ;
  }
  else
  {  /* requirements not ok */
  }
}


/*
** dump settings for debugging purpose
*/
void PrintSettings( struct Settings *MySettings )
{
  struct Context *MyContext ;
  ULONG Count ;

  if( NULL != MySettings )
  {  /* requirements seem ok */
    MyContext = MySettings->s_Context ;
    for( Count = 0 ; Count < NUM_SETTINGS ; Count++ )
    {  /* print all */
      switch( MySettings->s_Descriptors[ Count ].sd_Type )
      {  /* what we print depends on type */
        case SETTING_TYPE_STRING:  /* here we print a string */
          LogText( MyContext->c_Log, DEBUG_LEVEL, "%s: %s\n", MySettings->s_Descriptors[ Count ].sd_Name, ( char * )MySettings->s_Current[ Count ] ) ;
          break ;
        case SETTING_TYPE_NUMBER:  /* here we print a number */
          LogText( MyContext->c_Log, DEBUG_LEVEL, "%s: %ld\n", MySettings->s_Descriptors[ Count ].sd_Name, MySettings->s_Current[ Count ] ) ;
          break ;
        case SETTING_TYPE_SWITCH:  /* here we print TRUE or FALSE */
          LogText( MyContext->c_Log, DEBUG_LEVEL, "%s: %s\n", MySettings->s_Descriptors[ Count ].sd_Name, MySettings->s_Current[ Count ] ? "TRUE" : "FALSE" ) ;
          break ;
        default:  /* unknown type */
          break ;
      }
    }
  }
}
