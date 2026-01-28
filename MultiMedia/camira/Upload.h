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
** Upload.h
*/


#ifndef _UPLOAD_H
#define _UPLOAD_H


#include "Dispatcher.h"
#include "Timer.h"
#include "ImageProcessor.h"
#include <stddef.h>
#include <netinet/in.h>
#include <dos/dos.h>
#include <stdio.h>


/*
** upload buffers nodes
*/
#define DATABUFFER_SIZE ( 1024 )
struct DataBuffer
{
  struct Node db_Node ;
  UBYTE db_DataBuffer[ DATABUFFER_SIZE ] ;  /* buffer for actual data */
  UBYTE *db_DataPointer ;
  ULONG db_DataLength ;
  ULONG db_LastData ;
} ;


/*
** the upload context
*/
#define CONTROLBUFFER_SIZE ( 256 )
#define NUM_DATABUFFERS ( 2 )
#define UPLOADFILE_LENGTH ( 64 )
#define CURRENTUPLOADFILETEMPLATE_LENGTH ( 64 )
struct Upload
{
  struct Context *u_Context ;  /* store SysBase */
  struct DispatcherNode u_DispatcherNode ;  /* dispatcher node */
  struct TimerNode u_TimerNode ;  /* for server timeout detection */
  struct Library *u_SocketBase ;  /* store SocketBase */
  struct ImageBox *u_CurrentImageBox ;  /* image box currently in use */
  ULONG u_ErrorNumber ;  /* storage for error number */
  ULONG u_SignalBit ;  /* signal to inform the task on action */
  BPTR u_FileHandle ;  /* file to be uploaded */
  UBYTE u_ActiveCommand ;  /* command send to server */
  struct sockaddr_in u_ControlAddress ;  /* remote address */
  LONG u_ControlSocket ;  /* for control data */ 
  LONG u_ControlLength, u_ControlSent ;  /* status of send control */
  ULONG u_ControlReceived ;  /* when a line did not fit in control buffer */
  UBYTE u_ControlBuffer[ CONTROLBUFFER_SIZE ] ;  /* buffer for control messages */
  LONG u_MultilineControlCode ;  /* for detecting end of multiline server response */
  struct sockaddr_in u_DataAddress ;  /* remote address */
  LONG u_DataSocket ;  /* for actual data */ 
  LONG u_DataSent ;  /* status of send data */
  struct DataBuffer u_DataBuffer[ NUM_DATABUFFERS ] ;
  LONG u_DataServerSocket ;
  struct List u_FullList ;
  struct List u_EmptyList ;  
  ULONG u_CurrentUploadTimeout ;  /* how long we wait for server responce */
  ULONG u_CurrentUploadPassive ;  /* should we use passive mode or not */
  ULONG u_CurrentUploadCwd ;  /* do we need to send CWD command */
  BYTE u_UploadFile[ UPLOADFILE_LENGTH ] ;  /* to construct the upload file name */
  BYTE u_CurrentUploadFileTemplate[ CURRENTUPLOADFILETEMPLATE_LENGTH ] ;  /* current upload file template */
  ULONG u_CurrentUploadFormat ;  /* format to be uploaded */
  LONG u_UploadError ;  /* not 0 if upload process failed */
} ;


/*
** module functions
*/
struct Upload *CreateUpload( struct Context *MyContext ) ;
void DeleteUpload( struct Upload *OldUpload ) ;
void ConfigureUpload( struct Upload *MyUpload ) ;
LONG StartUpload( struct Upload *MyUpload, struct ImageBox *MyImageBox ) ;
void StopUpload( struct Upload *MyUpload ) ;
LONG CheckUpload( struct Upload *MyUpload ) ;


#endif  /* !_UPLOAD_H */
