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

struct ExecBase      *SysBase = NULL;
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
struct Library       *ScreenNotifyBase = NULL;
struct Library       *TimerBase = NULL;
struct Library       *LocaleBase = NULL;
struct Library       *SensorsBase = NULL;
struct Library       *IFFParseBase = NULL;
struct Library       *CharsetsBase = NULL;
struct Library       *IConvBase = NULL;
struct Library       *ThreadPoolBase = NULL;
struct Library       *DynLoadBase = NULL;
struct Library       *OpenURLBase = NULL;
struct Library       *GadToolsBase = NULL;
// struct Library		*LowLevelBase = NULL;

struct timerequest   GlobalTimeReq;

/**********************************************************************
	init_system
**********************************************************************/

static void init_system(LIBBASETYPEPTR LIBBASE)
{
#if (0)
	u_int32_t value;

	NewGetSystemAttrsA(&value, sizeof(value), SYSTEMINFOTYPE_PPC_DCACHEL1LINESIZE, NULL);

	if (value < 32)
		value = 32;

	DataL1LineSize = value;

	ULONG Altivec = 0;
	if (NewGetSystemAttrsA(&Altivec,sizeof(Altivec),SYSTEMINFOTYPE_PPC_ALTIVEC,NULL))
    {
		if (Altivec)
		{
			HasAltiVec = 1;
		}
	}
#endif
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

//		sort_ctdt(LIBBASE);
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
#if (0)
	register char *r13;
#endif

	GlobalBase = LIBBASE;
//	SysBase = sysBase;

#if (0)
	LIBBASE->_lib.lib_Node.ln_Pri = -5;
	LIBBASE->_lib.lib_Revision = COMPILE_REVISION;
	asm volatile ("lis %0,__r13_init@ha; addi %0,%0,__r13_init@l" : "=r" (r13));
	LIBBASE->SegList   = SegList;
	LIBBASE->DataSeg   = r13 - R13_OFFSET;
	LIBBASE->DataSize  = __dbsize();
#endif
	LIBBASE->Parent    = NULL;
//	LIBBASE->MySysBase = sysBase;
//	NEWLIST(&LIBBASE->TaskContext.TaskList);

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

static BOOL DeleteLib(LIBBASETYPEPTR LIBBASE, struct ExecBase *SysBase)
{
	BPTR SegList = 0;

	if (LIBBASE->_lib.lib_OpenCnt == 0)
	{
		CloseLibrary((struct Library *)LIBBASE->MyGfxBase);
		CloseLibrary((struct Library *)LIBBASE->MyDOSBase);
		CloseLibrary((struct Library *)LIBBASE->MyIntuiBase);
		CloseLibrary(UtilityBase);
		CloseDevice(&GlobalTimeReq.tr_node);
		return TRUE;
#if (0)
		SegList = LIBBASE->SegList;

		REMOVE(&LIBBASE->_lib.lib_Node);
		FreeMem((APTR)((IPTR)(LIBBASE) - (IPTR)(LIBBASE->_lib.lib_NegSize)), LIBBASE->_lib.lib_NegSize + LIBBASE->_lib.lib_PosSize);
#endif
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
	CloseLibrary(LIBBASE->MyScreenNotifyBase);
	CloseLibrary(LocaleBase);
	CloseLibrary(SensorsBase);
	CloseLibrary(IFFParseBase);
	CloseLibrary(CharsetsBase);
	CloseLibrary(IConvBase);
	CloseLibrary(ThreadPoolBase);
	CloseLibrary(DynLoadBase);
	CloseLibrary(OpenURLBase);
	CloseLibrary(GadToolsBase);

	CyberGfxBase     = LIBBASE->MyCyberGfxBase     = NULL;
	KeymapBase       = LIBBASE->MyKeymapBase       = NULL;
	WorkbenchBase    = LIBBASE->MyWorkbenchBase    = NULL;
	IconBase         = LIBBASE->MyIconBase         = NULL;
    MUIMasterBase    = LIBBASE->MyMUIMasterBase    = NULL;
	CxBase           = LIBBASE->MyCxBase           = NULL;
	ScreenNotifyBase = LIBBASE->MyScreenNotifyBase = NULL;

	LocaleBase = NULL;
	SensorsBase = NULL;
	IFFParseBase = NULL;
	CharsetsBase = NULL;
	IConvBase = NULL;
	ThreadPoolBase = NULL;
	DynLoadBase = NULL;
	OpenURLBase = NULL;
	GadToolsBase = NULL;
}

/**********************************************************************
	SDL2LIB_Expunge
**********************************************************************/

static int SDL2LIB_Expunge(LIBBASETYPEPTR LIBBASE)
{
	LIBBASE->_lib.lib_Flags |= LIBF_DELEXP;
	return DeleteLib(LIBBASE, LIBBASE->MySysBase);
}

/**********************************************************************
	LIB_Close
*********************************************************************/

static void SDL2LIB_Close(LIBBASETYPEPTR LIBBASE)
{
	struct ExecBase *SysBase = LIBBASE->MySysBase;

	if (LIBBASE->Parent)
	{
		struct SDL2Base *ChildBase = LIBBASE;

		if ((--ChildBase->_lib.lib_OpenCnt) > 0)
			return;

		LIBBASE = ChildBase->Parent;

//		REMOVE(&ChildBase->TaskContext.TaskNode.Node);

//		AROS_Cleanup(ChildBase);
//		FreeVecTaskPooled((APTR)((IPTR)(ChildBase) - (IPTR)(ChildBase->_lib.lib_NegSize)));
	}

	ObtainSemaphore(&LIBBASE->Semaphore);

	LIBBASE->_lib.lib_OpenCnt--;

	if (LIBBASE->_lib.lib_OpenCnt == 0)
	{
		UserLibClose(LIBBASE, SysBase);
	}

	ReleaseSemaphore(&LIBBASE->Semaphore);

	if (LIBBASE->_lib.lib_Flags & LIBF_DELEXP)
		DeleteLib(LIBBASE, SysBase);

	return;
}

/**********************************************************************
	SDL2LIB_Open
**********************************************************************/

static int SDL2LIB_Open(LIBBASETYPEPTR LIBBASE)
{
	struct SDL2Base	*newbase, *childbase;
	struct ExecBase *SysBase = LIBBASE->MySysBase;
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
	 && ((ScreenNotifyBase = LIBBASE->MyScreenNotifyBase = (APTR)OpenLibrary("screennotify.library" ,  0)) != NULL)
	 && ((LocaleBase       =                                     OpenLibrary("locale.library"       ,  0)) != NULL)
	 && ((SensorsBase      =                                     OpenLibrary("sensors.library"      , 53)) != NULL)
	 && ((IFFParseBase     =                                     OpenLibrary("iffparse.library"     ,  0)) != NULL)
	 && ((CharsetsBase     =                                     OpenLibrary("charsets.library"     , 53)) != NULL)
	 && ((IConvBase        =                                     OpenLibrary("iconv.library"        ,  0)) != NULL)
	 && ((ThreadPoolBase   =                                     OpenLibrary("threadpool.library"   , 53)) != NULL)
	 && ((DynLoadBase      =                                     OpenLibrary("dynload.library"      ,  0)) != NULL)
	 && ((GadToolsBase	   =									 OpenLibrary("gadtools.library"		,  0)) != NULL)
	 && ((OpenURLBase 	   = 									 OpenLibrary("openurl.library"		,  0)) != NULL))
	{
		return TRUE;
	}
#if (0)
	/* Has this task already opened a child? */
	ForeachNode(&LIBBASE->TaskContext.TaskList, ChildNode)
	{
		if (ChildNode->Task == MyTask)
		{
			/* Yep, return it */
			childbase = (APTR)(((IPTR)ChildNode) - offsetof(struct SDL2Base, TaskContext.TaskNode.Node));
			childbase->Library.lib_Flags &= ~LIBF_DELEXP;
			childbase->Library.lib_OpenCnt++;

			return TRUE;
		}
	}

	childbase  = NULL;
	
	MyBaseSize = LIBBASE->_lib.lib_NegSize + LIBBASE->_lib.lib_PosSize;
	LIBBASE->_lib.lib_Flags &= ~LIBF_DELEXP;
	LIBBASE->_lib.lib_OpenCnt++;

	ObtainSemaphore(&LIBBASE->Semaphore);

	if (LIBBASE->Alloc == 0)
	{
		if (((IntuitionBase    = LIBBASE->MyIntuiBase        = (APTR)OpenLibrary("intuition.library"    , 39)) != NULL)
		 && ((CyberGfxBase     = LIBBASE->MyCyberGfxBase     = (APTR)OpenLibrary("cybergraphics.library", 40)) != NULL)
		 && ((KeymapBase       = LIBBASE->MyKeymapBase       = (APTR)OpenLibrary("keymap.library"       , 36)) != NULL)
		 && ((WorkbenchBase    = LIBBASE->MyWorkbenchBase    = (APTR)OpenLibrary("workbench.library"    ,  0)) != NULL)
		 && ((IconBase         = LIBBASE->MyIconBase         = (APTR)OpenLibrary("icon.library"         ,  0)) != NULL)
		 && ((MUIMasterBase    = LIBBASE->MyMUIMasterBase    = (APTR)OpenLibrary("muimaster.library"    , 19)) != NULL)
		 && ((CxBase           = LIBBASE->MyCxBase           = (APTR)OpenLibrary("commodities.library"  , 37)) != NULL)
		 && ((ScreenNotifyBase = LIBBASE->MyScreenNotifyBase = (APTR)OpenLibrary("screennotify.library" ,  0)) != NULL)
		 && ((LocaleBase       =                                     OpenLibrary("locale.library"       ,  0)) != NULL)
		 && ((SensorsBase      =                                     OpenLibrary("sensors.library"      , 53)) != NULL)
		 && ((IFFParseBase     =                                     OpenLibrary("iffparse.library"     ,  0)) != NULL)
		 && ((CharsetsBase     =                                     OpenLibrary("charsets.library"     , 53)) != NULL)
		 && ((IConvBase        =                                     OpenLibrary("iconv.library"        ,  0)) != NULL)
		 && ((ThreadPoolBase   =                                     OpenLibrary("threadpool.library"   , 53)) != NULL)
         && ((DynLoadBase      =                                     OpenLibrary("dynload.library"      ,  0)) != NULL)
		 && ((GadToolsBase	   =									 OpenLibrary("gadtools.library"		,  0)) != NULL)
		 && ((OpenURLBase 	   = 									 OpenLibrary("openurl.library"		,  0)) != NULL))
		{
			LIBBASE->Alloc = 1;
		}
		else
		{
			goto error;
		}
	}

	if ((newbase = AllocVecTaskPooled(MyBaseSize + LIBBASE->DataSize + 15)) != NULL)
	{
		CopyMem((APTR)((IPTR)LIBBASE - (IPTR)LIBBASE->_lib.lib_NegSize), newbase, MyBaseSize);

		childbase = (APTR)((IPTR)newbase + (IPTR)LIBBASE->_lib.lib_NegSize);

		if (LIBBASE->DataSize)
		{
			char *orig   = LIBBASE->DataSeg;
			LONG *relocs = (LONG *) __datadata_relocs;
			int mem = ((int)newbase + MyBaseSize + 15) & (unsigned int) ~15;

			CopyMem(orig, (char *)mem, LIBBASE->DataSize);

			if (relocs[0] > 0)
			{
				int i, num_relocs = relocs[0];

				for (i = 0, relocs++; i < num_relocs; ++i, ++relocs)
				{
					*(long *)(mem + *relocs) -= (int)orig - mem;
				}
			}

			childbase->DataSeg = (char *)mem + R13_OFFSET;

			if (AROS_Startup(childbase) == 0)
			{
				AROS_Cleanup(childbase);
				FreeVecTaskPooled(newbase);
				childbase = 0;
				goto error;
			}
		}

		childbase->Parent = LIBBASE;
		childbase->Library.lib_OpenCnt = 1;

		/* Register which task opened this child */
		childbase->TaskContext.TaskNode.Task = MyTask;
		ADDTAIL(&LIBBASE->TaskContext.TaskList, &childbase->TaskContext.TaskNode.Node);
	}
	else
	{
error:
		LIBBASE->_lib.lib_OpenCnt--;

		if (LIBBASE->_lib.lib_OpenCnt == 0)
		{
			LIBBASE->Alloc	= 0;
			UserLibClose(LIBBASE, SysBase);
		}
	}

	ReleaseSemaphore(&LIBBASE->Semaphore);

	return (struct Library *)childbase;
#endif
	return FALSE;
}


ADD2INITLIB(SDL2LIB_Init, 0);
ADD2OPENLIB(SDL2LIB_Open, 0);
ADD2CLOSELIB(SDL2LIB_Close, 0);
ADD2EXPUNGELIB(SDL2LIB_Expunge, 0);
