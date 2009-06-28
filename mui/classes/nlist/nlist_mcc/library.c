/***************************************************************************

 NList.mcc - New List MUI Custom Class
 Registered MUI class, Serial Number: 1d51 0x9d510030 to 0x9d5100A0
                                           0x9d5100C0 to 0x9d5100FF

 Copyright (C) 1996-2001 by Gilles Masson
 Copyright (C) 2001-2005 by NList Open Source Team

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 NList classes Support Site:  http://www.sf.net/projects/nlist-classes

 $Id$

***************************************************************************/

/******************************************************************************/
/*                                                                            */
/* includes                                                                   */
/*                                                                            */
/******************************************************************************/

#include <proto/exec.h>
#include <proto/intuition.h>

/******************************************************************************/
/*                                                                            */
/* MCC/MCP name and version                                                   */
/*                                                                            */
/* ATTENTION:  The FIRST LETTER of NAME MUST be UPPERCASE                     */
/*                                                                            */
/******************************************************************************/

#include "private.h"
#include "version.h"
#include "NList_grp.h"

#define VERSION             LIB_VERSION
#define REVISION            LIB_REVISION

#define CLASS               MUIC_NList
#define SUPERCLASS          MUIC_Group

#define	INSTDATA            NLData

#define USERLIBID		        CLASS " " LIB_REV_STRING CPU " (" LIB_DATE ") " LIB_COPYRIGHT
#define MASTERVERSION	      19

#define USEDCLASSESP  used_classesP
static const char *used_classesP[] = { "NListviews.mcp", NULL };

#define	CLASSINIT
#define	CLASSEXPUNGE
#define MIN_STACKSIZE 16384

#if defined(__MORPHOS__)
struct Library *LayersBase = NULL;
struct Library *DiskfontBase = NULL;
struct Library *ConsoleDevice = NULL;
struct Library *IFFParseBase = NULL;
#else
struct Library *LayersBase = NULL;
struct Library *DiskfontBase = NULL;
struct Device *ConsoleDevice = NULL;
struct Library *IFFParseBase = NULL;
#endif

#if defined(__amigaos4__)
struct LayersIFace *ILayers = NULL;
struct DiskfontIFace *IDiskfont = NULL;
struct ConsoleIFace *IConsole = NULL;
struct IFFParseIFace *IIFFParse = NULL;
#endif

static struct IOStdReq ioreq;

/******************************************************************************/
/* define the functions used by the startup code ahead of including mccinit.c */
/******************************************************************************/
static BOOL ClassInit(UNUSED struct Library *base);
static VOID ClassExpunge(UNUSED struct Library *base);

/******************************************************************************/
/* include the lib startup code for the mcc/mcp  (and muimaster inlines)      */
/******************************************************************************/
#include "mccinit.c"

/******************************************************************************/
/* define all implementations of our user functions                           */
/******************************************************************************/
static BOOL ClassInit(UNUSED struct Library *base)
{
  if((LayersBase = OpenLibrary("layers.library", 37L)) &&
     GETINTERFACE(ILayers, struct LayersIFace *, LayersBase))
  {
    if((DiskfontBase = OpenLibrary("diskfont.library", 37L)) &&
       GETINTERFACE(IDiskfont, struct DiskfontIFace *, DiskfontBase))
    {
      ioreq.io_Message.mn_Length = sizeof(ioreq);
      if(!OpenDevice("console.device", -1L, (struct IORequest *)&ioreq, 0L))
      {
        ConsoleDevice = (APTR)ioreq.io_Device;
        if(GETINTERFACE(IConsole, struct ConsoleIFace *, ConsoleDevice))
        {
          if((IFFParseBase = OpenLibrary("iffparse.library", 37L)) &&
             GETINTERFACE(IIFFParse, struct IFFParseIFace *, IFFParseBase))
          {
            if(NGR_Create())
            {
              return(TRUE);
            }

            DROPINTERFACE(IIFFParse);
            CloseLibrary(IFFParseBase);
            IFFParseBase = NULL;
          }

          DROPINTERFACE(IConsole);
        }

        CloseDevice((struct IORequest *)&ioreq);
        ConsoleDevice = NULL;
      }

      DROPINTERFACE(IDiskfont);
      CloseLibrary(DiskfontBase);
      DiskfontBase = NULL;
    }

    DROPINTERFACE(ILayers);
    CloseLibrary(LayersBase);
    LayersBase = NULL;
  }

  return(FALSE);
}


static VOID ClassExpunge(UNUSED struct Library *base)
{
  NGR_Delete();

  if(IFFParseBase)
  {
    DROPINTERFACE(IIFFParse);
    CloseLibrary(IFFParseBase);
    IFFParseBase = NULL;
  }

  if(ConsoleDevice)
  {
    DROPINTERFACE(IConsole);
    CloseDevice((struct IORequest *)&ioreq);
    ConsoleDevice = NULL;
  }

  if(DiskfontBase)
  {
    DROPINTERFACE(IDiskfont);
    CloseLibrary(DiskfontBase);
    DiskfontBase = NULL;
  }

  if(LayersBase)
  {
    DROPINTERFACE(ILayers);
    CloseLibrary(LayersBase);
    LayersBase = NULL;
  }
}
