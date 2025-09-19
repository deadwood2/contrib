/*
 * Amiga Generic Set - set of libraries and includes to ease sw development for all Amiga platforms
 * Copyright (C) 2001-2011 Tomasz Wiszkowski Tomasz.Wiszkowski at gmail.com.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef _GENERIC_BOOPSI_H_
#define _GENERIC_BOOPSI_H_

#include "Types.h"
#include <intuition/classes.h>
#include <intuition/classusr.h>

namespace GenNS
{
   class GenericBOOPSI 
   {
   public:
                            GenericBOOPSI();
      virtual               ~GenericBOOPSI();  
      virtual IPTR          DoMtd(Object* pObject, Msg pMsg);
      virtual IPTR          DoSuperMtd(IClass *pClass, Object* pObject, Msg pMsg);
      virtual Object        *NewObj(char* Name, IPTR FirstTag, ...);
      virtual Object        *NewObj(Class* cls, IPTR FirstTag, ...);
      virtual void          DisposeObj(Object *obj);
      virtual void          AddChildObj(Object *parent, Object *child);
      virtual void          RemChildObj(Object *parent, Object *child);
   };
};
#endif

