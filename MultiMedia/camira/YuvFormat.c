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
** YuvFormat.c
*/


#include "YuvFormat.h"
#include "Log.h"
#include <proto/exec.h>


/*
** yuv progress hook
*/
static ULONG YuvProgressHook( struct ImageProcessor *MyImageProcessor, ULONG Current, ULONG Total )
{
  struct Context *MyContext ;
  ULONG Canceled = 0 ;
  
  MyContext = MyImageProcessor->ip_Context ;

  //Dispatcher( MyContext->c_Dispatcher, NONBLOCKING_MODE ) ;  /* dispatch non-blocking */
  
  return( Canceled ) ;
}


/*
** helper to clip to UBYTE
*/
static UBYTE Clip( LONG Value )
{
  if( 0 > Value )
  {
    Value = 0 ;
  }
  else if( 255 < Value )
  {
    Value = 255 ;
  }
  
  return( ( UBYTE )Value ) ;
}
 

/*
http://msdn.microsoft.com/en-us/library/windows/desktop/dd206750.aspx
C = Y - 16
D = U - 128
E = V - 128
R = clip(( 298 * C           + 409 * E + 128) >> 8)
G = clip(( 298 * C - 100 * D - 208 * E + 128) >> 8)
B = clip(( 298 * C + 516 * D           + 128) >> 8)
*/
/*
** create rgb 24 image based on yuv image
*/
struct vhi_image *CreateRgb24FromYuv( struct ImageProcessor *MyImageProcessor, struct vhi_image *YuvImage )
{
  struct Context *MyContext ;
  struct vhi_image *Rgb24Image ;
  UBYTE *Rgb24Data ;
  UBYTE *YData, *UData, *VData ;
  ULONG PosX, PosY ;
  UBYTE UVUse, UVMaxUse ;
  LONG C, D, E ;
  
  Rgb24Image = NULL ;
  
  if( ( NULL != MyImageProcessor ) && ( NULL != YuvImage ) )
  {  /* parameter seem to be ok */
    MyContext = MyImageProcessor->ip_Context ;
    
    Rgb24Image = AllocImage( MyContext, YuvImage->width, YuvImage->height, VHI_RGB_24 ) ;
    if( NULL != Rgb24Image )
    {  /* rgb 24 image struct ok */
      Rgb24Data = Rgb24Image->chunky ;
#if !defined(__AROS__)
      YData = ( UBYTE * )LONGWORDALIGN( YuvImage->y ) ;
      UData = ( UBYTE * )LONGWORDALIGN( YuvImage->u ) ;
      VData = ( UBYTE * )LONGWORDALIGN( YuvImage->v ) ;
#else
        
      YData = ( UBYTE * )((void*)(((IPTR)(YuvImage->y) + (IPTR)3) & ~(IPTR)3));
      UData = ( UBYTE * )((void*)(((IPTR)(YuvImage->u) + (IPTR)3) & ~(IPTR)3));
      VData = ( UBYTE * )((void*)(((IPTR)(YuvImage->v) + (IPTR)3) & ~(IPTR)3));
#endif
      UVUse = UVMaxUse = 0 ;
      switch( YuvImage->type )
      {
        case VHI_YUV_411:
          UVMaxUse = 4 ;
          break ;
        case VHI_YUV_422:
          UVMaxUse = 2 ;
          break ;
        case VHI_YUV_444:
        default:
          UVMaxUse = 1 ;
          break ;
      }
      UVUse = UVMaxUse ;
      for( PosY = 0 ; PosY < YuvImage->height ; PosY++ )
      {
        for( PosX = 0 ; PosX < YuvImage->width ; PosX++ )
        {
          C = ( *( YData++ ) ) - 16 ;
          if( UVUse == UVMaxUse )
          {
            D = ( *( UData++ ) ) - 128 ;
            E = ( *( VData++ ) ) - 128 ;
            UVUse = 0 ;
          }
          UVUse++ ;
          *( Rgb24Data ) = Clip( ( 298 * C           + 409 * E + 128 ) >> 8 ) ;
          Rgb24Data++ ;
          *( Rgb24Data ) = Clip( ( 298 * C - 100 * D - 208 * E + 128 ) >> 8 ) ;
          Rgb24Data++ ;
          *( Rgb24Data ) = Clip( ( 298 * C + 516 * D           + 128 ) >> 8 ) ;
          Rgb24Data++ ;
        }
        YuvProgressHook( MyImageProcessor, PosY, YuvImage->height ) ;
      }
    }
    else
    {  /* rgb 24 image struct not ok */
      LogText( MyContext->c_Log, ERROR_LEVEL, "rgb 24 image struct not ok" ) ;
    }
  }
  else
  {  /* parameter not ok */
  }
  
  return( Rgb24Image ) ;
}
