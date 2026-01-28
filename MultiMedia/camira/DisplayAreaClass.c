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
** DisplayAreaClass.c
*/


#include "DisplayAreaClass.h"
#include "Magic.h"
#include "MainWindow.h"
#include "IdleImage.h"
#include "Settings.h"
#include "Log.h"
#include <proto/exec.h>
#include <proto/muimaster.h>
#include <proto/utility.h>
#include <proto/guigfx.h>
#include <guigfx/guigfx.h>
#include <clib/alib_protos.h>
#include <stdio.h>


/*
** remove the scrollbars from the display area
*/
static ULONG DisplayAreaRemoveProps( Object *MyObject, struct DisplayAreaData *MyData )
{
  struct Context *MyContext ;
  struct Library *MUIMasterBase ;
  struct IntuitionBase *IntuitionBase ;
  struct MUI_RenderInfo *RenderInfo ;

  MyContext = MyData->dad_Context ;
  MUIMasterBase = MyContext->c_MUIMasterBase ;
  IntuitionBase = MyContext->c_IntuitionBase ;
  RenderInfo = muiRenderInfo( MyObject ) ;

  if( DoMethod( MyObject, MUIM_Group_InitChange ) )
  {  /* remove scoller objects */
    DoMethod( MyObject, OM_REMMEMBER, MyData->dad_HorizontalProp ) ;
    DoMethod( MyObject, OM_REMMEMBER, MyData->dad_VerticalProp ) ;
    MUI_DisposeObject( MyData->dad_HorizontalProp ) ;
    MyData->dad_HorizontalProp = NULL ;
    MUI_DisposeObject( MyData->dad_VerticalProp ) ;
    MyData->dad_VerticalProp = NULL ;
    DoMethod( MyObject, MUIM_Group_ExitChange ) ;
  }
  DoMethod( MyData->dad_WindowObject, MUIM_RemoveScrollBorders ) ;

  return( 0 ) ;
}


/*
** add scrollbars to the display area
*/
static ULONG DisplayAreaAddProps( Object *MyObject, struct DisplayAreaData *MyData )
{
  struct Context *MyContext ;
  struct Library *MUIMasterBase ;
  struct IntuitionBase *IntuitionBase ;
  struct MUI_RenderInfo *RenderInfo ;
 
  MyContext = MyData->dad_Context ;
  MUIMasterBase = MyContext->c_MUIMasterBase ;
  IntuitionBase = MyContext->c_IntuitionBase ;
  RenderInfo = muiRenderInfo( MyObject ) ;

  DoMethod( MyData->dad_WindowObject, MUIM_AddScrollBorders ) ;
  if( DoMethod( MyObject, MUIM_Group_InitChange ) )
  {  /* add scroller objects */
    MyData->dad_HorizontalProp = MUI_NewObject( MUIC_Prop,
      MUIA_Prop_UseWinBorder, MUIV_Prop_UseWinBorder_Bottom,
      TAG_END ) ;
    MyData->dad_VerticalProp = MUI_NewObject( MUIC_Prop,
      MUIA_Prop_UseWinBorder, MUIV_Prop_UseWinBorder_Right,
      TAG_END ) ;
    DoMethod( MyObject, OM_ADDMEMBER, MyData->dad_HorizontalProp ) ;
    DoMethod( MyObject, OM_ADDMEMBER, MyData->dad_VerticalProp ) ;
    DoMethod( MyData->dad_HorizontalProp, MUIM_Notify, MUIA_Prop_First, MUIV_EveryTime,
      MyObject, 3, MUIM_Set, MUIA_DisplayHorizontalOffset, MUIV_TriggerValue ) ;
    DoMethod( MyData->dad_VerticalProp, MUIM_Notify, MUIA_Prop_First, MUIV_EveryTime,
      MyObject, 3, MUIM_Set, MUIA_DisplayVerticalOffset, MUIV_TriggerValue ) ;
    DoMethod( MyObject, MUIM_Group_ExitChange ) ;
  }

  return( 0 ) ;
}


#if 0
/*
** set fit in window display area
*/
static ULONG DisplayAreaFitInWindow( Object *MyObject, struct DisplayAreaData *MyData )
{
  struct Context *MyContext ;
  struct Library *MUIMasterBase ;
  struct IntuitionBase *IntuitionBase ;

  MyContext = MyData->dad_Context ;
  MUIMasterBase = MyContext->c_MUIMasterBase ;
  IntuitionBase = MyContext->c_IntuitionBase ;

  if( DoMethod( MyObject, MUIM_Group_InitChange ) )
  {  /* swap the preview area */
    DoMethod( MyObject, OM_REMMEMBER, MyData->dad_DisplayRect ) ;
    MUI_DisposeObject( MyData->dad_DisplayRect ) ;
    MyData->dad_DisplayRect = MUI_NewObject( MUIC_Rectangle,
      TAG_END ) ;
    DoMethod( MyObject, OM_ADDMEMBER, MyData->dad_DisplayRect ) ;
    //DoMethod( MyObject, MUIM_Group_Sort, MyData->dad_LeftSpace, MyData->dad_DisplayRect, MyData->dad_RightSpace, NULL ) ;
    //DoMethod( MyObject, MUIM_Group_Sort, MyData->dad_DisplayRect, MyData->dad_RightSpace, NULL ) ;
    DoMethod( MyObject, MUIM_Group_ExitChange ) ;
  }

  return( 0 ) ;
}


/*
** set fix zoom display area
*/
static ULONG DisplayAreaFixZoom( Object *MyObject, struct DisplayAreaData *MyData )
{
  struct Context *MyContext ;
  struct Library *MUIMasterBase ;
  struct IntuitionBase *IntuitionBase ;
  ULONG ImageWidth, ImageHeight ;

  MyContext = MyData->dad_Context ;
  MUIMasterBase = MyContext->c_MUIMasterBase ;
  IntuitionBase = MyContext->c_IntuitionBase ;

  if( NULL != MyData->dad_CurrentImage )
  {  /* get dimensions from current image */
    ImageWidth = MyData->dad_CurrentImage->width ;
    ImageHeight = MyData->dad_CurrentImage->height ;
//  }
//  else
//  {  /* set some default dimensions */
//    ImageWidth = 320 ;
//    ImageHeight = 240 ;
//  }
  
    if( DoMethod( MyObject, MUIM_Group_InitChange ) )
    {  /* swap the preview area */
      DoMethod( MyObject, OM_REMMEMBER, MyData->dad_DisplayRect ) ;
      MUI_DisposeObject( MyData->dad_DisplayRect ) ;
      MyData->dad_DisplayRect = MUI_NewObject( MUIC_Rectangle,
        MUIA_MaxWidth, ImageWidth * MyData->dad_CurrentZoom / 100,
        MUIA_MaxHeight, ImageHeight * MyData->dad_CurrentZoom / 100,
        TAG_END ) ;
      DoMethod( MyObject, OM_ADDMEMBER, MyData->dad_DisplayRect ) ;
      //DoMethod( MyObject, MUIM_Group_Sort, MyData->dad_LeftSpace, MyData->dad_DisplayRect, MyData->dad_RightSpace, NULL ) ;
      //DoMethod( MyObject, MUIM_Group_Sort, MyData->dad_DisplayRect, MyData->dad_RightSpace, NULL ) ;
      DoMethod( MyObject, MUIM_Group_ExitChange ) ;
    }
  }
  else
  {  /* no current image, so no fixed size area */
  }
  
  return( 0 ) ;
}
#endif


/*
** adjust prop vlaues based on current sizes
*/
static ULONG DisplayAreaUpdateProps( Object *MyObject, struct DisplayAreaData *MyData )
{
  struct Context *MyContext ;
  struct Library *MUIMasterBase ;
  struct IntuitionBase *IntuitionBase ;
  ULONG ImageWidth, ImageHeight ;

  MyContext = MyData->dad_Context ;
  MUIMasterBase = MyContext->c_MUIMasterBase ;
  IntuitionBase = MyContext->c_IntuitionBase ;

  if( NULL != MyData->dad_CurrentImage )
  {  /* get dimensions fro mcurrent image */
    ImageWidth = MyData->dad_CurrentImage->width ;
    ImageHeight = MyData->dad_CurrentImage->height ;
  }
  else
  {  /* set some default dimensions */
    ImageWidth = MyData->dad_CurrentWidth ;
    ImageHeight = MyData->dad_CurrentHeight ;
  }

  if( 0 != MyData->dad_CurrentZoom )
  {  /* do this only in fix zoom mode */
    SetAttrs( MyData->dad_HorizontalProp, 
      MUIA_Prop_Entries, ImageWidth * MyData->dad_CurrentZoom / 100,
      MUIA_Prop_Visible, MyData->dad_CurrentWidth,
      TAG_END ) ;
    SetAttrs( MyData->dad_VerticalProp, 
      MUIA_Prop_Entries, ImageHeight * MyData->dad_CurrentZoom / 100, 
      MUIA_Prop_Visible, MyData->dad_CurrentHeight,
      TAG_END ) ;
    GetAttr( MUIA_Prop_First, MyData->dad_HorizontalProp, &MyData->dad_CurrentHorizontalOffset ) ;
    GetAttr( MUIA_Prop_First, MyData->dad_VerticalProp, &MyData->dad_CurrentVerticalOffset ) ;
  }

  return( 0 ) ;
}


/*
** adjust prop vlaues based on current sizes
*/
static ULONG DisplayAreaUpdateDimensions( Object *MyObject, struct DisplayAreaData *MyData )
{
  struct Context *MyContext ;
  struct Library *MUIMasterBase ;
  struct IntuitionBase *IntuitionBase ;
  ULONG NewWidth, NewHeight ;

  MyContext = MyData->dad_Context ;
  MUIMasterBase = MyContext->c_MUIMasterBase ;
  IntuitionBase = MyContext->c_IntuitionBase ;

  GetAttr( MUIA_LeftEdge, MyData->dad_DisplayRect, &MyData->dad_CurrentX ) ;
  GetAttr( MUIA_TopEdge, MyData->dad_DisplayRect, &MyData->dad_CurrentY ) ;
  GetAttr( MUIA_Width, MyData->dad_DisplayRect, &NewWidth ) ;
  GetAttr( MUIA_Height, MyData->dad_DisplayRect, &NewHeight ) ;
  if( ( MyData->dad_CurrentWidth != NewWidth ) ||
      ( MyData->dad_CurrentHeight != NewHeight ) )
    {  /* size of draw area changed */
      MyData->dad_CurrentWidth = NewWidth ;
      MyData->dad_CurrentHeight = NewHeight ;
      DisplayAreaUpdateProps( MyObject, MyData ) ;
      LogText( MyContext->c_Log, DEBUG_LEVEL, "new display area dimension: %dx%d", NewWidth, NewHeight ) ;
    }

  return( 0 ) ;
}


/*
** display area show
*/
static IPTR DisplayAreaShow( struct IClass *MyIClass, Object *MyObject, struct MUIP_Show *MyMsg )
{
  struct DisplayAreaData *MyData ;
  struct Context *MyContext ;
  static struct Library *GuiGFXBase ;
  struct MUI_RenderInfo *RenderInfo ;
  
  MyData = INST_DATA( MyIClass, MyObject ) ;
  MyContext = MyData->dad_Context ;
  GuiGFXBase = MyContext->c_GuiGFXBase ;
  
  DoSuperMethodA( MyIClass, MyObject, ( Msg )MyMsg ) ;

  RenderInfo = muiRenderInfo( MyObject ) ;
  if( NULL != GuiGFXBase )
  {  /* requirements ok */
    MyData->dad_DrawHandle = ObtainDrawHandle( NULL, RenderInfo->mri_RastPort, RenderInfo->mri_Screen->ViewPort.ColorMap, 
      TAG_END ) ;
  }
  else
  {  /* requirements not ok */
    LogText( MyContext->c_Log, WARNING_LEVEL, "requirements for draw handle not ok" ) ;
    MyData->dad_DrawHandle = NULL ;
  }

  return( TRUE ) ;
}


/*
** display area hide
*/
static IPTR DisplayAreaHide( struct IClass *MyIClass, Object *MyObject, struct MUIP_Hide * MyMsg )
{
  struct DisplayAreaData *MyData ;
  struct Context *MyContext ;
  static struct Library *GuiGFXBase ;
  
  MyData = INST_DATA( MyIClass, MyObject ) ;
  MyContext = MyData->dad_Context ;
  GuiGFXBase = MyContext->c_GuiGFXBase ;

  DoSuperMethodA( MyIClass, MyObject, ( Msg )MyMsg ) ;
  
  if( NULL != MyData->dad_DrawHandle )
  {  /* draw handle needs to be released */
    ReleaseDrawHandle( MyData->dad_DrawHandle ) ;
    MyData->dad_DrawHandle = NULL ;
  }

  return( TRUE ) ;
}


/*
** display area min max
*/
static IPTR DisplayAreaAskMinMax( struct IClass *MyIClass, Object *MyObject, struct MUIP_AskMinMax * MyMsg )
{
  struct DisplayAreaData *MyData ;
  struct Context *MyContext ;
  
  MyData = INST_DATA( MyIClass, MyObject ) ;
  MyContext = MyData->dad_Context ;

  DoSuperMethodA( MyIClass, MyObject, ( Msg )MyMsg ) ;
  
  if( ( NULL != MyData->dad_CurrentImage ) && ( MyData->dad_CurrentZoom ) )
  {  /* only with image set fix zoom */
    MyMsg->MinMaxInfo->MaxWidth = MyData->dad_CurrentImage->width * MyData->dad_CurrentZoom / 100 ;
    MyMsg->MinMaxInfo->MaxHeight = MyData->dad_CurrentImage->height * MyData->dad_CurrentZoom / 100 ;
  }
  else
  {  /* no image or free zoom */
    MyMsg->MinMaxInfo->MaxWidth = MUI_MAXMAX ;
    MyMsg->MinMaxInfo->MaxHeight = MUI_MAXMAX ;
  }

  return( 0 ) ;
}


/*
** gets called during DrawPicture()
*/
static SIPTR REGFUNC DrawPictureProgressHook( REG( a0, struct Hook *MyHook ),
                                             REG( a2, APTR Picture ),
                                             REG( a1, ULONG *Message ) )
{
  struct Context *MyContext ;
  struct DisplayAreaData *MyData ;
  struct ExecBase *SysBase ;

  MyData = MyHook->h_Data ;
  MyContext = MyData->dad_Context ;
  SysBase = MyContext->c_SysBase ;

  //LogText( MyContext->c_Log, DEBUG_LEVEL, "line drawn: %ld", Message[ 1 ]  ) ;
  Dispatcher( MyContext->c_Dispatcher, NONBLOCKING_MODE ) ;  /* dispatch non-blocking */

  return( !( ( TRUE == MyContext->c_ShutdownRequest ) || ( TRUE == MyData->dad_RedrawPending ) ) ) ; 
}


/*
** display area draw
*/
static IPTR DisplayAreaDraw( struct IClass *MyIClass, Object *MyObject, struct MUIP_Draw *MyMsg )
{
  struct DisplayAreaData *MyData ;
  struct Context *MyContext ;
  struct Library *MUIMasterBase ;
  struct Library *GuiGFXBase ;
  struct vhi_image *MyImage ;
  APTR Picture ;
  struct TagItem Tags[ 8 ] ;
  ULONG DrawAreaX, DrawAreaY ;
  ULONG MyZoom ;
  ULONG Success ;
  ULONG DrawFlags ;

  MyData = INST_DATA( MyIClass, MyObject ) ;
  MyContext = MyData->dad_Context ;
  MUIMasterBase = MyContext->c_MUIMasterBase ;
  GuiGFXBase = MyContext->c_GuiGFXBase ;
  MyImage = MyData->dad_CurrentImage ;
  MyZoom = MyData->dad_CurrentZoom ;

  DoSuperMethodA( MyIClass, MyObject, ( Msg )MyMsg ) ;

  if( ( MADF_DRAWOBJECT | MADF_DRAWUPDATE ) & MyMsg->flags )
  {  /* flags ok */
    if( TRUE == MyData->dad_DrawActive )
    {  /* draw is currently active */
      if( MADF_DRAWOBJECT & MyMsg->flags )
      {
        MyData->dad_RedrawPending = TRUE ;
      }
    }
    else
    {  /* draw is currently not active */
      DrawFlags = MyMsg->flags ;
      
      do
      {
        if( MADF_DRAWOBJECT & DrawFlags )
        {  /* also mui request, might be caused by new size */
          //LogText( MyContext->c_Log, INFO_LEVEL, "Draw start: MADF_DRAWOBJECT" ) ;
          DisplayAreaUpdateDimensions( MyObject, MyData ) ;
        }
        if( ( 0 < MyData->dad_CurrentWidth ) && ( 0 < MyData->dad_CurrentHeight ) )
        {  /* display area has some pixels */
          if( NULL == MyImage )
          {  /* use idle image if there is no current image */
            MyImage = MyData->dac_IdleImage ;
            MyZoom = 0 ;
          }
          if( NULL != MyImage )
          {  /* image ok */
            if( ( NULL != MyData->dad_DrawHandle ) )
            {  /* guigfx draw handle ok */
              Picture = MakePicture( MyImage->chunky, ( UWORD )MyImage->width, ( UWORD )MyImage->height,
                GGFX_PixelFormat, PIXFMT_RGB_24,
                TAG_END ) ;
              if( NULL != Picture )
              {  /* guigfx picture ok */
                DrawAreaX = MyData->dad_CurrentX ;
                DrawAreaY = MyData->dad_CurrentY ;
                if( 0 == MyZoom )
                {  /* free zoom mode */
                  if( FREE_RATIO == MyData->dad_CurrentRatio )
                  {  /* free aspect ratio */
                    Tags[ 0 ].ti_Tag = GGFX_DestWidth ;
                    Tags[ 0 ].ti_Data = MyData->dad_CurrentWidth ;
                    Tags[ 1 ].ti_Tag = GGFX_DestHeight ;
                    Tags[ 1 ].ti_Data = MyData->dad_CurrentHeight ;
                  }
                  else
                  {  /* fixed aspect ratio */
                    if( MyData->dad_CurrentWidth <= ( MyData->dad_CurrentHeight * MyImage->width / MyImage->height ) )
                    {  /* use full width and shrink height */
                      Tags[ 0 ].ti_Tag = GGFX_DestWidth ;
                      Tags[ 0 ].ti_Data = MyData->dad_CurrentWidth ;
                      Tags[ 1 ].ti_Tag = GGFX_DestHeight ;
                      Tags[ 1 ].ti_Data = ( MyData->dad_CurrentWidth * MyImage->height / MyImage->width ) ;
                      DrawAreaY += ( MyData->dad_CurrentHeight - Tags[ 1 ].ti_Data ) / 2 ;
                    }
                    else
                    {  /* use full height and shrink width */
                      Tags[ 0 ].ti_Tag = GGFX_DestWidth ;
                      Tags[ 0 ].ti_Data = ( MyData->dad_CurrentHeight * MyImage->width / MyImage->height );
                      DrawAreaX += ( MyData->dad_CurrentWidth - Tags[ 0 ].ti_Data ) / 2 ;
                      Tags[ 1 ].ti_Tag = GGFX_DestHeight ;
                      Tags[ 1 ].ti_Data = MyData->dad_CurrentHeight ;
                    }
                  }
                  Tags[ 2 ].ti_Tag = GGFX_CallBackHook ;
                  Tags[ 2 ].ti_Data = ( IPTR )( &MyData->dad_DrawPictureProgressHook ) ;
                  Tags[ 3 ].ti_Tag = TAG_END ;
                  Tags[ 3 ].ti_Data = 0 ;
                }
                else
                {  /* fixed zoom mode */
                  Tags[ 0 ].ti_Tag = GGFX_DestWidth ;
                  Tags[ 0 ].ti_Data = MyData->dad_CurrentWidth ;
                  Tags[ 1 ].ti_Tag = GGFX_DestHeight ;
                  Tags[ 1 ].ti_Data = MyData->dad_CurrentHeight ;
                  Tags[ 2 ].ti_Tag = GGFX_SourceX ;
                  Tags[ 2 ].ti_Data = MyData->dad_CurrentHorizontalOffset * 100 / MyZoom ;
                  Tags[ 3 ].ti_Tag = GGFX_SourceY ;
                  Tags[ 3 ].ti_Data = MyData->dad_CurrentVerticalOffset * 100 / MyZoom ;
                  Tags[ 4 ].ti_Tag = GGFX_SourceWidth ;
                  Tags[ 4 ].ti_Data = MyData->dad_CurrentWidth * 100 / MyZoom ;
                  Tags[ 5 ].ti_Tag = GGFX_SourceHeight ;
                  Tags[ 5 ].ti_Data = MyData->dad_CurrentHeight * 100 / MyZoom ;
                  Tags[ 6 ].ti_Tag = GGFX_CallBackHook ;
                  Tags[ 6 ].ti_Data = ( IPTR )( &MyData->dad_DrawPictureProgressHook ) ;
                  Tags[ 7 ].ti_Tag = TAG_END ;
                  Tags[ 7 ].ti_Data = 0 ;
                }
                MyData->dad_DrawActive = TRUE ;  /* lock draw */
                Success = DrawPictureA( MyData->dad_DrawHandle, Picture, ( UWORD )DrawAreaX, ( UWORD )DrawAreaY, Tags ) ;
                MyData->dad_DrawActive = FALSE ;
                DeletePicture( Picture ) ;
              }
              else
              {  /* guigfx picture ok */
                LogText( MyContext->c_Log, WARNING_LEVEL, "guigfx picture not ok" ) ;
              }
            }
            else
            {  /* guigfx draw handle not ok */
              LogText( MyContext->c_Log, WARNING_LEVEL, "guigfx draw handle not ok" ) ;
            }
          }
          else
          {  /* no image */
            LogText( MyContext->c_Log, WARNING_LEVEL, "no image to draw" ) ;
          }
        }
        else
        {  /* preview area has no pixels */
          //LogText( MyContext->c_Log, WARNING_LEVEL, "preview area has no pixels" ) ;
        }
        //LogText( MyContext->c_Log, INFO_LEVEL, "Draw end" ) ;
        if( TRUE ==  MyData->dad_RedrawPending )
        {  /* redraw with MADF_DRAWOBJECT only */
          MyData->dad_RedrawPending = FALSE ;
          DrawFlags = MADF_DRAWOBJECT ;
        }
        else
        {  /* no redraw */
          DrawFlags = 0 ;
        }
      } while( DrawFlags ) ;
    }
  }
  else
  {  /* draw flags not ok */
    LogText( MyContext->c_Log, WARNING_LEVEL, "draw flags not ok" ) ;
  }

  return( 0 ) ;  
}


/*
** set display area attributes
*/
static IPTR DisplayAreaSet( struct IClass *MyIClass, Object *MyObject, struct opSet *MyMsg )
{
  struct DisplayAreaData *MyData ;
  struct Context *MyContext ;
  struct Library *MUIMasterBase ;
  struct IntuitionBase *IntuitionBase ;
  struct Library *UtilityBase ;
  struct TagItem *Attribute, *Attributes ;
  ULONG TagCount ;
  struct vhi_image *NewImage ;
  ULONG NewZoom, NewRatio, NewHorizontalOffset, NewVerticalOffset ;
  BOOL NewImageDimensions ;

  MyData = INST_DATA( MyIClass, MyObject ) ;
  MyContext = MyData->dad_Context ;
  MUIMasterBase = MyContext->c_MUIMasterBase ;
  IntuitionBase = MyContext->c_IntuitionBase ;
  UtilityBase = MyContext->c_UtilityBase ;
  TagCount = 0 ;
  Attributes = MyMsg->ops_AttrList;

  while( NULL != ( Attribute = NextTagItem( &Attributes ) ) )
  {
    switch( Attribute->ti_Tag )
    {
      case MUIA_DisplayImage:
        NewImage = ( struct vhi_image * )Attribute->ti_Data ;
        if( NULL != NewImage )
        {  /* there is a new image */
          if( ( NULL != MyData->dad_CurrentImage ) &&
              ( NewImage->width == MyData->dad_CurrentImage->width ) &&
              ( NewImage->height == MyData->dad_CurrentImage->height ) )
          {  /* new image with unchanged dimensions */
            NewImageDimensions = FALSE ;
          }
          else
          {  /* new image with changed dimensions */
            NewImageDimensions = TRUE ;
          }
          MyData->dad_CurrentImage = NewImage ;
          if ( 0 == MyData->dad_CurrentZoom )
          {  /* in free zoom mode */
            if( ( TRUE == NewImageDimensions ) && ( KEEP_RATIO == MyData->dad_CurrentRatio ) )
            {  /* new image may not fully cover last drawn image */
              MUI_Redraw( MyObject, MADF_DRAWOBJECT ) ;
            }
            else
            {  /* new image will cover all parts of last drawn image */
              MUI_Redraw( MyObject, MADF_DRAWUPDATE ) ;
            }
          }
          else
          {  /* in fixed zoom mode */
            if( TRUE == NewImageDimensions )
            {  /* change size of draw area */
              //DisplayAreaFixZoom( MyObject, MyData ) ;
              DoMethod( MyObject, MUIM_Group_InitChange ) ;
              DoMethod( MyObject, MUIM_Group_ExitChange ) ;
              DisplayAreaUpdateProps( MyObject, MyData ) ;
            }
            else
            {  /* keep size of draw area */
              MUI_Redraw( MyObject, MADF_DRAWUPDATE ) ;
            }
          }
        }
        else
        {  /* there is no new immage */
          MyData->dad_CurrentImage = NULL ;
          MUI_Redraw( MyObject, MADF_DRAWOBJECT ) ;
        }
        TagCount++;
        break ;
      case MUIA_DisplayZoom:
        NewZoom = ( IPTR )Attribute->ti_Data ;
        if( ( 0 != MyData->dad_CurrentZoom ) && ( 0 == NewZoom ) )
        {  /* switching from fixed zoom to fit in window */
          MyData->dad_CurrentZoom = NewZoom ;
          DisplayAreaRemoveProps( MyObject, MyData ) ;
          //DisplayAreaFitInWindow( MyObject, MyData ) ;
          //MUI_Redraw( MyObject, MADF_DRAWUPDATE ) ;
        }
        else if( ( 0 == MyData->dad_CurrentZoom ) && ( 0 != NewZoom ) )
        {  /* switching from fit in window to fixed zoom */
          MyData->dad_CurrentZoom = NewZoom ;
          DisplayAreaAddProps( MyObject, MyData ) ;
          //DisplayAreaFixZoom( MyObject, MyData ) ;
          DoMethod( MyObject, MUIM_Group_InitChange ) ;
          DoMethod( MyObject, MUIM_Group_ExitChange ) ;
          DisplayAreaUpdateProps( MyObject, MyData ) ;
          //MUI_Redraw( MyObject, MADF_DRAWUPDATE ) ;
        }
        else if( ( 0 != MyData->dad_CurrentZoom ) && ( 0 != NewZoom ) && ( NewZoom != MyData->dad_CurrentZoom ) )
        {  /* changing zoom during fix zoom mode */
          MyData->dad_CurrentZoom = NewZoom ;
          //DisplayAreaFixZoom( MyObject, MyData ) ;
          DoMethod( MyObject, MUIM_Group_InitChange ) ;
          DoMethod( MyObject, MUIM_Group_ExitChange ) ;
          DisplayAreaUpdateProps( MyObject, MyData ) ;
          //MUI_Redraw( MyObject, MADF_DRAWUPDATE ) ;
        }
        else
        {  /* nothing to do */
        }
        TagCount++;
        break ;
      case MUIA_DisplayRatio:
        NewRatio = ( IPTR )Attribute->ti_Data ;
        if( MyData->dad_CurrentRatio != NewRatio )
        {  /* ratio changed */
          MyData->dad_CurrentRatio = NewRatio ;
          if( 0 == MyData->dad_CurrentZoom )
          {  /* only relevant in free zoom mode */
            //MUI_Redraw( MyObject, MADF_DRAWUPDATE ) ;
            MUI_Redraw( MyObject, MADF_DRAWOBJECT ) ;
          }
          else
          {  /* not relevant in fixed zoom mode */
          }
        }
        break ;
      case MUIA_DisplayHorizontalOffset:
        NewHorizontalOffset = ( IPTR )Attribute->ti_Data ;
        if( MyData->dad_CurrentHorizontalOffset != NewHorizontalOffset )
        {  /* horizontal prop was moved */
          MyData->dad_CurrentHorizontalOffset = NewHorizontalOffset ;
          MUI_Redraw( MyObject, MADF_DRAWUPDATE ) ;
        }
        TagCount++;
        break ;
      case MUIA_DisplayVerticalOffset:
        NewVerticalOffset = ( IPTR )Attribute->ti_Data ;
        if( MyData->dad_CurrentVerticalOffset != NewVerticalOffset )
        {  /* vertical prop was moved */
          MyData->dad_CurrentVerticalOffset = NewVerticalOffset ;
          MUI_Redraw( MyObject, MADF_DRAWUPDATE ) ;
        }
        TagCount++;
        break ;
      case MUIA_DisplayWindowObject:
        MyData->dad_WindowObject = ( Object * )Attribute->ti_Data ;
        TagCount++;
        break ;
      default:  /* some attribute we don't handle here */
        break ;
    }
  }

  TagCount += DoSuperMethodA( MyIClass, MyObject, ( Msg )MyMsg ) ;
  
  return( TagCount ) ;
}


/*
** make a new display area
*/
static IPTR DisplayAreaNew( struct IClass *MyIClass, Object *MyObject, struct opSet *MyMsg )
{
  struct Context *MyContext ;
  struct ExecBase *SysBase ;
  struct Library *MUIMasterBase ;
  struct DisplayAreaData *MyData ;
  IPTR Result ;
  
  MyContext = ( struct Context * )MyIClass->cl_UserData ;
  SysBase = MyContext->c_SysBase ;
  MUIMasterBase = MyContext->c_MUIMasterBase ;

  Result = DoSuperMethodA( MyIClass, MyObject, ( Msg )MyMsg ) ;
  if( 0 != Result )
  {  /* group object created */
    MyObject = ( Object * )Result ;
    MyData = INST_DATA( MyIClass, MyObject ) ;
    MyData->dad_Context = MyContext ;
    
    MyData->dad_DrawPictureProgressHook.h_Entry = ( HOOKFUNC )DrawPictureProgressHook ;
    MyData->dad_DrawPictureProgressHook.h_Data = MyData ;

    MyData->dad_DisplayRect = MUI_NewObject( MUIC_Rectangle,
      TAG_END ) ;
    DoMethod( MyObject, OM_ADDMEMBER, MyData->dad_DisplayRect ) ;
    
    if( ( NULL != MyData->dad_DisplayRect ) )
    {  /* display area complete */
      MyData->dac_Testbild.type = VHI_RGB_24 ;
      MyData->dac_Testbild.width = IdleImage.width ;
      MyData->dac_Testbild.height = IdleImage.height ;
      MyData->dac_Testbild.chunky = ( APTR )IdleImage.pixel_data ;
      MyData->dac_IdleImage = &MyData->dac_Testbild ;
    }
    else
    {  /* display area not complete */
      CoerceMethod( MyIClass, MyObject, OM_DISPOSE ) ;
      Result = 0 ;
    }
  }
  else
  {  /* group object could not be created */
  }
  
  return( Result ) ; 
}


/*
** setup method for the display area
*/
static IPTR DisplayAreaSetup( struct IClass *MyIClass, Object *MyObject, Msg MyMsg )
{
  struct Context *MyContext ;
  struct DisplayAreaData *MyData ;
  IPTR Result ;

  MyData = INST_DATA( MyIClass, MyObject ) ;
  MyContext = MyData->dad_Context ;

  Result = FALSE ;
  if( DoSuperMethodA( MyIClass, MyObject, MyMsg ) )
  {
    MyData->dad_EventHandlerNode.ehn_Priority = 0 ;
    MyData->dad_EventHandlerNode.ehn_Flags = 0 ;
    MyData->dad_EventHandlerNode.ehn_Object = MyObject ;
    MyData->dad_EventHandlerNode.ehn_Class = MyIClass ;
    MyData->dad_EventHandlerNode.ehn_Events = /* IDCMP_NEWSIZE | IDCMP_ACTIVEWINDOW | IDCMP_INACTIVEWINDOW | */ IDCMP_CHANGEWINDOW ;
    //DoMethod( MyData->dad_WindowObject, MUIM_Window_AddEventHandler, &MyData->dad_EventHandlerNode ) ;
    Result = TRUE ;
  }

  return( Result ) ;
}


/*
** cleanup mui method for the display area
*/
static IPTR DisplayAreaCleanup( struct IClass *MyIClass, Object *MyObject, Msg MyMsg )
{
  struct Context *MyContext ;
  struct DisplayAreaData *MyData ;
  IPTR Result ;

  MyData = INST_DATA( MyIClass, MyObject ) ;
  MyContext = MyData->dad_Context ;

  //DoMethod( MyData->dad_WindowObject, MUIM_Window_RemEventHandler, &MyData->dad_EventHandlerNode ) ;

  Result = DoSuperMethodA( MyIClass, MyObject, MyMsg ) ;

  return( Result ) ;
}


/*
** handle event mui method for the display area
*/
static IPTR DisplayAreaHandleEvent( struct IClass *MyIClass, Object *MyObject, struct MUIP_HandleInput *MyMsg )
{
  struct Context *MyContext ;
  struct Library *MUIMasterBase ;
  struct DisplayAreaData *MyData ;
  IPTR Result ;

  MyData = INST_DATA( MyIClass, MyObject ) ;
  MyContext = MyData->dad_Context ;
  MUIMasterBase = MyContext->c_MUIMasterBase ;
  Result = 0 ;
 
  if( NULL != MyMsg->imsg )
  {  /* intuition message */
    switch( MyMsg->imsg->Class )
    {
      case IDCMP_NEWSIZE:  /* not forwarded by MUI ?!? */
        //printf( "IDCMP_NEWSIZE\n" ) ;
        break ;
      case IDCMP_MOUSEBUTTONS:
        //printf( "IDCMP_MOUSBUTTONS\n" ) ;
        break ;
      case IDCMP_ACTIVEWINDOW:
        //printf( "IDCMP_ACTIVEWINDOW\n" ) ;
        break ;
      case IDCMP_INACTIVEWINDOW:
        //printf( "IDCMP_INACTIVEWINDOW\n" ) ;
        break ;
      case IDCMP_CHANGEWINDOW:
        //LogText( MyContext->c_Log, INFO_LEVEL, "IDCMP_CHANGEWINDOW" ) ;
        //if( MADF_DRAWUPDATE == MyData->dad_MagicDrawFlag )
        {  /* only cancel drawing, if triggered not by MUI */
          //MyContext->c_CancelDrawRequest = TRUE ;
          //MUI_Redraw( MyObject, MADF_DRAWOBJECT ) ;
          //Result = MUI_EventHandlerRC_Eat ;
        }
        break ;
      default:
        break ;
    }
  }

  return( Result ) ;
}


/*
** dispatcher for our preview area class
*/
IPTR DisplayAreaDispatcher( REG( a0, struct IClass *MyIClass ), REG( a2, Object *MyObject ), REG( a1, Msg MyMsg ) )
{
  IPTR Result ;
  
  Result = 0 ;
  
  switch( MyMsg->MethodID )
  {
    case OM_NEW:
      //printf( "DisplayAreaNew\n" ) ;
      Result = DisplayAreaNew( MyIClass, MyObject, ( struct opSet * )MyMsg ) ;
      break ;
    case OM_SET:
      //printf( "DisplayAreaSet\n" ) ;
      Result = DisplayAreaSet( MyIClass, MyObject, ( struct opSet * )MyMsg ) ;
      break ;
    case MUIM_Setup:
      //printf( "DisplayAreaSetup\n" ) ;
      Result = DisplayAreaSetup( MyIClass, MyObject, MyMsg ) ;
      break ;
    case MUIM_Show:
      //printf( "DisplayAreaShow\n" ) ;
      Result = DisplayAreaShow( MyIClass, MyObject, ( struct MUIP_Show * )MyMsg ) ;
      break ;
    case MUIM_Draw:
      //printf( "DisplayAreaDraw\n" ) ;  no printf here, seems to crash
      Result = DisplayAreaDraw( MyIClass, MyObject, ( struct MUIP_Draw * )MyMsg ) ;
      break ;
    case MUIM_Hide:
      //printf( "DisplayAreaHide\n" ) ;
      Result = DisplayAreaHide( MyIClass, MyObject, ( struct MUIP_Hide * )MyMsg ) ;
      break ;
    case MUIM_AskMinMax:
      //printf( "DisplayAreaAskMinMax\n" ) ;
      Result = DisplayAreaAskMinMax( MyIClass, MyObject, ( struct MUIP_AskMinMax * )MyMsg ) ;
      break ;
    case MUIM_Cleanup:
      //printf( "DisplayAreaCleanup\n" ) ;
      Result = DisplayAreaCleanup( MyIClass, MyObject, MyMsg ) ;
      break ;
    case MUIM_HandleEvent:
      //printf( "DisplayAreaHandleEvent\n" ) ;
      Result = DisplayAreaHandleEvent( MyIClass, MyObject, ( struct MUIP_HandleInput * )MyMsg ) ;
      break ;
    default:
      //printf( "DisplayAreaDispatcher: 0x%lX\n", MyMsg->MethodID ) ;
      Result = DoSuperMethodA( MyIClass, MyObject, MyMsg ) ;
      break ;
  }

  return( Result ) ;
}
