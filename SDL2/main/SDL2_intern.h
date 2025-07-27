#ifndef	_SDL2_INTERN_H
#define	_SDL2_INTERN_H

#include <exec/libraries.h>
#include <exec/semaphores.h>
#include <devices/timer.h>

struct SDL2Base
{
    struct Library          _lib;
	struct SDL2Base      	*Parent;
    struct SDL2Base      	*Root;
    //
	struct ExecBase         *MySysBase;
	struct DosLibrary       *MyDOSBase;
	struct IntuitionBase    *MyIntuiBase;
	struct GfxBase          *MyGfxBase;
	struct Library          *MyCyberGfxBase;
	struct Library          *MyKeymapBase;
	struct Library          *MyWorkbenchBase;
	struct Library          *MyIconBase;
	struct Library          *MyMUIMasterBase;
	struct Library          *MyCxBase;
	struct Library          *MyScreenNotifyBase;
	APTR                    aglcontext;
    struct Library          **GLABasePtr;
	void                    **GLAContext;
	unsigned int		    (*MyGetMaximumContextVersion)(struct Library *GLABase);

#if (0)
	union
	{
		struct MinList TaskList;
		struct TaskNode TaskNode;
	} TaskContext;
#endif

	// library management

	struct SignalSemaphore Semaphore;
#if (0)
	APTR ctdtlist;
	APTR last_ctdt;
#endif
};

#endif /* _SDL2_INTERN_H */
