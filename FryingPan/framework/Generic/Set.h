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

#ifndef _GENERIC_SET_H_
#define _GENERIC_SET_H_

/**
 * \file Set.h
 * \brief Handle an attribute set - a typical flag value handler. \b undocumented yet
 * \details This is the generic Set type class. You may want to use a specializet SetT class instead.
 */

#include "Types.h"

namespace GenNS
{
   /**
    * \class Set
    * \brief This is the generic Set type class. You may want to use a specializet SetT class instead.
    */
   class Set
   {
      uintptr_t                  val;
   public:
      Set();
      Set(uintptr_t val);
      virtual                ~Set();
      virtual Set&            operator << (uintptr_t);
      virtual Set&            operator >> (uintptr_t);
      virtual Set&            operator =  (uintptr_t);
      virtual bool            operator == (uintptr_t);
      virtual bool            ContainsAny(uintptr_t);
      virtual bool            ContainsAll(uintptr_t);
      virtual                 operator uintptr_t ();
   };
};
#endif

