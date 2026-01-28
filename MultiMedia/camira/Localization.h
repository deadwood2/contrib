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
** Localization.h
*/


#ifndef _LOCALIZATION_H
#define _LOCALIZATION_H


#include "Context.h"
#include <libraries/locale.h>


/*
** only let them know the string identifiers
*/
#define cAMIra_NUMBERS
#include "CatComp.h"
#undef cAMIra_NUMBERS


/*
** localization context
*/
struct Localization
{
  struct Context *l_Context ;  /* pointer to context */
  struct Library *l_LocaleBase ;  /* locale library */
  struct Locale *l_Locale ;  /* the locale */
  struct Catalog *l_Catalog ;  /* our localized catalog */
} ;


/*
** functions of this module
*/
struct Localization *CreateLocalization( struct Context *MyContext ) ;
void DeleteLocalization( struct Localization *OldLocalization ) ;
STRPTR GetLocalizedString( struct Localization *MyLocalization, LONG StringID ) ;


#endif  /* _LOCALIZATION_H */
