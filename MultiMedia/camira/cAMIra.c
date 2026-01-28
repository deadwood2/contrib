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
** cAMIra.c
*/


#include "cAMIra.h"
#include "Breaker.h"
#include "Dispatcher.h"
#include "Settings.h"
#include "Localization.h"
#include "MemoryManager.h"
#include "VideoHardware.h"
#include "ImageProcessor.h"
#include "Stamp.h"
#include "Magic.h"
#include "MainWindow.h"
#include "DisplayArea.h"
#include "ProgressBar.h"
#include "Timer.h"
#include "DosManager.h"
#include "Scheduler.h"
#include "Capture.h"
#include "Upload.h"
#include "SettingsWindow.h"
#include "AboutWindow.h"
#include "Log.h"
#include "LogWindow.h"
#include <proto/exec.h>
#include <proto/icon.h>
#include <dos/dos.h>
#include <clib/alib_protos.h>


/*
** close our libraries
*/
static void CloseLibraries( struct Context *MyContext )
{
  struct ExecBase *SysBase ;

  SysBase = MyContext->c_SysBase ;
  
  if( NULL != MyContext->c_GuiGFXBase )
  {  /* guigfx library needs to be closed */
    CloseLibrary( ( struct Library * )MyContext->c_GuiGFXBase ) ;
    MyContext->c_GuiGFXBase = NULL ;
  }
  if( NULL != MyContext->c_MUIMasterBase )
  {  /* muimaster library needs to be closed */
    CloseLibrary( ( struct Library * )MyContext->c_MUIMasterBase ) ;
    MyContext->c_MUIMasterBase = NULL ;
  }
  if( NULL != MyContext->c_ExpatBase )
  {  /* jpeg library needs to be closed */
    CloseLibrary( ( struct Library * )MyContext->c_ExpatBase ) ;
    MyContext->c_ExpatBase = NULL ;
  }
  if( NULL != MyContext->c_UtilityBase )
  {  /* utility library needs to be closed */
    CloseLibrary( ( struct Library * )MyContext->c_UtilityBase ) ;
    MyContext->c_UtilityBase = NULL ;
  }
  if( NULL != MyContext->c_DiskfontBase )
  {  /* diskfont library needs to be closed */
    CloseLibrary( ( struct Library * )MyContext->c_DiskfontBase ) ;
    MyContext->c_DiskfontBase = NULL ;
  }
  if( NULL != MyContext->c_GfxBase )
  {  /* graphics library needs to be closed */
    CloseLibrary( ( struct Library * )MyContext->c_GfxBase ) ;
    MyContext->c_GfxBase = NULL ;
  }
  if( NULL != MyContext->c_IconBase )
  {  /* icon library needs to be closed */
    CloseLibrary( ( struct Library * )MyContext->c_IconBase ) ;
    MyContext->c_IconBase = NULL ;
  }
  if( NULL != MyContext->c_IntuitionBase )
  {  /* intuition library needs to be closed */
    CloseLibrary( ( struct Library * )MyContext->c_IntuitionBase ) ;
    MyContext->c_IntuitionBase = NULL ;
  }
  if( NULL != MyContext->c_DOSBase )
  {  /* dos library needs to be closed */
    CloseLibrary( ( struct Library * )MyContext->c_DOSBase ) ;
    MyContext->c_DOSBase = NULL ;
  }

  return ;
}


/*
** try to open our libraries
*/
static LONG OpenLibraries( struct Context *MyContext )
{
  struct ExecBase *SysBase ;
  LONG Error ;

  SysBase = MyContext->c_SysBase ;
  Error = 1 ;

  MyContext->c_DOSBase = ( struct DosLibrary * )OpenLibrary( "dos.library", 36 ) ;
  if( NULL != MyContext->c_DOSBase )
  {  /* dos library ok */
    MyContext->c_IntuitionBase = ( struct IntuitionBase * )OpenLibrary( "intuition.library", 0 ) ;
    if( NULL != MyContext->c_IntuitionBase )
    {  /* intuition library ok */
      MyContext->c_UtilityBase = OpenLibrary( "utility.library", 0 ) ;
      if( NULL != MyContext->c_UtilityBase )
      {  /* utility library ok */
        MyContext->c_GfxBase = ( struct GfxBase * )OpenLibrary( "graphics.library", 0 ) ;
        if( NULL != MyContext->c_GfxBase )
        {  /* graphics library ok */
          MyContext->c_IconBase = OpenLibrary( "icon.library", 0 ) ;
          if( NULL != MyContext->c_IconBase )
          {  /* icon library ok */
            MyContext->c_DiskfontBase = OpenLibrary( "diskfont.library", 0 ) ;
            if( NULL != MyContext->c_DiskfontBase )
            {  /* diskfont library ok */
              MyContext->c_ExpatBase = OpenLibrary( "expat.library", 0 ) ;
              if( NULL == MyContext->c_ExpatBase )
              {  /* expat library not ok */
                LogText( MyContext->c_Log, WARNING_LEVEL, "expat library not ok" ) ;
              }
              MyContext->c_MUIMasterBase = OpenLibrary( "muimaster.library", 0 ) ;
              if( NULL == MyContext->c_MUIMasterBase )
              {  /* muimaster library not ok */
                LogText( MyContext->c_Log, WARNING_LEVEL, "muimaster library not ok" ) ;
              }
              MyContext->c_GuiGFXBase = OpenLibrary( "guigfx.library", 0 ) ;
              if( NULL == MyContext->c_GuiGFXBase )
              {  /* guigfx library not ok */
                LogText( MyContext->c_Log, WARNING_LEVEL, "guigfx library not ok" ) ;
              }
              Error = 0 ;
            }
            else
            {  /* diskfont library not ok */
              LogText( MyContext->c_Log, ERROR_LEVEL, "diskfont library not ok" ) ;
              CloseLibraries( MyContext ) ;
            }
          }
          else
          {  /* icon library not ok */
            LogText( MyContext->c_Log, ERROR_LEVEL, "icon library not ok" ) ;
            CloseLibraries( MyContext ) ;
          }
        }
        else
        {  /* graphics library not ok */
          LogText( MyContext->c_Log, ERROR_LEVEL, "graphics library not ok" ) ;
          CloseLibraries( MyContext ) ;
        }
      }
      else
      {  /* utility library not ok */
        LogText( MyContext->c_Log, ERROR_LEVEL, "utility library not ok" ) ;
        CloseLibraries( MyContext ) ;
      }
    }
    else
    {  /* intuition library not ok */
      LogText( MyContext->c_Log, ERROR_LEVEL, "intuition library not ok" ) ;
      CloseLibraries( MyContext ) ;
    }
  }
  else
  {  /* dos library not ok */
    LogText( MyContext->c_Log, ERROR_LEVEL, "dos library not ok" ) ;
  }

  return ( Error ) ;
}


/*
** the main loop
*/
LONG cAMIraMain( struct Context *MyContext, enum DispatchMode MyMode  )
{
  LONG Error ;

  Dispatcher( MyContext->c_Dispatcher, MyMode ) ;  /* dispatch blocking */

  if( MyContext->c_ShutdownRequest )
  {  /* check if it is save to shutdown */
    /* cancel all requests so far */
    MyContext->c_DisplayRequest = 0 ;
    MyContext->c_CaptureRequest = 0 ;
    MyContext->c_UploadRequest = 0 ;
    if( ( 0 >= CheckVideoHardware( MyContext->c_VideoHardware ) ) &&
        ( 0 >= CheckCapture( MyContext->c_Capture ) ) )
    {  /* yes, we can shut down */
      MyContext->c_Shutdown = 1 ;
    }
  }

  if( MyContext->c_ConfigureRequest )
  {  /* settings changed in some way */
    UseSettings( MyContext->c_Settings ) ;
    ConfigureLog( MyContext->c_Log ) ;
    ConfigureVideoHardware( MyContext->c_VideoHardware ) ;
    ConfigureStamp( MyContext->c_Stamp ) ;
    ConfigureDisplayArea( MyContext->c_DisplayArea ) ;
    ConfigureScheduler( MyContext->c_Scheduler ) ;
    ConfigureCapture( MyContext->c_Capture ) ;
    ConfigureUpload( MyContext->c_Upload ) ;
    ConfigureImageProcessor( MyContext->c_ImageProcessor ) ;
    MyContext->c_ConfigureRequest = 0 ;
  }

  if( MyContext->c_LogRequest )
  {  /* log request */
    UnlockLog( MyContext->c_Log ) ;
    MyContext->c_LogRequest = 0 ;
  }
  
  if( MyContext->c_ReleaseRequest )
  {  /* release request */
    StartCapture( MyContext->c_Capture, GetDisplayImage( MyContext->c_DisplayArea ) ) ;
    MyContext->c_ReleaseRequest = 0 ;
  }

  if( ( MyContext->c_DisplayRequest ) || ( MyContext->c_CaptureRequest ) || ( MyContext->c_UploadRequest ) )
  {  /* some request that requires a new image */
    if( NULL != MyContext->c_NewImageBox )
    {  /* there is a new image box */
      if( ( NULL != MyContext->c_NewImageBox->ib_VhiImage ) || ( 0 < MyContext->c_NewImageBox->ib_UseCounter ) )
      {  /* ... with a digitized image in the new box */
        if( 0 == MyContext->c_NewImageBox->ib_UseCounter )
        {  /* stamp only if still unused */
          StampImage( MyContext->c_Stamp, MyContext->c_NewImageBox ) ;
        }
        MyContext->c_NewImageBox->ib_UseCounter++ ;
        if( MyContext->c_DisplayRequest )
        {  /* we shall display it */
          MyContext->c_DisplayRequest = 0 ;
          DisplayImage( MyContext->c_DisplayArea, MyContext->c_NewImageBox ) ;
        }
        if( MyContext->c_CaptureRequest )
        {  /* we shall capture it */
          MyContext->c_CaptureRequest = 0 ;
          Error = StartCapture( MyContext->c_Capture, MyContext->c_NewImageBox ) ;
          if( ( Error ) )
          {  /* starting capture failed */
            LogText( MyContext->c_Log, FAILURE_LEVEL, "starting capture failed" ) ;
          }
        }
        if( MyContext->c_UploadRequest )
        {  /* we shall upload it */
          MyContext->c_UploadRequest = 0 ;
          Error = StartUpload( MyContext->c_Upload, MyContext->c_NewImageBox ) ;
          if( ( Error ) )
          {  /* starting upload failed */
            LogText( MyContext->c_Log, FAILURE_LEVEL, "starting upload failed" ) ;
          }
        }
      }
      else
      {  /* ... with no digitized image in the new box */
        if( MyContext->c_NewImageBox->ib_Error )
        {  /* ... because of some digitize error */
          MyContext->c_DisplayRequest = 0 ;
          MyContext->c_CaptureRequest = 0 ;
          MyContext->c_UploadRequest = 0 ;
        }
        else
        {  /* because of some other unkown reason, try resetting hardware */
          CloseVideoHardware( MyContext->c_VideoHardware ) ;
          MyContext->c_ConfigureRequest = 1 ;
        }
      }
      MyContext->c_NewImageBox->ib_UseCounter-- ;
      DeleteImageBox( MyContext, MyContext->c_NewImageBox ) ;
      MyContext->c_NewImageBox = NULL ;
    }
    else
    {  /* there is no new image */
      Error = CheckVideoHardware( MyContext->c_VideoHardware ) ;
      if( !( Error ) )
      {  /* video hardware is ready */
        MyContext->c_NewImageBox = CreateImageBox( MyContext ) ;
        if( NULL != MyContext->c_NewImageBox )
        {  /* a box to put the digitized image in is ok */
          DigitizeImage( MyContext->c_VideoHardware, MyContext->c_NewImageBox ) ;
          MyContext->c_NewImageBox = NULL ;
        }
      }
      else if( 0 > Error )
      {  /* video hardware not available */
        LogText( MyContext->c_Log, DEBUG_LEVEL, "video hardware not available" ) ;
        MyContext->c_DisplayRequest = 0 ;
        MyContext->c_CaptureRequest = 0 ;
        MyContext->c_UploadRequest = 0 ;
      }
      else
      {  /* video hardware busy */
      }
    }
  }
  else
  {  /* no request that requires a new image */
    if( NULL != MyContext->c_NewImageBox )
    {  /* but there is a new image */
      DeleteImageBox( MyContext, MyContext->c_NewImageBox ) ;
      MyContext->c_NewImageBox = NULL ;
    }
  }

  return( MyContext->c_Shutdown ) ;
}


/*
** common entry
*/
static LONG cAMIraEntry( int argc, char *argv[] )
{
#if !defined(__AROS__)
  struct ExecBase *SysBase ;
#endif
  struct Context *MyContext ;
  struct Library *IconBase ;
  LONG Result ;
  LONG Error ;

#if !defined(__AROS__)
  SysBase = *( struct ExecBase ** )4 ;
#endif
  Result = RETURN_OK ;

  MyContext = CreateContext( SysBase ) ;
  if( NULL != MyContext )
  {  /* memory for context ok */
    MyContext->c_SysBase = SysBase ;
    //LogText( NULL, ERROR_LEVEL, "test error on serial console" ) ;
    MyContext->c_Log = CreateLog( MyContext ) ;
    MyContext->c_Dispatcher = CreateDispatcher( MyContext ) ;
    if( NULL != MyContext->c_Dispatcher )
    {  /* dispatcher ok */
      MyContext->c_Breaker = CreateBreaker( MyContext ) ;
      if( NULL != MyContext->c_Breaker )
      {  /* breaker ok */
        MyContext->c_Timer = CreateTimer( MyContext ) ;
        if( NULL != MyContext->c_Timer )
        {  /* timer ok */
          Error = OpenLibraries( MyContext ) ;
          if( !( Error ) )
          {  /* essential libraries ok */
            IconBase = MyContext->c_IconBase ;
            MyContext->c_Settings = CreateSettings( MyContext ) ;
            if( NULL != MyContext->c_Settings )
            {  /* settings are ok */
              SetDefaultSettings( MyContext->c_Settings ) ;
              LoadSettings( MyContext->c_Settings ) ;
              if( 0 == argc )
              {  /* start from workbench */
                MyContext->c_DiskObject = ParseToolTypes( MyContext->c_Settings, ( struct WBStartup * )argv ) ;
              }
              else
              {  /* start from cli */
                MyContext->c_DiskObject = ParseParameters( MyContext->c_Settings, argc, argv ) ;
              }
              ConfigureLog( MyContext->c_Log ) ;
              //LogText( MyContext->c_Log, ERROR_LEVEL, "test error after configuring log: 0x%p", MyContext ) ;
              MyContext->c_Localization = CreateLocalization( MyContext ) ;
              MyContext->c_MemoryManager = CreateMemoryManager( MyContext ) ;
              ConfigureVideoHardware( MyContext->c_VideoHardware = CreateVideoHardware( MyContext ) ) ;
              ConfigureImageProcessor( MyContext->c_ImageProcessor = CreateImageProcessor( MyContext ) ) ;
              ConfigureStamp( MyContext->c_Stamp = CreateStamp( MyContext ) ) ;
              MyContext->c_Magic = CreateMagic( MyContext ) ;
              MyContext->c_MainWindow = CreateMainWindow( MyContext ) ;
              ConfigureDisplayArea( MyContext->c_DisplayArea = CreateDisplayArea( MyContext ) ) ;
              MyContext->c_SettingsWindow = CreateSettingsWindow( MyContext ) ;
              MyContext->c_AboutWindow = CreateAboutWindow( MyContext ) ;
              MyContext->c_LogWindow = CreateLogWindow( MyContext ) ;
              MyContext->c_ProgressBar = CreateProgressBar( MyContext ) ;
              MyContext->c_DosManager = CreateDosManager( MyContext ) ;
              ConfigureScheduler( MyContext->c_Scheduler = CreateScheduler( MyContext ) ) ;
              ConfigureCapture( MyContext->c_Capture = CreateCapture( MyContext ) ) ;
              ConfigureUpload( MyContext->c_Upload = CreateUpload( MyContext ) ) ;

              LogText( MyContext->c_Log, INFO_LEVEL, "cAMIra start" ) ;
              OpenMainWindow( MyContext->c_MainWindow ) ;
              while( !( MyContext->c_Shutdown ) )
              {  /* as long as it goes ... */
                Result = cAMIraMain( MyContext, BLOCKING_MODE ) ;
              }
              CloseMainWindow( MyContext->c_MainWindow ) ;
              SaveSettings( MyContext->c_Settings ) ;
              LogText( MyContext->c_Log, INFO_LEVEL, "cAMIra stop" ) ;

              DeleteUpload( MyContext->c_Upload ) ;
              MyContext->c_Upload = NULL ;
              DeleteCapture( MyContext->c_Capture ) ;
              MyContext->c_Capture = NULL ;
              DeleteScheduler( MyContext->c_Scheduler ) ;
              MyContext->c_Scheduler = NULL ;
              DeleteDosManager( MyContext->c_DosManager ) ;
              MyContext->c_DosManager = NULL ;
              DeleteProgressBar( MyContext->c_ProgressBar ) ;
              MyContext->c_ProgressBar = NULL ;
              DeleteLogWindow( MyContext->c_LogWindow ) ;
              MyContext->c_LogWindow = NULL ;
              DeleteAboutWindow( MyContext->c_AboutWindow ) ;
              MyContext->c_AboutWindow = NULL ;
              DeleteSettingsWindow( MyContext->c_SettingsWindow ) ;
              MyContext->c_SettingsWindow = NULL ;
              DeleteDisplayArea( MyContext->c_DisplayArea ) ;
              MyContext->c_DisplayArea = NULL ;
              DeleteMainWindow( MyContext->c_MainWindow ) ;
              MyContext->c_MainWindow = NULL ;
              DeleteMagic( MyContext->c_Magic ) ;
              MyContext->c_Magic = NULL ;
              DeleteStamp( MyContext->c_Stamp ) ;
              MyContext->c_Stamp = NULL ;
              DeleteImageProcessor( MyContext->c_ImageProcessor ) ;
              MyContext->c_ImageProcessor = NULL ;
              DeleteVideoHardware( MyContext->c_VideoHardware ) ;
              MyContext->c_VideoHardware = NULL ;
              DeleteMemoryManager( MyContext->c_MemoryManager ) ;
              MyContext->c_MemoryManager= NULL ;
              DeleteLocalization( MyContext->c_Localization ) ;
              MyContext->c_Localization = NULL ;
              if( NULL != MyContext->c_DiskObject )
              {  /* disk object needs to be freed */
                FreeDiskObject( MyContext->c_DiskObject ) ;
                MyContext->c_DiskObject = NULL ;
              }
              DeleteSettings( MyContext->c_Settings ) ;
              MyContext->c_Settings = NULL ;
            }
            else
            {  /* settings are not ok */
              LogText( MyContext->c_Log, ERROR_LEVEL, "settings not ok" ) ;
              Result = RETURN_ERROR ;
            }
            CloseLibraries( MyContext ) ;
          }
          else
          {  /* essential libraries not ok */
            Result = RETURN_ERROR ;
          }
          DeleteTimer( MyContext->c_Timer ) ;
          MyContext->c_Timer = NULL ;
        }
        else
        {  /* timer not ok */
          LogText( MyContext->c_Log, ERROR_LEVEL, "timer not ok" ) ;
          Result = RETURN_ERROR ;
        }
        DeleteBreaker( MyContext->c_Breaker ) ;
        MyContext->c_Breaker = NULL ;
      }
      else
      {  /* breaker not ok */
        LogText( MyContext->c_Log, ERROR_LEVEL, "breaker not ok" ) ;
        Result = RETURN_ERROR ;
      }
      DeleteDispatcher( MyContext->c_Dispatcher ) ;
      MyContext->c_Dispatcher = NULL ;
    }
    else
    {  /* dispatcher not ok */
      LogText( MyContext->c_Log, ERROR_LEVEL, "dispatcher not ok" ) ;
      Result = RETURN_ERROR ;
    }
    DeleteLog( MyContext->c_Log ) ;
    MyContext->c_Log = NULL ;
    DeleteContext( MyContext ) ;
  }
  else
  {  /* memory for context not ok */
  }

  return( Result ) ;
}


#if 0
#define TESTBUFFER_LENGHT ( 48 )
char TestBuffer[ TESTBUFFER_LENGHT ] ;
int TestLength ;
static void snprintf_test( void )
{
  const char* s = "Hello";
  TestLength = snprintf( TestBuffer, TESTBUFFER_LENGHT, "Strings:\n" ) ;
  printf( "%s; %d\n", TestBuffer, TestLength ) ;
  TestLength = snprintf( TestBuffer, TESTBUFFER_LENGHT, "\t.%10s.\n\t.%-10s.\n\t.%*s.\n", s, s, 10, s);
  printf( "%s; %d\n", TestBuffer, TestLength ) ;
  TestLength = snprintf( TestBuffer, 11, "\t.%10s.\n\t.%-10s.\n\t.%*s.\n", s, s, 10, s);
  printf( "%s; %d\n", TestBuffer, TestLength ) ;

  TestLength = snprintf( TestBuffer, TESTBUFFER_LENGHT, "Characters:\t%c %%\n", 65);
  printf( "%s; %d\n", TestBuffer, TestLength ) ;

  TestLength = snprintf( TestBuffer, TESTBUFFER_LENGHT, "Integers:\n" ) ;
  printf( "%s; %d\n", TestBuffer, TestLength ) ;
  TestLength = snprintf( TestBuffer, TESTBUFFER_LENGHT, "Decimal:\t%i %d %.6i %i %.0i %+i %u\n", 1, 2, 3, 0, 0, 4, -1);
  printf( "%s; %d\n", TestBuffer, TestLength ) ;
  TestLength = snprintf( TestBuffer, TESTBUFFER_LENGHT, "Hexadecimal:\t%x %x %X %#x\n", 5, 10, 10, 6);
  printf( "%s; %d\n", TestBuffer, TestLength ) ;
  
  TestLength = snprintf( TestBuffer, TESTBUFFER_LENGHT, "Hello!" ) ;
  printf( "%s; %d\n", TestBuffer, TestLength ) ;
  TestLength = snprintf( TestBuffer, 3, "Hello!" ) ;
  printf( "%s; %d\n", TestBuffer, TestLength ) ;
  TestLength = snprintf( TestBuffer, TESTBUFFER_LENGHT, "%s", "Hello!" ) ;
  printf( "%s; %d\n", TestBuffer, TestLength ) ;
  TestLength = snprintf( TestBuffer, 3, "%s", "Hello!" ) ;
  printf( "%s; %d\n", TestBuffer, TestLength ) ;
  TestLength = snprintf( TestBuffer, TESTBUFFER_LENGHT, "%+016d", -123456789 ) ;
  printf( "%s; %d\n", TestBuffer, TestLength ) ;
  TestLength = snprintf( TestBuffer, 10, "%-+016d", -123456789 ) ;
  printf( "%s; %d\n", TestBuffer, TestLength ) ;
}
#endif


/*
** called by strom startup when launched from cli or wbmain
*/
int main( int argc, char *argv[] )
{
  //snprintf_test( ) ;
  //return( 0 ) ;
  return( cAMIraEntry( argc, argv ) ) ;
}


/*
** called by strom startup when launched from workbench
*/
void wbmain( struct WBStartup *MyWBStartup )
{
  cAMIraEntry( 0, ( char ** )MyWBStartup ) ;
}
