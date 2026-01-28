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
** VideoHardware.c
*/


#include "VideoHardware.h"
#include "Settings.h"
#include "Timer.h"
#include "Log.h"
#include "ab_stdio.h"
#include <proto/exec.h>
#include <proto/dos.h>
#include <dos/dostags.h>
#include <proto/vhi.h>



/*
** helper to convert a vhi error to a string 
*/
static STRPTR VhiErrorToString( ULONG VhiError )
{
  STRPTR ErrorString ;

  switch( VhiError )
  {
    case VHI_ERR_UNKNOWN_METHOD:
      ErrorString = "VHI_ERR_UNKNOWN_METHOD" ;
      break ;
    case VHI_ERR_COULD_NOT_INIT:
      ErrorString = "VHI_ERR_COULD_NOT_INIT" ;
      break ;
    case VHI_ERR_NO_HARDWARE:
      ErrorString = "VHI_ERR_NO_HARDWARE" ;
      break ;
    case VHI_ERR_NO_INPUT_DATA:
      ErrorString = "VHI_ERR_NO_INPUT_DATA" ;
      break ;
    case VHI_ERR_ERR_WHILE_DIG:
      ErrorString = "VHI_ERR_ERR_WHILE_DIG" ;
      break ;
    case VHI_ERR_OUT_OF_MEMORY:
      ErrorString = "VHI_ERR_OUT_OF_MEMORY" ;
      break ;
    case VHI_ERR_INTERNAL_ERROR:
      ErrorString = "VHI_ERR_INTERNAL_ERROR" ;
      break ;
    case VHI_ERR_UNKNOWN_OPTION:
      ErrorString = "VHI_ERR_UNKNOWN_OPTION" ;
      break ;
    default:
      ErrorString = "???" ;
      break ;
  }

  return( ErrorString ) ;
}


/*
** called from vhi task to alloc memory, runs in vhi task context
*/
static void *CustomAllocVec( ULONG Size, ULONG Attributes )
{
  struct ExecBase *SysBase ;
  struct Process *MyProcess ;
  struct VideoHardware *MyVideoHardware ;
  struct Context *MyContext ;
  struct MemoryMessage *MyMessage ;
  void *Result ;

  SysBase = *( struct ExecBase ** )4 ;
  MyProcess = ( struct Process * )FindTask( NULL ) ;
  MyVideoHardware = MyProcess->pr_Task.tc_UserData ;
  Result = NULL ;

  if( ( NULL != MyVideoHardware ) && ( NULL != MyVideoHardware->vh_MemoryReplyMsgPort ) )
  {  /* requirements ok */
    MyContext = MyVideoHardware->vh_Context ;
    SysBase = MyContext->c_SysBase ;
    
    MyMessage = &MyVideoHardware->vh_MemoryMessage ;
    MyMessage->mm_Message.mn_ReplyPort = MyVideoHardware->vh_MemoryReplyMsgPort ;
    MyMessage->mm_Message.mn_Length = sizeof( struct MemoryMessage ) ;
    MyMessage->mm_Size = Size ;
    MyMessage->mm_Attributes = Attributes ;
    MyMessage->mm_MemoryChunk = NULL ;
    PutMsg( MyContext->c_MemoryManager->mm_MsgPort, ( struct Message * )MyMessage ) ;
    WaitPort( MyVideoHardware->vh_MemoryReplyMsgPort ) ;
    GetMsg( MyVideoHardware->vh_MemoryReplyMsgPort ) ;
    Result = MyMessage->mm_MemoryChunk ;
  }
  else
  {
  }

  return( Result ) ;
}


/*
** called from vhi task to free memory
*/
static void CustomFreeVec( void *MemoryChunk )
{
  struct ExecBase *SysBase ;
  struct Process *MyProcess ;
  struct VideoHardware *MyVideoHardware ;
  struct Context *MyContext ;
  struct MemoryMessage *MyMessage ;

  SysBase = *( struct ExecBase ** )4 ;
  MyProcess = ( struct Process * )FindTask( NULL ) ;
  MyVideoHardware = MyProcess->pr_Task.tc_UserData ;

  if( ( NULL != MyVideoHardware ) && ( NULL != MyVideoHardware->vh_MemoryReplyMsgPort ) )
  {  /* requirements ok */
    MyContext = MyVideoHardware->vh_Context ;
    SysBase = MyContext->c_SysBase ;

    MyMessage = &MyVideoHardware->vh_MemoryMessage ;
    MyMessage->mm_Message.mn_ReplyPort = MyVideoHardware->vh_MemoryReplyMsgPort ;
    MyMessage->mm_Message.mn_Length = sizeof( struct MemoryMessage ) ;
    MyMessage->mm_Size = 0 ;
    MyMessage->mm_Attributes = 0 ;
    MyMessage->mm_MemoryChunk = MemoryChunk ;
    PutMsg( MyContext->c_MemoryManager->mm_MsgPort, ( struct Message * )MyMessage ) ;
    WaitPort( MyVideoHardware->vh_MemoryReplyMsgPort ) ;
    GetMsg( MyVideoHardware->vh_MemoryReplyMsgPort ) ;
  }

  return ;
}


/*
** vhi task function
*/
static void VhiSubProcess( void )
{
  struct Process *MyProcess ;
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct Library *VHIBase ;
  struct VideoHardware *MyVideoHardware ;
  struct VhiMessage *MyMessage ;
  ULONG Done ;

  SysBase = *( struct ExecBase ** )4 ;
  MyProcess = ( struct Process * )FindTask( NULL ) ;
  WaitPort( &MyProcess->pr_MsgPort ) ;
  MyMessage = ( struct VhiMessage * )GetMsg( &MyProcess->pr_MsgPort ) ;
  if( VHI_METHOD_STARTUP == MyMessage->vm_Method )
  {  /* this is our startup message */
    MyVideoHardware = MyMessage->vm_Attribute ;
    MyProcess->pr_Task.tc_UserData = MyVideoHardware ;
  }
  else
  {  /* hmmm, not our startup message */
    MyVideoHardware = NULL ;
  }

  if( ( NULL != MyVideoHardware ) && ( NULL != MyVideoHardware->vh_VHIBase ) )
  {  /* requirements ok */
    MyContext = MyVideoHardware->vh_Context ;
    SysBase = MyContext->c_SysBase ;
    VHIBase = MyVideoHardware->vh_VHIBase ;

    MyVideoHardware->vh_VhiMsgPort = CreateMsgPort( ) ;
    if( NULL != MyVideoHardware->vh_VhiMsgPort )
    {  /* vhi message port ok */
      MyVideoHardware->vh_MemoryReplyMsgPort = CreateMsgPort( ) ;
      if( NULL != MyVideoHardware->vh_MemoryReplyMsgPort )
      {  /* memory reply message port ok */
        Done = FALSE ;
        while( !( Done ) )
        {  /* as long as not done ... */
          ReplyMsg( ( struct Message * )MyMessage ) ;
          WaitPort( MyVideoHardware->vh_VhiMsgPort ) ;
          MyMessage = ( struct VhiMessage * )GetMsg( MyVideoHardware->vh_VhiMsgPort ) ;
          switch( MyMessage->vm_Method )
          {
            case VHI_METHOD_FREEVEC:  /* the vhi process shall free memory */
              if( 0 == MyMessage->vm_SubMethod )
              {  /* standard mem handling */
                FreeVec( MyMessage->vm_Attribute ) ;
              }
              else
              {  /* custom mem handling */
              }
              break ;
            case VHI_METHOD_SHUTDOWN:  /* the vhi process shall quit */
              Done = TRUE ;
              break ;
            default:  /* some real vhi method */
              MyMessage->vm_Result = vhi_api( MyMessage->vm_Method,
                                              MyMessage->vm_SubMethod,
                                              MyMessage->vm_Attribute,
                                              MyMessage->vm_ErrorCode,
                                              MyMessage->vm_Handle ) ;
              break ;
          }
        }
        DeleteMsgPort( MyVideoHardware->vh_MemoryReplyMsgPort ) ;
        MyVideoHardware->vh_MemoryReplyMsgPort = NULL ;
      }
      else
      {  /* memory reply message port not ok */
      }
      DeleteMsgPort( MyVideoHardware->vh_VhiMsgPort ) ;
      MyVideoHardware->vh_VhiMsgPort = NULL ;
    }
    else
    {  /* vhi message port not ok */
    }
  }
  else
  {  /* requirements not ok */
  }

  Forbid( ) ;
  ReplyMsg( ( struct Message * )MyMessage ) ;
}


/*
** helper to set parameters of the video hardware
*/
static IPTR VhiComm( struct VideoHardware *MyVideoHardware, ULONG Method, ULONG SubMethod, APTR Attribute, ULONG Blocking )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct VhiMessage *MyMessage ;
  IPTR Result ;

  Result = 0 ;

  if( NULL != MyVideoHardware )
  {  /* requirements ok */
    MyContext = MyVideoHardware->vh_Context ;
    SysBase = MyContext->c_SysBase ;
    MyMessage = &MyVideoHardware->vh_VhiMessage ;

    while( MyVideoHardware->vh_Busy )
    {  /* in case it is busy, wait for the video hardware to become ready */
      Dispatcher( MyContext->c_Dispatcher, BLOCKING_MODE ) ;  /* dispatch blocking */
    }

    MyVideoHardware->vh_ErrorCode = 0 ;
    MyMessage->vm_Method = Method ;
    MyMessage->vm_SubMethod = SubMethod ;
    MyMessage->vm_Attribute = Attribute ;
    MyMessage->vm_ErrorCode = &MyVideoHardware->vh_ErrorCode ;
    MyMessage->vm_Handle = MyVideoHardware->vh_VhiHandle ;
    MyMessage->vm_Result = 0 ;
    PutMsg( MyVideoHardware->vh_VhiMsgPort, ( struct Message * )MyMessage ) ;
    MyVideoHardware->vh_Busy = 1 ;

    if( BLOCKING_MODE == Blocking )
    {  /* caller requests to wait for the result */
      while( MyVideoHardware->vh_Busy )
      {  /* wait for the video hardware to process this message */
        Dispatcher( MyContext->c_Dispatcher, BLOCKING_MODE ) ;  /* dispatch blocking */
      }
      Result = MyMessage->vm_Result ;
    }
  }
  else
  {  /* requirements not ok */
  }

  return( Result ) ;
}


/*
** helper function to set vhi options
*/
static IPTR VhiSetOption( struct VideoHardware *MyVideoHardware, ULONG Option, IPTR Value )
{
  struct vhi_setoptions MySetOptions ;
  IPTR Result ;

  Result = 0 ;

  MySetOptions.option = Option ;
  MySetOptions.value = Value ;
  Result = VhiComm( MyVideoHardware, VHI_METHOD_SET, VHI_OPTIONS, ( APTR )&MySetOptions, BLOCKING_MODE ) ;

  return( Result ) ;
}


/*
** get static properties from the vhi driver
*/
static void StaticQueryVideoHardware( struct VideoHardware *MyVideoHardware )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  ULONG Count ;
  ULONG NumberOfVideoFormats ;

  MyContext = MyVideoHardware->vh_Context ;
  SysBase = MyContext->c_SysBase ;

  if( NULL != MyVideoHardware )
  {  /* requirements ok */
    MyVideoHardware->vh_CardName = ( STRPTR )VhiComm( MyVideoHardware, VHI_METHOD_GET, VHI_CARD_NAME, NULL, BLOCKING_MODE ) ;
    MyVideoHardware->vh_CardManufacturer = ( STRPTR )VhiComm( MyVideoHardware, VHI_METHOD_GET, VHI_CARD_MANUFACTURER, NULL, BLOCKING_MODE ) ;
    MyVideoHardware->vh_CardDriverAuthor = ( STRPTR )VhiComm( MyVideoHardware, VHI_METHOD_GET, VHI_CARD_DRIVERAUTHOR, NULL, BLOCKING_MODE ) ;
    MyVideoHardware->vh_CardDriverVersion = VhiComm( MyVideoHardware, VHI_METHOD_GET, VHI_CARD_DRIVERVERSION, NULL, BLOCKING_MODE ) ;
    MyVideoHardware->vh_CardDriverRevision = VhiComm( MyVideoHardware, VHI_METHOD_GET, VHI_CARD_DRIVERREVISION, NULL, BLOCKING_MODE ) ;
    MyVideoHardware->vh_CardVersion = VhiComm( MyVideoHardware, VHI_METHOD_GET, VHI_CARD_VERSION, NULL, BLOCKING_MODE ) ;
    MyVideoHardware->vh_CardRevision = VhiComm( MyVideoHardware, VHI_METHOD_GET, VHI_CARD_REVISION, NULL, BLOCKING_MODE ) ;
    MyVideoHardware->vh_SupportedColorModes = VhiComm( MyVideoHardware, VHI_METHOD_GET, VHI_SUPPORTED_MODES, NULL, BLOCKING_MODE ) ;
    MyVideoHardware->vh_NumberOfInputs = VhiComm( MyVideoHardware, VHI_METHOD_GET, VHI_NUMBER_OF_INPUTS, NULL, BLOCKING_MODE ) ;
    MyVideoHardware->vh_NameOfInput = AllocVec( ( ( MyVideoHardware->vh_NumberOfInputs + 1 ) * sizeof( STRPTR ) ), MEMF_PUBLIC ) ;
    if( NULL != MyVideoHardware->vh_NameOfInput )
    {  /* space to store input names ok */
      for( Count = 0 ; Count < MyVideoHardware->vh_NumberOfInputs ; Count++ )
      {  /* get all input names */
        MyVideoHardware->vh_NameOfInput[ Count ] = ( STRPTR )VhiComm( MyVideoHardware, VHI_METHOD_GET, VHI_NAME_OF_INPUT, ( APTR )( IPTR )Count, BLOCKING_MODE ) ;
      }
      MyVideoHardware->vh_NameOfInput[ Count ] = NULL ;
    }
    MyVideoHardware->vh_SupportedOptions = VhiComm( MyVideoHardware, VHI_METHOD_GET, VHI_SUPPORTED_OPTIONS, NULL, BLOCKING_MODE ) ;
    MyVideoHardware->vh_SupportedVideoFormats = VhiComm( MyVideoHardware, VHI_METHOD_GET, VHI_SUPPORTED_VIDEOFORMATS, NULL, BLOCKING_MODE ) ;
    NumberOfVideoFormats = 0 ;
    for( Count = 0 ; Count < 8 ; Count++ )
    {
      if( MyVideoHardware->vh_SupportedVideoFormats & ( 1UL << Count ) )
      {
        NumberOfVideoFormats++ ;
      }
    }
    MyVideoHardware->vh_NameOfVideoFormat = AllocVec( ( ( NumberOfVideoFormats + 1 ) * sizeof( STRPTR ) ), MEMF_PUBLIC ) ;
    if( NULL != MyVideoHardware->vh_NameOfVideoFormat )
    {  /* space to store video format names ok */
      for( Count = 0 ; Count < 8 ; Count++ )
      {  /* get all video format names */
        if( MyVideoHardware->vh_SupportedVideoFormats & ( 1UL << Count ) )
        {
          switch( 1UL << Count )
          {
            case VHI_FORMAT_PAL:
              MyVideoHardware->vh_NameOfVideoFormat[ Count ] = "PAL" ;
              break ;
            case VHI_FORMAT_NTSC:
              MyVideoHardware->vh_NameOfVideoFormat[ Count ] = "NTSC" ;
              break ;
            case VHI_FORMAT_SECAM:
              MyVideoHardware->vh_NameOfVideoFormat[ Count ] = "SECAM" ;
              break ;
            case VHI_FORMAT_PAL60:
              MyVideoHardware->vh_NameOfVideoFormat[ Count ] = "NTSC" ;
              break ;
            default:
              MyVideoHardware->vh_NameOfVideoFormat[ Count ] = "?" ;
              break ;
          }
        }
      }
      MyVideoHardware->vh_NameOfVideoFormat[ NumberOfVideoFormats ] = NULL ;
    }
  }
}


/*
** free static properties obtained from the vhi driver
*/
static void StaticUnqueryVideoHardware( struct VideoHardware *MyVideoHardware )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  ULONG Count ;

  if( NULL != MyVideoHardware )
  {  /* requirements ok */
    MyContext = MyVideoHardware->vh_Context ;
    SysBase = MyContext->c_SysBase ;

    if( NULL != MyVideoHardware->vh_NameOfVideoFormat )
    {  /* space where video format names pointers were stored needs to be freed */
      FreeVec( MyVideoHardware->vh_NameOfVideoFormat ) ;
    }
    if( NULL != MyVideoHardware->vh_NameOfInput )
    {  /* space where input names pointers were stored needs to be freed */
      for( Count = 0 ; Count < MyVideoHardware->vh_NumberOfInputs ; Count++ )
      {  /* free all input names */
        VhiComm( MyVideoHardware, VHI_METHOD_FREEVEC, 0, ( APTR )MyVideoHardware->vh_NameOfInput[ Count ], BLOCKING_MODE ) ;
      }
      FreeVec( MyVideoHardware->vh_NameOfInput ) ;
    }
    VhiComm( MyVideoHardware, VHI_METHOD_FREEVEC, 0, ( APTR )MyVideoHardware->vh_CardDriverAuthor, BLOCKING_MODE ) ;
    VhiComm( MyVideoHardware, VHI_METHOD_FREEVEC, 0, ( APTR )MyVideoHardware->vh_CardManufacturer, BLOCKING_MODE ) ;
    VhiComm( MyVideoHardware, VHI_METHOD_FREEVEC, 0, ( APTR )MyVideoHardware->vh_CardName, BLOCKING_MODE ) ;
  }
}


/*
** get dynamic properties from the vhi driver
*/
void DynamicQueryVideoHardware( struct VideoHardware *MyVideoHardware )
{
  if( NULL != MyVideoHardware )
  {  /* requirements ok */
    VhiComm( MyVideoHardware, VHI_METHOD_GET, VHI_MAXIMUM_SIZE, ( APTR )&MyVideoHardware->vh_MaximumSize, BLOCKING_MODE ) ;
  }
}


/*
** close vhi hardware driver and stop the vhi task
*/
void CloseVideoHardware( struct VideoHardware *MyVideoHardware )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;

  if( NULL != MyVideoHardware )
  {  /* requirements ok */
    MyContext = MyVideoHardware->vh_Context ;
    SysBase = MyContext->c_SysBase ;
    if( NULL != MyVideoHardware->vh_VHIBase )
    {  /* vhi driver library needs to be closed */
      if( NULL != MyVideoHardware->vh_VhiProcess )
      {  /* vhi process needs to be deleted */
        if( NULL != MyVideoHardware->vh_VhiMsgPort )
        {  /* vhi task needs to be stopped */
          if( NULL != MyVideoHardware->vh_VhiHandle )
          {  /* vhi hardware needs to be closed */
            StaticUnqueryVideoHardware( MyVideoHardware ) ;
            VhiComm( MyVideoHardware, VHI_METHOD_CLOSE, 0, NULL, BLOCKING_MODE ) ;
            MyVideoHardware->vh_VhiHandle = NULL ;
          }
          /* MyVideoHardware->vh_VHIMsgPort is gone with the vhi process */
        }
        VhiComm( MyVideoHardware, VHI_METHOD_SHUTDOWN, 0, NULL, BLOCKING_MODE ) ;
        MyVideoHardware->vh_VhiProcess = NULL ;
      }
      CloseLibrary( MyVideoHardware->vh_VHIBase ) ;
      MyVideoHardware->vh_VHIBase = NULL ;
    }
  }
}


/*
** start vhi process and open the vhi hardware driver
*/
void OpenVideoHardware( struct VideoHardware *MyVideoHardware )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct DosLibrary *DOSBase ;

  if( ( NULL != MyVideoHardware ) )
  {  /* requirements seems ok */
    MyContext = MyVideoHardware->vh_Context ;
    SysBase = MyContext->c_SysBase ;
    DOSBase = MyContext->c_DOSBase ;
    MyVideoHardware->vh_VHIBase = OpenLibrary( ( const char * )MyVideoHardware->vh_CurrentVhiDriver, 0 ) ;
    if( NULL != MyVideoHardware->vh_VHIBase )
    {  /* vhi driver library ok */
      snprintf( MyVideoHardware->vh_VhiProcessName, VH_VHIPROCESSNAME_LENGTH, "cAMIra VideoHardware (%s)", MyVideoHardware->vh_CurrentVhiDriver ) ;
      MyVideoHardware->vh_VhiProcess = CreateNewProcTags( NP_Entry, ( IPTR )&VhiSubProcess, 
        NP_Name, ( IPTR )MyVideoHardware->vh_VhiProcessName,
        NP_Priority, ( ( MyContext->c_Process->pr_Task.tc_Node.ln_Pri ) - 1 ),
        TAG_END ) ;
      if( NULL != MyVideoHardware->vh_VhiProcess )
      {  /* vhi process created */
        MyVideoHardware->vh_ErrorCode = 0 ;
        MyVideoHardware->vh_VhiMessage.vm_Method = VHI_METHOD_STARTUP ;
        MyVideoHardware->vh_VhiMessage.vm_Attribute = MyVideoHardware ;
        MyVideoHardware->vh_VhiMessage.vm_ErrorCode = &MyVideoHardware->vh_ErrorCode ;
        PutMsg( &MyVideoHardware->vh_VhiProcess->pr_MsgPort, ( struct Message * )&MyVideoHardware->vh_VhiMessage ) ;
        MyVideoHardware->vh_Busy = 1 ;
        while( MyVideoHardware->vh_Busy )
        {  /* wait for the video hardware to process this message */
          Dispatcher( MyContext->c_Dispatcher, BLOCKING_MODE ) ;  /* dispatch blocking */
        }
        if( NULL != MyVideoHardware->vh_VhiMsgPort )
        {  /* vhi task could create the message port, so it runs */
          MyVideoHardware->vh_VhiHandle = ( APTR )VhiComm( MyVideoHardware, VHI_METHOD_OPEN, 0, NULL, BLOCKING_MODE ) ;
          if( NULL != MyVideoHardware->vh_VhiHandle )
          {  /* could open vhi hardware */
            StaticQueryVideoHardware( MyVideoHardware ) ;
            MyVideoHardware->vh_CurrentVhiInput = ( ULONG )-1 ; /* force selecting also input during configure */
            MyVideoHardware->vh_CurrentVhiVideoFormat = ( ULONG )-1 ; /* force selecting also video format during configure */
            MyVideoHardware->vh_CurrentVhiColorMode = VhiComm( MyVideoHardware, VHI_METHOD_GET, VHI_COLOR_MODE, NULL, BLOCKING_MODE ) ;
            if( !( MyVideoHardware->vh_SupportedColorModes & ( 1 << ( MyVideoHardware->vh_CurrentVhiColorMode ^ 1 ) ) ) )
            {  /* driver is in unsupported color mode?!? */
              LogText( MyContext->c_Log, WARNING_LEVEL, "initially unsupported color mode reported" ) ;
              SelectVhiColorMode( MyVideoHardware, ( MyVideoHardware->vh_CurrentVhiColorMode ^ 1 ) ) ;
            }
            if( VHI_OPT_INTERLACE & MyVideoHardware->vh_SupportedOptions )
            {  /* setting interlace option is supported */
              LogText( MyContext->c_Log, INFO_LEVEL, "enabling VHI intercace mode" ) ;
              VhiSetOption( MyVideoHardware, VHI_OPT_INTERLACE, TRUE ) ;
            }
            /* try to use the trustme mode */
            VhiComm( MyVideoHardware, VHI_METHOD_SET, VHI_TRUSTME_MODE, ( APTR )( 1 ), BLOCKING_MODE ) ;
            if( !( MyVideoHardware->vh_ErrorCode ) )
            {  /* trustme mode avaialble */
              if( ( MyVideoHardware->vh_TrustmeMode = VhiComm( MyVideoHardware, VHI_METHOD_GET, VHI_TRUSTME_MODE, NULL, BLOCKING_MODE ) ) )
              {
                LogText( MyContext->c_Log, INFO_LEVEL, "Trustme Mode enabled" ) ;
              }
            }
            else
            {  /* trusme mode not available */
              LogText( MyContext->c_Log, INFO_LEVEL, "Trustme Mode not available" ) ;
            }
            /* setup the custom memory handling for the driver */
            MyVideoHardware->vh_Digitize.custom_memhandling = TRUE ;
            MyVideoHardware->vh_Digitize.CstAllocVec = CustomAllocVec ;
            MyVideoHardware->vh_Digitize.CstFreeVec = CustomFreeVec ;
          }
          else
          {  /* could not open vhi hardware */
            LogText( MyContext->c_Log, ERROR_LEVEL, "could not open vhi hardware" ) ;
            CloseVideoHardware( MyVideoHardware ) ;
          }
        }
        else
        {  /* vhi task could not create the message port */
          LogText( MyContext->c_Log, ERROR_LEVEL, "vhi task could not create the message port" ) ;
          CloseVideoHardware( MyVideoHardware ) ;
        }
      }
      else
      {  /* vhi process could not be created */
        LogText( MyContext->c_Log, ERROR_LEVEL, "vhi process could not be created" ) ;
        CloseVideoHardware( MyVideoHardware ) ;
      }
    }
    else
    {  /* vhi driver library not ok */
      LogText( MyContext->c_Log, ERROR_LEVEL, "vhi driver not ok" ) ;
    }
  }
  else
  {  /* required parameters not ok */
  }
}


/*
** select input of the video hardware
*/
BOOL SelectVhiInput( struct VideoHardware *MyVideoHardware, ULONG NewVhiInput )
{
  struct Context *MyContext ;
  BOOL VhiInputChanged ;

  VhiInputChanged = FALSE ;

  if( NULL != MyVideoHardware )
  {  /* requirements ok */
    MyContext = MyVideoHardware->vh_Context ;

    if( MyVideoHardware->vh_CurrentVhiInput != NewVhiInput )
    {  /* input change required */
      if( VHI_OPT_INPUT & MyVideoHardware->vh_SupportedOptions )
      {  /* setting input option is supported */
        if( MyVideoHardware->vh_NumberOfInputs > NewVhiInput )
        {  /* vhi input in range */
          if( 0 == VhiSetOption( MyVideoHardware, VHI_OPT_INPUT, NewVhiInput ) )
          {  /* no error, new input selected */
            LogText( MyContext->c_Log, DEBUG_LEVEL, "configured vhi input: %s", MyVideoHardware->vh_NameOfInput[ NewVhiInput ] ) ;
            MyVideoHardware->vh_CurrentVhiInput = NewVhiInput ;
            VhiInputChanged = TRUE ;
          }
        }
        else
        {  /* vhi input out of range ?!? */
          LogText( MyContext->c_Log, WARNING_LEVEL, "unsuported vhi input: %ld", NewVhiInput ) ;
        }
      }
      else
      {  /* setting input option is not supported, current input must be input 0 */
        MyVideoHardware->vh_CurrentVhiInput = 0 ;
      }
    }
    else
    {  /* no input change required */
    }
  }
  else
  {  /* requirements not ok */
  }

  return( VhiInputChanged ) ;
}


/*
** select color mode of the video hardware
*/
BOOL SelectVhiColorMode( struct VideoHardware *MyVideoHardware, ULONG NewVhiColorMode )
{
  struct Context *MyContext ;
  BOOL VhiColorModeChanged ;

  VhiColorModeChanged = FALSE ;

  if( NULL != MyVideoHardware )
  {  /* requirements ok */
    MyContext = MyVideoHardware->vh_Context ;

    if( MyVideoHardware->vh_CurrentVhiColorMode != NewVhiColorMode )
    {  /* no colormode change required */
      if( ( ( NewVhiColorMode ) && ( VHI_MODE_COLOR & MyVideoHardware->vh_SupportedColorModes ) ) ||
          ( ( FALSE == NewVhiColorMode ) && ( VHI_MODE_GRAYSCALE & MyVideoHardware->vh_SupportedColorModes ) ) )
      {  /* supported vhi color mode setting */
        if( 0 == VhiComm( MyVideoHardware, VHI_METHOD_SET, VHI_COLOR_MODE, ( APTR )( IPTR )( NewVhiColorMode ), BLOCKING_MODE ) )
        {
          LogText( MyContext->c_Log, DEBUG_LEVEL, "configured vhi color mode: %ld", NewVhiColorMode ) ;
          MyVideoHardware->vh_CurrentVhiColorMode = NewVhiColorMode ;
          VhiColorModeChanged = TRUE ;
        }
      }
      else
      {  /* unsupported vhi color mode setting ?!? */
        LogText( MyContext->c_Log, WARNING_LEVEL, "unsuported vhi color mode: %ld", NewVhiColorMode ) ;
      }
    }
    else
    {  /* no colormode change required */
    }
  }
  else
  {  /* requirements not ok */
  }

  return( VhiColorModeChanged ) ;
}



/*
** select video format of the video hardware
*/
BOOL SelectVhiVideoFormat( struct VideoHardware *MyVideoHardware, ULONG NewVhiVideoFormat )
{
  struct Context *MyContext ;
  BOOL VhiVideoFormatChanged ;

  VhiVideoFormatChanged = FALSE ;

  if( NULL != MyVideoHardware )
  {  /* requirements ok */
    MyContext = MyVideoHardware->vh_Context ;

    if( MyVideoHardware->vh_CurrentVhiVideoFormat != NewVhiVideoFormat )
    {  /* no video format change required */
      if( ( ( 1UL << NewVhiVideoFormat ) & ( MyVideoHardware->vh_SupportedVideoFormats ) ) )
       {  /* supported vhi video format setting */
        if( 0 == VhiComm( MyVideoHardware, VHI_METHOD_SET, VHI_VIDEOFORMAT, ( APTR )( 1UL << NewVhiVideoFormat ), BLOCKING_MODE ) )
        {
          LogText( MyContext->c_Log, DEBUG_LEVEL, "configured vhi video format: %s", MyVideoHardware->vh_NameOfVideoFormat[ NewVhiVideoFormat ] ) ;
          MyVideoHardware->vh_CurrentVhiVideoFormat = NewVhiVideoFormat ;
          VhiVideoFormatChanged = TRUE ;
        }
      }
      else
      {  /* unsupported vhi color mode setting ?!? */
        LogText( MyContext->c_Log, WARNING_LEVEL, "unsuported vhi video format: %ld", NewVhiVideoFormat ) ;
      }
    }
    else
    {  /* no colormode change required */
    }
  }
  else
  {  /* requirements not ok */
  }

  return( VhiVideoFormatChanged ) ;
}


/*
** configure the video hardware
*/
void ConfigureVideoHardware( struct VideoHardware *MyVideoHardware )
{
  struct Context *MyContext ;
  struct DosLibrary *DOSBase ;
  SIPTR SettingValue ;
  STRPTR NewVhiDriver ;
  ULONG NewVhiInput ;
  ULONG NewVhiColorMode ;
  ULONG NewVhiVideoFormat ;

  if( NULL != MyVideoHardware )
  {  /* requirements ok */
    MyContext = MyVideoHardware->vh_Context ;
    DOSBase = MyContext->c_DOSBase ;

    SettingValue = GetSetting( MyContext->c_Settings, VhiDriver ) ;
    NewVhiDriver = ( STRPTR )SettingValue ;

    if( ( NULL == NewVhiDriver ) ||
        ( 0 != strcmp( FilePart( MyVideoHardware->vh_CurrentVhiDriver ), FilePart( NewVhiDriver ) ) ) )
    {  /* no or different driver */
      if( NULL != MyVideoHardware->vh_VhiHandle )
      {  /* then we have to close the current vhi driver */
        LogText( MyContext->c_Log, DEBUG_LEVEL, "close vhi driver: %s", MyVideoHardware->vh_CurrentVhiDriver ) ;
        CloseVideoHardware( MyVideoHardware ) ;
      }
      MyVideoHardware->vh_CurrentVhiDriver[ 0 ] = '\0' ;
    }

    if( NULL != NewVhiDriver )
    {  /* there is a driver specified in the settings */
      if( NULL == MyVideoHardware->vh_VhiHandle )
      {  /* then we need to open the new vhi driver */
        snprintf( MyVideoHardware->vh_CurrentVhiDriver, VH_CURRENTVHIDRIVER_LENGTH, "%s", NewVhiDriver ) ;
        LogText( MyContext->c_Log, DEBUG_LEVEL, "open vhi driver: %s", MyVideoHardware->vh_CurrentVhiDriver ) ;
        OpenVideoHardware( MyVideoHardware ) ;
      }
      if( NULL != MyVideoHardware->vh_VhiHandle )
      {  /* there is a open vhi driver */
        NewVhiInput = ( ULONG )GetSetting( MyContext->c_Settings, VhiInput ) ;
        SelectVhiInput( MyVideoHardware, NewVhiInput ) ;
        NewVhiColorMode = ( ULONG )GetSetting( MyContext->c_Settings, VhiColormode ) ;
        SelectVhiColorMode( MyVideoHardware, NewVhiColorMode ) ;
        NewVhiVideoFormat = ( ULONG )GetSetting( MyContext->c_Settings, VhiVideoformat ) ;
        SelectVhiVideoFormat( MyVideoHardware, NewVhiVideoFormat ) ;
        DynamicQueryVideoHardware( MyVideoHardware ) ;
        /* check cropping */
        if( !( MyVideoHardware->vh_MaximumSize.fixed ) )
        {  /* cropping */
          SettingValue = GetSetting( MyContext->c_Settings, VhiX1 ) ;
          if( ( ( 0 ) <= SettingValue ) && ( ( MyVideoHardware->vh_MaximumSize.max_width - 1 ) >= SettingValue ) )
          {  /* use custom x1 */
            MyVideoHardware->vh_CurrentX1 = SettingValue ;
          }
          else
          {  /* use default x1 */
            MyVideoHardware->vh_CurrentX1 = 0 ;
          }
          SettingValue = GetSetting( MyContext->c_Settings, VhiY1 ) ;
          if( ( ( 0 ) <= SettingValue ) && ( ( MyVideoHardware->vh_MaximumSize.max_height - 1 ) >= SettingValue ) )
          {  /* use custom y1 */
            MyVideoHardware->vh_CurrentY1 = SettingValue ;
          }
          else
          {  /* use default y1 */
            MyVideoHardware->vh_CurrentY1 = 0 ;
          }
          SettingValue = GetSetting( MyContext->c_Settings, VhiX2 ) ;
          if( ( ( 0 ) <= SettingValue ) && ( ( MyVideoHardware->vh_MaximumSize.max_width - 1 ) >= SettingValue ) )
          {  /* use custom x2 */
            MyVideoHardware->vh_CurrentX2 = SettingValue ;
          }
          else
          {  /* use default x2 */
            MyVideoHardware->vh_CurrentX2 = ( MyVideoHardware->vh_MaximumSize.max_width - 1 ) ;
          }
          SettingValue = GetSetting( MyContext->c_Settings, VhiY2 ) ;
          if( ( ( 0 ) <= SettingValue ) && ( ( MyVideoHardware->vh_MaximumSize.max_height - 1 ) >= SettingValue ) )
          {  /* use custom y2 */
            MyVideoHardware->vh_CurrentY2 = SettingValue ;
          }
          else
          {  /* use default y2 */
            MyVideoHardware->vh_CurrentY2 = ( MyVideoHardware->vh_MaximumSize.max_height - 1 ) ;
          }
        }
        else
        {  /* no cropping */
          MyVideoHardware->vh_CurrentX1 = 0 ;
          MyVideoHardware->vh_CurrentY1 = 0 ;
          MyVideoHardware->vh_CurrentX2 = ( MyVideoHardware->vh_MaximumSize.max_width - 1 ) ;
          MyVideoHardware->vh_CurrentY2 = ( MyVideoHardware->vh_MaximumSize.max_height - 1 ) ;
        }
        /* check scaling */
        if( MyVideoHardware->vh_MaximumSize.scalable )
        {  /* scaling */
          SettingValue = GetSetting( MyContext->c_Settings, VhiWidth ) ;
          MyVideoHardware->vh_CurrentWidth = SettingValue ;
          SettingValue = GetSetting( MyContext->c_Settings, VhiHeight ) ;
          MyVideoHardware->vh_CurrentHeight = SettingValue ;
        }
        else
        {  /* no scaling */
          MyVideoHardware->vh_CurrentWidth = 0 ;
          MyVideoHardware->vh_CurrentHeight = 0 ;
        }
        /* prepare digitize structure */
        MyVideoHardware->vh_Digitize.dim.x1 = MyVideoHardware->vh_CurrentX1 ;
        MyVideoHardware->vh_Digitize.dim.y1 = MyVideoHardware->vh_CurrentY1 ;
        MyVideoHardware->vh_Digitize.dim.x2 = MyVideoHardware->vh_CurrentX2 ;
        MyVideoHardware->vh_Digitize.dim.y2 = MyVideoHardware->vh_CurrentY2 ;
        MyVideoHardware->vh_Digitize.dim.dst_width = MyVideoHardware->vh_CurrentWidth ;
        MyVideoHardware->vh_Digitize.dim.dst_height = MyVideoHardware->vh_CurrentHeight ;
        if( MyVideoHardware->vh_TrustmeMode )
        {  /* set trustme size */
          VhiComm( MyVideoHardware, VHI_METHOD_PERFORM, VHI_CHECK_DIGITIZE_SIZE, ( APTR )( &MyVideoHardware->vh_Digitize.dim ), BLOCKING_MODE ) ;
          VhiComm( MyVideoHardware, VHI_METHOD_SET, VHI_TRUSTME_SIZE, ( APTR )( &MyVideoHardware->vh_Digitize.dim ), BLOCKING_MODE ) ;
          if( !(MyVideoHardware->vh_ErrorCode ) )
          {  /* setting trustme dimensions ok */
            LogText( MyContext->c_Log, DEBUG_LEVEL, "configured vhi trustme dimensions: (%ld:%ld)-(%ld:%ld), %ldx%ld",
              MyVideoHardware->vh_Digitize.dim.x1,
              MyVideoHardware->vh_Digitize.dim.y1,
              MyVideoHardware->vh_Digitize.dim.x2,
              MyVideoHardware->vh_Digitize.dim.y2,
              MyVideoHardware->vh_Digitize.dim.dst_width,
              MyVideoHardware->vh_Digitize.dim.dst_height ) ;
          }
          else
          {  /* setting trustme dimensions failed */
            LogText( MyContext->c_Log, ERROR_LEVEL, "sending trustme size failed (%ld)", MyVideoHardware->vh_ErrorCode ) ;
          }
        }
      }
    }
  }
  else
  {  /* requirements not ok */
  }
}


/*
** handle video hardware evets
*/
static ULONG DoVideoHardware( struct VideoHardware *MyVideoHardware, ULONG TriggerSignals )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct VhiMessage *MyMessage ;

  if( NULL != MyVideoHardware )
  {  /* requirements seem ok */
    MyContext = MyVideoHardware->vh_Context ;
    SysBase = MyContext->c_SysBase ;

    MyMessage = ( struct VhiMessage * )GetMsg( MyVideoHardware->vh_VhiReplyMsgPort ) ;
    if( ( &MyVideoHardware->vh_VhiMessage == MyMessage ) )
    {  /* the message returned */
      switch( MyMessage->vm_Method )
      {
        case VHI_METHOD_PERFORM:  /* something was performed */
          switch( MyMessage->vm_SubMethod )
          {
            case VHI_DIGITIZE_PICTURE:  /* a picture was digitized */
              GetClock( MyContext->c_Timer, &MyVideoHardware->vh_CurrentImageBox->ib_ClockData ) ;
              if( ( 0 != MyMessage->vm_Result ) && ( 0 == MyVideoHardware->vh_ErrorCode ) )
              {  /* it seems, there was no problem */
                MyVideoHardware->vh_CurrentImageBox->ib_VhiImage = ( struct vhi_image * )MyMessage->vm_Result ;
              }
              else
              {  /* it seems, there was a problem */
                MyVideoHardware->vh_CurrentImageBox->ib_VhiImage = NULL ;
                MyVideoHardware->vh_CurrentImageBox->ib_Error = MyVideoHardware->vh_ErrorCode ;
                LogText( MyContext->c_Log, ERROR_LEVEL, "digitize error: %s", VhiErrorToString( MyVideoHardware->vh_ErrorCode ) ) ;
              }
              MyContext->c_NewImageBox = MyVideoHardware->vh_CurrentImageBox ;
              MyVideoHardware->vh_CurrentImageBox = NULL ;
              break ;
            default:  /* unsupported stuff was performed */
              break ;
          }
          break ;
        default:  /* unsupported vhi method */
          break ;
      }
      MyVideoHardware->vh_Busy = 0 ;
    }
    else
    {  /* no message returned */
    }
  }
  else
  {  /* requirements not ok */
  }

  TriggerSignals = ( 1UL << MyVideoHardware->vh_VhiReplyMsgPort->mp_SigBit ) ;

  return( TriggerSignals ) ;
}


/*
** delete video hardware
*/
void DeleteVideoHardware( struct VideoHardware *OldVideoHardware )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;

  if( NULL != OldVideoHardware )
  {  /* video hardware context memory needs to be freed */
    MyContext = OldVideoHardware->vh_Context ;
    SysBase = MyContext->c_SysBase ;
    if( NULL != OldVideoHardware->vh_VhiReplyMsgPort )
    {  /* vhi reply message port needs to be closed */
      CloseVideoHardware( OldVideoHardware ) ;
      RemDispatcherNode( MyContext->c_Dispatcher, &OldVideoHardware->vh_DispatcherNode ) ;
      DeleteMsgPort( OldVideoHardware->vh_VhiReplyMsgPort ) ;
    }
    FreeVec( OldVideoHardware ) ;
  }
}


/*
** create video hardware
*/
struct VideoHardware *CreateVideoHardware( struct Context *MyContext )
{
  struct ExecBase *SysBase ;
  struct VideoHardware *NewVideoHardware ;

  NewVideoHardware = NULL ;

  if( ( NULL != MyContext->c_MemoryManager ) )
  {  /* requirements seems ok */
    SysBase = MyContext->c_SysBase ;
    NewVideoHardware = AllocVec( sizeof( struct VideoHardware ), MEMF_ANY | MEMF_CLEAR ) ;
    if( NULL != NewVideoHardware )
    {  /* video hardware context ok */
      NewVideoHardware->vh_Context = MyContext ;
      NewVideoHardware->vh_VhiReplyMsgPort = CreateMsgPort( ) ;
      if( NULL != NewVideoHardware->vh_VhiReplyMsgPort )
      {  /* vhi reply message port ok */
        NewVideoHardware->vh_VhiMessage.vm_Message.mn_ReplyPort = NewVideoHardware->vh_VhiReplyMsgPort ;
        NewVideoHardware->vh_VhiMessage.vm_Message.mn_Length = sizeof( struct VhiMessage ) ;
        NewVideoHardware->vh_DispatcherNode.dn_Signals = ( 1UL << NewVideoHardware->vh_VhiReplyMsgPort->mp_SigBit ) ;
        NewVideoHardware->vh_DispatcherNode.dn_DoFunction = ( DISPATCHER_DOFUNCTION )DoVideoHardware ;
        NewVideoHardware->vh_DispatcherNode.dn_DoData = NewVideoHardware ;
        AddDispatcherNode( MyContext->c_Dispatcher, &NewVideoHardware->vh_DispatcherNode ) ;
      }
      else
      {  /* vhi reply message port not ok */
        LogText( MyContext->c_Log, ERROR_LEVEL, "vhi reply message port not ok" ) ;
        DeleteVideoHardware( NewVideoHardware ) ;
        NewVideoHardware = NULL ;
      }
    }
    else
    {  /* video hardware context not ok */
      LogText( MyContext->c_Log, ERROR_LEVEL, "video hardware context not ok" ) ;
    }
  }
  else
  {  /* required parameters not ok */
  }

  return( NewVideoHardware ) ;
}


/*
** check status of the video hardware
*/
LONG CheckVideoHardware( struct VideoHardware *MyVideoHardware )
{
  LONG Result ;

  Result = 0 ;

  if( NULL != MyVideoHardware )
  {  /* requirements ok */
    if( NULL != MyVideoHardware->vh_VhiHandle )
    {  /* video hardware is open */
      if( NULL != MyVideoHardware->vh_CurrentImageBox )
      {  /* currently busy */
        Result = 1 ;
      }
      else
      {  /* currently idle */
      }
    }
    else
    {  /* video hardware is not open */
      Result = -2 ;
    }
  }
  else
  {  /* requirements not ok */
    Result = -1 ;
  }

  return( Result ) ;
}


/*
** start digitizing asynchonly
*/
LONG DigitizeImage( struct VideoHardware *MyVideoHardware, struct ImageBox *MyImageBox )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  LONG Error ;

  Error = CheckVideoHardware( MyVideoHardware ) ;
  if( !( Error ) && ( NULL != MyImageBox ) )
  {  /* requirements seem ok */
    MyContext = MyVideoHardware->vh_Context ;
    SysBase = MyContext->c_SysBase ;

    MyImageBox->ib_Error = 0 ;
    MyVideoHardware->vh_CurrentImageBox = MyImageBox ;
    Error = VhiComm( MyVideoHardware, VHI_METHOD_PERFORM, VHI_DIGITIZE_PICTURE, ( APTR )&MyVideoHardware->vh_Digitize, NONBLOCKING_MODE ) ;
  }
  else
  {  /* requirements not ok */
  }

  return( Error ) ;
}
