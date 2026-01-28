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
** DisplayArea.h
*/


#ifndef _DISPLAYAREA_H
#define _DISPLAYAREA_H


#include "DisplayAreaClass.h"
#include "Timer.h"
#include "ImageProcessor.h"


/*
** display area context
*/
struct DisplayArea
{
  struct Context *da_Context ;  /* the context */
  struct TimerNode da_TimerNode ;  /* for the timing */
  APTR da_AreaObject ;  /* the magic display area object */
  ULONG da_AddedToMainWindow ;  /* remember if the area was added to the main window */
  struct ImageBox *da_CurrentImageBox ;  /* image box currently processed */
  ULONG da_CurrentInterval ;  /* current time between display requests */
} ;


/*
** functions of this module
*/
struct DisplayArea *CreateDisplayArea( struct Context *MyContext ) ;
void DeleteDisplayArea( struct DisplayArea *OldDisplayArea ) ;
void ConfigureDisplayArea( struct DisplayArea *MyDisplayArea ) ;
LONG CheckDisplayArea( struct DisplayArea *MyDisplayArea ) ;
void DisplayImage( struct DisplayArea *MyDisplayArea, struct ImageBox *MyImageBox ) ;
struct ImageBox *GetDisplayImage( struct DisplayArea *MyDisplayArea ) ;


#endif  /* !_DISPLAYAREA_H */
