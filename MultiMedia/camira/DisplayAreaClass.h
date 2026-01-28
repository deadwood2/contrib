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
** DisplayAreaClass.h
*/


#ifndef _DISPLAYAREACLASS_H
#define _DISPLAYAREACLASS_H


#include "Context.h"
#include "CompilerExtensions.h"
#include <intuition/classes.h>
#include <vhi/vhi.h>
#include <utility/hooks.h>


/*
** custom methods of display area class
*/


/*
** custom attributes of display area class
*/
#define MUIA_DisplayImage ( MUIB_DISPLAYAREA + 0x01 )
#define MUIA_DisplayZoom ( MUIB_DISPLAYAREA + 0x02 )
#define MUIA_DisplayRatio ( MUIB_DISPLAYAREA + 0x03 )
#define MUIA_DisplayHorizontalOffset ( MUIB_DISPLAYAREA + 0x04 )
#define MUIA_DisplayVerticalOffset ( MUIB_DISPLAYAREA + 0x05 )
#define MUIA_DisplayWindowObject ( MUIB_DISPLAYAREA + 0x06 )


/*
** private data for the display area class
*/
struct DisplayAreaData
{
  struct Context *dad_Context ;
  struct MUI_EventHandlerNode dad_EventHandlerNode ;
  APTR dad_WindowObject ;
  APTR dad_DisplayRect ;
  APTR dad_HorizontalProp ;
  APTR dad_VerticalProp ;
  APTR dad_DrawHandle ;
  ULONG dad_CurrentZoom ;
  ULONG dad_CurrentRatio ;
  struct vhi_image *dad_CurrentImage ;
  ULONG dad_DrawActive ;
  ULONG dad_RedrawPending ;
  ULONG dad_CurrentX ;
  ULONG dad_CurrentY ;
  ULONG dad_CurrentWidth ;
  ULONG dad_CurrentHeight ;
  ULONG dad_CurrentHorizontalOffset ;
  ULONG dad_CurrentVerticalOffset ;
  struct vhi_image *dac_IdleImage ;  /* our idle image */
  struct vhi_image dac_Testbild ;  /* our idle image */
  struct Hook dad_DrawPictureProgressHook ;  /* DrawPicture() callback */
} ;


/*
** functions of this module
*/
IPTR DisplayAreaDispatcher( REG( a0, struct IClass *MyIClass ), REG( a2, Object *MyObject ), REG( a1, Msg MyMsg ) ) ;


#endif  /* !_DISPLAYAREACLASS_H */
