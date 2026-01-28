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
** Dispatcher.h
*/


#ifndef _DISPATCHER_H
#define _DISPATCHER_H


#include "Context.h"
#include <exec/lists.h>


/*
** dispatch modes
*/
enum DispatchMode
{
  NONBLOCKING_MODE,
  BLOCKING_MODE
} ;


/*
** dispatcher context
*/
struct Dispatcher
{
  struct Context *d_Context ;
  struct List d_HandlerList ;  /* list of installed signal handlers */
  ULONG d_SignalMask ;  /* signal mask of all the clients */
  ULONG d_SignalMaskDiff ;  /* if not null, mask update required */
} ;


/*
** helper macro to cast callback function pointers
*/
#define DISPATCHER_DOFUNCTION ULONG ( * )( APTR, ULONG )


/*
** common data for all signal handlers
*/
struct DispatcherNode
{
  struct Node dn_Node ;  /* to build a list of handlers */
  ULONG dn_Signals ;  /* the signals the handler acts on, can be updated by handler return value */
  ULONG ( *dn_DoFunction )( APTR DoData, ULONG TriggerSignals ) ;  /* actual handler function */
  APTR dn_DoData ;  /* parameter for do function */
} ;


/*
** module functions
*/
struct Dispatcher *CreateDispatcher( struct Context *MyContext ) ;
void DeleteDispatcher( struct Dispatcher *OldDispatcher ) ;
void AddDispatcherNode( struct Dispatcher *MyDispatcher, struct DispatcherNode *AddNode ) ;
void RemDispatcherNode( struct Dispatcher *MyDispatcher, struct DispatcherNode *RemNode ) ;
void Dispatcher( struct Dispatcher *MyDispatcher, enum DispatchMode MyMode ) ;


#endif  /* _DISPATCHER_H */
