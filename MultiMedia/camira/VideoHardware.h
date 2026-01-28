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
** VideoHardware.h
*/


#ifndef _VIDEOHARDWARE_H
#define _VIDEOHARDWARE_H


#include "MemoryManager.h"
#include "ImageProcessor.h"


/*
** some custom "vhi" methods
*/
#define VHI_METHOD_STARTUP ( 10L )
#define VHI_METHOD_FREEVEC ( 11L )
#define VHI_METHOD_SHUTDOWN ( 12L )


/*
** message format to communicate with vhi sub process
*/
struct VhiMessage
{
  struct Message vm_Message ;  /* this is a message */
  ULONG vm_Method ;  /* vhi method */
  ULONG vm_SubMethod ;  /* vhi submethod */
  APTR vm_Attribute ;  /* vhi attributs */
  ULONG *vm_ErrorCode ;  /* where to store errors */
  APTR vm_Handle ;  /* the vhi handle */
  IPTR vm_Result ;  /* the vhi result */
} ;


/*
** context of video hardware
*/
#define VH_VHIPROCESSNAME_LENGTH ( 128 )
#define VH_CURRENTVHIDRIVER_LENGTH ( VH_VHIPROCESSNAME_LENGTH - 24 )
struct VideoHardware
{
  struct DispatcherNode vh_DispatcherNode ;  /* this is a dispatcher client */
  struct Context *vh_Context ;  /* pointer to context */
  struct Library *vh_VHIBase ;  /* vhi driver base */
  struct ImageBox *vh_CurrentImageBox ;  /* image box currently processed */
  APTR vh_VhiHandle ;  /* the vhi handle when opened */
  ULONG vh_ErrorCode ;  /* error code used by vhi functions */
  STRPTR vh_CardName ;  /* name of the hardware */
  STRPTR vh_CardManufacturer ;  /* manufacturer of the hardware */
  ULONG vh_CardVersion ;  /* hardware version */
  ULONG vh_CardRevision ;  /* hardware revision */
  STRPTR vh_CardDriverAuthor ;  /* driver author */
  ULONG vh_CardDriverVersion ;  /* driver version */
  ULONG vh_CardDriverRevision ;  /* driver revision */
  ULONG vh_NumberOfInputs ;  /* number of inputs */
  STRPTR *vh_NameOfInput ;  /* array of strings with input names */
  STRPTR *x_vh_NameOfColormode ;  /* array of strings with color names */
  ULONG vh_SupportedOptions ;  /* options supported by the driver */
  ULONG vh_SupportedVideoFormats ;  /* video formats supported by the driver */
  STRPTR *vh_NameOfVideoFormat ;  /* array of strings with video format names */
  ULONG vh_SupportedColorModes ;  /* colormodes supported by the driver */
  ULONG vh_TrustmeMode ;  /* status of the trustme mode */
  struct vhi_size vh_MaximumSize ;  /* maximum size of current settings */
  struct vhi_digitize vh_Digitize ;  /* digitize struct */
  BYTE vh_CurrentVhiDriver[ VH_CURRENTVHIDRIVER_LENGTH ] ;  /* current vhi driver name string */
  ULONG vh_CurrentVhiInput ;  /* current selected vhi input */
  ULONG vh_CurrentVhiColorMode ;  /* current selected vhi colormode */
  ULONG vh_CurrentVhiVideoFormat ;  /* current selected video format */
  ULONG vh_CurrentX1 ;  /* current digitize selection */
  ULONG vh_CurrentY1 ;  /* current digitize selection */
  ULONG vh_CurrentX2 ;  /* current digitize selection */
  ULONG vh_CurrentY2 ;  /* current digitize selection */
  ULONG vh_CurrentWidth ;  /* current digitize selection */
  ULONG vh_CurrentHeight ;  /* current digitize selection */
  BOOL vh_Busy ;  /* set when vhi process is active */
  struct VhiMessage vh_VhiMessage ;  /* the message we will send around */
  struct MsgPort *vh_VhiReplyMsgPort ;  /* port to where the vhi task will return messages */
  struct Process *vh_VhiProcess ;  /* the vhi sub process, that can use DOS */
  struct MsgPort *vh_VhiMsgPort ;  /* to here we send messages to the vhi sub process */
  struct MemoryMessage vh_MemoryMessage ;  /* used by the vhi task to talk to memory manager */
  struct MsgPort *vh_MemoryReplyMsgPort ;
  BYTE vh_VhiProcessName[ VH_VHIPROCESSNAME_LENGTH ] ;  /* current vhi driver name string */
} ;


/*
** functions of this module
*/
struct VideoHardware *CreateVideoHardware( struct Context *MyContext ) ;
void DeleteVideoHardware( struct VideoHardware *OldVideoHardware ) ;
void ConfigureVideoHardware( struct VideoHardware *MyVideoHardware ) ;
void OpenVideoHardware( struct VideoHardware *MyVideoHardware ) ;
void CloseVideoHardware( struct VideoHardware *MyVideoHardware ) ;
void DynamicQueryVideoHardware( struct VideoHardware *MyVideoHardware ) ;
BOOL SelectVhiInput( struct VideoHardware *MyVideoHardware, ULONG NewVhiInput ) ;
BOOL SelectVhiColorMode( struct VideoHardware *MyVideoHardware, ULONG NewVhiColorMode ) ;
BOOL SelectVhiVideoFormat( struct VideoHardware *MyVideoHardware, ULONG NewVhiVideoFormat ) ;
LONG DigitizeImage( struct VideoHardware *MyVideoHardware, struct ImageBox *MyImageBox ) ;
LONG CheckVideoHardware( struct VideoHardware *MyVideoHardware ) ;


#endif  /* !_VIDEOHARDWARE_H */
