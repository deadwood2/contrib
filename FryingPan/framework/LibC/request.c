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

#include <proto/intuition.h>
#include "LibC.h"

int32 request(const char* title, const char* message, const char* gadgets, const IPTR params)
{
#ifdef __amigaos4__
   const IPTR es[] = { 20, 0, (IPTR)title, (IPTR)message, (IPTR)gadgets};
   if (__InternalIntuitionIFace)
      return __InternalIntuitionIFace->EasyRequestArgs(0, (struct EasyStruct*)&es, 0, (void*)params);
#else
   struct EasyStruct es;
   es.es_StructSize = sizeof(es);
   es.es_Flags = 0;
   es.es_Title = title;
   es.es_TextFormat = message;
   es.es_GadgetFormat = gadgets;
   register struct Library *IntuitionBase = __InternalIntuitionBase;
   if (__InternalIntuitionBase)
      return EasyRequestArgs(0, (struct EasyStruct*)&es, 0, (void*)params);
 #endif
   return 0;
};
