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
** Scheduler.h
*/


#ifndef _SCHEDULER_H
#define _SCHEDULER_H


#include "Timer.h"
#include "ProgressBar.h"
#include "Timer.h"


/*
** display area context
*/
struct Scheduler
{
  struct TimerNode s_TimerNode ;  /* for the timing */
  struct Context *s_Context ;  /* pointer to the global context */
  struct ProgressBarNode s_ProgressBarNode ;  /* for display */
  ULONG s_CurrentInterval ;  /* current time between display requests */
  ULONG s_Counter ;  /* we count the rate in secondes */
  ULONG s_CurrentCaptureEnable ;  /* that we know, if we have to trigger the capture process */
  ULONG s_CurrentUploadEnable ;  /* that we know, if we have to trigger the upload process */
} ;


/*
** functions of this module
*/
struct Scheduler *CreateScheduler( struct Context *MyContext ) ;
void DeleteScheduler( struct Scheduler *OldScheduler ) ;
void ConfigureScheduler( struct Scheduler *MyScheduler ) ;


#endif  /* !_SCHEDULER_H */
