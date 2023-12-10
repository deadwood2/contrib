/*
  Copyright © 1995-2011, The AROS Development Team. All rights reserved.
  $Id$

  Desc: Regina code for initialization during opening and closing of the library
*/

#define DEBUG 0
#include <exec/types.h>
#include <exec/memory.h>
#include <aros/libcall.h>
#include <aros/debug.h>
#include <proto/exec.h>
#include <proto/alib.h>
#include <proto/dos.h>

#include <stddef.h>
#include "rexx.h"

#include <aros/symbolsets.h>
#include LC_LIBDEFS_FILE

struct MinList *__regina_tsdlist = NULL;

static int InitLib(LIBBASETYPEPTR LIBBASE)
{
   D(bug("Inside Init func of regina.library\n"));

   __regina_semaphorepool = CreatePool(MEMF_PUBLIC, 1024, 256);
  
   __regina_tsdlist = (struct MinList *)AllocPooled (__regina_semaphorepool, sizeof(struct MinList));
   NewList((struct List *)__regina_tsdlist);
    
   return TRUE;
}

static int ExpungeLib(LIBBASETYPEPTR LIBBASE)
{
   D(bug("Inside Expunge func of regina.library\n"));

   DeletePool(__regina_semaphorepool);
   
   return TRUE;
}

ADD2INITLIB(InitLib, 0);
ADD2EXPUNGELIB(ExpungeLib, 0);

const ULONG __aros_rellib_base_CrtBase = 0;
SETRELLIBOFFSET(CrtBase, LIBBASETYPE, crtBase);

struct Library * __modonly_mayget_shareable(struct Library *base);

int _OpenLib(LIBBASETYPEPTR ReginaBase)
{
    struct Library *tmp = NULL;

    ReginaBase->crtBase = OpenLibrary("crt.library", 3L);
    if (ReginaBase->crtBase == NULL) return 0;
    ReginaBase->crtBaseClose = TRUE;

    tmp = __modonly_mayget_shareable(ReginaBase->crtBase);
    if (tmp != ReginaBase->crtBase) /* ReginaBase->crtBase must not be used and... */
    {
        CloseLibrary(ReginaBase->crtBase);
        ReginaBase->crtBaseClose = FALSE;
        if (tmp == NULL) /* ...there is no sharable base available or...*/
            return 0;

        ReginaBase->crtBase = tmp; /* ...there is a sharable base that should be used. */
    }

    return 1;
}

void _CloseLib(LIBBASETYPEPTR ReginaBase)
{
    if (ReginaBase->crtBaseClose)
        CloseLibrary(ReginaBase->crtBase);
}

ADD2OPENLIB(_OpenLib, 0);
ADD2CLOSELIB(_CloseLib, 0);

/*
    This implementation of atexit is different than the definition of atexit
    function due to how libraries work in AROS.

    Under Linux, when an .so file is used by an application, the library's code
    is being shared but the library's data (global, static variables) are COPIED for
    each process. Then, an atexit call inside .so will only operate on COPY of data
    and thus can for example free memory allocated by one process without
    influencing other processes.

    Under AROS, when a .library file is used by an application, library code AND
    library data is shared. This means, an atexit call inside .library which was
    initially coded under Linux cannot be executed when process is finishing
    (for example at CloseLibrary) because such call will most likely free shared
    data which will make other processes crash. The best approximation of atexit
    in case of .library is to call the atexit functions at library expunge/exit.

    TODO: Check atexit() usage and determine best time to call atexit registered
    functions.
*/

static struct exit_list {
    struct exit_list *next;
    void (*func)(void);
} *exit_list = NULL;

int atexit(void (*function)(void))
{
    struct exit_list *el;

    el = malloc(sizeof(*el));
    if (el == NULL)
        return -1;

    el->next = exit_list;
    el->func = function;
    exit_list = el;

    return 0;
}

void __exit_emul(void)
{
    while (exit_list) {
        struct exit_list *el = exit_list->next;

        exit_list->func();
        free(exit_list);
        exit_list = el;
    }
}

ADD2EXIT(__exit_emul, 0);
