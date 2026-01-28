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
** ImageProcessor.h
*/


#ifndef _IMAGEPROCESSOR_H
#define _IMAGEPROCESSOR_H


#include "Dispatcher.h"
#include <vhi/vhi.h>
#include <utility/date.h>


/*
**
*/
#define VHI_PNG ( 10 )


/*
** data belonging to a cAMIra image
*/
struct ImageBox
{
  struct ClockData ib_ClockData ;  /* the time the image was digitized */
  ULONG ib_SequenceNumber ;  /* a capture counter */
  struct vhi_image *ib_VhiImage ;  /* as it comes from the vhi driver */
  struct vhi_image *ib_RgbImage ;  /* the rgb image for post processing */
  struct vhi_image *ib_CaptureImage ;  /* the captured image */
  struct vhi_image *ib_UploadImage ;  /* the uploaded image */
  ULONG ib_UseCounter ;  /* indicates, if box is still in use */
  LONG ib_Error ;  /* error that happend during last task */
} ;


/*
** send to the image processor
*/
struct ImageRequest
{
  struct Message ir_Message ;
  ULONG ir_Command ;
  struct ImageBox *ir_ImageBox ;  
  struct vhi_image *ir_ResultImage ;
} ;


/*
** image request commands
*/
#define IMAGE_COMMAND_CREATE_RGB24 ( 1 )
#define IMAGE_COMMAND_CREATE_JPEG ( 2 )
#define IMAGE_COMMAND_CREATE_PNG ( 3 )


/*
** image processoe context
*/
struct ImageProcessor
{
  struct DispatcherNode ip_DispatcherNode ;  /* this is a dispatcher client */
  struct Context *ip_Context ;
  struct ImageRequest ip_ImageRequest ;
  struct Process *ip_Process ;
  struct MsgPort *ip_ReplyMsgPort ;
  BOOL ip_Busy ;
  ULONG ip_CurrentJpegQuality ;
  BOOL ip_CurrentJpegProgressive ;
  ULONG ip_CurrentPngCompression ;
  struct MsgPort *ip_RequestMsgPort ;
  struct Library *ip_JpegBase ;
} ;


/*
** functions of the module
*/
struct vhi_image *AllocImage( struct Context *MyContext, ULONG Width, ULONG Height, ULONG Type ) ;
void FreeImage( struct Context *MyContext, struct vhi_image *MyImage ) ;
struct ImageBox *CreateImageBox( struct Context *MyContext ) ;
void DeleteImageBox( struct Context *MyContext, struct ImageBox *MyImageBox ) ;
struct vhi_image *CommandImageProcessor( struct ImageProcessor *MyImageProcessor, ULONG Command, struct ImageBox *MyImageBox ) ;
void ConfigureImageProcessor( struct ImageProcessor *MyImageProcessor ) ;
void DeleteImageProcessor( struct ImageProcessor *MyImageProcessor ) ;
struct ImageProcessor *CreateImageProcessor( struct Context *MyContext ) ;


#endif  /* !_IMAGEPROCESSOR_H */
