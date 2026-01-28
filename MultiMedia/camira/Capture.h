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
** Capture.h
*/


#ifndef _CAPTURE_H
#define _CAPTURE_H


#include "DosManager.h"
#include "ProgressBar.h"
#include "ImageProcessor.h"


enum CaptureState
{
  CAPTURE_START,
  CAPTURE_LOCKDRAWER,
  CAPTURE_UNLOCKDIR,
  CAPTURE_LOCKDIR,
  CAPTURE_CREATEDIR,
  CAPTURE_OPENOLDFILE,
  CAPTURE_CLOSEOLDFILE,
  CAPTURE_OPENNEWFILE,
  CAPTURE_WRITENEWFILE,
  CAPTURE_CLOSENEWFILE,
} ;


/*
** capture context
*/
#define CAPTURE_FILENAME_LENGTH ( 64 )
#define CAPTURE_DRAWER_LENGTH ( 64 )
#define CURRENTCAPTUREFILETEMPLATE_LENGTH ( 64 )
struct Capture
{
  struct Context *c_Context ;  /* pointer to the global context */
  struct ImageBox *c_CurrentImageBox ;  /* image box currently processed */
  struct DosNode *c_DosNode ;  /* for async writing */
  ULONG c_SequenceCounter ;  /* we count the number of captures */
  ULONG c_StopRequest ;  /* if stop is requested */
  BYTE c_FileName[ CAPTURE_FILENAME_LENGTH ] ;  /* to construct the capture file name */
  BYTE c_CurrentDrawer[ CAPTURE_DRAWER_LENGTH ] ;  /* current capture drawer */
  BYTE c_CurrentCaptureFileTemplate[ CURRENTCAPTUREFILETEMPLATE_LENGTH ] ;  /* current capture file template */
  BYTE c_CurrentCaptureFormat ;  /* current capture format */
  LONG c_Error ;  /* not 0 if capture process failed */
  enum CaptureState c_State ;
  BPTR c_File ;
  STRPTR c_RelativePath, c_NextRelativePath ;
} ;


/*
** functions of this module
*/
struct Capture *CreateCapture( struct Context *MyContext ) ;
void DeleteCapture( struct Capture *OldCapture ) ;
void ConfigureCapture( struct Capture *MyCapture ) ;
LONG StartCapture( struct Capture *MyCapture, struct ImageBox *MyImageBox ) ;
void StopCapture( struct Capture *MyCapture ) ;
LONG CheckCapture( struct Capture *MyCapture ) ;


#endif  /* !_CAPTURE_H */
