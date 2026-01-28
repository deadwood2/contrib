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
** Magic.h
*/


#ifndef _MAGIC_H
#define _MAGIC_H


#include "Dispatcher.h"
#include <libraries/mui.h>


/*
** defines for custom MUI identifiers
*/
#define MUIB_CAMIRA ( 0x90AB0000 )
#define MUIB_APPLICATION ( MUIB_CAMIRA | 0x0000 )
#define MUIB_MAINWINDOW ( MUIB_CAMIRA | 0x0100 )
#define MUIB_DISPLAYAREA ( MUIB_CAMIRA | 0x0200 )
#define MUIB_SETTINGSWINDOW ( MUIB_CAMIRA | 0x0300 )
#define MUIB_LOGWINDOW ( MUIB_CAMIRA | 0x0400 )


/*
** magic context
*/
#define M_STRINGBENCH_LENGTH ( 512 )
struct Magic
{
  struct DispatcherNode m_DispatcherNode ;  /* this is a dispatcher client */
  struct Context *m_Context ;  /* pointer to context */
  APTR m_Application ;  /* the mui application object */
  struct MUI_CustomClass *m_ApplicationClass ;  /* custom application class */
  struct MUI_CustomClass *m_MainWindowClass ;  /* custom main window class */
  struct MUI_CustomClass *m_DisplayAreaClass ;  /* custom display area class */
  struct MUI_CustomClass *m_SettingsWindowClass ;  /* custom settings window class */
  struct MUI_CustomClass *m_LogWindowClass ;  /* custom log window class */
  BYTE m_StringBench[ M_STRINGBENCH_LENGTH ] ;  /* for working on strings outside string.mui */
} ;


/*
** functions of this module
*/
struct Magic *CreateMagic( struct Context *MyContext ) ;
void DeleteMagic( struct Magic *OldMagic ) ;
APTR MagicSpace( struct Magic *MyMagic ) ;
APTR MagicButton( struct Magic *MyMagic, STRPTR Name, ULONG Key, STRPTR Help ) ;
APTR MagicToggleButton( struct Magic *MyMagic, STRPTR Name, ULONG Key, STRPTR Help ) ;
APTR MagicLabel( struct Magic *MyMagic, STRPTR Name, ULONG Key ) ;
APTR MagicCheckmark( struct Magic *MyMagic, ULONG Key, STRPTR Help ) ;
APTR MagicString( struct Magic *MyMagic, ULONG Key, STRPTR Help ) ;
APTR MagicSecretString( struct Magic *MyMagic, ULONG Key, STRPTR Help ) ;
APTR MagicInteger( struct Magic *MyMagic, ULONG Key, STRPTR Help ) ;
APTR MagicText( struct Magic *MyMagic, STRPTR Contents ) ;
APTR MagicImage( struct Magic *MyMagic, ULONG Spec, ULONG Key ) ;
APTR MagicCycle( struct Magic *MyMagic, STRPTR *Names, ULONG Key, STRPTR Help ) ;
APTR MagicSlider( struct Magic *MyMagic, ULONG Key, STRPTR Help ) ;
void MagicStringInsert( struct Magic *MyMagic, APTR MyString, STRPTR MyText ) ;


#endif  /* !_MAGIC_H */
