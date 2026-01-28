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


#include "Stamp.h"
#include "Settings.h"
#include "ImageProcessor.h"
#include "cAMIraLib.h"
#include "Log.h"
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/diskfont.h>
#include <string.h>


/*
** delete stamp context
*/
void DeleteStamp( struct Stamp *OldStamp )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct GfxBase *GfxBase ;
  
  if( NULL != OldStamp )
  {  /* stamp context needs to be freed */
    MyContext = OldStamp->s_Context ;
    SysBase = MyContext->c_SysBase ;
    GfxBase = MyContext->c_GfxBase ;
    
    if( NULL != OldStamp->s_TextFont )
    {  /* text font needs to be closed */
      CloseFont( OldStamp->s_TextFont ) ;
    }
    FreeVec( OldStamp ) ;
  }
}


/*
** configure images tools stuff according to setings
*/
void ConfigureStamp( struct Stamp *MyStamp )
{
  struct Context *MyContext ;
  struct GfxBase *GfxBase ;
  struct Library *DiskfontBase ;
  SIPTR SettingValue ;
  ULONG NewStampEnable ;
  STRPTR NewStampTemplate ;
  STRPTR NewStampFont ;
  ULONG NewStampFontsize ;
  ULONG NewStampX, NewStampY ;
  
  if( ( NULL != MyStamp ) )
  {  /* requirements ok */
    MyContext = MyStamp->s_Context ;
    GfxBase = MyContext->c_GfxBase ;
    DiskfontBase = MyContext->c_DiskfontBase ;
    
    SettingValue = GetSetting( MyContext->c_Settings, StampEnable ) ;
    NewStampEnable = ( ULONG )SettingValue ;
    SettingValue = GetSetting( MyContext->c_Settings, StampText ) ;
    NewStampTemplate = ( STRPTR )SettingValue ;
    SettingValue = GetSetting( MyContext->c_Settings, StampFont ) ;
    NewStampFont = ( STRPTR )SettingValue ;
    SettingValue = GetSetting( MyContext->c_Settings, StampFontsize ) ;
    NewStampFontsize = ( ULONG )SettingValue ;
    SettingValue = GetSetting( MyContext->c_Settings, StampX ) ;
    NewStampX = ( ULONG )SettingValue ;
    SettingValue = GetSetting( MyContext->c_Settings, StampY ) ;
    NewStampY = ( ULONG )SettingValue ;

    if( !( NewStampEnable ) ||
        ( NULL == NewStampTemplate ) ||
        ( 0 != strcmp( MyStamp->s_CurrentStampFont, NewStampFont ) ) ||
        ( MyStamp->s_CurrentStampFontsize != NewStampFontsize ) )
    {  /* no tamplate or font changed */
      if( NULL != MyStamp->s_TextFont )
      {  /* requires closing the font */
        LogText( MyContext->c_Log, DEBUG_LEVEL, "closing stamp font" ) ;
        CloseFont( MyStamp->s_TextFont ) ;
        MyStamp->s_TextFont = NULL ;
        MyStamp->s_CurrentStampFont[ 0 ] = '\0' ;
      }
      MyStamp->s_CurrentStampTemplate[ 0 ] = '\0' ;
    }
    
    if( ( NewStampEnable ) && ( NULL != NewStampTemplate ) )
    {  /* stamp enabled and there is a template */
      if( NULL == MyStamp->s_TextFont )
      {  /* for this we need a font */
        LogText( MyContext->c_Log, DEBUG_LEVEL, "open stamp font: %s/%d", NewStampFont, NewStampFontsize ) ;
        MyStamp->s_TextAttr.ta_Name = NewStampFont ;
        MyStamp->s_TextAttr.ta_YSize = NewStampFontsize ;
        MyStamp->s_TextAttr.ta_Style = FS_NORMAL ;
        MyStamp->s_TextAttr.ta_Flags = FPF_DISKFONT ;
        MyStamp->s_TextFont = OpenDiskFont( &MyStamp->s_TextAttr ) ;
      }
      if( NULL != MyStamp->s_TextFont )
      {  /* text font ok */
        strncpy( MyStamp->s_CurrentStampFont, NewStampFont, S_CURRENTSTAMPFONT_LENGTH ) ;
        MyStamp->s_CurrentStampFontsize = NewStampFontsize ;
        strncpy( MyStamp->s_CurrentStampTemplate, NewStampTemplate, S_CURRENTSTAMPTEMPLATE_LENGTH ) ;
        MyStamp->s_CurrentStampX = NewStampX ;
        MyStamp->s_CurrentStampY = NewStampY ;
      }
      else
      {  /* text font not ok */
        LogText( MyContext->c_Log, WARNING_LEVEL, "stamp font not ok" ) ;
      }
    }
  } 
}


/*
** create context for the stamp
*/
struct Stamp *CreateStamp( struct Context *MyContext )
{
  struct ExecBase *SysBase ;
  struct Stamp *NewStamp ;
  
  NewStamp = NULL ;
  
  if( 1 )
  {  /* requirements seem ok */
    SysBase = MyContext->c_SysBase ;
    
    NewStamp = AllocVec( sizeof( struct Stamp ), MEMF_ANY | MEMF_CLEAR ) ;
    if( NULL != NewStamp )
    {  /* memory for memory manager context ok */
      NewStamp->s_Context = MyContext ;
    }
    else
    {  /* memory for stamp context not ok */
    }
  }
  else
  {  /* requirements not ok */
  }

  return( NewStamp ) ;
}


/*
** does the actual render job
*/
static void RenderStampText( struct vhi_image *MyImage, STRPTR MyText, struct TextFont *MyTextFont, ULONG MyImageX, ULONG MyImageY, ULONG MyRed, ULONG MyGreen, ULONG MyBlue )
{
  ULONG CharSelector, AsciiCode ;
  ULONG GlyphX, GlyphY, GlyphPrefix, GlyphSuffix, GlyphWidth ;
  ULONG GlyphOffset, BitSelector, RowOffset ;
  UBYTE *RowPointer ;
  LONG  ImageOffset ;

  CharSelector = 0 ;
  while( 0 != MyText[ CharSelector ] )
  {  /* one by one char of the stamp text */
    AsciiCode = MyText[ CharSelector ] ;
    if( ( MyTextFont->tf_LoChar > AsciiCode ) || ( MyTextFont->tf_HiChar < AsciiCode ) )
    {  /* ascii code not available in this font, use default char */ 
      AsciiCode = MyTextFont->tf_HiChar ;
    }
    GlyphOffset = ( ( ( ( ( ULONG * )MyTextFont->tf_CharLoc )[ AsciiCode - MyTextFont->tf_LoChar ] ) >> 16 ) & 0xFFFF ) ;
    GlyphWidth = ( ( ( ( ( ULONG * )MyTextFont->tf_CharLoc )[ AsciiCode - MyTextFont->tf_LoChar ] ) >> 0 ) & 0xFFFF ) ;
    if( NULL != MyTextFont->tf_CharKern )
    {  /* kerning table available */
      GlyphPrefix = ( ( WORD * )MyTextFont->tf_CharKern )[ AsciiCode - MyTextFont->tf_LoChar ] ;
    }
    else
    {  /* kerning table not available */
      GlyphPrefix = 0 ;
    }
    if( ( FPF_PROPORTIONAL & MyTextFont->tf_Flags ) && ( NULL != MyTextFont->tf_CharSpace ) )
    {  /* spacing table available */
      GlyphSuffix = ( ( WORD * )MyTextFont->tf_CharSpace )[ AsciiCode - MyTextFont->tf_LoChar ] ;
    }
    else
    {  /* spacing table not available */
      GlyphSuffix = MyTextFont->tf_XSize ;
    }
    GlyphSuffix = GlyphSuffix - GlyphWidth ;
    MyImageX += GlyphPrefix ;
    for( GlyphY = 0 ; GlyphY < MyTextFont->tf_YSize ; GlyphY++ )
    {  /* all rows of this glyph */
      RowPointer = ( ( UBYTE * )MyTextFont->tf_CharData ) + ( GlyphY * MyTextFont->tf_Modulo ) ;
      RowOffset = ( GlyphOffset / 8 ) ;  /* byte offset in the row */
      BitSelector = ( GlyphOffset % 8 ) ;  /* start bit in byte at row offset */
      for( GlyphX = 0 ; GlyphX < GlyphWidth ; GlyphX++ )
      {  /* glyph data of this row */
        if( ( ( MyImageX + GlyphX ) >= 0 ) && ( ( MyImageX + GlyphX ) < MyImage->width ) &&
            ( ( MyImageY + GlyphY ) >= 0 ) && ( ( MyImageY + GlyphY ) < MyImage->height ) )
        {  /* we draw inside the image */
          if( ( 1 << ( 7 - BitSelector ) ) & RowPointer[ RowOffset ] )
          {  /* this bit is set in the char data of the font */
            ImageOffset = ( ( ( MyImageY + GlyphY ) * MyImage->width ) + ( MyImageX + GlyphX ) ) * 3 ;
            ( ( UBYTE * )MyImage->chunky )[ ImageOffset + 0 ] = MyRed ;  /* r */
            ( ( UBYTE * )MyImage->chunky )[ ImageOffset + 1 ] = MyGreen ;  /* g */
            ( ( UBYTE * )MyImage->chunky )[ ImageOffset + 2 ] = MyBlue ;  /* b */
          }
          else
          {  /* this bit is no set in the char data of the font */
          }
        }
        else
        {  /* we are outside the image */
        }
        BitSelector = BitSelector + 1 ;
        if( 8 <= BitSelector )
        {  /* switch to next byte of this row */
          BitSelector = 0 ;
          RowOffset = RowOffset + 1 ;
        }
      }
    }
    MyImageX += GlyphWidth ;
    MyImageX += GlyphSuffix ;
    CharSelector++ ;
  }
}


/*
** print some text in the rgb image
*/
LONG StampImage( struct Stamp *MyStamp, struct ImageBox *MyImageBox )
{
  struct Context *MyContext ;
  LONG Error ;
  struct ExpandData MyExpandData ;
  
  if( ( NULL != MyStamp ) && ( NULL != MyStamp->s_TextFont ) && ( NULL != MyImageBox ) )
  {  /* requirements ok */
    MyContext = MyStamp->s_Context ;
    
    MyImageBox->ib_RgbImage = CommandImageProcessor( MyContext->c_ImageProcessor, IMAGE_COMMAND_CREATE_RGB24, MyImageBox ) ;
    if( NULL != MyImageBox->ib_RgbImage )
    {  /* there is a rgb image in the box */
      if( NULL != MyImageBox->ib_VhiImage )
      {  /* we modify the rgb image, so the present vhi image is invalid */
        if( MyImageBox->ib_VhiImage != MyImageBox->ib_RgbImage )
        {  /* as it was a different format, free it */
          FreeImage( MyContext, MyImageBox->ib_VhiImage ) ;
        }
        MyImageBox->ib_VhiImage = NULL ;
      }
      MyExpandData.ed_ClockData = &MyImageBox->ib_ClockData ;
      MyExpandData.ed_Number = NULL ;
      ExpandString( MyStamp->s_StampText, S_STAMPTEXT_LENGTH, MyStamp->s_CurrentStampTemplate, &MyExpandData ) ;
      //ParseImageBoxString( MyContext, MyStamp->s_CurrentStampTemplate, MyImageBox, MyStamp->s_StampText, STAMPTEXT_LENGTH ) ;
      RenderStampText( MyImageBox->ib_RgbImage, MyStamp->s_StampText, MyStamp->s_TextFont, MyStamp->s_CurrentStampX + 1, MyStamp->s_CurrentStampY + 1, 0, 0, 0 ) ;
      RenderStampText( MyImageBox->ib_RgbImage, MyStamp->s_StampText, MyStamp->s_TextFont, MyStamp->s_CurrentStampX + 0, MyStamp->s_CurrentStampY + 0, 255, 255, 255 ) ;
    }
    else
    {  /* there is no rgb image in the box */
      Error = -2 ;
    }
  }
  else
  {  /* requirements not ok */
    Error = -1 ;
  }
  
  return( Error ) ;
}

