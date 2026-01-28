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
** Context.h
*/

#ifndef _CONTEXT_H
#define _CONTEXT_H


#include <exec/execbase.h>
#include <dos/dosextens.h>
#include <intuition/intuitionbase.h>
#include <libraries/mui.h>


/*
** forward declarations
*/
struct Dispatcher ;
struct Breaker ;
struct Settings ;
struct MemoryManager ;
struct VideoHardware ;
struct ImageProcessor ;
struct Stamp ;
struct Magic ;
struct MainWindow ;
struct DisplayArea ;
struct ProgressBar ;
struct Timer ;
struct DosManager ;
struct Scheduler ;
struct Capture ;
struct Upload ;
struct Localization ;
struct SettingsWindow ;
struct AboutWindow ;
struct LogWindow ;


/*
** the master context
*/
struct Context
{
  struct ExecBase *c_SysBase ;  /* exec library */
  struct DosLibrary *c_DOSBase ;  /* dos library */
  struct IntuitionBase *c_IntuitionBase ;  /* intuition library */
  struct GfxBase *c_GfxBase ; /* graphics library */
  struct Library *c_IconBase ;  /* icon library */
  struct Library *c_DiskfontBase ;  /* diskfont library */
  struct Library *c_UtilityBase ;  /* utility library */
  struct Library *c_ExpatBase ;  /* expat library */
  struct Library *c_MUIMasterBase ;  /* muimaster library */
  struct Library *c_GuiGFXBase ;  /* guigfx library */
  struct Process *c_Process ;  /* our main process */
  struct DiskObject *c_DiskObject ;  /* our icon */
  struct Log *c_Log ;  /* log context */
  struct Dispatcher *c_Dispatcher ;  /* dispatcher context */
  struct Breaker *c_Breaker ;  /* CTRL_C handler context */
  struct Settings *c_Settings ;  /* settings context */
  struct MemoryManager *c_MemoryManager ;  /* for vhi custom memhandling */
  struct VideoHardware *c_VideoHardware ;  /* the vhi interface */
  struct ImageProcessor *c_ImageProcessor ;  /* the image processor */
  struct Stamp *c_Stamp ;  /* for stamping text in an image */
  struct Magic *c_Magic ;  /* magic (mui) context */
  struct MainWindow *c_MainWindow ;
  struct DisplayArea *c_DisplayArea ;
  struct ProgressBar *c_ProgressBar ;
  struct Timer *c_Timer ;
  struct DosManager *c_DosManager ;
  struct Scheduler *c_Scheduler ;
  struct Capture *c_Capture ;
  struct Upload *c_Upload ;
  struct Localization *c_Localization ;
  struct SettingsWindow *c_SettingsWindow ;
  struct AboutWindow *c_AboutWindow ;
  struct LogWindow *c_LogWindow ;
  ULONG c_LogRequest ;
  ULONG c_DisplayRequest ;
  ULONG c_ConfigureRequest ;
  ULONG c_ShutdownRequest ;
  ULONG c_CaptureRequest ;
  ULONG c_ReleaseRequest ;
  ULONG c_UploadRequest ;
  struct ImageBox *c_NewImageBox ;
  ULONG c_Shutdown ;
} ;


/*
** module functions
*/
struct Context *CreateContext( struct ExecBase *SysBase ) ;
void DeleteContext( struct Context * ) ;

struct SWinMsg
{
    STACKED ULONG       MethodID;
    STACKED ULONG       Catagory;
};

struct ParamMsg
{
    STACKED ULONG       MethodID;
    STACKED APTR        MsgParam;
};

#endif  /* !_CONTEXT_H */
