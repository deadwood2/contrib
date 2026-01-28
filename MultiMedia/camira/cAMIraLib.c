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
** cAMIraLib.c
*/


#include "cAMIraLib.h"
#include "ab_stdio.h"


/* 
** cAMIra spesific string expansion
*/
LONG ExpandString( STRPTR MyString, LONG MyLength, STRPTR MyTemplate, struct ExpandData *MyExpandData )
{
  LONG StringIndex, StringDelta, TemplateIndex ;
  
  TemplateIndex = 0 ;
  StringIndex = 0 ;
    
  if( ( NULL != MyString ) && ( NULL != MyTemplate ) && ( NULL != MyExpandData ) && ( 1 < MyLength ) )
  {  /* requirements ok */
    
    while( ( '\0' != MyTemplate[ TemplateIndex ] ) && ( ( MyLength - 1 ) > StringIndex ) )
    {  /* walk through the template */
      if( '%' != MyTemplate[ TemplateIndex ] )
      {  /* no specifier */
        MyString[ StringIndex ] = MyTemplate[ TemplateIndex ] ;
        TemplateIndex++ ;
        StringIndex++ ;
      }
      else
      {  /* a specifier */
        TemplateIndex++ ;
        StringDelta = 0 ;
        switch( MyTemplate[ TemplateIndex ] )
        {  /* time/data placeholder taken from strftime() */
          case EXPAND_SPECIFIER_YEAR:  /* year with 4 digits */
            if( NULL != MyExpandData->ed_ClockData )
            {
              snprintf( &MyString[ StringIndex ], MyLength - StringIndex, "%04d%n",
                MyExpandData->ed_ClockData->year, &StringDelta ) ;
            }
            break ;
          case EXPAND_SPECIFIER_MONTH:  /* month, 2 digits */
            if( NULL != MyExpandData->ed_ClockData )
            {
             snprintf( &MyString[ StringIndex ], MyLength - StringIndex, "%02d%n",
                MyExpandData->ed_ClockData->month, &StringDelta ) ;
            }
            break ;
          case EXPAND_SPECIFIER_DAY:  /* day, 2 digits */
            if( NULL != MyExpandData->ed_ClockData )
            {
              snprintf( &MyString[ StringIndex ], MyLength - StringIndex, "%02d%n",
                MyExpandData->ed_ClockData->mday, &StringDelta ) ;
            }
            break ;
          case EXPAND_SPECIFIER_HOUR:  /* hour, 24h format, 2 digits */
            if( NULL != MyExpandData->ed_ClockData )
            {
              snprintf( &MyString[ StringIndex ], MyLength - StringIndex, "%02d%n", 
                MyExpandData->ed_ClockData->hour, &StringDelta ) ;
            }
            break ;
          case EXPAND_SPECIFIER_MINUTE:  /* minutes, 2 digits */
            if( NULL != MyExpandData->ed_ClockData )
            {
              snprintf( &MyString[ StringIndex ], MyLength - StringIndex, "%02d%n",
                MyExpandData->ed_ClockData->min, &StringDelta ) ;
            }
            break ;
          case EXPAND_SPECIFIER_SECOND:  /* seconds, 2 digits */
            if( NULL != MyExpandData->ed_ClockData )
            {
              snprintf( &MyString[ StringIndex ], MyLength - StringIndex, "%02d%n",
                MyExpandData->ed_ClockData->sec, &StringDelta ) ;
            }
            break ;
          /* some cAMIra specific specifiers */
          case EXPAND_SPECIFIER_COUNTER:  /* sequence number, 4 digits */
            if( NULL != MyExpandData->ed_Number )
            {
              snprintf( &MyString[ StringIndex ], MyLength - StringIndex, "%04ld%n",
                *MyExpandData->ed_Number, &StringDelta ) ;
            }
            break ;
          default:  /* unhandled specifier */
            break ;
        }
        StringIndex += StringDelta ;
        TemplateIndex++ ;
      }
    }
    MyString[ StringIndex ] = '\0' ;
  }
  else
  {  /* requirements not ok */
  }
  
  return( StringIndex ) ;
}
