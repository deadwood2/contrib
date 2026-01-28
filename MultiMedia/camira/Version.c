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
** Version.c
*/


#include "Version.h"


/*
** version definitions
*/
#define CAMIRA_MAJOR 0
#define CAMIRA_MINOR 3
#define CAMIRA_REVISION 6


/*
** string definitions
*/
#define QUOTE(_str) #_str
#define EXPAND_AND_QUOTE(_str) QUOTE(_str)

#define NAME_STRING "cAMIra"
#define MAJOR_STRING EXPAND_AND_QUOTE( CAMIRA_MAJOR )
#define MINOR_STRING EXPAND_AND_QUOTE( CAMIRA_MINOR )
#define REVISION_STRING EXPAND_AND_QUOTE( CAMIRA_REVISION )
#define DATE_STRING __DATE__
#define COPYRIGHTPERIOD_STRING "Copyright 2012-2020"
#define AUTHOR_STRING "Andreas Barth"


/*
** standard version tag
*/
static const char VersionTag[ ] = "\0$VER: " NAME_STRING " " MAJOR_STRING "." MINOR_STRING "." REVISION_STRING " (" DATE_STRING ")" ;
static const char Copyright[ ] = COPYRIGHTPERIOD_STRING " " AUTHOR_STRING ;

/*
** return pointer to the version string including $VER:
*/
char *GetVersionTagString( void )
{
  return( ( char * )&VersionTag[ 1 ] ) ;
}


/*
** return pointer to the version string excluding $VER:
*/
char *GetVersionString( void )
{
  return( ( char * )&VersionTag[ 6 ] ) ;
}


/*
** return pointer to the copyright string
*/
char *GetCopyrightString( void )
{
  return( ( char * )&Copyright[ 0 ] ) ;
}


/*
** return pointer to the author string
*/
char *GetAuthorString( void )
{
  return( ( char * )&Copyright[ 6 ] ) ;
}
