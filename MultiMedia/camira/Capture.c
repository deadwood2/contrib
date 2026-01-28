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
** Capture.c
*/


#include "Capture.h"
#include "cAMIraLib.h"
#include "Settings.h"
#include "Log.h"
#include "ab_stdio.h"
#include <proto/exec.h>
#include <proto/dos.h>
#include <clib/alib_protos.h>
#include <string.h>


/*
** delete capture
*/
void DeleteCapture( struct Capture *OldCapture )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
 
  if( NULL != OldCapture )
  {  /* capture context needs to be freed */
    MyContext = OldCapture->c_Context ;
    SysBase = MyContext->c_SysBase ;
    
    StopCapture( OldCapture ) ;
    FreeVec( OldCapture ) ;
  }
}


/*
** dos manager callback of the capture process
*/
static void DoCapture( struct Capture *MyCapture )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct DosLibrary *DOSBase ;
  struct ExpandData MyExpandData ;
  BOOL Repeat ;
  BPTR MyLock ;

  if( NULL != MyCapture )
  {  /* requirements ok */
    MyContext = MyCapture->c_Context ;
    SysBase = MyContext->c_SysBase ;
    DOSBase = MyContext->c_DOSBase ;

    do
    {
      Repeat = FALSE ;
      switch( MyCapture->c_State )
      {
        case CAPTURE_START:  /* starting point of the capture state machine */
          MyCapture->c_RelativePath = strchr( MyCapture->c_CurrentDrawer, ':' ) ;
          if( NULL != MyCapture->c_RelativePath )
          {  /* drawer with volume or assign part */
            MyCapture->c_RelativePath++ ;
          }
          else
          {  /* drawer is relative */
            MyCapture->c_RelativePath = MyCapture->c_CurrentDrawer ;
          }
          if( !( BeforeLock( MyContext->c_DosManager, MyCapture->c_DosNode, MyCapture->c_RelativePath, SHARED_LOCK ) ) )
          {  /* locking the drawer prepared */
            SendDosNode( MyContext->c_DosManager, MyCapture->c_DosNode ) ;
            MyCapture->c_State = CAPTURE_LOCKDRAWER ;
          }
          else
          {  /* locking the drawer not prepared */
          }
          break ; 
        case CAPTURE_LOCKDRAWER:  /* check if we could lock the drawer */
          MyLock = AfterLock( MyContext->c_DosManager, MyCapture->c_DosNode, &MyCapture->c_Error ) ;
          if( NULL != MyLock )
          {  /* locking the drawer ok */
            MyCapture->c_CurrentImageBox->ib_SequenceNumber = MyCapture->c_SequenceCounter ;
            MyExpandData.ed_ClockData = &MyCapture->c_CurrentImageBox->ib_ClockData ;
            MyExpandData.ed_Number = &MyCapture->c_CurrentImageBox->ib_SequenceNumber ;
            ExpandString( MyCapture->c_FileName, CAPTURE_FILENAME_LENGTH, MyCapture->c_CurrentCaptureFileTemplate, &MyExpandData ) ;
            MyCapture->c_RelativePath = MyCapture->c_FileName + ( '/' == MyCapture->c_FileName[ 0 ] ) ;
            MyLock = CurrentDirDosNode( MyContext->c_DosManager, MyCapture->c_DosNode, MyLock ) ;
            if( !( BeforeUnLock( MyContext->c_DosManager, MyCapture->c_DosNode, MyLock ) ) )
            {  /* unlock prepared */
              SendDosNode( MyContext->c_DosManager, MyCapture->c_DosNode ) ;
              MyCapture->c_State = CAPTURE_UNLOCKDIR ;
            }
            else
            {  /* unlock not prepared */
            }
          }
          else
          {  /* locking the drawer not ok */
            LogText( MyContext->c_Log, ERROR_LEVEL, "capture drawer lock error: %ld (%s)", MyCapture->c_Error, DosManagerFault( MyContext->c_DosManager, MyCapture->c_Error ) ) ;
            StopCapture( MyCapture ) ;
          }
          break ;
        case CAPTURE_UNLOCKDIR:  /* after unlocking a previous current dir, go down further or use the current dir */
          if( AfterUnLock( MyContext->c_DosManager, MyCapture->c_DosNode, &MyCapture->c_Error ) )
          {  /* unlock ok */
            MyCapture->c_NextRelativePath = strchr( MyCapture->c_RelativePath, '/' ) ;
            if( NULL != MyCapture->c_NextRelativePath )
            {  /* still more subfolders */
              MyCapture->c_NextRelativePath[ 0 ] = '\0' ;
              if( !( BeforeLock( MyContext->c_DosManager, MyCapture->c_DosNode, MyCapture->c_RelativePath, SHARED_LOCK ) ) )
              {  /* lock prepared */
                SendDosNode( MyContext->c_DosManager, MyCapture->c_DosNode ) ;
                MyCapture->c_State = CAPTURE_LOCKDIR ;
              }
              else
              {  /* lock not prepared */
              }
            }
            else
            {  /* no more subfolders */
              if( !( BeforeOpen( MyContext->c_DosManager, MyCapture->c_DosNode, MyCapture->c_RelativePath, MODE_OLDFILE ) ) )
              {  /* open prepared */
                SendDosNode( MyContext->c_DosManager, MyCapture->c_DosNode ) ;
                MyCapture->c_State = CAPTURE_OPENOLDFILE ;
              }
              else
              {  /* open not prepared */
              }
            }
          }
          else
          {  /* unlock not ok */
          }
          break ;
        case CAPTURE_LOCKDIR:  /* check if a subfolder could be locked, if not initiate creation */
          MyLock = AfterLock( MyContext->c_DosManager, MyCapture->c_DosNode, &MyCapture->c_Error ) ;
          if( NULL != MyLock )
          {  /* lock ok, so subfolder already exists */
            MyCapture->c_NextRelativePath[ 0 ] = '/' ;
            MyCapture->c_RelativePath = &MyCapture->c_NextRelativePath[ 1 ] ;
            MyLock = CurrentDirDosNode( MyContext->c_DosManager, MyCapture->c_DosNode, MyLock ) ;
            if( !( BeforeUnLock( MyContext->c_DosManager, MyCapture->c_DosNode, MyLock ) ) )
            {  /* unlock prepared */
              SendDosNode( MyContext->c_DosManager, MyCapture->c_DosNode ) ;
              MyCapture->c_State = CAPTURE_UNLOCKDIR ;
            }
            else
            {  /* unlock not prepared */
            }
          }
          else
          {  /* lock not ok, so subfolder needs to be created */
            if( !( BeforeCreateDir( MyContext->c_DosManager, MyCapture->c_DosNode, MyCapture->c_RelativePath ) ) )
            {  /* create dir prepared */
              SendDosNode( MyContext->c_DosManager, MyCapture->c_DosNode ) ;
              MyCapture->c_State = CAPTURE_CREATEDIR ;
            }
            else
            {  /* create dir not prepared */
            }
          }
          break ;
        case CAPTURE_CREATEDIR:
          MyLock = AfterCreateDir( MyContext->c_DosManager, MyCapture->c_DosNode, &MyCapture->c_Error ) ;
          if( NULL != MyLock )
          {  /* create dir ok */
            MyCapture->c_NextRelativePath[ 0 ] = '/' ;
            MyCapture->c_RelativePath = &MyCapture->c_NextRelativePath[ 1 ] ;
            MyLock = CurrentDirDosNode( MyContext->c_DosManager, MyCapture->c_DosNode, MyLock ) ;
            if( !( BeforeUnLock( MyContext->c_DosManager, MyCapture->c_DosNode, MyLock ) ) )
            {  /* unlock prepared */
              SendDosNode( MyContext->c_DosManager, MyCapture->c_DosNode ) ;
              MyCapture->c_State = CAPTURE_UNLOCKDIR ;
            }
            else
            {  /* unlock not prepared */
            }
          }
          else
          {  /* create dir not ok */
            LogText( MyContext->c_Log, ERROR_LEVEL, "capture create dir error: %ld (%s)", MyCapture->c_Error, DosManagerFault( MyContext->c_DosManager, MyCapture->c_Error ) ) ;
            StopCapture( MyCapture ) ;
          }
          break ;
        case CAPTURE_OPENOLDFILE:
          MyCapture->c_File = AfterOpen( MyContext->c_DosManager, MyCapture->c_DosNode, &MyCapture->c_Error ) ;
          if( NULL != MyCapture->c_File )
          {  /* file already exists */
            LogText( MyContext->c_Log, INFO_LEVEL, "capture file already exists: %s", MyCapture->c_FileName ) ;
            if( !( BeforeClose( MyContext->c_DosManager, MyCapture->c_DosNode, MyCapture->c_File ) ) )
            {  /* close prepared */
              SendDosNode( MyContext->c_DosManager, MyCapture->c_DosNode ) ;
              MyCapture->c_State = CAPTURE_CLOSEOLDFILE ;
            }
            else
            {  /* close not prepared */
            }
          }
          else
          {  /* file does not exist */
            switch( MyCapture->c_Error )
            {          
              case ERROR_OBJECT_NOT_FOUND:
                if( !( BeforeOpen( MyContext->c_DosManager, MyCapture->c_DosNode, MyCapture->c_RelativePath, MODE_NEWFILE ) ) )
                {  /* open prepared */
                  SendDosNode( MyContext->c_DosManager, MyCapture->c_DosNode ) ;
                  MyCapture->c_State = CAPTURE_OPENNEWFILE ;
                }
                else
                {  /* open not prepared */
                }
                break ;
              case ERROR_DIR_NOT_FOUND:
              default:
                StopCapture( MyCapture ) ;
                break ;
            }
          }
          break ;
        case CAPTURE_CLOSEOLDFILE:
          if( AfterClose( MyContext->c_DosManager, MyCapture->c_DosNode, &MyCapture->c_Error ) )
          {  /* existing file closed */
            MyCapture->c_File = NULL ;
            if( strstr( MyCapture->c_CurrentCaptureFileTemplate, "%N" ) )
            {  /* because of %N we can restart */
              MyCapture->c_SequenceCounter++ ;
              MyLock = CurrentDirDosNode( MyContext->c_DosManager, MyCapture->c_DosNode, NULL ) ;
              if( !( BeforeUnLock( MyContext->c_DosManager, MyCapture->c_DosNode, MyLock ) ) )
              {  /* unlock prepared */
                SendDosNode( MyContext->c_DosManager, MyCapture->c_DosNode ) ;
                //MyCapture->c_State = CAPTURE_UNLOCKDIR ;
                MyCapture->c_State = CAPTURE_START ;
              }
              else
              {  /* unlock not prepared */
              }
              //Repeat = TRUE ;
            }
            else
            {  /* we can only overwrite */
              if( !( BeforeOpen( MyContext->c_DosManager, MyCapture->c_DosNode, MyCapture->c_RelativePath, MODE_NEWFILE ) ) )
              {  /* open prepared */
                SendDosNode( MyContext->c_DosManager, MyCapture->c_DosNode ) ;
                MyCapture->c_State = CAPTURE_OPENNEWFILE ;
              }
              else
              {  /* open not prepared */
              }
            }
          }
          else
          {  /* existing file not closed */
          }
          break ;
        case CAPTURE_OPENNEWFILE:
          MyCapture->c_File = AfterOpen( MyContext->c_DosManager, MyCapture->c_DosNode, &MyCapture->c_Error ) ;
          if( NULL != MyCapture->c_File )
          {  /* new file open */
            LogText( MyContext->c_Log, INFO_LEVEL, "capture to file: %s", MyCapture->c_FileName ) ;
            if( !( BeforeWrite( MyContext->c_DosManager, MyCapture->c_DosNode, MyCapture->c_File, MyCapture->c_CurrentImageBox->ib_CaptureImage->chunky, ( LONG )MyCapture->c_CurrentImageBox->ib_CaptureImage->width ) ) )
            {  /* write prepared */
              SendDosNode( MyContext->c_DosManager, MyCapture->c_DosNode ) ;
              MyCapture->c_State = CAPTURE_WRITENEWFILE ;
            }
            else
            {  /* write not prepared */
            }
          }
          else
          {  /* new file not open */
            LogText( MyContext->c_Log, ERROR_LEVEL, "capture file open error: %ld (%s)", MyCapture->c_Error, DosManagerFault( MyContext->c_DosManager, MyCapture->c_Error ) ) ;
            StopCapture( MyCapture ) ;
          }
          break ;
        case CAPTURE_WRITENEWFILE:
          if( MyCapture->c_CurrentImageBox->ib_CaptureImage->width == AfterWrite( MyContext->c_DosManager, MyCapture->c_DosNode, &MyCapture->c_Error ) )
          {  /* data was written */
            MyCapture->c_Error = 0 ;
          }
          else
          {  /* data was not written */
            LogText( MyContext->c_Log, ERROR_LEVEL, "capture file write error: %ld (%s)", MyCapture->c_Error, DosManagerFault( MyContext->c_DosManager, MyCapture->c_Error ) ) ;
          }
          if( !( BeforeClose( MyContext->c_DosManager, MyCapture->c_DosNode, MyCapture->c_File ) ) )
          {  /* close prepared */
            SendDosNode( MyContext->c_DosManager, MyCapture->c_DosNode ) ;
            MyCapture->c_State = CAPTURE_CLOSENEWFILE ;
          }
          else
          {  /* close not prepared */
          }
          break ;
        case CAPTURE_CLOSENEWFILE:
          if( AfterClose( MyContext->c_DosManager, MyCapture->c_DosNode, &MyCapture->c_Error ) )
          {  /* file closed */
            MyCapture->c_SequenceCounter++ ;
            StopCapture( MyCapture ) ;
          }
          else
          {  /* file not closed */
            LogText( MyContext->c_Log, ERROR_LEVEL, "capture file close error: %ld (%s)", MyCapture->c_Error, DosManagerFault( MyContext->c_DosManager, MyCapture->c_Error ) ) ;
            StopCapture( MyCapture ) ;
          }
          break ;
      }
    } while( Repeat ) ;
  }
  else
  {  /* requirements not ok */
  }
}


/*
** prepare/configure the capture
*/
void ConfigureCapture( struct Capture *MyCapture )
{
  struct Context *MyContext ;
  SIPTR SettingValue ;
  STRPTR NewCaptureDrawer ;
  STRPTR NewCaptureFileTemplate ;
  ULONG NewCaptureFormat ;
  
  if( NULL != MyCapture )
  {  /* requirements ok */
    MyContext = MyCapture->c_Context ;
    
    SettingValue = GetSetting( MyContext->c_Settings, CaptureDrawer ) ;
    NewCaptureDrawer = ( STRPTR )SettingValue ;
    SettingValue = GetSetting( MyContext->c_Settings, CaptureFile ) ;
    NewCaptureFileTemplate = ( STRPTR )SettingValue ;
    SettingValue = GetSetting( MyContext->c_Settings, CaptureFormat ) ;
    NewCaptureFormat = ( ULONG )SettingValue ;
      
    MyCapture->c_CurrentDrawer[ 0 ] = '\0' ;  /* reset drawer */
    MyCapture->c_CurrentCaptureFileTemplate[ 0 ] = '\0' ;  /* reset file template */
      
    if( NULL != NewCaptureDrawer )
    {  /* take over the drawer */
      snprintf( MyCapture->c_CurrentDrawer, CAPTURE_DRAWER_LENGTH, NewCaptureDrawer ) ;
    }
    if( NULL != NewCaptureFileTemplate )
    {  /* take file template from settings */
    }
    else
    {  /* use a default file name */
      NewCaptureFileTemplate = "cAMIra_capture" ;
    }
    MyCapture->c_CurrentCaptureFormat = NewCaptureFormat ;
    switch( MyCapture->c_CurrentCaptureFormat )
    {  /* append extension depending on format */
      case JPG_FORMAT:
        snprintf( MyCapture->c_CurrentCaptureFileTemplate, CURRENTCAPTUREFILETEMPLATE_LENGTH, "%s.jpg", NewCaptureFileTemplate ) ;
        break ;
      case PNG_FORMAT:
        snprintf( MyCapture->c_CurrentCaptureFileTemplate, CURRENTCAPTUREFILETEMPLATE_LENGTH, "%s.png", NewCaptureFileTemplate ) ;
        break ;
      default:
        break ;
    }
  }
  else
  {  /* requirements not ok */
  }
}


/*
** create the capture
*/
struct Capture *CreateCapture( struct Context *MyContext )
{
  struct ExecBase *SysBase ;
  struct Capture *NewCapture ;

  NewCapture = NULL ;
  
  if( ( NULL != MyContext ) )
  {  /* requirements seem ok */
    SysBase = MyContext->c_SysBase ;
    NewCapture = AllocVec( sizeof( struct Capture ), MEMF_ANY | MEMF_CLEAR ) ;
    if( NULL != NewCapture )
    {  /* capture context ok */
      NewCapture->c_Context = MyContext ;
    }
    else
    {  /* capture context not ok */
      LogText( MyContext->c_Log, WARNING_LEVEL, "capture context not ok" ) ;
    }
  }
  else
  {  /* requirements not ok */
  }

  return( NewCapture ) ;
}


/*
** stop the capture
*/
void StopCapture( struct Capture *MyCapture )
{
  struct Context *MyContext ;

  if( NULL != MyCapture )
  {  /* requirements ok */
    MyContext = MyCapture->c_Context ;
    
    if( NULL != MyCapture->c_CurrentImageBox )
    {  /* image box needs to be dumped */
      if( NULL != MyCapture->c_DosNode )
      {  /* dos manager node needs to be deleted */
        DeleteDosNode( MyContext->c_DosManager, MyCapture->c_DosNode ) ;
        MyCapture->c_DosNode = NULL ;
        if( !( MyCapture->c_Error ) )
        {  /* this capture finished ok */
          LogText( MyContext->c_Log, INFO_LEVEL, "capture finished" ) ;
        }
        else
        {  /* this capture failed */
          LogText( MyContext->c_Log, FAILURE_LEVEL, "capture failed" ) ;
        }
      }
      MyCapture->c_CurrentImageBox->ib_UseCounter-- ;
      DeleteImageBox( MyContext, MyCapture->c_CurrentImageBox ) ;
      MyCapture->c_CurrentImageBox = NULL ;
    }
  }
  else
  {  /* requirements not ok */
  }
}


/*
** the main job purpose
*/
LONG StartCapture( struct Capture *MyCapture, struct ImageBox *MyImageBox )
{
  struct Context *MyContext ;
  struct DosLibrary *DOSBase ;
  LONG Error ;

  Error = TRUE ;
  
  if( ( NULL != MyCapture ) && ( NULL == MyCapture->c_CurrentImageBox ) && ( NULL != MyImageBox ) )
  {  /* requirements ok */
    MyContext = MyCapture->c_Context ;
    DOSBase = MyContext->c_DOSBase ;
    
    LogText( MyContext->c_Log, INFO_LEVEL, "starting capture ..." ) ;
    MyCapture->c_Error = 0 ;
    MyCapture->c_CurrentImageBox = MyImageBox ;
    MyCapture->c_CurrentImageBox->ib_UseCounter++ ;
    switch( MyCapture->c_CurrentCaptureFormat )
    {  /* create cature image depending on selected format */
      case JPG_FORMAT:
        MyCapture->c_CurrentImageBox->ib_CaptureImage = CommandImageProcessor( MyContext->c_ImageProcessor,
          IMAGE_COMMAND_CREATE_JPEG, MyCapture->c_CurrentImageBox ) ;
        break ;
      case PNG_FORMAT:
        MyCapture->c_CurrentImageBox->ib_CaptureImage = CommandImageProcessor( MyContext->c_ImageProcessor,
          IMAGE_COMMAND_CREATE_PNG, MyCapture->c_CurrentImageBox ) ;
        break ;
      default:
        MyCapture->c_CurrentImageBox->ib_CaptureImage = NULL ;
        break ;
    }
    if( NULL != MyCapture->c_CurrentImageBox->ib_CaptureImage )
    {  /* there is a image to capture in the box */
      MyCapture->c_DosNode = CreateDosNode( MyContext->c_DosManager, MyCapture->c_CurrentDrawer, &MyCapture->c_Error ) ;
      if( NULL != MyCapture->c_DosNode )
      {  /* dos manager node ok */
        MyCapture->c_DosNode->dn_DoFunction = ( void ( * )( APTR ) )DoCapture ;
        MyCapture->c_DosNode->dn_DoData = MyCapture ;
        MyCapture->c_State = CAPTURE_START ;
        DoCapture( MyCapture ) ;
        /* all done */
        Error = FALSE ;
      }
      else
      {  /* dos manager node not ok */
        LogText( MyContext->c_Log, ERROR_LEVEL, "dos manager node not ok: %ld (%s)", MyCapture->c_Error, DosManagerFault( MyContext->c_DosManager, MyCapture->c_Error ) ) ;
        StopCapture( MyCapture ) ;
        Error = 3 ;
      }
    }
    else
    {  /* there is no image to capture in the box */
      LogText( MyContext->c_Log, ERROR_LEVEL, "there is no image to capture in the box" ) ;
      StopCapture( MyCapture ) ;
      Error = 2 ;
    }
  }
  else
  {  /* requirements not ok */
    Error = 1 ;
  }
  
  return( Error ) ;
}


/*
** report, if this image box is still in use by the capture
*/
LONG CheckCapture( struct Capture *MyCapture )
{
  LONG Result ;
  
  Result = 0 ;
  
  if( NULL != MyCapture )
  {  /* capture context ok */
    if( NULL != MyCapture->c_CurrentImageBox )
    {  /* we are currently busy with some image box */
      Result = 1 ;
    }
    else
    {  /* we are currently not busy with some image box */
    }
  }
  else
  {  /* capture context not ok */
    Result = -1 ;
  }
  
  return( Result ) ;
}
