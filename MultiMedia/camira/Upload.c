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
** Upload.c
*/

#include "Upload.h"
#include "Settings.h"
#include "cAMIraLib.h"
#include "Log.h"
#include "ab_stdio.h"
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/socket.h>
#include <clib/alib_protos.h>
#include <bsdsocket/socketbasetags.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/errno.h>
#include <netdb.h>
#include <ctype.h>

#include <string.h>

/*
** ftp commands we use
*/
#define FTP_CMD_NONE ( 0 )
#define FTP_CMD_USER ( 1 )
#define FTP_CMD_PASS ( 2 )
#define FTP_CMD_CWD ( 3 )
#define FTP_CMD_TYPE ( 4 )
#define FTP_CMD_PORT ( 5 )
#define FTP_CMD_PASV ( 6 )
#define FTP_CMD_STOR ( 7 )
#define FTP_CMD_QUIT ( 8 )


/*
** called on server timeout
*/
static void DoUploadTimer( struct Upload *MyUpload )
{
  struct Context *MyContext ;
  
  MyContext = MyUpload->u_Context ;

  LogText( MyContext->c_Log, ERROR_LEVEL, "server timeout" ) ;
  MyUpload->u_UploadError = TRUE ;
  StopUpload( MyUpload ) ;
}


/*
** start the timeout timer
*/
static void StartUploadTimer( struct Upload *MyUpload )
{
  struct Context *MyContext ;
  
  MyContext = MyUpload->u_Context ;

  MyUpload->u_TimerNode.tn_DeltaTime.tv_secs = MyUpload->u_CurrentUploadTimeout  ;
  MyUpload->u_TimerNode.tn_DeltaTime.tv_micro = 0 ;
  MyUpload->u_TimerNode.tn_AbsoluteTime.tv_secs = 0 ;  /* from now on */
  MyUpload->u_TimerNode.tn_DoFunction = ( void ( * )( APTR ) )DoUploadTimer ;
  MyUpload->u_TimerNode.tn_DoData = MyUpload ;
  AddTimerNode( MyContext->c_Timer, &MyUpload->u_TimerNode ) ;
}


/*
** stop the timeout timer
*/
static void StopUploadTimer( struct Upload *MyUpload )
{
  struct Context *MyContext ;
  
  MyContext = MyUpload->u_Context ;

  RemTimerNode( MyContext->c_Timer, &MyUpload->u_TimerNode ) ;
}


/*
** delete one of our async sockets
*/
static void DeleteSocket( struct Upload *MyUpload, LONG OldSocket )
{
  struct Context *MyContext ;
  struct Library *SocketBase ;

  MyContext = MyUpload->u_Context ;
  SocketBase = MyUpload->u_SocketBase ;

  if( 0 <= OldSocket )
  {  /* socket needs to be closed */
#if 0
    if( 0 <= shutdown( OldSocket, SHUT_RDWR ) )
    {  /* could shutdown socket */
    }
    else
    {  /* could not shutdown socket */
      LogText( MyContext->c_Log, WARNING_LEVEL, "could not shutdown socket %ld: %ld", OldSocket, MyUpload->u_ErrorNumber ) ; 
    }
#endif
    if( 0 <= CloseSocket( OldSocket ) )
    {  /* could close socket */
    }
    else
    {  /* could not close socket */
      LogText( MyContext->c_Log, WARNING_LEVEL, "could not close socket %ld: %ld", OldSocket, MyUpload->u_ErrorNumber ) ; 
    }
  }
}


/*
** prepare a sockets for async operation
*/
static LONG PrepareSocket( struct Upload *MyUpload, LONG MySocket, LONG MyEventMask )
{
  struct Context *MyContext ;
  struct Library *SocketBase ;
  LONG Error ;
  LONG OptionValue ;
  
  MyContext = MyUpload->u_Context ;
  SocketBase = MyUpload->u_SocketBase ;

  if( 0 <= MySocket )
  {  /* requirements ok */
    OptionValue = 1 ;
    Error = IoctlSocket( MySocket, FIOASYNC, ( char * )&OptionValue ) ;
    if( !( Error ) )
    {  /* could enable async mode */
      OptionValue = 1 ;
      Error = IoctlSocket( MySocket, FIONBIO, ( char * )&OptionValue ) ;
      if( !( Error ) )
      {  /* could enable non-blocking mode */
        Error = setsockopt( MySocket, SOL_SOCKET, SO_EVENTMASK, &MyEventMask, sizeof( MyEventMask ) ) ;
        if( !( Error ) )
        {  /* could set event mask */
        }
        else
        {  /* could not set event mask */
          LogText( MyContext->c_Log, ERROR_LEVEL, "could not set socket event mask: %ld", MyUpload->u_ErrorNumber ) ; 
        }
      }
      else
      {  /* could not enable non-blocking mode */
        LogText( MyContext->c_Log, ERROR_LEVEL, "could not enable socket non-blocking mode: %ld", MyUpload->u_ErrorNumber ) ; 
      }
    }
    else
    {  /* could not enable async mode */
      LogText( MyContext->c_Log, ERROR_LEVEL, "could not enable socket async mode: %ld", MyUpload->u_ErrorNumber ) ; 
    }
  }
  else
  {  /* requirements not ok */
    Error = -1 ;
  }

  return( Error ) ;
}


/*
** create a sockets
*/
static LONG CreateSocket( struct Upload *MyUpload )
{
  struct Context *MyContext ;
  struct Library *SocketBase ;
  LONG NewSocket ;
  
  MyContext = MyUpload->u_Context ;
  SocketBase = MyUpload->u_SocketBase ;

  NewSocket = socket( AF_INET, SOCK_STREAM, 0 ) ;
  if( 0 <= NewSocket )
  {  /* could created socket */
  }
  else
  {  /* could not create socket */
    LogText( MyContext->c_Log, ERROR_LEVEL, "could not create socket: %ld", MyUpload->u_ErrorNumber ) ; 
  }
  
  return( NewSocket ) ;
}


/*
** get ip adress of the hostname
*/
static void ResolveHostname( struct Upload *MyUpload, const char *Hostname, struct sockaddr_in *Address )
{
  struct Context *MyContext ;
  struct Library *SocketBase ;
  struct hostent *HostInfo ;
  
  MyContext = MyUpload->u_Context ;
  SocketBase = MyUpload->u_SocketBase ;
  
  HostInfo = gethostbyname( Hostname ) ;
  if( NULL != HostInfo )
  {  /* found by name server */
    Address->sin_family = HostInfo->h_addrtype ;
    memcpy( &Address->sin_addr.s_addr, HostInfo->h_addr, sizeof( struct in_addr ) ) ;
    Address->sin_port = htons( 21 ) ;  /* ftp */
  }
  else
  {  /* not found by name server, try the name as ip address string */
    Address->sin_family = AF_INET ;
    Address->sin_addr.s_addr = inet_addr( Hostname ) ;
    Address->sin_port = htons( 21 ) ;  /* ftp */
  }
}


/*
** get address to which we shall connect in passive mode and conect to it
*/
static LONG ConnectPassive( struct Upload *MyUpload )
{
  struct Context *MyContext ;
  struct Library *SocketBase ;
  LONG Walker ;
  ULONG Address1, Address2, Address3, Address4, Port1, Port2 ;
  LONG Error ;
  
  MyContext = MyUpload->u_Context ;
  SocketBase = MyUpload->u_SocketBase ;
  
  Walker = 0 ;
  while( isdigit( MyUpload->u_ControlBuffer[ Walker ] ) )
  {  /* skip leading responce code */
    Walker++ ;
  }
  while( !isdigit( MyUpload->u_ControlBuffer[ Walker ] ) )
  {  /* look for beginning of the ip address */
    Walker++ ;
  }
  sscanf( &MyUpload->u_ControlBuffer[ Walker ], "%ld,%ld,%ld,%ld,%ld,%ld", &Address1, &Address2, &Address3, &Address4, &Port1, &Port2 ) ;
  LogText( MyContext->c_Log, DEBUG_LEVEL, "passive address: %ld.%ld.%ld.%ld:%ld", Address1, Address2, Address3, Address4, ( ( Port1 << 8 ) + ( Port2 << 0 ) ) ) ;
  
  MyUpload->u_DataAddress.sin_addr.s_addr = htonl( ( Address1 << 24 ) + ( Address2 << 16 ) + ( Address3 << 8 ) + ( Address4 << 0 ) ) ;
  MyUpload->u_DataAddress.sin_family = AF_INET ;
  MyUpload->u_DataAddress.sin_port = htons( ( Port1 << 8 ) + ( Port2 << 0 ) ) ;

  MyUpload->u_DataSocket = CreateSocket( MyUpload ) ;
  if( 0 <= MyUpload->u_DataSocket )
  {  /* passive passive data socket ok */
    Error = PrepareSocket( MyUpload, MyUpload->u_DataSocket, ( FD_WRITE | FD_CLOSE | FD_ERROR ) ) ;
    if( !( Error ) )
    {  /* passive data socket in async mode */
      Error = connect( MyUpload->u_DataSocket, ( struct sockaddr * )&MyUpload->u_DataAddress, sizeof( struct sockaddr_in ) ) ;
      if( !( Error ) || ( EINPROGRESS == MyUpload->u_ErrorNumber ) )
      {  /* passive data socket did start to connect */
        Error = 0 ;
      }
      else
      {  /* passive data socket did not start to connect */
        LogText( MyContext->c_Log, ERROR_LEVEL, "passive data socket did not start to connect: %ld", MyUpload->u_ErrorNumber ) ; 
      }
    }
    else
    {  /* passive data socket not in async mode */
      LogText( MyContext->c_Log, ERROR_LEVEL, "passive data socket not in async mode" ) ; 
    }
  }
  else
  {  /* passive data socket not ok */
    Error = -1 ;
    LogText( MyContext->c_Log, ERROR_LEVEL, "passive data socket not ok" ) ; 
  }
  
  return( Error ) ;
}


/*
** open a listen socket based on control socket address
*/
static LONG ConnectActive( struct Upload *MyUpload )
{
  struct Context *MyContext ;
  struct Library *SocketBase ;
  LONG SizeOfAddress ;
  LONG Error ;
  
  MyContext = MyUpload->u_Context ;
  SocketBase = MyUpload->u_SocketBase ;
  
  MyUpload->u_DataServerSocket = CreateSocket( MyUpload ) ;
  if( 0 <= MyUpload->u_DataServerSocket )
  {  /* data listen socket ok */
    Error = PrepareSocket( MyUpload, MyUpload->u_DataServerSocket, ( FD_ACCEPT | FD_CLOSE | FD_ERROR ) ) ;
    if( !( Error ) )
    {  /* data listen socket in async mode */
      SizeOfAddress = sizeof( MyUpload->u_DataAddress ) ;
      Error = getsockname( MyUpload->u_ControlSocket, ( struct sockaddr * )&MyUpload->u_DataAddress, &SizeOfAddress ) ;
      if( !( Error ) )
      {  /* could get address of control socket */
        do
        {  /* find a free socket to that we can bind */
          MyUpload->u_DataAddress.sin_port = htons( ntohs( MyUpload->u_DataAddress.sin_port ) + 1 ) ;
          Error = bind( MyUpload->u_DataServerSocket, ( struct sockaddr * )&MyUpload->u_DataAddress, SizeOfAddress ) ;
        }
        while( Error ) ;
        if( !( Error ) )
        {  /* data socket could be bind to its address */
          Error = listen( MyUpload->u_DataServerSocket, 1 ) ;
          if( !( Error ) )
          {  /* data listen socket did start to listen */
          }
          else
          {  /* data listen socket did not start to listen */
            LogText( MyContext->c_Log, ERROR_LEVEL, "ddata listen socket did not start to listen: %ld", MyUpload->u_ErrorNumber ) ; 
          }
        }
        else
        {  /* data listen socket could not be bind to its address */
          LogText( MyContext->c_Log, ERROR_LEVEL, "data listen socket could not be bind to its address: %ld", MyUpload->u_ErrorNumber ) ; 
        }
      }
      else
      {  /* could not get address of control socket */
        LogText( MyContext->c_Log, ERROR_LEVEL, "could not get address of control socket: %ld", MyUpload->u_ErrorNumber ) ; 
      }
    }
    else
    {  /* data listen socket not in async mode */
      LogText( MyContext->c_Log, ERROR_LEVEL, "data listen socket not in async mode" ) ; 
    }
  }
  else
  {  /* data listen socket not ok */
    Error = -1 ;
    LogText( MyContext->c_Log, ERROR_LEVEL, "data listen socket not ok" ) ; 
  }

  return( Error ) ;
}


/*
** send control buffer with ftp command to the server
*/
static void SendControl( struct Upload * MyUpload )
{
  struct Library *SocketBase ;

  SocketBase = MyUpload->u_SocketBase ;

  if( 0 <= MyUpload->u_ControlSocket )
  {  /* control socket ok */
    if( MyUpload->u_ControlSent < MyUpload->u_ControlLength )
    {  /* current control buffer not completely sent */
      MyUpload->u_ControlSent += send( MyUpload->u_ControlSocket, 
                                        MyUpload->u_ControlBuffer + MyUpload->u_ControlSent, 
                                        MyUpload->u_ControlLength - MyUpload->u_ControlSent, 
                                        0 ) ;
    }
  }
  else
  {  /* control socket not (yet) ok */
  }
}


/*
** send data buffer to the server
*/
static void SendData( struct Upload * MyUpload )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct Library *SocketBase ;
  struct DataBuffer *CurrentBuffer ;
  LONG BytesSent ;
  
  MyContext = MyUpload->u_Context ;
  SysBase = MyContext->c_SysBase ;
  SocketBase = MyUpload->u_SocketBase ;
  
  if( 0 <= MyUpload->u_DataSocket )
  {  /* data socket ok */
    while( !( IsListEmpty( &MyUpload->u_FullList ) ) )
    {
      CurrentBuffer = ( struct DataBuffer * )MyUpload->u_FullList.lh_Head ;
      BytesSent = send( MyUpload->u_DataSocket, 
                        CurrentBuffer->db_DataPointer + MyUpload->u_DataSent, 
                        ( LONG )CurrentBuffer->db_DataLength - MyUpload->u_DataSent, 
                        0 ) ;
      if( 0 < BytesSent )
      {  /* some bytes were send */
        LogText( MyContext->c_Log, DEBUG_LEVEL, "%ld bytes sent on socket %ld", BytesSent, MyUpload->u_DataSocket ) ;
        MyUpload->u_DataSent += BytesSent ;
        //printf( "sent %ld of %ld\n", MyUpload->u_DataSent, CurrentBuffer->db_DataLength ) ;
        if( MyUpload->u_DataSent >= CurrentBuffer->db_DataLength )
        {  /* current buffer is completely sent */
          if( CurrentBuffer == ( struct DataBuffer * )RemHead( &MyUpload->u_FullList ) )
          {  /* current buffer was still the head of the list */
            if( CurrentBuffer->db_LastData )
            {  /* this was the last data of this upload */
              DeleteSocket( MyUpload, MyUpload->u_DataSocket ) ;
              MyUpload->u_DataSocket = -1 ;
            }
            else
            {  /* this was not the last data of this upload */
              if( IsListEmpty( &MyUpload->u_EmptyList ) )
              {  /* empty list currently empty */
  
              }
              else
              {  /* empty list currently not empty */
                AddTail( &MyUpload->u_EmptyList, ( struct Node * )CurrentBuffer ) ;
              }
            }
          }
          else
          {  /* current buffer was not head of the list, should never happen */
          }
        }
        else
        {  /* current buffer is not completely sent */
        }
      }
      else
      {  /* could not send data */
        if( EWOULDBLOCK != MyUpload->u_ErrorNumber )
        {  /* some real error */
          LogText( MyContext->c_Log, WARNING_LEVEL, "could not send data: %ld", MyUpload->u_ErrorNumber ) ;
        }
        break ;
      }
    }
  }
  else
  {  /* data socket not (yet) ok */
  }
}


/*
** build a ftp command which can be sent to the server
*/
static void SendClientMessage( struct Upload * MyUpload, LONG ClientControlCode )
{
  struct Context *MyContext ;
  struct Library *SocketBase ;
  SIPTR SettingValue ;
  UBYTE Address1, Address2, Address3, Address4, Port1, Port2 ;

  MyContext = MyUpload->u_Context ;
  SocketBase = MyUpload->u_SocketBase ;

  MyUpload->u_ActiveCommand = ClientControlCode ;
  switch( MyUpload->u_ActiveCommand )
  {
    case FTP_CMD_USER:
      SettingValue = GetSetting( MyContext->c_Settings, UploadUser ) ;
      snprintf( MyUpload->u_ControlBuffer, CONTROLBUFFER_SIZE - 2, "USER %s%n", ( STRPTR )SettingValue, &MyUpload->u_ControlLength ) ;
      break ;
    case FTP_CMD_PASS:
      SettingValue = GetSetting( MyContext->c_Settings, UploadPassword ) ;
      snprintf( MyUpload->u_ControlBuffer, CONTROLBUFFER_SIZE - 2, "PASS %s%n", ( STRPTR )SettingValue, &MyUpload->u_ControlLength ) ;
      break ;
    case FTP_CMD_CWD:
      SettingValue = GetSetting( MyContext->c_Settings, UploadDrawer ) ;
      snprintf( MyUpload->u_ControlBuffer, CONTROLBUFFER_SIZE - 2, "CWD %s%n", ( STRPTR )SettingValue, &MyUpload->u_ControlLength ) ;
      break ;
    case FTP_CMD_TYPE:
      snprintf( MyUpload->u_ControlBuffer, CONTROLBUFFER_SIZE - 2, "TYPE I%n", &MyUpload->u_ControlLength ) ;
      break ;
    case FTP_CMD_PASV:
      snprintf( MyUpload->u_ControlBuffer, CONTROLBUFFER_SIZE - 2, "PASV%n", &MyUpload->u_ControlLength ) ;
      break ;
    case FTP_CMD_PORT:
      Address1 = ( ntohl( MyUpload->u_DataAddress.sin_addr.s_addr ) >> 24 ) ;
      Address2 = ( ntohl( MyUpload->u_DataAddress.sin_addr.s_addr ) >> 16 ) ;
      Address3 = ( ntohl( MyUpload->u_DataAddress.sin_addr.s_addr ) >> 8 ) ;
      Address4 = ( ntohl( MyUpload->u_DataAddress.sin_addr.s_addr ) >> 0 ) ;
      Port1 = ( ntohs( MyUpload->u_DataAddress.sin_port ) >> 8 ) ;
      Port2 = ( ntohs( MyUpload->u_DataAddress.sin_port ) >> 0 ) ;
      snprintf( MyUpload->u_ControlBuffer, CONTROLBUFFER_SIZE - 2, "PORT %d,%d,%d,%d,%d,%d%n", Address1, Address2, Address3, Address4, Port1, Port2, &MyUpload->u_ControlLength ) ;
      break ;
    case FTP_CMD_STOR:
      snprintf( MyUpload->u_ControlBuffer, CONTROLBUFFER_SIZE - 2, "STOR %s%n", MyUpload->u_UploadFile, &MyUpload->u_ControlLength ) ;
      break ;
    case FTP_CMD_QUIT:
    default:  /* no supported command */
      snprintf( MyUpload->u_ControlBuffer, CONTROLBUFFER_SIZE - 2, "QUIT%n", &MyUpload->u_ControlLength ) ;
      break ;
  }
  LogText( MyContext->c_Log, INFO_LEVEL, "C: %s", MyUpload->u_ControlBuffer ) ;
  MyUpload->u_ControlBuffer[ MyUpload->u_ControlLength++ ] = '\r' ;
  MyUpload->u_ControlBuffer[ MyUpload->u_ControlLength++ ] = '\n' ;
  MyUpload->u_ControlBuffer[ MyUpload->u_ControlLength ] = '\0' ;
  MyUpload->u_ControlSent = 0 ;
  SendControl( MyUpload ) ;
}


/*
** get the server message and return the control code
*/
static LONG ReceiveServerMessage( struct Upload *MyUpload )
{
  struct Context *MyContext ;
  struct Library *SocketBase ;
  LONG ControlCode ;
  LONG BytesReceived ;
  UBYTE *LineStart ;
  ULONG LineLength ;
  
  MyContext = MyUpload->u_Context ;
  SocketBase = MyUpload->u_SocketBase ;
  ControlCode = -1 ;
  
  while( 0 < ( BytesReceived = recv( MyUpload->u_ControlSocket, &MyUpload->u_ControlBuffer[ MyUpload->u_ControlReceived ], ( LONG )( CONTROLBUFFER_SIZE - MyUpload->u_ControlReceived ), 0 ) ) )
  {  /* read all received data from the command socket */
    MyUpload->u_ControlReceived += BytesReceived ;
    LineStart = MyUpload->u_ControlBuffer ;
    LineLength = 0 ;
    while( LineLength < MyUpload->u_ControlReceived )
    {  /* look the next end-of-line marker in the received buffer */
      LineLength++ ;
      switch( LineLength )
      {
        case 3:  /* try to determine the control code send by the server */
          ControlCode = ( LineStart[ 0 ] - '0' ) * 100 + ( LineStart[ 1 ] - '0' ) * 10 + ( LineStart[ 2 ] - '0' ) ;
          if( MyUpload->u_MultilineControlCode )
          {
            if( MyUpload->u_MultilineControlCode == ControlCode )
            {  /* end of multiline response */
              //LogText( MyContext->c_Log, DEBUG_LEVEL, "end multiline responce: %d", MyUpload->u_MultilineControlCode ) ;
              MyUpload->u_MultilineControlCode = 0 ;
            }
            else
            {  /* still not the end of the multiline response */
              ControlCode = -1 ;
            }
          }
          break ;
        case 4:  /* check if a multiline response starts */
          if( !( MyUpload->u_MultilineControlCode ) )
          {
            if( '-' == LineStart[ LineLength - 1 ] )
            {
              MyUpload->u_MultilineControlCode = ControlCode ;
              ControlCode = -1 ;
              //LogText( MyContext->c_Log, DEBUG_LEVEL, "begin multiline responce: %d", MyUpload->u_MultilineControlCode ) ;
            }
          }
          break ;
        default:  /* esle, check for the end of the line */
          if( '\n' == LineStart[ LineLength - 1 ] )
          {  /* end of line found */
            if( '\r' == LineStart[ LineLength - 2 ] )
            {
              LineStart[ LineLength - 2 ] = '\0' ;
            }
            else
            {
              LineStart[ LineLength - 1 ] = '\0' ;
            }
            LogText( MyContext->c_Log, INFO_LEVEL, "S: %s", LineStart ) ;
            LineStart += LineLength ;
            MyUpload->u_ControlReceived -= LineLength ;
            LineLength = 0 ;
          }
          break ;
      }
    }
    if( 0 < MyUpload->u_ControlReceived )
    {  /* end of current line not in control buffer */
      if( LineStart == MyUpload->u_ControlBuffer )
      {  /* control buffer too small for server response */
        LogText( MyContext->c_Log, WARNING_LEVEL, "control buffer too small for server response" ) ;
        MyUpload->u_ControlReceived = 0 ;
      }
      else
      {
        LogText( MyContext->c_Log, DEBUG_LEVEL, "the current line was not completely received yet" ) ;
        memcpy( MyUpload->u_ControlBuffer, LineStart, MyUpload->u_ControlReceived ) ;
      }
    }
  }
  
  //LogText( MyContext->c_Log, DEBUG_LEVEL, "server responce control code: %d", ControlCode ) ;
  return( ControlCode ) ;
}


/*
** delete upload context
*/
void DeleteUpload( struct Upload *OldUpload )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
 
  if( NULL != OldUpload )
  {  /* upload context needs to be freed */
    MyContext = OldUpload->u_Context ;
    SysBase = MyContext->c_SysBase ;
    
    if( 0 != OldUpload->u_SignalBit )
    {  /* signal bit needs to be released */
      StopUpload( OldUpload ) ;
      RemDispatcherNode( MyContext->c_Dispatcher, &OldUpload->u_DispatcherNode ) ;
      FreeSignal( ( LONG )OldUpload->u_SignalBit ) ;
    }
    FreeVec( OldUpload ) ;
  }
}


/*
** prepare/configure the upload process
*/
void ConfigureUpload( struct Upload *MyUpload )
{
  struct Context *MyContext ;
  SIPTR SettingValue ;
  ULONG NewUploadEnable ;
  ULONG NewUploadPassive ;
  ULONG NewUploadTimeout ;
  STRPTR NewUploadFileTemplate ;
  ULONG NewUploadFormat ;
  STRPTR NewUploadDrawer ;
  
  if( NULL != MyUpload )
  {  /* requirements ok */
    MyContext = MyUpload->u_Context ;
     
    SettingValue = GetSetting( MyContext->c_Settings, UploadEnable ) ;
    NewUploadEnable = ( ULONG )SettingValue ;
    SettingValue = GetSetting( MyContext->c_Settings, UploadPassive ) ;
    NewUploadPassive = ( ULONG )SettingValue ;
    SettingValue = GetSetting( MyContext->c_Settings, UploadTimeout ) ;
    NewUploadTimeout = ( ULONG )SettingValue ;
    SettingValue = GetSetting( MyContext->c_Settings, UploadFile ) ;
    NewUploadFileTemplate = ( STRPTR )SettingValue ;
    SettingValue = GetSetting( MyContext->c_Settings, UploadFormat ) ;
    NewUploadFormat = ( ULONG )SettingValue ;
    SettingValue = GetSetting( MyContext->c_Settings, UploadDrawer ) ;
    NewUploadDrawer = ( STRPTR )SettingValue ;
    
    MyUpload->u_CurrentUploadFileTemplate[ 0 ] = '\0' ;

    if( ( NewUploadEnable ) )
    {  /* upload enabled */
      MyUpload->u_CurrentUploadPassive = NewUploadPassive ;
      MyUpload->u_CurrentUploadTimeout = NewUploadTimeout ;
      if( NULL != NewUploadFileTemplate )
      {  /* take file template from settings */
      }
      else
      {  /* use a default file name */
        NewUploadFileTemplate = "cAMIra_upload" ;
      }
      MyUpload->u_CurrentUploadFormat = NewUploadFormat ;
      switch( MyUpload->u_CurrentUploadFormat )
      {  /* append extension depending on format */
        case JPG_FORMAT:
          snprintf( MyUpload->u_CurrentUploadFileTemplate, CURRENTUPLOADFILETEMPLATE_LENGTH, "%s.jpg", NewUploadFileTemplate ) ;
          break ;
        case PNG_FORMAT:
          snprintf( MyUpload->u_CurrentUploadFileTemplate, CURRENTUPLOADFILETEMPLATE_LENGTH, "%s.png", NewUploadFileTemplate ) ;
          break ;
        default:
          break ;
      }
      MyUpload->u_CurrentUploadCwd = ( NULL != NewUploadDrawer ) && ( '\0' != NewUploadDrawer[ 0 ] ) ;
    }
  }
  else
  {  /* requirements not ok */
  }
}


/*
** dispatcher callback function
*/
static ULONG DoUpload( struct Upload *MyUpload, ULONG TriggerSignals )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct Library *SocketBase ;
  LONG ActionSocket ;
  ULONG SocketEvents ;
  LONG ServerControlCode, ClientControlCode ;
  ULONG UploadDone ;
  LONG SizeOfAddress ;
  LONG Error ;
  
  MyContext = MyUpload->u_Context ;
  SysBase = MyContext->c_SysBase ;
  SocketBase = MyUpload->u_SocketBase ;
  UploadDone = FALSE ;

  StopUploadTimer( MyUpload ) ;
  
  while( 0 <= ( ActionSocket = GetSocketEvents( &SocketEvents ) ) )
  {  /* as long as there are sockets with pending events */
    ClientControlCode = FTP_CMD_NONE ;
    if( MyUpload->u_ControlSocket == ActionSocket )
    {  /* our control socket */
      if( FD_CONNECT & SocketEvents )
      {  /* control socket is connected */
      }
      if( FD_READ & SocketEvents )
      {  /* command reply from server */
        ServerControlCode = ReceiveServerMessage( MyUpload ) ;
        if( 0 <= ServerControlCode )
        {  /* a server control code was received */
          switch( ServerControlCode )
          {  /* what did the server tell us? */
            case 150:  /* data connection established, start transmit */
              SendData( MyUpload ) ;
              break ;
            case 200:  /* command okay */
              switch( MyUpload->u_ActiveCommand )
              {  /* what command was ok ? */
                case FTP_CMD_TYPE:  /* TYPE command ok */ 
                  if( MyUpload->u_CurrentUploadPassive )
                  {  /* data connection opens in passive mode */
                    ClientControlCode = FTP_CMD_PASV ;
                  }
                  else
                  {  /* data connection opens in active mode */
                    Error = ConnectActive( MyUpload ) ;
                    if( !( Error ) )
                    {  /* active data connection could be initiated */
                      ClientControlCode = FTP_CMD_PORT ;
                    }
                    else
                    {  /* active data connection could not be initiated */
                      LogText( MyContext->c_Log, ERROR_LEVEL, "active data connection could not be initiated" ) ;
                      MyUpload->u_UploadError = TRUE ;
                      ClientControlCode = FTP_CMD_QUIT ;
                    }
                  }
                  break ;
                case FTP_CMD_PORT:
                  ClientControlCode = FTP_CMD_STOR ;
                  break ;
              }
              break ;
            case 220:  /* service ready for new user */
              ClientControlCode = FTP_CMD_USER ;
              break ;
            case 221:  /* goodbye */
              break ;
            case 226:  /* closing data connection */
              ClientControlCode = FTP_CMD_QUIT ;
              break ;
            case 227:  /* entering Passive Mode (h1,h2,h3,h4,p1,p2) */
              Error = ConnectPassive( MyUpload ) ;
              if( !( Error ) )
              {  /* passive data connection could be initiated */
                ClientControlCode = FTP_CMD_STOR ;
              }
              else
              {  /* passive data connection could not be initiated */
                LogText( MyContext->c_Log, ERROR_LEVEL, "passive data connection could not be initiated" ) ;
                MyUpload->u_UploadError = TRUE ;
                ClientControlCode = FTP_CMD_QUIT ;
              }
              break ;
            case 230:  /* user logged in, proceed */
              if( MyUpload->u_CurrentUploadCwd )
              {  /* change to the requested upload drawer */
                ClientControlCode = FTP_CMD_CWD ;
              }
              else
              {  /* no upload drawer specified */
                ClientControlCode = FTP_CMD_TYPE ;
              }
              break ;
            case 250:  /* requested file action okay, completed */
              ClientControlCode = FTP_CMD_TYPE ;
              break ;
            case 331:  /* user name okay, need password */
              ClientControlCode = FTP_CMD_PASS ;
              break ;
            case 421:  /* service not available, closing control connection */
              break ;
            case 500:  /* syntax error */
              MyUpload->u_UploadError = TRUE ;
              ClientControlCode = FTP_CMD_QUIT ;
              break ;
            case 530:  /* login incorrect */
              ClientControlCode = FTP_CMD_QUIT ;
              break ;
            case 550:  /* requested action not taken */
              switch( MyUpload->u_ActiveCommand )
              {  /* what action could not be taken */
                case FTP_CMD_CWD:
                case FTP_CMD_PASS:
                default:
                  MyUpload->u_UploadError = TRUE ;
                  ClientControlCode = FTP_CMD_QUIT ;
                  break ;
              }
              break ;
            case 553:  /* file name not allowed */
              MyUpload->u_UploadError = TRUE ;
              ClientControlCode = FTP_CMD_QUIT ;
              break ;
            default:  /* unknown control code */
              LogText( MyContext->c_Log, DEBUG_LEVEL, "unknown server control code: %ld", ServerControlCode ) ;
              break ;
          }
        }
        else
        {  /* no server control code was received */
          /* maybe unexpected read event for control socket */
        }
      }
      if( FD_WRITE & SocketEvents )
      {  /* control was not sent at once */
        SendControl( MyUpload ) ;
      }
      if( FD_CLOSE & SocketEvents )
      {  /* control socket closed */
        LogText( MyContext->c_Log, DEBUG_LEVEL, "control socket close event" ) ;
        UploadDone = TRUE ;
      }
      if( FD_ERROR & SocketEvents )
      {  /* control socket error */
        LogText( MyContext->c_Log, DEBUG_LEVEL, "control socket error event" ) ;
      }
    }
    else if( MyUpload->u_DataSocket == ActionSocket )
    {  /* our data socket */
      if( FD_CONNECT & SocketEvents )
      {  /* data socket is connected */
      }
      if( FD_READ & SocketEvents )
      {  /* we should never get this as we only upload */
      }
      if( FD_WRITE & SocketEvents )
      {  /* data was not sent at once */
        LogText( MyContext->c_Log, DEBUG_LEVEL, "data socket write event" ) ;
        SendData( MyUpload ) ;
      }
      if( FD_CLOSE & SocketEvents )
      {  /* data socket closed */
        LogText( MyContext->c_Log, DEBUG_LEVEL, "data socket close event" ) ;
      }
      if( FD_ERROR & SocketEvents )
      {  /* data socket error */
        LogText( MyContext->c_Log, DEBUG_LEVEL, "data socket error event" ) ;
      }
    }
    else if( MyUpload->u_DataServerSocket == ActionSocket )
    {
      if( FD_ACCEPT & SocketEvents )
      {  /* data listen socket accept event */
        //print_f( "data listen socket accept event\n" ) ;
        MyUpload->u_DataSocket = accept( MyUpload->u_DataServerSocket, ( struct sockaddr * )&MyUpload->u_DataAddress, &SizeOfAddress ) ;
        if( 0 <= MyUpload->u_DataSocket )
        {  /* active data socket ok */
          Error = PrepareSocket( MyUpload, MyUpload->u_DataSocket, ( FD_WRITE | FD_ERROR ) ) ;
          if( !( Error ) )
          {  /* active data socket in async mode */
          }
          else
          {  /* active data socket not in async mode */
            LogText( MyContext->c_Log, ERROR_LEVEL, "active data socket not in async mode" ) ;
          }
        }
        else
        {  /* active data socket not ok */
          LogText( MyContext->c_Log, ERROR_LEVEL, "active data socket not ok" ) ;
        }
        DeleteSocket( MyUpload, MyUpload->u_DataServerSocket ) ;
        MyUpload->u_DataServerSocket = -1 ;
      }
      if( FD_ERROR & SocketEvents )
      {  /* data listen socket error event */
      }
    }
    else
    {  /* unknown socket */
      LogText( MyContext->c_Log, DEBUG_LEVEL, "unknown action socket" ) ;
    }
    
    if( FTP_CMD_NONE != ClientControlCode )
    {  /* we have to send a control message to the server */
      SendClientMessage( MyUpload, ClientControlCode ) ;
    }
  }
  
  if( TRUE == UploadDone )
  {  /* upload finished */
    StopUpload( MyUpload ) ;
  }
  else
  {  /* we wait for the next socket event */
    StartUploadTimer( MyUpload ) ;
  }

  return( TriggerSignals ) ;
}


/*
** create the upload context
*/
struct Upload *CreateUpload( struct Context *MyContext )
{
  struct ExecBase *SysBase ;
  struct Upload *NewUpload ;

  NewUpload = NULL ;
  
  if( ( NULL != MyContext ) )
  {  /* requirements seem ok */
    SysBase = MyContext->c_SysBase ;
    
    NewUpload = AllocVec( sizeof( struct Upload ), MEMF_ANY | MEMF_CLEAR ) ;
    if( NULL != NewUpload )
    {  /* memory for upload context ok */
      NewUpload->u_Context = MyContext ;
      NewUpload->u_SignalBit = AllocSignal( -1 ) ;
      if( 0 != NewUpload->u_SignalBit )
      {  /* signal bit ok */
        NewUpload->u_DispatcherNode.dn_Signals = ( 1UL << NewUpload->u_SignalBit ) ;
        NewUpload->u_DispatcherNode.dn_DoFunction = ( DISPATCHER_DOFUNCTION )DoUpload ;
        NewUpload->u_DispatcherNode.dn_DoData = NewUpload ;
        AddDispatcherNode( MyContext->c_Dispatcher, &NewUpload->u_DispatcherNode ) ;
      }
      else
      {  /* signal bit not ok */
        DeleteUpload( NewUpload ) ;
        NewUpload = NULL ;
      }
    }
    else
    {  /* memory for upload context not ok */
    }
  }
  else
  {  /* requirements not ok */
  }

  return( NewUpload ) ;
}


/*
** stop the current upload process
*/
void StopUpload( struct Upload *MyUpload )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;

  if( NULL != MyUpload )
  {  /* requirements ok */
    MyContext = MyUpload->u_Context ;
    SysBase = MyContext->c_SysBase ;

    if( NULL != MyUpload->u_CurrentImageBox ) 
    {  /* image box needs to be dumped */
      if( NULL != MyUpload->u_SocketBase )
      {  /* bsdsocket library needs to be closed */
        if( 0 <= MyUpload->u_ControlSocket )
        {  /* control socket needs to be closed */
          if( 0 <= MyUpload->u_DataSocket )
          {  /* data socket needs to be closed */
            //LogText( MyContext->c_Log, DEBUG_LEVEL, "data socket %d needs to be closed", MyUpload->u_DataSocket ) ;
            DeleteSocket( MyUpload, MyUpload->u_DataSocket ) ;
            MyUpload->u_DataSocket = -1 ;
          }
          if( 0 <= MyUpload->u_DataServerSocket )
          {  /* data socket needs to be closed */
            //LogText( MyContext->c_Log, DEBUG_LEVEL, "data server socket %d needs to be closed", MyUpload->u_DataServerSocket ) ;
            DeleteSocket( MyUpload, MyUpload->u_DataServerSocket ) ;
            MyUpload->u_DataServerSocket = -1 ;
          }
          StopUploadTimer( MyUpload ) ;
          //LogText( MyContext->c_Log, DEBUG_LEVEL, "control socket %d needs to be closed", MyUpload->u_ControlSocket ) ;
          DeleteSocket( MyUpload, MyUpload->u_ControlSocket ) ;
          MyUpload->u_ControlSocket = -1 ;
          if( !( MyUpload->u_UploadError ) )
          {  /* this upload finished ok */
            LogText( MyContext->c_Log, INFO_LEVEL, "upload finished" ) ;
          }
          else
          {  /* this upload failed */
            LogText( MyContext->c_Log, FAILURE_LEVEL, "upload failed" ) ;
          }
        }
        CloseLibrary( MyUpload->u_SocketBase ) ;
        MyUpload->u_SocketBase = NULL ;
      }
      MyUpload->u_CurrentImageBox->ib_UseCounter-- ;
      DeleteImageBox( MyContext, MyUpload->u_CurrentImageBox ) ;
      MyUpload->u_CurrentImageBox = NULL ;
    }
    if( MyUpload->u_DispatcherNode.dn_Signals & SetSignal( 0L, 0L ) )
    {  /* clear pending signal */
      Wait( MyUpload->u_DispatcherNode.dn_Signals ) ; 
    }
  }
  else
  {  /* requirements not ok */
  }
}


/*
** start a upload process
*/
LONG StartUpload( struct Upload *MyUpload, struct ImageBox *MyImageBox )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct Library *SocketBase ;
  LONG Error ;
  SIPTR SettingValue ;
  struct ExpandData MyExpandData ;
  
  Error = 0 ;
  
  if( ( NULL != MyUpload ) && ( NULL == MyUpload->u_CurrentImageBox ) && ( NULL != MyImageBox ) )
  {  /* requirements ok */
    MyContext = MyUpload->u_Context ;
    SysBase = MyContext->c_SysBase ;
    
    LogText( MyContext->c_Log, INFO_LEVEL, "starting upload ..." ) ;
    MyUpload->u_UploadError = 0 ;
    MyUpload->u_CurrentImageBox = MyImageBox ;
    MyUpload->u_CurrentImageBox->ib_UseCounter++ ;
    switch( MyUpload->u_CurrentUploadFormat )
    {  /* create cature image depending on selected format */
      case JPG_FORMAT:
        MyUpload->u_CurrentImageBox->ib_UploadImage = CommandImageProcessor( MyContext->c_ImageProcessor,
          IMAGE_COMMAND_CREATE_JPEG, MyUpload->u_CurrentImageBox ) ;
        break ;
      case PNG_FORMAT:
        MyUpload->u_CurrentImageBox->ib_UploadImage = CommandImageProcessor( MyContext->c_ImageProcessor,
          IMAGE_COMMAND_CREATE_PNG, MyUpload->u_CurrentImageBox ) ;
        break ;
      default:
        MyUpload->u_CurrentImageBox->ib_UploadImage = NULL ;
        break ;
    }
    if( NULL != MyUpload->u_CurrentImageBox->ib_UploadImage )
    {  /* there is a image to upload in the box */
      MyExpandData.ed_ClockData = &MyUpload->u_CurrentImageBox->ib_ClockData ;
      MyExpandData.ed_Number = &MyUpload->u_CurrentImageBox->ib_SequenceNumber ;
      ExpandString( MyUpload->u_UploadFile, UPLOADFILE_LENGTH, MyUpload->u_CurrentUploadFileTemplate, &MyExpandData ) ;
      //ParseImageBoxString( MyContext, MyUpload->u_CurrentUploadFileTemplate, MyUpload->u_CurrentImageBox, MyUpload->u_UploadFile, UPLOADFILE_LENGTH ) ;
      MyUpload->u_SocketBase = OpenLibrary( "bsdsocket.library", 0 ) ;
      if( NULL != MyUpload->u_SocketBase )
      {  /* bsdsocket library ok */
        SocketBase = MyUpload->u_SocketBase ;
        MyUpload->u_DataServerSocket = MyUpload->u_DataSocket = -1 ;
        MyUpload->u_ControlSocket = CreateSocket( MyUpload ) ;
        if( 0 <= MyUpload->u_ControlSocket )
        {  /* control socket ok */
          Error = PrepareSocket( MyUpload, MyUpload->u_ControlSocket, ( FD_READ | FD_WRITE | FD_CLOSE | FD_ERROR ) ) ;
          if( !( Error ) )
          {  /* control socket in async mode */
            Error = SocketBaseTags( SBTM_SETVAL( SBTC_SIGEVENTMASK ), ( 1UL << MyUpload->u_SignalBit ),
                                    SBTM_SETVAL( SBTC_ERRNOLONGPTR ), ( IPTR )&MyUpload->u_ErrorNumber ,
                                    TAG_END ) ;
            if( !( Error ) )
            {  /* base tags could be set */ 
              SettingValue = GetSetting( MyContext->c_Settings, UploadServer ) ;
              ResolveHostname( MyUpload, ( STRPTR )SettingValue, &MyUpload->u_ControlAddress ) ;
              NewList( &MyUpload->u_EmptyList ) ;
              NewList( &MyUpload->u_FullList ) ;
              MyUpload->u_ControlReceived = 0 ;
              MyUpload->u_MultilineControlCode = 0 ;
              if( INADDR_NONE != MyUpload->u_ControlAddress.sin_addr.s_addr )
              {  /* hostname could be resolved */
                Error = connect( MyUpload->u_ControlSocket, ( struct sockaddr * )&MyUpload->u_ControlAddress, sizeof( struct sockaddr_in ) ) ;
                if( !( Error ) || ( EINPROGRESS == MyUpload->u_ErrorNumber ) )
                {  /* control connect started */
                  Error = 0 ;
                  MyUpload->u_DataBuffer[ 0 ].db_DataPointer = MyUpload->u_CurrentImageBox->ib_UploadImage->chunky ;
                  MyUpload->u_DataBuffer[ 0 ].db_DataLength = MyUpload->u_CurrentImageBox->ib_UploadImage->width ;
                  MyUpload->u_DataBuffer[ 0 ].db_LastData = TRUE ;
                  MyUpload->u_DataSent = 0 ;
                  AddTail( &MyUpload->u_FullList, ( struct Node * )&MyUpload->u_DataBuffer[ 0 ] ) ;
                  StartUploadTimer( MyUpload ) ;
                }
                else
                {  /* control connect did not start */
                  LogText( MyContext->c_Log, ERROR_LEVEL, "control connect did not start: %ld", MyUpload->u_ErrorNumber ) ; 
                  StopUpload( MyUpload ) ;
                }
              }
              else
              {  /* hostname could not be resolved */
                Error = -1 ;
                LogText( MyContext->c_Log, ERROR_LEVEL, "hostname could not be resolved" ) ;
                StopUpload( MyUpload ) ;
              }
            }
            else
            {  /* base tags could be set */ 
              LogText( MyContext->c_Log, ERROR_LEVEL, "base tags could be set" ) ;
              StopUpload( MyUpload ) ;
            }
          }
          else
          {  /* control socket not in async mode */
            Error = -1 ;
            LogText( MyContext->c_Log, ERROR_LEVEL, "control socket not in async mode" ) ;
            StopUpload( MyUpload ) ;
          }
        }
        else
        {  /* control socket not ok */
          Error = -1 ;
          LogText( MyContext->c_Log, ERROR_LEVEL, "control socket not ok" ) ;
          StopUpload( MyUpload ) ;
        }
      }
      else
      {  /* bsdsocket library not ok */
        Error = -1 ;
        LogText( MyContext->c_Log, ERROR_LEVEL, "bsdsocket library not ok" ) ;
        StopUpload( MyUpload ) ;
      }
    }
    else
    {  /* there is no image to upload in the box */
      Error = -1 ;
      LogText( MyContext->c_Log, ERROR_LEVEL, "there is no image to upload in the box" ) ;
      StopUpload( MyUpload ) ;
    }
  }
  else
  {  /* requirements not ok */
    Error = -1 ;
  }
  
  return( Error ) ;
}


/*
** report, if upload is busy and if image box is still in use by the capture
*/
LONG CheckUpload( struct Upload *MyUpload )
{
  LONG Result ;
  
  Result = 0 ;
  
  if( NULL != MyUpload )
  {  /* upload context ok */
    if( NULL != MyUpload->u_CurrentImageBox )
    {  /* we are currently busy with some image box */
      Result = 1 ;
    }
    else
    {  /* we currently do nothing */
    }
  }
  else
  {  /* upload context not ok */
    Result = -1 ;
  }
  
  return( Result ) ;
}

