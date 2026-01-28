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
** Grayscale8Format.c
*/


#include "Grayscale8Format.h"
#include "Log.h"
#include <proto/exec.h>


/*
** grayscale 8 progress hook
*/
static ULONG Grayscale8ProgressHook( struct ImageProcessor *MyImageProcessor, ULONG Current, ULONG Total )
{
  struct Context *MyContext ;
  ULONG Canceled = 0 ;
  
  MyContext = MyImageProcessor->ip_Context ;

  //Dispatcher( MyContext->c_Dispatcher, NONBLOCKING_MODE ) ;  /* dispatch non-blocking */
  
  return( Canceled ) ;
}


/*
** create rgb 24 image based on grayscale 8 image
*/
struct vhi_image *CreateRgb24FromGrayscale8( struct ImageProcessor *MyImageProcessor, struct vhi_image *Grayscale8Image )
{
  struct Context *MyContext ;
  struct vhi_image *Rgb24Image ;
  ULONG PosX, PosY ;
  UBYTE *Rgb24Data, *Grayscale8Data ;
  
  Rgb24Image = NULL ;
  
  if( ( NULL != MyImageProcessor ) && ( NULL != Grayscale8Image ) )
  {  /* parameter seem to be ok */
    MyContext = MyImageProcessor->ip_Context ;
    
    Rgb24Image = AllocImage( MyContext, Grayscale8Image->width, Grayscale8Image->height, VHI_RGB_24 ) ;
    if( NULL != Rgb24Image )
    {  /* rgb 24 image struct ok */
      Grayscale8Data = Grayscale8Image->chunky ;
      Rgb24Data = Rgb24Image->chunky ;
      for( PosY = 0 ; PosY < Grayscale8Image->height ; PosY++ )
      {
        for( PosX = 0 ; PosX < Grayscale8Image->width ; PosX++ )
        {
          *( Rgb24Data ) = *( Grayscale8Data ) ;
          Rgb24Data++ ;
          *( Rgb24Data ) = *( Grayscale8Data ) ;
          Rgb24Data++ ;
          *( Rgb24Data ) = *( Grayscale8Data ) ;
          Rgb24Data++ ;
          Grayscale8Data++ ;
        }
        Grayscale8ProgressHook( MyImageProcessor, PosY, Grayscale8Image->height ) ;
      }
    }
    else
    {  /* rgb 24 image struct not ok */
      LogText( MyContext->c_Log, WARNING_LEVEL, "rgb 24 image struct not ok" ) ;
    }
  }
  else
  {  /* parameter not ok */
  }
  
  return( Rgb24Image ) ;
}
