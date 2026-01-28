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
** ab_stdio.c
*/


#include "ab_stdio.h"


#define FORMATFLAG_LONG ( 1UL<<0 )
#define FORMATFLAG_SIGNED ( 1UL<<1 )
#define X_FORMATFLAG_CAPITAL ( 1UL<<2 )
#define FORMATFLAG_LEFT ( 1UL<<3 )
#define FORMATFLAG_ZEROS ( 1UL<<4 )
#define FORMATFLAG_PLUS ( 1UL<<5 )
#define FORMATFLAG_SPACE ( 1UL<<6 )
#define FORMATFLAG_SHORT ( 1UL<<7 )


/*
** basic vsnprintf replacement
*/
#define COPYBUFFER_LENGTH ( 32 )
int ab_vsnprintf( char *Buffer, int Length, const char *Format, va_list Arguments )
{
  int BufferIndex ;
  int FormatIndex ;
  int CopyLength ;
  int Status ;
  int FormatFlags ;
  int Width ;
  char *CopySource ;
  long Value ;
  char CopyBuffer[ COPYBUFFER_LENGTH ] ;
  int CopyIndex ;
  int BufferLength ;
  char BufferChar ;
  char FormatStep ;

  BufferLength = 0 ;
  if( ( Buffer ) && ( Format ) )
  {  /* requirements ok */
    Status = 0 ;
    BufferChar = 0 ;
    BufferIndex = 0 ;
    FormatIndex = 0 ;
    while( Format[ FormatIndex ] )
    {  /* as long as there are characters in the format string */
      FormatStep = 1 ;
      switch( Status )
      {
        case 0:  /* we are in copy mode and also look for format placeholders */
          if( '%' == Format[ FormatIndex ] )
          {  /* found a format specifier */
            FormatFlags = 0 ;
            Width = 0 ;
            Status = 1 ;  /* analyze the format specifier as next */
          }
          else
          {  /* just copy from format string to output string */
            BufferChar = Format[ FormatIndex ] ;
          }
          break ;
        case 1:  /* check for flags */
          switch( Format[ FormatIndex ] )
          {
            case '-':  /* flags */
              FormatFlags |= FORMATFLAG_LEFT ;
              break ;
            case '0':  /* flags */
              FormatFlags |= FORMATFLAG_ZEROS ;
              break ;
            case '+':  /* flags */
              FormatFlags |= FORMATFLAG_PLUS ;
              break ;
            case ' ':  /* flags */
              FormatFlags |= FORMATFLAG_SPACE ;
              break ;
            case '#':  /* flags */
              /* ignored so far */
              break ;
            default:  /* no more flags */
              FormatStep = 0 ;
              Status = 2 ;
              break ;
          }
          break ;
        case 2:  /* check for width */          
          switch( Format[ FormatIndex ] )
          {
            case '0':  /* width */
            case '1':  /* width */
            case '2':  /* width */
            case '3':  /* width */
            case '4':  /* width */
            case '5':  /* width */
            case '6':  /* width */
            case '7':  /* width */
            case '8':  /* width */
            case '9':  /* width */
              Width *= 10 ;
              Width += Format[ FormatIndex ] - 0x30 ;
              break ;
            case '*':  /* width */
              Width = va_arg( Arguments, int ) ;
              break ;
            case '.':  /* precission */
              /* ignored so far */
              break ;
            default:  /* no more width */
              FormatStep = 0 ;
              Status = 3 ;
              break ;
          }
          break ;
        case 3:  /* check for length */
          switch( Format[ FormatIndex ] )
          {
            case 'h':
              FormatFlags |= FORMATFLAG_SHORT ;
              break ;
            case 'l':
              FormatFlags |= FORMATFLAG_LONG ;
              break ;
            default:  /* no more length */
              FormatStep = 0 ;
              Status = 4 ;
              break ;
          }
          break ;
        case 4:  /* check for specifier */
          switch( Format[ FormatIndex ] )
          {
            case 'i':  /* replace by signed integer */
            case 'd':  /* replace by signed integer */
              FormatFlags |= FORMATFLAG_SIGNED ;
            case 'u':  /* replace by unsigned integer */
            case 'X':  /* replace by hex number */
            case 'x':  /* replace by hex number */
            case 'p':  /* replace by pointer */
              if( FormatFlags & FORMATFLAG_LONG )
              {  /* convert a long value */
                Value = va_arg( Arguments, long ) ;
              }
              else if( FormatFlags & FORMATFLAG_SHORT )
              {  /* convert a short value */
                Value = va_arg( Arguments, short ) ;
              }
              else
              {  /* convert a int value */
                Value = va_arg( Arguments, int ) ;
              }
              if( FormatFlags & FORMATFLAG_SIGNED )
              {  /* check for negative value */
                if( 0 > Value )
                {  /* it is neagive, leave signed flag to draw the '-' */
                  Value *= -1 ;
                }
                else
                {  /* not negative */
                  FormatFlags &= ~FORMATFLAG_SIGNED ;
                }
              }
              CopyIndex = COPYBUFFER_LENGTH ;
              do
              {  /* integer to ascii loop */
                CopyIndex-- ;
                switch( Format[ FormatIndex ] )
                {
                  case 'i':  /* signed decimal conversion */
                  case 'd':  /* signed decimal conversion */
                    CopyBuffer[ CopyIndex ] = ( '0' + ( Value % 10 ) ) ;
                    Value /= 10 ;
                    break ;  /* unsigned decimal conversion */
                  case 'u':
                    unsigned long ulv = (unsigned long)Value;
                    CopyBuffer[ CopyIndex ] = ( '0' + ( ulv % 10 ) ) ;
                    ulv /= 10 ;
                    Value = (long)ulv;
                    break ;
                  case 'X':  /* hex conversion */
                  case 'x':  /* hex conversion */
                  case 'p':  /* hex conversion */
                    CopyBuffer[ CopyIndex ] = ( '0' + ( Value & 0xF ) ) ;
                    Value >>= 4 ;
                    if( CopyBuffer[ CopyIndex ] > '9' )
                    {  /* adjust for hex values */
                      CopyBuffer[ CopyIndex ] += 0x7 + ( 0x20 * ( 'x' == Format[ FormatIndex ] ) ) ;
                    }
                    break ;
                }
              }
              while( ( Value ) && ( CopyIndex ) ) ;
              CopySource = &CopyBuffer[ CopyIndex ] ;
              CopyLength = ( COPYBUFFER_LENGTH - CopyIndex ) ;
              if( Width < ( CopyLength + ( 1 && ( FormatFlags & ( FORMATFLAG_SIGNED | FORMATFLAG_PLUS | FORMATFLAG_SPACE ) ) ) ) )
              {  /* extend width as required */
                Width = ( CopyLength + ( 1 && ( FormatFlags & ( FORMATFLAG_SIGNED | FORMATFLAG_PLUS | FORMATFLAG_SPACE ) ) ) ) ;
              }
              CopyIndex = 0 ;
              FormatStep = 0 ;
              Status = 5 ;
              break ;
            case 's':  /* replace by string */
              CopySource = ( char * )va_arg( Arguments, char * ) ;
              if( CopySource )
              {  /* seems to be a valid string pointer */
                CopyLength = 0 ;
                while( '\0' != CopySource[ CopyLength ] )
                {  /* get length of string to to replace %s */
                  CopyLength++ ;
                }
                if( Width < CopyLength )
                {  /* extend width as required */
                  Width = CopyLength ;
                }
                CopyIndex = 0 ;
                FormatStep = 0 ;
                Status = 5 ;
              }
              else
              {  /* no valid string pointer */
                Status = 0 ;
              }
              break ;
            case '%':  /* replace by the % sign */
              CopyBuffer[ 0 ] = Format[ FormatIndex ] ;
              CopySource = &CopyBuffer[ 0 ] ;
              CopyLength = 1 ;
              if( Width < CopyLength )
              {  /* extend width as required */
                Width = CopyLength ;
              }
              CopyIndex = 0 ;
              FormatStep = 0 ;
              Status = 5 ;
              break ;
            case 'c':  /* replace by a character */
              CopyBuffer[ 0 ] = va_arg( Arguments, int ) ;
              CopySource = &CopyBuffer[ 0 ] ;
              CopyLength = 1 ;
              if( Width < CopyLength )
              {  /* extend width as required */
                Width = CopyLength ;
              }
              CopyIndex = 0 ;
              FormatStep = 0 ;
              Status = 5 ;
              break ;
            case 'n':  /* store current buffer index */
              Value = va_arg( Arguments, int ) ;
              *( ( int * )Value ) = BufferIndex ;
              Status = 0 ;
              break ;
            default:  /* unhandled format specifier */
              break ;
          }
          break ;
        case 5:  /* we copy something */
          if( 0 != Width )
          {  /* still something to fill in */
            if( ( FormatFlags & ( FORMATFLAG_SIGNED | FORMATFLAG_PLUS | FORMATFLAG_SPACE ) ) && 
                ( ( FormatFlags & ( FORMATFLAG_LEFT ) ) || ( FormatFlags & ( FORMATFLAG_ZEROS ) ) || ( Width == CopyLength + 1 ) ) )
            {
              if( FormatFlags & FORMATFLAG_SIGNED )
              {  /* add minus */
                BufferChar = '-' ;
              }
              else if( FormatFlags & FORMATFLAG_PLUS )
              {  /* add plus */
                BufferChar = '+' ;
              }
              else if( FormatFlags & FORMATFLAG_SPACE )
              {  /* add space */
                BufferChar = ' ' ;
              }
              FormatFlags &= ~( FORMATFLAG_SIGNED | FORMATFLAG_PLUS | FORMATFLAG_SPACE ) ;
            }
            else
            {
              if( ( ( FormatFlags & ( FORMATFLAG_LEFT ) ) && ( CopyLength ) ) ||
                  ( Width == CopyLength ) )
              {
                BufferChar = CopySource[ CopyIndex ] ;
                CopyIndex++ ;
                CopyLength-- ;
              }
              else
              {
                if( ( FormatFlags & ( FORMATFLAG_ZEROS ) ) )
                {
                  BufferChar = '0' ;
                }
                else
                {
                  BufferChar = ' ' ;
                }
              }
            }
            Width-- ;
            FormatStep = 0 ;
          }
          else
          {  /* done */
            Status = 0 ;
          }
          break ;
        default:  /* unknown status */
          break ;
      }
    
      if( BufferChar )
      {  /* there is a char to copy into the buffer */
        if( BufferIndex < ( Length - 1 ) )
        {  /* we can still copy into the buffer */
          Buffer[ BufferIndex ] = BufferChar ;
          BufferIndex++ ;
        }
        BufferChar = 0 ;
        BufferLength++ ;
      }
    
      FormatIndex = FormatIndex + FormatStep ;
    }
    if( BufferIndex < Length )
    {  /* 0-char still fits */
      Buffer[ BufferIndex ] = '\0' ;
    }
  }
  else
  {  /* requirements not ok */
  }
  
  return( BufferLength ) ;
}


/*
** basic snprintf replacement
*/
int ab_snprintf( char *Buffer, int Length, const char *Format, ... )
{
  va_list Arguments ;
  int Result ;
  
  va_start( Arguments, Format ) ;
  Result = ab_vsnprintf( Buffer, Length, Format, Arguments ) ;
  va_end( Arguments ) ;

  return( Result ) ;
}


/*
** basic vsprintf replacement
*/
int ab_vsprintf( char *Buffer, const char *Format, va_list Arguments )
{
  int Result ;
  
  Result = ab_vsnprintf( Buffer, 0xFFFF, Format, Arguments ) ;

  return( Result ) ;
}


/*
** basic sprintf replacement
*/
int ab_sprintf( char *Buffer, const char *Format, ... )
{
  va_list Arguments ;
  int Result ;
  
  va_start( Arguments, Format ) ;
  Result = ab_vsnprintf( Buffer, 0xFFFF, Format, Arguments ) ;
  va_end( Arguments ) ;

  return( Result ) ;
}

