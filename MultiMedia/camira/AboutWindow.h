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
** AboutWindow.h
*/


#ifndef _ABOUTWINDOW_H
#define _ABOUTWINDOW_H


#include "Context.h"


/*
** about window context
*/
#define AW_ABOUTTEXT_LENGTH ( 512 )
struct AboutWindow
{
  struct Context *aw_Context ;
  APTR aw_WindowObject ;
  BYTE aw_AboutText[ AW_ABOUTTEXT_LENGTH ] ;
} ;


/*
** functions of this module
*/
struct AboutWindow *CreateAboutWindow( struct Context *MyContext ) ;
void DeleteAboutWindow( struct AboutWindow *OldAboutWindow ) ;


#endif  /* !_ABOUTWINDOW_H */
