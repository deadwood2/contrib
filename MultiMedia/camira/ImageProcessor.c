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
** ImageProcessor.c
*/


#include "ImageProcessor.h"
#include "JpegFormat.h"
#include "PngFormat.h"
#include "YuvFormat.h"
#include "Grayscale8Format.h"
#include "Settings.h"
#include "Log.h"
#include "proto/exec.h"
#include "proto/dos.h"
#include "dos/dostags.h"


/*
** create a rgb image from the data in this image box
*/
static struct vhi_image *CreateRgb24Image( struct ImageProcessor *MyImageProcessor, struct ImageBox *MyImageBox )
{
  struct Context *MyContext ;
  struct vhi_image *Rgb24Image ;
  
  Rgb24Image = NULL ;
  
  if( ( NULL != MyImageProcessor ) && ( NULL != MyImageBox ) )
  {  /* requirements ok */
    MyContext = MyImageProcessor->ip_Context ;

    if( NULL != MyImageBox->ib_RgbImage )
    {  /* there is already a rgb image in the box */
      Rgb24Image = MyImageBox->ib_RgbImage ;
    }
    else
    {  /* no rgb image yet in the box */
      switch( MyImageBox->ib_VhiImage->type )
      {  /* handle different image formats */
        case VHI_JPEG:  /* needs to be converted in rgb format */
          Rgb24Image = CreateRgb24FromJpeg( MyImageProcessor, MyImageBox->ib_VhiImage ) ;
          break ;
        case VHI_RGB_24:  /* can be used as it is */
          Rgb24Image = MyImageBox->ib_VhiImage ;
          break ;
        case VHI_GRAYSCALE_8:
          Rgb24Image = CreateRgb24FromGrayscale8( MyImageProcessor, MyImageBox->ib_VhiImage ) ;
          break ;
        case VHI_YUV_411:  /* all others currently not supported */
        case VHI_YUV_422:
        case VHI_YUV_444:
          Rgb24Image = CreateRgb24FromYuv( MyImageProcessor, MyImageBox->ib_VhiImage ) ;
          break ;
        case VHI_YUV_ACCUPAK:
        case VHI_EXIF_JPEG:
        default:
          break ;
      }
    }
  }
  else
  {  /* requirements not ok */
  }

  return( Rgb24Image ) ;  
}


/*
** create a jpg image from the data in this image box
*/
static struct vhi_image *CreateJpegImage( struct ImageProcessor *MyImageProcessor, struct ImageBox *MyImageBox )
{
  struct Context *MyContext ;
  struct vhi_image *JpegImage ;
  struct vhi_image *Rgb24Image ;
  
  JpegImage = NULL ;
  
  if( ( NULL != MyImageProcessor ) && ( NULL != MyImageBox ) )
  {  /* requirements ok */
    MyContext = MyImageProcessor->ip_Context ;
    
    if( ( NULL != MyImageBox->ib_VhiImage ) && ( VHI_JPEG == MyImageBox->ib_VhiImage->type ) )
    {  /* vhi image still there and jpg, so use this */
      JpegImage = MyImageBox->ib_VhiImage ;
    }
    else
    {  /* vhi image not there or not jpg */
      Rgb24Image = CreateRgb24Image( MyImageProcessor, MyImageBox ) ;
      if( NULL != Rgb24Image )
      {  /* could get a rgb image from the box */
        JpegImage = CreateJpegFromRgb24( MyImageProcessor, Rgb24Image ) ;
        if( NULL != JpegImage )
        {  /* rgb24 to jpeg ok */
        }
        else
        {  /* rgb24 to jpeg not ok */
          LogText( MyContext->c_Log, ERROR_LEVEL, "rgb24 to jpeg not ok" ) ;
        }
      }
      else
      {  /* could not get a rgb image from the box */
        LogText( MyContext->c_Log, ERROR_LEVEL, "rgb24 to create jpeg not ok" ) ;
      }
    }
  }
  else
  {  /* requirements not ok */
  }
  
  return( JpegImage ) ;
}


/*
** create a png image from the data in this image box
*/
static struct vhi_image *CreatePngImage( struct ImageProcessor *MyImageProcessor, struct ImageBox *MyImageBox )
{
  struct Context *MyContext ;
  struct vhi_image *Rgb24Image ;
  struct vhi_image *PngImage ;
  
  PngImage = NULL ;
  
  if( ( NULL != MyImageProcessor ) && ( NULL != MyImageBox ) )
  {  /* requirements ok */
    MyContext = MyImageProcessor->ip_Context ;
    
    Rgb24Image = CreateRgb24Image( MyImageProcessor, MyImageBox ) ;
    if( NULL != Rgb24Image )
    {  /* could get a rgb image from the box */
      PngImage = CreatePngFromRgb24( MyImageProcessor, Rgb24Image ) ;
      if( NULL != PngImage )
      {  /* rgb24 to png ok */
      }
      else
      {  /* rgb24 to png not ok */
        LogText( MyContext->c_Log, ERROR_LEVEL, "rgb24 to png not ok" ) ;
      }
    }
    else
    {  /* could not get a rgb image from the box */
    }
  }
  else
  {  /* requirements not ok */
  }
  
  return( PngImage ) ;
}


/*
** some private commands
*/
#define IMAGE_COMMAND_PROCESSOR_STARTUP ( 10 )
#define IMAGE_COMMAND_PROCESSOR_SHUTDOWN ( 11 )


/*
** the ImageProcessor function, runs in it's own context
*/
static void ImageProcessorEntry( void )
{
#if !defined(__AROS__)
  struct ExecBase *SysBase ;
#endif
  struct Process *MyProcess ;
  struct ImageProcessor *MyImageProcessor ;
  struct Context *MyContext ;
  struct ImageRequest *MyImageRequest ;
  BOOL Done ;

#if !defined(__AROS__)
  SysBase = *( struct ExecBase ** )4 ;
#endif
  MyProcess = ( struct Process * )FindTask( NULL ) ;
  WaitPort( &MyProcess->pr_MsgPort ) ;
  MyImageRequest = ( struct ImageRequest * )GetMsg( &MyProcess->pr_MsgPort ) ;
  if( IMAGE_COMMAND_PROCESSOR_STARTUP == MyImageRequest->ir_Command )
 {  /* this is our startup message */
    MyImageProcessor = ( struct ImageProcessor * )MyImageRequest->ir_ImageBox ;
  }
  else
  {  /* hmmm, not our startup message */
    MyImageProcessor = NULL ;
  }

  if( NULL != MyImageProcessor )
  {  /* image processor ok */
    MyContext = MyImageProcessor->ip_Context ;
    SysBase = MyContext->c_SysBase ;
    
    MyImageProcessor->ip_RequestMsgPort = CreateMsgPort( ) ;
    if( NULL != MyImageProcessor->ip_RequestMsgPort )
    {  /* request message port ok */
      MyImageProcessor->ip_JpegBase = OpenLibrary( "jpeg.library", 6 ) ;
      Done = FALSE ;
      while( !( Done ) )
      {
        ReplyMsg( ( struct Message * )MyImageRequest ) ;
        WaitPort( MyImageProcessor->ip_RequestMsgPort ) ;
        MyImageRequest = ( struct ImageRequest * )GetMsg( MyImageProcessor->ip_RequestMsgPort ) ;
        switch( MyImageRequest->ir_Command )
        {
          case IMAGE_COMMAND_CREATE_RGB24:  /* create a rgb24 image */
            MyImageRequest->ir_ResultImage = CreateRgb24Image( MyImageProcessor, MyImageRequest->ir_ImageBox ) ;
            break ;
          case IMAGE_COMMAND_CREATE_JPEG:  /* create a jpeg image */
            MyImageRequest->ir_ResultImage = CreateJpegImage( MyImageProcessor, MyImageRequest->ir_ImageBox ) ;
            break ;
          case IMAGE_COMMAND_CREATE_PNG:  /* create a png image */
            MyImageRequest->ir_ResultImage = CreatePngImage( MyImageProcessor, MyImageRequest->ir_ImageBox ) ;
            break ;
          case IMAGE_COMMAND_PROCESSOR_SHUTDOWN:  /* the image processor process shall quit */
            Done = TRUE ;
            break ;
          default:  /* unknown image processor command */
            break ;
        }
      }
      if( NULL != MyImageProcessor->ip_JpegBase )
      {
        CloseLibrary( MyImageProcessor->ip_JpegBase ) ;
      }
      DeleteMsgPort( MyImageProcessor->ip_RequestMsgPort ) ;
      MyImageProcessor->ip_RequestMsgPort = NULL ;
    }
    else
    {  /* request message port not ok */
    }
  }
  else
  {  /* image processor not ok */
  }
  
  Forbid( ) ;
  ReplyMsg( ( struct Message * )MyImageRequest ) ;
}


/*
** send a image request to the image processor
*/
struct vhi_image *CommandImageProcessor( struct ImageProcessor *MyImageProcessor, ULONG Command, struct ImageBox *MyImageBox )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct vhi_image *ResultImage ;
  
  ResultImage = NULL ;
  
  if( NULL != MyImageProcessor )
  {  /* requirements ok */
    MyContext = MyImageProcessor->ip_Context ;
    SysBase = MyContext->c_SysBase ;

    MyImageProcessor->ip_ImageRequest.ir_Command = Command ;
    MyImageProcessor->ip_ImageRequest.ir_ImageBox = MyImageBox ;
    MyImageProcessor->ip_ImageRequest.ir_ResultImage = NULL ;
    PutMsg( MyImageProcessor->ip_RequestMsgPort, ( struct Message * )&MyImageProcessor->ip_ImageRequest ) ;
    MyImageProcessor->ip_Busy = TRUE ;
    while( MyImageProcessor->ip_Busy )
    {  /* in case it is busy, wait for the image processor to become ready */
      Dispatcher( MyContext->c_Dispatcher, BLOCKING_MODE ) ;  /* dispatch blocking */
    }
    ResultImage = MyImageProcessor->ip_ImageRequest.ir_ResultImage ;
  }
  else
  {  /* requirements not ok */
  }

  return( ResultImage ) ;
}


/*
** image processor events
*/
static ULONG DoImageProcessor( struct ImageProcessor *MyImageProcessor, ULONG TriggerSignals )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct ImageRequest *MyImageRequest ;
  
  MyContext = MyImageProcessor->ip_Context ;
  SysBase = MyContext->c_SysBase ;
    
  MyImageRequest = ( struct ImageRequest * )GetMsg( MyImageProcessor->ip_ReplyMsgPort ) ;
  if( NULL != MyImageRequest )
  {  /* the message returned */
    switch( MyImageRequest->ir_Command )
    {
      default:  /* unsupported image processor command */
        break ;
    }
    MyImageProcessor->ip_Busy = 0 ;
  }
  else
  {  /* no message returned */
  }
  
  return( TriggerSignals ) ;
}


/*
** configure the image processor
*/
void ConfigureImageProcessor( struct ImageProcessor *MyImageProcessor )
{
  struct Context *MyContext ;
  LONG SettingValue ;

  if( NULL != MyImageProcessor )
  {  /* requirements ok */
    MyContext = MyImageProcessor->ip_Context ;

    SettingValue = GetSetting( MyContext->c_Settings, JpegQuality ) ;
    MyImageProcessor->ip_CurrentJpegQuality = ( ULONG )SettingValue ;
    SettingValue = GetSetting( MyContext->c_Settings, JpegProgressive ) ;
    MyImageProcessor->ip_CurrentJpegProgressive = ( BOOL )SettingValue ;
    SettingValue = GetSetting( MyContext->c_Settings, PngCompression ) ;
    MyImageProcessor->ip_CurrentPngCompression = ( ULONG )SettingValue ;
  }
  else
  {  /* requirements not ok */
  }
}


/*
** delete image processor
*/
void DeleteImageProcessor( struct ImageProcessor *MyImageProcessor )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;

  if( NULL != MyImageProcessor )
  {  /* context memory needs to be freed */
    MyContext = MyImageProcessor->ip_Context ;
    SysBase = MyContext->c_SysBase ;
    if( NULL != MyImageProcessor->ip_ReplyMsgPort )
    {  /* reply message port needs to be deleted */
      if( NULL != MyImageProcessor->ip_Process )
      {  /* process needs to be deleted */
        while( NULL != MyImageProcessor->ip_RequestMsgPort )
        {  /* process needs to be shutdown */
          CommandImageProcessor( MyImageProcessor, IMAGE_COMMAND_PROCESSOR_SHUTDOWN, NULL ) ;
        }
        /* deleting the process gets done by the OS */
      }
      RemDispatcherNode( MyContext->c_Dispatcher, &MyImageProcessor->ip_DispatcherNode ) ;
      DeleteMsgPort( MyImageProcessor->ip_ReplyMsgPort ) ;
    }
    FreeVec( MyImageProcessor ) ;
  }

  return ;
}


/*
** create image processor
*/
struct ImageProcessor *CreateImageProcessor( struct Context *MyContext )
{
  struct ExecBase *SysBase ;
  struct DosLibrary *DOSBase ;
  struct ImageProcessor *MyImageProcessor ;

  MyImageProcessor = NULL ;

  if( NULL != MyContext )
  {  /* requirements ok */
    SysBase = MyContext->c_SysBase ;
    DOSBase = MyContext->c_DOSBase ;

    MyImageProcessor = AllocVec( sizeof( struct ImageProcessor ), ( MEMF_ANY | MEMF_CLEAR ) ) ;
    if( NULL != MyImageProcessor )
    {  /* context memory ok */
      MyImageProcessor->ip_Context = MyContext ;
      MyImageProcessor->ip_ReplyMsgPort = CreateMsgPort( ) ;
      if( NULL != MyImageProcessor->ip_ReplyMsgPort )
      {  /* reply message port ok */
        MyImageProcessor->ip_ImageRequest.ir_Message.mn_ReplyPort = MyImageProcessor->ip_ReplyMsgPort ;
        MyImageProcessor->ip_ImageRequest.ir_Message.mn_Length = sizeof( struct ImageRequest ) ;
        
        MyImageProcessor->ip_DispatcherNode.dn_Signals = ( 1UL << MyImageProcessor->ip_ReplyMsgPort->mp_SigBit ) ;
        MyImageProcessor->ip_DispatcherNode.dn_DoFunction = ( DISPATCHER_DOFUNCTION )DoImageProcessor ;
        MyImageProcessor->ip_DispatcherNode.dn_DoData = MyImageProcessor ;
        AddDispatcherNode( MyContext->c_Dispatcher, &MyImageProcessor->ip_DispatcherNode ) ;

        MyImageProcessor->ip_Process = CreateNewProcTags( NP_Entry, ( IPTR )&ImageProcessorEntry, 
          NP_Name, ( IPTR )"cAMIra ImageProcessor",
          NP_Priority, ( ( MyContext->c_Process->pr_Task.tc_Node.ln_Pri ) - 1 ),
          TAG_END ) ;
        if( NULL != MyImageProcessor->ip_Process )
        {  /* process ok */
          MyImageProcessor->ip_ImageRequest.ir_Command = IMAGE_COMMAND_PROCESSOR_STARTUP ;
          MyImageProcessor->ip_ImageRequest.ir_ImageBox = ( struct ImageBox * )MyImageProcessor ;
          PutMsg( &MyImageProcessor->ip_Process->pr_MsgPort, ( struct Message * )&MyImageProcessor->ip_ImageRequest ) ;
          MyImageProcessor->ip_Busy = TRUE ;
          while( MyImageProcessor->ip_Busy )
          {  /* wait for the image processor to process the startup message */
            Dispatcher( MyContext->c_Dispatcher, BLOCKING_MODE ) ;  /* dispatch blocking */
          }
          if( NULL != MyImageProcessor->ip_RequestMsgPort )
          {  /* command message port ok */
          /* all done */
          }
          else
          {  /* command message port not ok */
            DeleteImageProcessor( MyImageProcessor ) ;
            MyImageProcessor = NULL ;
          }
        }
        else
        {  /* process not ok */
          DeleteImageProcessor( MyImageProcessor ) ;
          MyImageProcessor = NULL ;
        }
      }
      else
      {  /* reply message port not ok */
        DeleteImageProcessor( MyImageProcessor ) ;
        MyImageProcessor = NULL ;
      }
    }
    else
    {  /* context memory not ok */
    }
  }
  else
  {  /* requirements not ok */
  }

  return( MyImageProcessor ) ;
}


/*
** free resources of a vhi image
*/
void FreeImage( struct Context *MyContext, struct vhi_image *OldImage )
{
  struct ExecBase *SysBase ;

  if( ( NULL != MyContext ) && ( NULL != OldImage ) )
  {  /* there seems to ba a vhi image to be freed */
    SysBase = MyContext->c_SysBase ;
    
    switch( OldImage->type )
    {  /* what has to be freed based on image type */
      case VHI_GRAYSCALE_8:
      case VHI_RGB_24:
      case VHI_JPEG:
      case VHI_EXIF_JPEG:
      case VHI_PNG:
        if( NULL != OldImage->chunky )
        {  /* chunky, jpg, png buffer needs to be freed */
          FreeVec( OldImage->chunky ) ;
        }
        break ;
      case VHI_YUV_411:
      case VHI_YUV_422:
      case VHI_YUV_444:
      case VHI_YUV_ACCUPAK:
        if( NULL != OldImage->y )
        {  /* y buffer needs to be freed */
          FreeVec( OldImage->y ) ;
        }
        if( NULL != OldImage->u )
        {  /* u buffer needs to be freed */
          FreeVec( OldImage->u ) ;
        }
        if( NULL != OldImage->v )
        {  /* v buffer needs to be freed */
          FreeVec( OldImage->v ) ;
        }
        break ;
      default:
        break ;
    }
    FreeVec( OldImage ) ;
  }
  else
  {  /* there is no vhi image to be freed */
  }
}


/*
** allocate resources for a vhi image of given size
*/
struct vhi_image *AllocImage( struct Context *MyContext, ULONG Width, ULONG Height, ULONG Type )
{
  struct ExecBase *SysBase ;
  struct vhi_image *NewImage ;
  UBYTE UVDiv ;
  
  NewImage = NULL ;
  UVDiv = 0 ;
  
  if( ( NULL != MyContext ) && ( ( ( 0 != Width ) && ( 0 != Height ) ) || ( VHI_JPEG == Type ) ) )
  {  /* requirements seems ok */
    SysBase = MyContext->c_SysBase ;
    
    NewImage = AllocVec( sizeof( struct vhi_image ), MEMF_ANY ) ;
    if( NULL != NewImage )
    {  /* image struct ok */
      NewImage->type = 0 ;
      NewImage->chunky = NewImage->y = NewImage->u = NewImage->v = NULL ;
      NewImage->scaled = 0 ;
      switch( Type )
      {
        case VHI_GRAYSCALE_8:
          NewImage->chunky = AllocVec( ( 1UL * Width * Height ), MEMF_ANY ) ;
          if( NULL != NewImage->chunky )
          {  /* rgb buffer ok */
            NewImage->width = Width ;
            NewImage->height = Height ;
            NewImage->type = VHI_GRAYSCALE_8 ;
          }
          break ;
        case VHI_RGB_24:
          NewImage->chunky = AllocVec( ( 3UL * Width * Height ), MEMF_ANY ) ;
          if( NULL != NewImage->chunky )
          {  /* rgb buffer ok */
            NewImage->width = Width ;
            NewImage->height = Height ;
            NewImage->type = VHI_RGB_24 ;
          }
          break ;
        case VHI_JPEG:
          NewImage->width = 0 ;  /* size of chunky buffer is 0 so far */
          NewImage->height = 0 ;  /* zero for jpeg */
          NewImage->type = VHI_JPEG ;
          /* nothing else to do, jpeg.library will allocate image data memory */
          break ;
        case VHI_PNG:
          NewImage->chunky = AllocVec( ( 3UL * Width * Height ), MEMF_ANY ) ;
          if( NULL != NewImage->chunky )
          {  /* png buffer ok */
            NewImage->width = ( 3UL * Width * Height ) ;  /* actual image size in bytes */
            NewImage->height = 0 ;  /* not used */
            NewImage->type = VHI_PNG ;
          }
          break ;
        case VHI_YUV_411:
          UVDiv++ ;
        case VHI_YUV_422:
          UVDiv++ ;
        case VHI_YUV_444:
          UVDiv++ ;
          NewImage->y = AllocVec( ( 1UL * Width * Height ), MEMF_ANY ) ;
          if( NULL != NewImage->y )
          {  /* y buffer ok */
            NewImage->u = AllocVec( ( ( 1UL * Width * Height ) + ( UVDiv - 1 ) ) / UVDiv, MEMF_ANY ) ;
            if( NULL != NewImage->u )
            {  /* u buffer ok */
              NewImage->v = AllocVec( ( ( 1UL * Width * Height ) + ( UVDiv - 1 ) ) / UVDiv, MEMF_ANY ) ;
              if( NULL != NewImage->v )
              {  /* v buffer ok */
                NewImage->width = Width ;
                NewImage->height = Height ;
                NewImage->type = Type ;
              }
            }
          }
          break ;
        default:          
          break ;
      }
      if( 0 == NewImage->type )
      {  /* no format, so image not complete */
        FreeImage( MyContext, NewImage ) ;
        NewImage = NULL ;
      }
    }
    else
    {  /* image struct not ok */
    }
  }
  else
  {  /* requirements not ok */
  }

  return( NewImage ) ;
}


/*
** create a cAMIra image box
*/
struct ImageBox *CreateImageBox( struct Context *MyContext )
{
  struct ExecBase *SysBase ;
  struct ImageBox *MyImageBox ;
  
  MyImageBox = NULL ;

  if( ( NULL != MyContext )  )
  {  /* requirements ok */
    SysBase = MyContext->c_SysBase ;
    MyImageBox = AllocVec( sizeof( struct ImageBox ), MEMF_ANY | MEMF_CLEAR ) ;
    if( NULL != MyImageBox )
    {  /* memory for image box ok */
    }
    else
    {  /* memory for image box not ok */
    }
  }
  else
  {  /* requirements not ok */
  }
  
  return( MyImageBox ) ;
}


/*
** delete a cAMIra image box
*/
void DeleteImageBox( struct Context *MyContext, struct ImageBox *MyImageBox )
{
  struct ExecBase *SysBase ;
  
  if( ( NULL != MyImageBox ) && ( 0 == MyImageBox->ib_UseCounter ) )
  {  /* check, if this box can be freed */
    SysBase = MyContext->c_SysBase ;

    if( MyImageBox->ib_UploadImage )
    {  /* there is a upload image in the box */
      if( MyImageBox->ib_CaptureImage == MyImageBox->ib_UploadImage )
      {  /* capture image is the same */
        MyImageBox->ib_CaptureImage = NULL ;
      }
      if( MyImageBox->ib_VhiImage == MyImageBox->ib_UploadImage )
      {  /* vhi image is the same */
        MyImageBox->ib_VhiImage = NULL ;
      }
      FreeImage( MyContext, MyImageBox->ib_UploadImage ) ;
      MyImageBox->ib_UploadImage = NULL ;
    }
    if( ( NULL != MyImageBox->ib_CaptureImage ) )
    {  /* there is a capture image in the box */
      if( MyImageBox->ib_VhiImage == MyImageBox->ib_CaptureImage )
      {  /* vhi image is the same */
        MyImageBox->ib_VhiImage = NULL ;
      }
      FreeImage( MyContext, MyImageBox->ib_CaptureImage ) ;
      MyImageBox->ib_CaptureImage = NULL ;
    }
    if( ( NULL != MyImageBox->ib_RgbImage ) )
    {  /* there is a rgb image in the box */
      if( MyImageBox->ib_VhiImage == MyImageBox->ib_RgbImage )
      {  /* vhi image is the same */
        MyImageBox->ib_VhiImage = NULL ;
      }
      FreeImage( MyContext, MyImageBox->ib_RgbImage ) ;
      MyImageBox->ib_RgbImage = NULL ;
    }
    if( ( NULL != MyImageBox->ib_VhiImage ) )
    {  /* there is a vhi image in the box */
      FreeImage( MyContext, MyImageBox->ib_VhiImage ) ;
      MyImageBox->ib_VhiImage = NULL ;
    }
    FreeVec( MyImageBox ) ;
  }
  else
  {  /* no box to check */
  }
}

