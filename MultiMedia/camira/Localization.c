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
** Localization.c
*/


#include "Localization.h"
#include <proto/exec.h>
#include <proto/locale.h>


/*
** now only get the string array
*/
#undef cAMIra_STRINGS_H
#define cAMIra_ARRAY
#include "CatComp.h"


/*
** get a string from the catalog or the builtin version
*/
STRPTR GetLocalizedString( struct Localization *MyLocalization, LONG StringID )
{
  struct Context *MyContext ;
  struct Library *LocaleBase ;
  STRPTR LocalizedString ;
  LONG Count, NumberOfStrings ;
  

  NumberOfStrings = ( sizeof( cAMIra_Array ) / sizeof( struct cAMIra_ArrayType ) ) ;
  for( Count = 0 ; Count < NumberOfStrings ; Count++ )
  {  /* look for the requested sting identifier */
    if( cAMIra_Array[ Count ].cca_ID == StringID )
    {  /* string identifier match */
      LocalizedString = cAMIra_Array[ Count ].cca_Str ;
      break ;
    }
  }
  if( Count == NumberOfStrings )
  {  /* string was not found */
    LocalizedString = "string error" ;
  }

  if( ( NULL != MyLocalization ) && ( NULL != MyLocalization->l_Catalog ) )
  {  /* requirements ok, get string from catalog */
    MyContext = MyLocalization->l_Context ;
    LocaleBase = MyLocalization->l_LocaleBase ;
    LocalizedString = (STRPTR)GetCatalogStr( MyLocalization->l_Catalog, StringID, LocalizedString ) ;
  }
  else
  {  /* requirements not ok, so use internal strings */
  }

  return( LocalizedString ) ;
}


/*
** delete localization context
*/
void DeleteLocalization( struct Localization *OldLocalization )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct Library *LocaleBase ;

  if( NULL != OldLocalization )
  {  /* localization context needs to be freed */
    MyContext = OldLocalization->l_Context ;
    SysBase = MyContext->c_SysBase ;
    
    if( NULL != OldLocalization->l_LocaleBase )
    {  /* locale library needs to be closed */
      LocaleBase = OldLocalization->l_LocaleBase ;
      if( NULL != OldLocalization->l_Locale )
      {  /* locale needs to be closed */
        if( NULL != OldLocalization->l_Catalog )
        {  /* catalog needs to be closed */
          CloseCatalog( OldLocalization->l_Catalog ) ;
        }
        CloseLocale( OldLocalization->l_Locale ) ;
      }
      CloseLibrary( ( struct Library * )OldLocalization->l_LocaleBase ) ;
    }
    FreeVec( OldLocalization ) ;
  }
}


/*
** create context for the localization stuff 
*/
struct Localization *CreateLocalization( struct Context *MyContext )
{
  struct ExecBase *SysBase ;
  struct Library *LocaleBase ;
  struct Localization *NewLocalization ;
  
  NewLocalization = NULL ;
  
  if( NULL != MyContext )
  {  /* requirements seem ok */
    SysBase = MyContext->c_SysBase ;

    NewLocalization = AllocVec( sizeof( struct Localization ), MEMF_ANY | MEMF_CLEAR ) ;
    if( NULL != NewLocalization )
    {  /* memory for localization context ok */
      NewLocalization->l_Context = MyContext ;
      NewLocalization->l_LocaleBase = OpenLibrary( "locale.library", 0 ) ;
      if( NULL != NewLocalization->l_LocaleBase )
      {  /* locale library ok */
        LocaleBase = NewLocalization->l_LocaleBase ;
        NewLocalization->l_Locale = OpenLocale( NULL ) ;
        if( NULL != NewLocalization->l_Locale )
        {  /* locale ok */
          NewLocalization->l_Catalog = OpenCatalog( NewLocalization->l_Locale,
                                                     "cAMIra.catalog",
                                                     OC_BuiltInLanguage, ( IPTR )"english",
                                                     OC_Version, ( IPTR )0,
                                                     TAG_END ) ;
          if( NULL != NewLocalization->l_Catalog )
          {  /* catalog ok */
          }
          else
          {  /* catalog not ok */
            DeleteLocalization( NewLocalization ) ;
            NewLocalization = NULL ;
          }
        }
        else
        {  /* locale not ok */
          DeleteLocalization( NewLocalization ) ;
          NewLocalization = NULL ;
        }
      }
      else
      {  /* locale library ok */
        DeleteLocalization( NewLocalization ) ;
        NewLocalization = NULL ;
      }
    }
    else
    {  /* memory for localization context not ok */
    }
  }
  else
  {  /* requirements not ok */
  }

  return( NewLocalization ) ;
}
