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
** MemoryManager.h
*/


#ifndef _MEMORYMANAGER_H
#define _MEMORYMANAGER_H


#include "Dispatcher.h"
#include <exec/ports.h>


/*
** message format processed by the memory manager
*/
struct MemoryMessage
{
  struct Message mm_Message ;  /* it is actually a message */
  ULONG mm_Size ;  /* requested amount of memory */
  ULONG mm_Attributes ;  /* attributes for the requested memory */
  void *mm_MemoryChunk ;  /* this != NULL means we should free it */
} ;


/*
** memory manager context
*/
struct MemoryManager
{
  struct DispatcherNode mm_DispatcherNode ;  /* this is a dispatcher client */
  struct Context *mm_Context ;  /* pointer to context */
  struct MsgPort *mm_MsgPort ;  /* message port of the memory manager */
} ;


/*
** functions of this module
*/
struct MemoryManager *CreateMemoryManager( struct Context *MyContext ) ;
void DeleteMemoryManager( struct MemoryManager *OldMemoryManager ) ;


#endif  /* !_MEMORYMANAGER_H */
