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
** Timer.h
*/


#ifndef _TIMER_H
#define _TIMER_H


#include "Dispatcher.h"
#include <devices/timer.h>
#include <utility/date.h>


/*
** common data for all timer nodes
*/
struct TimerNode
{
  struct Node tn_Node ;  /* to be inserted in the list */
  struct timeval tn_DeltaTime ;  /* difference time, added to absolute time */
  struct timeval tn_AbsoluteTime ;  /* reference time becoming the absolute timeout time */
  void ( *tn_DoFunction )( APTR DoData ) ;  /* callback function when timed out */
  APTR tn_DoData ;  /* parameter for callback function */
} ;


/*
** context of the timer handler
*/
struct Timer
{
  struct DispatcherNode t_DispatcherNode ;  /* to insert in the list handled by the dispatcher */
  struct Context *t_Context ;  /* pointer to context */
  struct Device *t_TimerBase ;
  struct List t_TimerList ;  /* our list of timer nodes, active timers */
  struct MsgPort *t_MsgPort ;  /* our message port for communicating with the device */
  struct timerequest *t_TimeRequest ;  /* our io request for communicating with the device */
} ;


/*
** module functions
*/
struct Timer *CreateTimer( struct Context *MyContext ) ;
void DeleteTimer( struct Timer *OldTimer ) ;
LONG AddTimerNode( struct Timer *MyTimer, struct TimerNode *MyNode ) ;
void RemTimerNode( struct Timer *MyTimer, struct TimerNode *MyNode ) ;
void GetClock( struct Timer *MyTimer, struct ClockData *MyClockData ) ;


#endif  /* _TIMER_H */
