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
** PngFormat.c
*/


#include "PngFormat.h"
#include "Log.h"
#include <proto/exec.h>
#include <png.h>
#include <setjmp.h>


/*
** png progress hook
*/
static ULONG PngProgressHook( struct ImageProcessor *MyImageProcessor, ULONG Current, ULONG Total )
{
  struct Context *MyContext ;
  ULONG Canceled = 0 ;
  
  MyContext = MyImageProcessor->ip_Context ;

  //Dispatcher( MyContext->c_Dispatcher, NONBLOCKING_MODE ) ;  /* dispatch non-blocking */
  
  return( Canceled ) ;
}


/*
** custom png error context
*/
struct PngUser
{
  struct ImageProcessor *pu_ImageProcessor ;
  struct vhi_image *pu_Image ;
  jmp_buf pu_JumpBuffer ;
} ;


/*
** custom png malloc function
*/
static png_voidp user_malloc_fn( png_structp png_ptr, png_alloc_size_t size )
{
  struct PngUser *MyPngUser ;
  struct ImageProcessor *MyImageProcessor ;
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  
  MyPngUser = png_get_mem_ptr( png_ptr ) ;
  MyImageProcessor = MyPngUser->pu_ImageProcessor ;
  MyContext = MyImageProcessor->ip_Context ;
  SysBase = MyContext->c_SysBase ;
  
  return( AllocVec( size, MEMF_ANY ) ) ;
}


/*
** custom png free function
*/
static void user_free_fn( png_structp png_ptr, png_voidp ptr )
{
  struct PngUser *MyPngUser ;
  struct ImageProcessor *MyImageProcessor ;
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  
  MyPngUser = png_get_mem_ptr( png_ptr ) ;
  MyImageProcessor = MyPngUser->pu_ImageProcessor ;
  MyContext = MyImageProcessor->ip_Context ;
  SysBase = MyContext->c_SysBase ;
  
  FreeVec( ptr ) ;
}


/*
** custom png error function
*/
static void user_error_fn( png_structp png_ptr, png_const_charp error_msg )
{
  struct PngUser *MyPngUser ;
  struct ImageProcessor *MyImageProcessor ;
  struct Context *MyContext ;
  
  MyPngUser = ( struct PngUser * )png_get_error_ptr( png_ptr ) ;
  MyImageProcessor = MyPngUser->pu_ImageProcessor ;
  MyContext = MyImageProcessor->ip_Context ;
  
  LogText( MyContext->c_Log, ERROR_LEVEL, ( STRPTR ) error_msg ) ;
  
  longjmp( MyPngUser->pu_JumpBuffer, 1 ) ;
}


/*
** custom png warning function
*/
static void user_warning_fn( png_structp png_ptr, png_const_charp warning_msg )
{
  struct PngUser *MyPngUser ;
  struct ImageProcessor *MyImageProcessor ;
  struct Context *MyContext ;
  
  MyPngUser = ( struct PngUser * )png_get_error_ptr( png_ptr ) ;
  MyImageProcessor = MyPngUser->pu_ImageProcessor ;
  MyContext = MyImageProcessor->ip_Context ;
  
  LogText( MyContext->c_Log, WARNING_LEVEL, ( STRPTR )warning_msg ) ;
}


/*
** custom png write function
*/
static void user_write_fn( png_structp png_write_ptr, png_bytep data, png_size_t length )
{
  struct PngUser *MyPngUser ;
  struct vhi_image *MyImage ;
  ULONG Count ;
  
  MyPngUser = png_get_io_ptr( png_write_ptr ) ;
  MyImage = MyPngUser->pu_Image ;
  
  if( MyImage->width >= MyImage->height + length )
  {  /* new data still fits in the current buffer */
  }
  else
  {  /* extend the buffer */
  }
  
  if( MyImage->chunky )
  {  /* there is a buffer for the new data */
    for( Count = 0 ; Count < length ; Count++ )
    {  /* copy bytes */
      ( ( BYTE * )MyImage->chunky )[ MyImage->height + Count ] = data[ Count ] ;
    }
    MyImage->height += length ;  /* new fill level of this buffer */
  }
}


/*
** custom png flush function
*/
static void user_flush_fn( png_structp png_ptr )
{
}


/*
** create jpeg image based on rgb image data
*/
struct vhi_image *CreatePngFromRgb24( struct ImageProcessor *MyImageProcessor, struct vhi_image *Rgb24Image )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct PngUser *MyPngUser ;
  png_structp png_write_ptr ;
  png_infop png_info_ptr ;
  png_bytep row_ptr ;
  ULONG Count ;
  ULONG Error ;
  struct vhi_image *PngImage ;

  PngImage = NULL ;

  if( ( NULL != MyImageProcessor ) && ( NULL != Rgb24Image ) )
  {  /* requirements ok */
    MyContext = MyImageProcessor->ip_Context ;
    SysBase = MyContext->c_SysBase ;

    PngImage = AllocImage( MyContext, Rgb24Image->width, Rgb24Image->height, VHI_PNG ) ;
    if( NULL != PngImage )
    {  /* png image struct ok */
      PngImage->height = 0 ;  /* used as actual size counter in the custom write function */
      MyPngUser = AllocVec( sizeof( struct PngUser ), MEMF_ANY ) ;
      if( NULL != MyPngUser )
      {  /* png user context ok */
        MyPngUser->pu_ImageProcessor = MyImageProcessor ;
        MyPngUser->pu_Image = PngImage ;
        png_write_ptr = png_create_write_struct_2( PNG_LIBPNG_VER_STRING, 
          MyPngUser, user_error_fn, user_warning_fn,
          MyPngUser, user_malloc_fn, user_free_fn ) ;
        if( NULL != png_write_ptr )
        {  /* png write struct ok */
          png_info_ptr = png_create_info_struct( png_write_ptr ) ;
          if( NULL != png_info_ptr )
          {  /* png info struct ok */
            if( 0 == ( Error = setjmp( MyPngUser->pu_JumpBuffer ) ) )
            {  /* so far no error */
              png_set_write_fn( png_write_ptr, MyPngUser, user_write_fn, user_flush_fn ) ;
              png_set_compression_level( png_write_ptr, ( LONG )MyImageProcessor->ip_CurrentPngCompression ) ;
              png_set_IHDR( png_write_ptr, png_info_ptr, Rgb24Image->width, Rgb24Image->height,
                            8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                            PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT ) ;
              png_write_info( png_write_ptr, png_info_ptr ) ;
              for( Count = 0 ; Count < Rgb24Image->height ; Count++ )
              {  /* write all rows */
                PngProgressHook( MyImageProcessor, Count, Rgb24Image->height ) ;
                row_ptr = Rgb24Image->chunky + Rgb24Image->width * 3 /* RGB format */ * Count  ;
                png_write_row( png_write_ptr, row_ptr ) ;
              }
              png_write_end( png_write_ptr, NULL /* png_info_ptr */ ) ;
              PngImage->width = PngImage->height ;  /* set actual size */
            }
            else
            {  /* critical error occured inside png_*(), error was reported with custom error function */
              FreeImage( MyContext, PngImage ) ;
              PngImage = NULL ;
            }
            png_free_data( png_write_ptr, png_info_ptr, PNG_FREE_ALL, -1 ) ;
          }
          else
          {  /* png info struct not ok */
            LogText( MyContext->c_Log, WARNING_LEVEL, "png info struct not ok" ) ;
            FreeImage( MyContext, PngImage ) ;
            PngImage = NULL ;
          }
          png_destroy_write_struct( &png_write_ptr, ( png_infopp )NULL ) ;
          //png_destroy_write_struct( &png_write_ptr, &png_info_ptr ) ;
        }
        else
        {  /* png write struct not ok */
          LogText( MyContext->c_Log, WARNING_LEVEL, "png write struct not ok" ) ;
          FreeImage( MyContext, PngImage ) ;
          PngImage = NULL ;
        }
        FreeVec( MyPngUser ) ;
      }
      else
      {  /* png error context not ok */
        LogText( MyContext->c_Log, WARNING_LEVEL, "png error context not ok" ) ;
        FreeImage( MyContext, PngImage ) ;
        PngImage = NULL ;
      }
    }
    else
    {  /* png image struct not ok */
      LogText( MyContext->c_Log, WARNING_LEVEL, "png image struct not ok" ) ;
    }
  }
  else
  {  /* requirements not ok */
  }
  
  return( PngImage ) ;
}
