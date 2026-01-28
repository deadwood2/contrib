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
** DosManager.h
*/


#ifndef _DOSMANAGER_H
#define _DOSMANAGER_H


#include "Dispatcher.h"
#include <exec/nodes.h>
#include <dos/dosextens.h>


#if 1


/*
** context of the dos manager
*/
#define DOSMANAGER_PATHEXPANDER_LENGTH ( 128 )
#define DOSMANAGER_ERRORTEXT_LENGTH ( 64 )
struct DosManager
{
  struct DispatcherNode dm_DispatcherNode ;  /* to insert in the list handled by the dispatcher */
  struct Context *dm_Context ;  /* pointer to context */
  struct List dm_List ;  /* list of active dos manager nodes */
  struct MsgPort *dm_MsgPort ;  /* message port for communicating with dos */
  BYTE dm_PathExpander[ DOSMANAGER_PATHEXPANDER_LENGTH ] ;  /* to get the correct device process */
  BYTE dm_ErrorText[ DOSMANAGER_ERRORTEXT_LENGTH ] ;  /* generate error text */
} ;


/*
** dos node managed by the dos manager
*/
#define DOSNODE_BCSTR_LENGTH ( 64 )
struct DosNode
{
  struct Node dn_Node ;  /* to be inserted in a list */
  struct DevProc *dn_DevProc ;  /* the process to send the packets to */
  struct DosPacket *dn_DosPacket ;  /* the dos packet */
  BPTR dn_CurrentDirectoryLock ;  /* lock of the working directory */
  BYTE dn_CStr[ DOSNODE_BCSTR_LENGTH + 3 /* for alinment of BSTR */ ] ;  /* for converting B and C strings */
  BYTE *dn_BStr ;  /* aligned pointer into the BCSTR */
  void ( *dn_DoFunction )( APTR DoData ) ;  /* callback function when dos opperation finished */
  APTR dn_DoData ;  /* parameter for callback function */
} ;



/*
** module functions
*/
struct DosManager *CreateDosManager( struct Context *MyContext ) ;
void DeleteDosManager( struct DosManager *OldDosManager ) ;
struct DosNode *CreateDosNode( struct DosManager *MyDosManager, STRPTR MyPath, LONG *MyError ) ;
void DeleteDosNode( struct DosManager *MyDosManager, struct DosNode *OldDosNode ) ;
LONG CheckDosNode( struct DosManager *MyDosManager, struct DosNode *MyDosNode ) ;
void SendDosNode( struct DosManager *MyDosManager, struct DosNode *MyDosNode ) ;
void AbortDosNode( struct DosManager *MyDosManager, struct DosNode *MyDosNode ) ;
BPTR CurrentDirDosNode( struct DosManager *MyDosManager, struct DosNode *MyDosNode, BPTR MyLock ) ;

LONG BeforeLock( struct DosManager *MyDosManager, struct DosNode *MyDosNode, STRPTR MyName, LONG MyMode ) ;
BPTR AfterLock( struct DosManager *MyDosManager, struct DosNode *MyDosNode, LONG *MyError ) ;
LONG BeforeUnLock( struct DosManager *MyDosManager, struct DosNode *MyDosNode, BPTR MyLock ) ;
BOOL AfterUnLock( struct DosManager *MyDosManager, struct DosNode *MyDosNode, LONG *MyError ) ;
LONG BeforeOpen( struct DosManager *MyDosManager, struct DosNode *MyDosNode, STRPTR MyName, LONG MyMode ) ;
BPTR AfterOpen( struct DosManager *MyDosManager, struct DosNode *MyDosNode, LONG *MyError ) ;
LONG BeforeClose( struct DosManager *MyDosManager, struct DosNode *MyDosNode, BPTR MyFile ) ;
BOOL AfterClose( struct DosManager *MyDosManager, struct DosNode *MyDosNode, LONG *MyError ) ;
LONG BeforeRead( struct DosManager *MyDosManager, struct DosNode *MyDosNode, BPTR MyFile, void *MyBuffer, LONG MyLength ) ;
LONG AfterRead( struct DosManager *MyDosManager, struct DosNode *MyDosNode, LONG *MyError ) ;
LONG BeforeWrite( struct DosManager *MyDosManager, struct DosNode *MyDosNode, BPTR MyFile, void *MyBuffer, LONG MyLength ) ;
LONG AfterWrite( struct DosManager *MyDosManager, struct DosNode *MyDosNode, LONG *MyError ) ;
LONG BeforeCreateDir( struct DosManager *MyDosManager, struct DosNode *MyDosNode, STRPTR MyName ) ;
BPTR AfterCreateDir( struct DosManager *MyDosManager, struct DosNode *MyDosNode, LONG *MyError ) ;

STRPTR DosManagerFault( struct DosManager *MyDosManager, LONG ErrorCode ) ;


#else


/*
** possible actions that can be done by the dos manager
*/
enum DosManagerActions
{
  OPENNEW_ACTION,
  WRITE_ACTION,
  CLOSE_ACTION,
  NUM_ACTIONS  /* number of actions */
} ;


/*
** common data for all dos manager nodes
*/
#define BCSTR_LENGTH ( 64 )
struct DosManagerNode
{
  struct Node dmn_Node ;  /* to be inserted in the list */
  enum DosManagerActions dmn_Action ;  /* action to be done by the dos manager */
  STRPTR dmn_Path ;
  void *dmn_Buffer ;
  ULONG dmn_BufferSize ;
  ULONG dmn_Result ;
  LONG dmn_Error ;
  void ( *dmn_DoFunction )( APTR DoData ) ;  /* callback function when dos opperation finished */
  APTR dmn_DoData ;  /* parameter for callback function */
  /* private data */
  BPTR dmn_DirectoryLock ;  /* lock of the working directory */
  struct DevProc *dmn_DevProc ;  /* the process to send the packets to */
  struct DosPacket *dmn_DosPacket ;  /* the dos packet */  
  struct FileHandle *dmn_FileHandle ;  /* the dos file handle */
  BYTE dmn_CStr[ BCSTR_LENGTH + 3 /* for alinment of BSTR */ ] ;  /* for converting B and C strings */
  BYTE *dmn_BStr ;  /* aligned pointer into the BCSTR */
} ;


/*
** defines for dos manager context
*/
#define PATHEXPANDER_LENGTH ( 128 )
#define ERRORTEXT_LENGTH ( 64 )


/*
** context of the timer handler
*/
struct DosManagerContext
{
  struct DispatcherNode dmc_DispatcherNode ;  /* to insert in the list handled by the dispatcher */
  struct Context *dmc_Context ;  /* pointer to context */
  struct List dmc_List ;  /* list of active dos manager nodes */
  struct MsgPort *dmc_MsgPort ;  /* message port for communicating with dos */
  BYTE dmc_PathExpander[ PATHEXPANDER_LENGTH ] ;  /* to get the correct device process */
  BYTE dmc_ErrorText[ ERRORTEXT_LENGTH ] ;  /* generate error text */
} ;


/*
** module functions
*/
struct DosManagerContext *CreateDosManager( struct Context *MyContext ) ;
void DeleteDosManager( struct DosManagerContext *OldDosManager ) ;
struct DosManagerNode *CreateDosManagerNode( struct DosManagerContext *MyDosManager, STRPTR MyPath, LONG *MyError ) ;
void DeleteDosManagerNode( struct DosManagerContext *MyDosManager, struct DosManagerNode *OldDosManagerNode ) ;
LONG AddDosManagerNode( struct DosManagerContext *MyDosManager, struct DosManagerNode *AddNode ) ;
void AbortDosManagerNode( struct DosManagerContext *MyDosManager, struct DosManagerNode *AbortNode ) ;
void RemDosManagerNode( struct DosManagerContext *MyDosManager, struct DosManagerNode *RemNode ) ;
STRPTR DosManagerFault( struct DosManagerContext *MyDosManager, LONG ErrorCode ) ;
#endif

#endif  /* _DOSMANAGER_H */
