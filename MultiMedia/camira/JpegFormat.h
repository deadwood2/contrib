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
** JpegFormat.h
*/


#ifndef _JPEGFORMAT_H
#define _JPEGFORMAT_H


#include "ImageProcessor.h"


/*
** functions of this module
*/
struct vhi_image *CreateJpegFromRgb24( struct ImageProcessor *MyImageProcessor, struct vhi_image *Rgb24Image ) ;
struct vhi_image *CreateRgb24FromJpeg( struct ImageProcessor *MyImageProcessor, struct vhi_image *JpegImage ) ;


#endif  /* !_JPEGFORMAT_H */
