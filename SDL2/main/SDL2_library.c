/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2022 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#include <aros/debug.h>
#include <aros/atomic.h>
#include <aros/symbolsets.h>

#include <libraries/gadtools.h>
#include <proto/intuition.h>

#include <devices/timer.h>
#include <exec/execbase.h>
#include <exec/resident.h>
#include <proto/exec.h>
#include <proto/gadtools.h>

#include <stddef.h>
#include <stdlib.h>

#include LC_LIBDEFS_FILE

#include "SDL2_intern.h"

struct SDL2Base   *GlobalBase = NULL;

struct DosLibrary    *DOSBase = NULL;
struct IntuitionBase *IntuitionBase = NULL;
struct GfxBase       *GfxBase = NULL;
struct Library       *UtilityBase = NULL;
struct Library       *CyberGfxBase = NULL;
struct Library       *KeymapBase = NULL;
struct Library       *WorkbenchBase = NULL;
struct Library       *IconBase = NULL;
struct Library       *MUIMasterBase = NULL;
struct Library       *CxBase = NULL;
struct Library       *TimerBase = NULL;
struct Library       *LocaleBase = NULL;
struct Library       *IFFParseBase = NULL;
struct Library       *OpenURLBase = NULL;
struct Library       *GadToolsBase = NULL;

struct timerequest   GlobalTimeReq;

/**********************************************************************
	init_system
**********************************************************************/

static void init_system(LIBBASETYPEPTR LIBBASE)
{
	// Detect platform/chipset feature availability
}

/**********************************************************************
	init_libs
**********************************************************************/

static int init_libs(LIBBASETYPEPTR LIBBASE)
{
	if ((GfxBase = LIBBASE->MyGfxBase = (APTR)OpenLibrary("graphics.library", 39)) != NULL)
	if ((DOSBase = LIBBASE->MyDOSBase = (APTR)OpenLibrary("dos.library", 36)) != NULL)
	if ((IntuitionBase = LIBBASE->MyIntuiBase = (APTR)OpenLibrary("intuition.library", 39)) != NULL)
	if ((UtilityBase = OpenLibrary("utility.library", 36)) != NULL)
	if (OpenDevice("timer.device", UNIT_MICROHZ, &GlobalTimeReq.tr_node, 0) == 0)
	{
		TimerBase = (struct Library *)GlobalTimeReq.tr_node.io_Device;

		init_system(LIBBASE);

		return 1;
	}

	return 0;
}

/**********************************************************************
	SDL2LIB_Init
**********************************************************************/

static int SDL2LIB_Init(LIBBASETYPEPTR LIBBASE)
{

	GlobalBase = LIBBASE;
	LIBBASE->Parent    = NULL;

	InitSemaphore(&LIBBASE->Semaphore);

	if (init_libs(LIBBASE) == 0)
	{
		return FALSE;
	}

    return TRUE;
}

/**********************************************************************
	DeleteLib
**********************************************************************/

static BOOL DeleteLib(LIBBASETYPEPTR LIBBASE)
{
	if (LIBBASE->_lib.lib_OpenCnt == 0)
	{
		CloseLibrary((struct Library *)LIBBASE->MyGfxBase);
		CloseLibrary((struct Library *)LIBBASE->MyDOSBase);
		CloseLibrary((struct Library *)LIBBASE->MyIntuiBase);
		CloseLibrary(UtilityBase);
		CloseDevice(&GlobalTimeReq.tr_node);
		return TRUE;
	}

	return FALSE;
}

/**********************************************************************
	UserLibClose
**********************************************************************/

static void UserLibClose(LIBBASETYPEPTR LIBBASE, struct ExecBase *SysBase)
{
	CloseLibrary(LIBBASE->MyCyberGfxBase);
	CloseLibrary(LIBBASE->MyKeymapBase);
	CloseLibrary(LIBBASE->MyWorkbenchBase);
	CloseLibrary(LIBBASE->MyIconBase);
	CloseLibrary(LIBBASE->MyMUIMasterBase);
	CloseLibrary(LIBBASE->MyCxBase);
	CloseLibrary(LocaleBase);
	CloseLibrary(IFFParseBase);
	CloseLibrary(OpenURLBase);
	CloseLibrary(GadToolsBase);

	CyberGfxBase     = LIBBASE->MyCyberGfxBase     = NULL;
	KeymapBase       = LIBBASE->MyKeymapBase       = NULL;
	WorkbenchBase    = LIBBASE->MyWorkbenchBase    = NULL;
	IconBase         = LIBBASE->MyIconBase         = NULL;
    MUIMasterBase    = LIBBASE->MyMUIMasterBase    = NULL;
	CxBase           = LIBBASE->MyCxBase           = NULL;

	LocaleBase = NULL;
	IFFParseBase = NULL;
	OpenURLBase = NULL;
	GadToolsBase = NULL;
}

/**********************************************************************
	SDL2LIB_Expunge
**********************************************************************/

static int SDL2LIB_Expunge(LIBBASETYPEPTR LIBBASE)
{
	LIBBASE->_lib.lib_Flags |= LIBF_DELEXP;
	return DeleteLib(LIBBASE);
}

/**********************************************************************
	LIB_Close
*********************************************************************/

static void SDL2LIB_Close(LIBBASETYPEPTR LIBBASE)
{
	ObtainSemaphore(&LIBBASE->Semaphore);

	LIBBASE->_lib.lib_OpenCnt--;

	if (LIBBASE->_lib.lib_OpenCnt == 0)
	{
		UserLibClose(LIBBASE, SysBase);
	}

	ReleaseSemaphore(&LIBBASE->Semaphore);

	if (LIBBASE->_lib.lib_Flags & LIBF_DELEXP)
		DeleteLib(LIBBASE);

	return;
}

/**********************************************************************
	SDL2LIB_Open
**********************************************************************/

static int SDL2LIB_Open(LIBBASETYPEPTR LIBBASE)
{
	struct SDL2Base	*newbase, *childbase;
	struct Task *MyTask = FindTask(NULL);
	struct TaskNode *ChildNode;
	IPTR MyBaseSize;

	if (((IntuitionBase    = LIBBASE->MyIntuiBase        = (APTR)OpenLibrary("intuition.library"    , 39)) != NULL)
	 && ((CyberGfxBase     = LIBBASE->MyCyberGfxBase     = (APTR)OpenLibrary("cybergraphics.library", 40)) != NULL)
	 && ((KeymapBase       = LIBBASE->MyKeymapBase       = (APTR)OpenLibrary("keymap.library"       , 36)) != NULL)
	 && ((WorkbenchBase    = LIBBASE->MyWorkbenchBase    = (APTR)OpenLibrary("workbench.library"    ,  0)) != NULL)
	 && ((IconBase         = LIBBASE->MyIconBase         = (APTR)OpenLibrary("icon.library"         ,  0)) != NULL)
	 && ((MUIMasterBase    = LIBBASE->MyMUIMasterBase    = (APTR)OpenLibrary("muimaster.library"    , 19)) != NULL)
	 && ((CxBase           = LIBBASE->MyCxBase           = (APTR)OpenLibrary("commodities.library"  , 37)) != NULL)
	 && ((LocaleBase       =                                     OpenLibrary("locale.library"       ,  0)) != NULL)
	 && ((IFFParseBase     =                                     OpenLibrary("iffparse.library"     ,  0)) != NULL)
	 && ((GadToolsBase	   =									 OpenLibrary("gadtools.library"		,  0)) != NULL)
	 && ((OpenURLBase 	   = 									 OpenLibrary("openurl.library"		,  0)) != NULL))
	{
		return TRUE;
	}
	return FALSE;
}


ADD2INITLIB(SDL2LIB_Init, 0);
ADD2OPENLIB(SDL2LIB_Open, 0);
ADD2CLOSELIB(SDL2LIB_Close, 0);
ADD2EXPUNGELIB(SDL2LIB_Expunge, 0);
