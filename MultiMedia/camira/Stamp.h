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
** Stamp.c
*/


#ifndef _STAMP_H
#define _STAMP_H


#include "Context.h"
#include <graphics/text.h>


/*
** context of the stamp
*/
#define S_STAMPTEXT_LENGTH ( 128 )
#define S_CURRENTSTAMPTEMPLATE_LENGTH ( 128 )
#define S_CURRENTSTAMPFONT_LENGTH ( 32 )
struct Stamp
{
  struct Context *s_Context ;
  struct TextAttr s_TextAttr ;
  struct TextFont *s_TextFont ;
  BYTE s_StampText[ S_STAMPTEXT_LENGTH ] ;
  BYTE s_CurrentStampTemplate[ S_CURRENTSTAMPTEMPLATE_LENGTH ] ;
  BYTE s_CurrentStampFont[ S_CURRENTSTAMPFONT_LENGTH ] ;
  ULONG s_CurrentStampFontsize ;
  ULONG s_CurrentStampX ;
  ULONG s_CurrentStampY ;
} ;


/*
** functions of this module
*/
struct Stamp *CreateStamp( struct Context *MyContext ) ;
void DeleteStamp( struct Stamp *OldStamp ) ;
void ConfigureStamp( struct Stamp *MyStamp ) ;
LONG StampImage( struct Stamp *MyStamp, struct ImageBox *MyImageBox ) ;
ULONG ParseImageString( char *MyString, char *MyTemplate, struct ImageBox *MyImageBox ) ;


#endif  /* !_STAMP_H */
