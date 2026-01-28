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
** ProgressBar.h
*/


#ifndef _PROGRESSBAR_H
#define _PROGRESSBAR_H


#include "Context.h"


/*
** progress context
*/
struct ProgressBar
{
  struct Context *pb_Context ;
  struct List pb_NodesList ;  /* one with highest prio will be shown */
  APTR pb_ProgressGauge ;  /* the gauge object */
  APTR pb_ProgressButton ;  /* the button object */
  APTR pb_ProgressButtonGroup ;  /* the button group object */
  APTR pb_BarObject ;  /* the group object */
  ULONG pb_AddedToMainWindow ;  /* keep status of adding to main window */
} ;


/*
** multiple of this can be registered, but only the one with highest prio is shown
*/
struct ProgressBarNode
{
  struct Node pbn_Node ;  /* so we can handle this in a queue */
  STRPTR pbn_InfoString ;  /* info string when displayed */
  ULONG pbn_Limit ;  /* max value the gauge can indicate */
  ULONG pbn_LastCurrent ;  /* last current value of the gauge */
  ULONG pbn_ButtonMode ;
  ULONG pbn_ButtonState ;
} ;


/*
** module functions
*/
struct ProgressBar *CreateProgressBar( struct Context *MyContext ) ;
void DeleteProgressBar( struct ProgressBar *OldProgressBar ) ;
void AddProgressBarNode( struct ProgressBar *MyProgressBar, struct ProgressBarNode *AddNode, ULONG Initial ) ;
void RemProgressBarNode( struct ProgressBar *MyProgressBar, struct ProgressBarNode *RemNode ) ;
void ReportProgress( struct ProgressBar *MyProgressBar, struct ProgressBarNode *MyNode, ULONG Current ) ;


#endif  /* !_PROGRESSBAR_H */
