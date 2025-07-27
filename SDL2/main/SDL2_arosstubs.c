/*
    Copyright (C) 2025, The AROS Development Team. All rights reserved.
*/
#include <exec/types.h>
#include <exec/libraries.h>
#include <exec/resident.h>
#include <aros/libcall.h>
#include <aros/asmcall.h>
#include <aros/symbolsets.h>
#include <aros/genmodule.h>
#include <dos/dos.h>

#include "SDL2_libdefs.h"

#include <proto/posixc.h>
#include <proto/stdc.h>

#undef SysBase
#undef OOPBase
#undef UtilityBase

#include <proto/exec.h>
#include <proto/task.h>
#include <proto/alib.h>

#ifndef SDL_h_
#include <SDL2/SDL.h>
#endif

#include "SDL2_arosstubs.h"

AROS_LH1(int, SDL_Init,
         AROS_LHA(Uint32, flags, D0),
         LIBBASETYPEPTR, SDL2Base, 5, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_Init_REAL(flags);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(VOID, SDL_GetVersion,
         AROS_LHA(SDL_version *, version, A0),
         LIBBASETYPEPTR, SDL2Base, 6, SDL2
)
{
    AROS_LIBFUNC_INIT

    SDL_GetVersion_REAL(version);

    AROS_LIBFUNC_EXIT
}

AROS_LH0(SDL_version, SDL_Linked_Version,
         LIBBASETYPEPTR, SDL2Base, 7, SDL2
)
{
    AROS_LIBFUNC_INIT

//    return SDL_Linked_Version();

    AROS_LIBFUNC_EXIT
}

AROS_LH0(VOID, SDL_Quit,
         LIBBASETYPEPTR, SDL2Base, 8, SDL2
)
{
    AROS_LIBFUNC_INIT

    SDL_Quit_REAL();

    AROS_LIBFUNC_EXIT
}

AROS_LH1(int, SDL_InitSubSystem,
         AROS_LHA(Uint32, flags, D0),
         LIBBASETYPEPTR, SDL2Base, 9, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_InitSubSystem_REAL(flags);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(VOID, SDL_QuitSubSystem,
         AROS_LHA(Uint32, flags, D0),
         LIBBASETYPEPTR, SDL2Base, 10, SDL2
)
{
    AROS_LIBFUNC_INIT

    SDL_QuitSubSystem_REAL(flags);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(Uint32, SDL_WasInit,
         AROS_LHA(Uint32, flags, D0),
         LIBBASETYPEPTR, SDL2Base, 11, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_WasInit_REAL(flags);

    AROS_LIBFUNC_EXIT
}

AROS_LH6(SDL_Window*, SDL_CreateWindow,
         AROS_LHA(const char *, title, A0),
         AROS_LHA(int, x, D0),
         AROS_LHA(int, y, D1),
         AROS_LHA(int, w, D2),
         AROS_LHA(int, h, D3),
         AROS_LHA(Uint32, flags, D4),
         LIBBASETYPEPTR, SDL2Base, 12, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_CreateWindow_REAL(title, x, y, w, h, flags);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(VOID, SDL_DestroyWindow,
         AROS_LHA(SDL_Window *, window, A0),
         LIBBASETYPEPTR, SDL2Base, 13, SDL2
)
{
    AROS_LIBFUNC_INIT

    SDL_DestroyWindow_REAL(window);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(VOID, SDL_ShowWindow,
         AROS_LHA(SDL_Window *, window, A0),
         LIBBASETYPEPTR, SDL2Base, 14, SDL2
)
{
    AROS_LIBFUNC_INIT

    SDL_ShowWindow_REAL(window);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(VOID, SDL_HideWindow,
         AROS_LHA(SDL_Window *, window, A0),
         LIBBASETYPEPTR, SDL2Base, 15, SDL2
)
{
    AROS_LIBFUNC_INIT

    SDL_HideWindow_REAL(window);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(VOID, SDL_RaiseWindow,
         AROS_LHA(SDL_Window *, window, A0),
         LIBBASETYPEPTR, SDL2Base, 16, SDL2
)
{
    AROS_LIBFUNC_INIT

    SDL_RaiseWindow_REAL(window);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(VOID, SDL_MaximizeWindow,
         AROS_LHA(SDL_Window *, window, A0),
         LIBBASETYPEPTR, SDL2Base, 17, SDL2
)
{
    AROS_LIBFUNC_INIT

    SDL_MaximizeWindow_REAL(window);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(VOID, SDL_MinimizeWindow,
         AROS_LHA(SDL_Window *, window, A0),
         LIBBASETYPEPTR, SDL2Base, 18, SDL2
)
{
    AROS_LIBFUNC_INIT

    SDL_MinimizeWindow_REAL(window);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(VOID, SDL_RestoreWindow,
         AROS_LHA(SDL_Window *, window, A0),
         LIBBASETYPEPTR, SDL2Base, 19, SDL2
)
{
    AROS_LIBFUNC_INIT

    SDL_RestoreWindow_REAL(window);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(VOID, SDL_SetWindowTitle,
         AROS_LHA(SDL_Window *, window, A0),
         AROS_LHA(const char *, title, A1),
         LIBBASETYPEPTR, SDL2Base, 20, SDL2
)
{
    AROS_LIBFUNC_INIT

    SDL_SetWindowTitle_REAL(window, title);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(const char*, SDL_GetWindowTitle,
         AROS_LHA(SDL_Window *, window, A0),
         LIBBASETYPEPTR, SDL2Base, 21, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_GetWindowTitle_REAL(window);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(int, SDL_GetWindowDisplayIndex,
         AROS_LHA(SDL_Window *, window, A0),
         LIBBASETYPEPTR, SDL2Base, 22, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_GetWindowDisplayIndex_REAL(window);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(VOID, SDL_GetWindowPosition,
         AROS_LHA(SDL_Window *, window, A0),
         AROS_LHA(int *, x, A1),
         AROS_LHA(int *, y, A2),
         LIBBASETYPEPTR, SDL2Base, 23, SDL2
)
{
    AROS_LIBFUNC_INIT

    SDL_GetWindowPosition_REAL(window, x, y);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(VOID, SDL_GetWindowSize,
         AROS_LHA(SDL_Window *, window, A0),
         AROS_LHA(int *, w, A1),
         AROS_LHA(int *, h, A2),
         LIBBASETYPEPTR, SDL2Base, 24, SDL2
)
{
    AROS_LIBFUNC_INIT

    SDL_GetWindowSize_REAL(window, w, h);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(VOID, SDL_SetWindowPosition,
         AROS_LHA(SDL_Window *, window, A0),
         AROS_LHA(int, x, D0),
         AROS_LHA(int, y, D1),
         LIBBASETYPEPTR, SDL2Base, 25, SDL2
)
{
    AROS_LIBFUNC_INIT

    SDL_SetWindowPosition_REAL(window, x, y);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(VOID, SDL_SetWindowSize,
         AROS_LHA(SDL_Window *, window, A0),
         AROS_LHA(int, w, D0),
         AROS_LHA(int, h, D1),
         LIBBASETYPEPTR, SDL2Base, 26, SDL2
)
{
    AROS_LIBFUNC_INIT

    SDL_SetWindowSize_REAL(window, w, h);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(SDL_Surface*, SDL_GetWindowSurface,
         AROS_LHA(SDL_Window *, window, A0),
         LIBBASETYPEPTR, SDL2Base, 27, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_GetWindowSurface_REAL(window);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(int, SDL_UpdateWindowSurface,
         AROS_LHA(SDL_Window *, window, A0),
         LIBBASETYPEPTR, SDL2Base, 28, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_UpdateWindowSurface_REAL(window);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(int, SDL_UpdateWindowSurfaceRects,
         AROS_LHA(SDL_Window *, window, A0),
         AROS_LHA(const SDL_Rect *, rects, A1),
         AROS_LHA(int, numrects, D0),
         LIBBASETYPEPTR, SDL2Base, 29, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_UpdateWindowSurfaceRects_REAL(window, rects, numrects);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(int, SDL_PollEvent,
         AROS_LHA(SDL_Event *, event, A0),
         LIBBASETYPEPTR, SDL2Base, 30, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_PollEvent_REAL(event);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(int, SDL_WaitEvent,
         AROS_LHA(SDL_Event *, event, A0),
         LIBBASETYPEPTR, SDL2Base, 31, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_WaitEvent_REAL(event);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(int, SDL_PeepEvents,
         AROS_LHA(SDL_Event *, events, A0),
         AROS_LHA(int, numevents, D0),
         AROS_LHA(SDL_eventaction, action, D1),
         AROS_LHA(Uint32, minType, D2),
         AROS_LHA(Uint32, maxType, D3),
         LIBBASETYPEPTR, SDL2Base, 32, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_PeepEvents_REAL(events, numevents, action, minType, maxType);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(int, SDL_PushEvent,
         AROS_LHA(SDL_Event *, event, A0),
         LIBBASETYPEPTR, SDL2Base, 33, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_PushEvent_REAL(event);

    AROS_LIBFUNC_EXIT
}

AROS_LH0(VOID, SDL_PumpEvents,
         LIBBASETYPEPTR, SDL2Base, 34, SDL2
)
{
    AROS_LIBFUNC_INIT

    SDL_PumpEvents_REAL();

    AROS_LIBFUNC_EXIT
}

AROS_LH0(Uint32, SDL_GetTicks,
         LIBBASETYPEPTR, SDL2Base, 35, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_GetTicks_REAL();

    AROS_LIBFUNC_EXIT
}

AROS_LH1(VOID, SDL_Delay,
         AROS_LHA(Uint32, ms, D0),
         LIBBASETYPEPTR, SDL2Base, 36, SDL2
)
{
    AROS_LIBFUNC_INIT

    SDL_Delay_REAL(ms);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(SDL_Renderer*, SDL_CreateRenderer,
         AROS_LHA(SDL_Window *, window, A0),
         AROS_LHA(int, index, D0),
         AROS_LHA(Uint32, flags, D1),
         LIBBASETYPEPTR, SDL2Base, 37, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_CreateRenderer_REAL(window, index, flags);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(int, SDL_RenderClear,
         AROS_LHA(SDL_Renderer *, renderer, A0),
         LIBBASETYPEPTR, SDL2Base, 38, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_RenderClear_REAL(renderer);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(VOID, SDL_RenderPresent,
         AROS_LHA(SDL_Renderer *, renderer, A0),
         LIBBASETYPEPTR, SDL2Base, 39, SDL2
)
{
    AROS_LIBFUNC_INIT

    SDL_RenderPresent_REAL(renderer);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(VOID, SDL_DestroyRenderer,
         AROS_LHA(SDL_Renderer *, renderer, A0),
         LIBBASETYPEPTR, SDL2Base, 40, SDL2
)
{
    AROS_LIBFUNC_INIT

    SDL_DestroyRenderer_REAL(renderer);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(int, SDL_SetRenderDrawColor,
         AROS_LHA(SDL_Renderer *, renderer, A0),
         AROS_LHA(Uint8, r, D0),
         AROS_LHA(Uint8, g, D1),
         AROS_LHA(Uint8, b, D2),
         AROS_LHA(Uint8, a, D3),
         LIBBASETYPEPTR, SDL2Base, 41, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_SetRenderDrawColor_REAL(renderer, r, g, b, a);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(int, SDL_RenderDrawLine,
         AROS_LHA(SDL_Renderer *, renderer, A0),
         AROS_LHA(int, x1, D0),
         AROS_LHA(int, y1, D1),
         AROS_LHA(int, x2, D2),
         AROS_LHA(int, y2, D3),
         LIBBASETYPEPTR, SDL2Base, 42, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_RenderDrawLine_REAL(renderer, x1, y1, x2, y2);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(int, SDL_RenderDrawRect,
         AROS_LHA(SDL_Renderer *, renderer, A0),
         AROS_LHA(const SDL_Rect *, rect, A1),
         LIBBASETYPEPTR, SDL2Base, 43, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_RenderDrawRect_REAL(renderer, rect);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(int, SDL_RenderFillRect,
         AROS_LHA(SDL_Renderer *, renderer, A0),
         AROS_LHA(const SDL_Rect *, rect, A1),
         LIBBASETYPEPTR, SDL2Base, 44, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_RenderFillRect_REAL(renderer, rect);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(int, SDL_CreateWindowAndRenderer,
         AROS_LHA(int, width, D0),
         AROS_LHA(int, height, D1),
         AROS_LHA(Uint32, window_flags, D2),
         AROS_LHA(SDL_Window **, window, A0),
         AROS_LHA(SDL_Renderer **, renderer, A1),
         LIBBASETYPEPTR, SDL2Base, 45, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_CreateWindowAndRenderer_REAL(width, height, window_flags, window, renderer);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(int, SDL_SetHint,
         AROS_LHA(const char *, name, A0),
         AROS_LHA(const char *, value, A1),
         LIBBASETYPEPTR, SDL2Base, 46, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_SetHint_REAL(name, value);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(const char*, SDL_GetHint,
         AROS_LHA(const char *, name, A0),
         LIBBASETYPEPTR, SDL2Base, 47, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_GetHint_REAL(name);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(SDL_RWops*, SDL_RWFromFile,
         AROS_LHA(const char *, file, A0),
         AROS_LHA(const char *, mode, A1),
         LIBBASETYPEPTR, SDL2Base, 48, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_RWFromFile_REAL(file, mode);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(SDL_RWops*, SDL_RWFromMem,
         AROS_LHA(void *, mem, A0),
         AROS_LHA(int, size, D0),
         LIBBASETYPEPTR, SDL2Base, 49, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_RWFromMem_REAL(mem, size);

    AROS_LIBFUNC_EXIT
}

AROS_LH0(SDL_RWops*, SDL_AllocRW,
         LIBBASETYPEPTR, SDL2Base, 50, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_AllocRW_REAL();

    AROS_LIBFUNC_EXIT
}

AROS_LH1(VOID, SDL_FreeRW,
         AROS_LHA(SDL_RWops *, area, A0),
         LIBBASETYPEPTR, SDL2Base, 51, SDL2
)
{
    AROS_LIBFUNC_INIT

    SDL_FreeRW_REAL(area);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(SDL_Surface*, SDL_LoadBMP_RW,
         AROS_LHA(SDL_RWops *, src, A0),
         AROS_LHA(int, freesrc, D0),
         LIBBASETYPEPTR, SDL2Base, 52, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_LoadBMP_RW_REAL(src, freesrc);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(int, SDL_SaveBMP_RW,
         AROS_LHA(SDL_Surface *, surface, A0),
         AROS_LHA(SDL_RWops *, dst, A1),
         AROS_LHA(int, freedst, D0),
         LIBBASETYPEPTR, SDL2Base, 53, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_SaveBMP_RW_REAL(surface, dst, freedst);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(SDL_Surface*, SDL_ConvertSurface,
         AROS_LHA(SDL_Surface *, src, A0),
         AROS_LHA(SDL_PixelFormat *, fmt, A1),
         AROS_LHA(Uint32, flags, D0),
         LIBBASETYPEPTR, SDL2Base, 54, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_ConvertSurface_REAL(src, fmt, flags);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(SDL_Surface*, SDL_DisplayFormat,
         AROS_LHA(SDL_Surface *, surface, A0),
         LIBBASETYPEPTR, SDL2Base, 55, SDL2
)
{
    AROS_LIBFUNC_INIT

#if (0)
    return SDL_DisplayFormat_REAL(surface);
#else
    return NULL;
#endif

    AROS_LIBFUNC_EXIT
}

AROS_LH1(SDL_Surface*, SDL_DisplayFormatAlpha,
         AROS_LHA(SDL_Surface *, surface, A0),
         LIBBASETYPEPTR, SDL2Base, 56, SDL2
)
{
    AROS_LIBFUNC_INIT

#if (0)
    return SDL_DisplayFormatAlpha_REAL(surface);
#else
    return NULL;
#endif

    AROS_LIBFUNC_EXIT
}

AROS_LH1(VOID, SDL_FreeSurface,
         AROS_LHA(SDL_Surface *, surface, A0),
         LIBBASETYPEPTR, SDL2Base, 57, SDL2
)
{
    AROS_LIBFUNC_INIT

    SDL_FreeSurface_REAL(surface);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(int, SDL_SetColorKey,
         AROS_LHA(SDL_Surface *, surface, A0),
         AROS_LHA(Uint32, flag, D0),
         AROS_LHA(Uint32, key, D1),
         LIBBASETYPEPTR, SDL2Base, 58, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_SetColorKey_REAL(surface, flag, key);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(int, SDL_SetAlpha,
         AROS_LHA(SDL_Surface *, surface, A0),
         AROS_LHA(Uint32, flag, D0),
         AROS_LHA(Uint8, alpha, D1),
         LIBBASETYPEPTR, SDL2Base, 59, SDL2
)
{
    AROS_LIBFUNC_INIT

#if (0)
    return SDL_SetAlpha_REAL(surface, flag, alpha);
#else
    return 0;
#endif

    AROS_LIBFUNC_EXIT
}

AROS_LH3(int, SDL_FillRect,
         AROS_LHA(SDL_Surface *, dst, A0),
         AROS_LHA(SDL_Rect *, dstrect, A1),
         AROS_LHA(Uint32, color, D0),
         LIBBASETYPEPTR, SDL2Base, 60, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_FillRect_REAL(dst, dstrect, color);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(int, SDL_UpperBlit,
         AROS_LHA(SDL_Surface *, src, A0),
         AROS_LHA(SDL_Rect *, srcrect, A1),
         AROS_LHA(SDL_Surface *, dst, A2),
         AROS_LHA(SDL_Rect *, dstrect, A3),
         LIBBASETYPEPTR, SDL2Base, 61, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_UpperBlit_REAL(src, srcrect, dst, dstrect);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(int, SDL_LowerBlit,
         AROS_LHA(SDL_Surface *, src, A0),
         AROS_LHA(SDL_Rect *, srcrect, A1),
         AROS_LHA(SDL_Surface *, dst, A2),
         AROS_LHA(SDL_Rect *, dstrect, A3),
         LIBBASETYPEPTR, SDL2Base, 62, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_LowerBlit_REAL(src, srcrect, dst, dstrect);

    AROS_LIBFUNC_EXIT
}

AROS_LH0(int, SDL_NumJoysticks,
         LIBBASETYPEPTR, SDL2Base, 63, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_NumJoysticks_REAL();

    AROS_LIBFUNC_EXIT
}

AROS_LH1(const char*, SDL_JoystickName,
         AROS_LHA(SDL_Joystick *, joystick, D0),
         LIBBASETYPEPTR, SDL2Base, 64, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_JoystickName_REAL(joystick);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(SDL_Joystick*, SDL_JoystickOpen,
         AROS_LHA(int, device_index, D0),
         LIBBASETYPEPTR, SDL2Base, 65, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_JoystickOpen_REAL(device_index);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(int, SDL_JoystickOpened,
         AROS_LHA(int, device_index, D0),
         LIBBASETYPEPTR, SDL2Base, 66, SDL2
)
{
    AROS_LIBFUNC_INIT

#if (0)
    return SDL_JoystickOpened_REAL(device_index);
#else
    return 0;
#endif

    AROS_LIBFUNC_EXIT
}

AROS_LH1(int, SDL_JoystickIndex,
         AROS_LHA(SDL_Joystick *, joystick, A0),
         LIBBASETYPEPTR, SDL2Base, 67, SDL2
)
{
    AROS_LIBFUNC_INIT

#if (0)
    return SDL_JoystickIndex_REAL(joystick);
#else
    return 0;
#endif

    AROS_LIBFUNC_EXIT
}

AROS_LH1(int, SDL_JoystickNumAxes,
         AROS_LHA(SDL_Joystick *, joystick, A0),
         LIBBASETYPEPTR, SDL2Base, 68, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_JoystickNumAxes_REAL(joystick);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(int, SDL_JoystickNumBalls,
         AROS_LHA(SDL_Joystick *, joystick, A0),
         LIBBASETYPEPTR, SDL2Base, 69, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_JoystickNumBalls_REAL(joystick);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(int, SDL_JoystickNumHats,
         AROS_LHA(SDL_Joystick *, joystick, A0),
         LIBBASETYPEPTR, SDL2Base, 70, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_JoystickNumHats_REAL(joystick);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH1(int, SDL_JoystickNumButtons,
         AROS_LHA(SDL_Joystick *, joystick, A0),
         LIBBASETYPEPTR, SDL2Base, 71, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_JoystickNumButtons_REAL(joystick);

    AROS_LIBFUNC_EXIT
}

AROS_LH0(VOID, SDL_JoystickUpdate,
         LIBBASETYPEPTR, SDL2Base, 72, SDL2
)
{
    AROS_LIBFUNC_INIT

    SDL_JoystickUpdate_REAL();

    AROS_LIBFUNC_EXIT
}

AROS_LH1(int, SDL_JoystickEventState,
         AROS_LHA(int, state, D0),
         LIBBASETYPEPTR, SDL2Base, 73, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_JoystickEventState_REAL(state);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(Sint16, SDL_JoystickGetAxis,
         AROS_LHA(SDL_Joystick *, joystick, A0),
         AROS_LHA(int, axis, D0),
         LIBBASETYPEPTR, SDL2Base, 74, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_JoystickGetAxis_REAL(joystick, axis);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(Uint8, SDL_JoystickGetHat,
         AROS_LHA(SDL_Joystick *, joystick, A0),
         AROS_LHA(int, hat, D0),
         LIBBASETYPEPTR, SDL2Base, 75, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_JoystickGetHat_REAL(joystick, hat);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(int, SDL_JoystickGetBall,
         AROS_LHA(SDL_Joystick *, joystick, A0),
         AROS_LHA(int, ball, D0),
         AROS_LHA(int *, dx, A1),
         AROS_LHA(int *, dy, A2),
         LIBBASETYPEPTR, SDL2Base, 76, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_JoystickGetBall_REAL(joystick, ball, dx, dy);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(Uint8, SDL_JoystickGetButton,
         AROS_LHA(SDL_Joystick *, joystick, A0),
         AROS_LHA(int, button, D0),
         LIBBASETYPEPTR, SDL2Base, 77, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_JoystickGetButton_REAL(joystick, button);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(VOID, SDL_JoystickClose,
         AROS_LHA(SDL_Joystick *, joystick, A0),
         LIBBASETYPEPTR, SDL2Base, 78, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_JoystickClose_REAL(joystick);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(int, SDL_SetClipboardText,
         AROS_LHA(const char *, text, A0),
         LIBBASETYPEPTR, SDL2Base, 79, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_SetClipboardText_REAL(text);

    AROS_LIBFUNC_EXIT
}

AROS_LH0(char*, SDL_GetClipboardText,
         LIBBASETYPEPTR, SDL2Base, 80, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_GetClipboardText_REAL();

    AROS_LIBFUNC_EXIT
}

AROS_LH0(SDL_bool, SDL_HasClipboardText,
         LIBBASETYPEPTR, SDL2Base, 81, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_HasClipboardText_REAL();

    AROS_LIBFUNC_EXIT
}

AROS_LH5(SDL_AudioSpec*, SDL_LoadWAV_RW,
         AROS_LHA(SDL_RWops *, src, A0),
         AROS_LHA(int, freesrc, D0),
         AROS_LHA(SDL_AudioSpec *, spec, A1),
         AROS_LHA(Uint8 **, audio_buf, A2),
         AROS_LHA(Uint32 *, audio_len, A3),
         LIBBASETYPEPTR, SDL2Base, 82, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_LoadWAV_RW_REAL(src, freesrc, spec, audio_buf, audio_len);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(VOID, SDL_FreeWAV,
         AROS_LHA(Uint8 *, audio_buf, A0),
         LIBBASETYPEPTR, SDL2Base, 83, SDL2
)
{
    AROS_LIBFUNC_INIT

    SDL_FreeWAV_REAL(audio_buf);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(int, SDL_OpenAudio,
         AROS_LHA(SDL_AudioSpec *, desired, A0),
         AROS_LHA(SDL_AudioSpec *, obtained, A1),
         LIBBASETYPEPTR, SDL2Base, 84, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_OpenAudio_REAL(desired, obtained);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(VOID, SDL_PauseAudio,
         AROS_LHA(int, pause_on, D0),
         LIBBASETYPEPTR, SDL2Base, 85, SDL2
)
{
    AROS_LIBFUNC_INIT

    SDL_PauseAudio_REAL(pause_on);

    AROS_LIBFUNC_EXIT
}

AROS_LH0(SDL_AudioStatus, SDL_GetAudioStatus,
         LIBBASETYPEPTR, SDL2Base, 86, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_GetAudioStatus_REAL();

    AROS_LIBFUNC_EXIT
}

AROS_LH0(VOID, SDL_CloseAudio,
         LIBBASETYPEPTR, SDL2Base, 87, SDL2
)
{
    AROS_LIBFUNC_INIT

    SDL_CloseAudio_REAL();

    AROS_LIBFUNC_EXIT
}

AROS_LH3(SDL_TimerID, SDL_AddTimer,
         AROS_LHA(Uint32, interval, D0),
         AROS_LHA(SDL_TimerCallback, callback, A0),
         AROS_LHA(void *, param, A1),
         LIBBASETYPEPTR, SDL2Base, 88, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_AddTimer_REAL(interval, callback, param);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(SDL_bool, SDL_RemoveTimer,
         AROS_LHA(SDL_TimerID, t, D0),
         LIBBASETYPEPTR, SDL2Base, 89, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_RemoveTimer_REAL(t);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(int, SDL_SetTimer,
         AROS_LHA(Uint32, interval, D0),
         AROS_LHA(SDL_TimerCallback, callback, A0),
         LIBBASETYPEPTR, SDL2Base, 90, SDL2
)
{
    AROS_LIBFUNC_INIT

#if (0)
    return SDL_SetTimer_REAL(interval, callback);
#else
    return 0;
#endif

    AROS_LIBFUNC_EXIT
}

AROS_LH1(SDL_sem*, SDL_CreateSemaphore,
         AROS_LHA(Uint32, initial_value, D0),
         LIBBASETYPEPTR, SDL2Base, 91, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_CreateSemaphore_REAL(initial_value);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(VOID, SDL_DestroySemaphore,
         AROS_LHA(SDL_sem *, sem, A0),
         LIBBASETYPEPTR, SDL2Base, 92, SDL2
)
{
    AROS_LIBFUNC_INIT

    SDL_DestroySemaphore_REAL(sem);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(int, SDL_SemWait,
         AROS_LHA(SDL_sem *, sem, A0),
         LIBBASETYPEPTR, SDL2Base, 93, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_SemWait_REAL(sem);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(int, SDL_SemTryWait,
         AROS_LHA(SDL_sem *, sem, A0),
         LIBBASETYPEPTR, SDL2Base, 94, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_SemTryWait_REAL(sem);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(int, SDL_SemWaitTimeout,
         AROS_LHA(SDL_sem *, sem, A0),
         AROS_LHA(Uint32, timeout, D0),
         LIBBASETYPEPTR, SDL2Base, 95, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_SemWaitTimeout_REAL(sem, timeout);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(int, SDL_SemPost,
         AROS_LHA(SDL_sem *, sem, A0),
         LIBBASETYPEPTR, SDL2Base, 96, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_SemPost_REAL(sem);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(Uint32, SDL_SemValue,
         AROS_LHA(SDL_sem *, sem, A0),
         LIBBASETYPEPTR, SDL2Base, 97, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_SemValue_REAL(sem);

    AROS_LIBFUNC_EXIT
}

AROS_LH0(SDL_mutex*, SDL_CreateMutex,
         LIBBASETYPEPTR, SDL2Base, 98, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_CreateMutex_REAL();

    AROS_LIBFUNC_EXIT
}

AROS_LH1(VOID, SDL_DestroyMutex,
         AROS_LHA(SDL_mutex *, mutex, A0),
         LIBBASETYPEPTR, SDL2Base, 99, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_DestroyMutex_REAL(mutex);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(int, SDL_LockMutex,
         AROS_LHA(SDL_mutex *, mutex, A0),
         LIBBASETYPEPTR, SDL2Base, 100, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_LockMutex_REAL(mutex);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(int, SDL_UnlockMutex,
         AROS_LHA(SDL_mutex *, mutex, A0),
         LIBBASETYPEPTR, SDL2Base, 101, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_UnlockMutex_REAL(mutex);

    AROS_LIBFUNC_EXIT
}

AROS_LH0(SDL_cond*, SDL_CreateCond,
         LIBBASETYPEPTR, SDL2Base, 102, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_CreateCond_REAL();

    AROS_LIBFUNC_EXIT
}

AROS_LH1(VOID, SDL_DestroyCond,
         AROS_LHA(SDL_cond *, cond, A0),
         LIBBASETYPEPTR, SDL2Base, 103, SDL2
)
{
    AROS_LIBFUNC_INIT

    SDL_DestroyCond_REAL(cond);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(int, SDL_CondSignal,
         AROS_LHA(SDL_cond *, cond, A0),
         LIBBASETYPEPTR, SDL2Base, 104, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_CondSignal_REAL(cond);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(int, SDL_CondBroadcast,
         AROS_LHA(SDL_cond *, cond, A0),
         LIBBASETYPEPTR, SDL2Base, 105, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_CondBroadcast_REAL(cond);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(int, SDL_CondWait,
         AROS_LHA(SDL_cond *, cond, A0),
         AROS_LHA(SDL_mutex *, mutex, A1),
         LIBBASETYPEPTR, SDL2Base, 106, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_CondWait_REAL(cond, mutex);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(int, SDL_CondWaitTimeout,
         AROS_LHA(SDL_cond *, cond, A0),
         AROS_LHA(SDL_mutex *, mutex, A1),
         AROS_LHA(Uint32, timeout, D0),
         LIBBASETYPEPTR, SDL2Base, 107, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_CondWaitTimeout_REAL(cond, mutex, timeout);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(int, SDL_GetNumAudioDevices,
         AROS_LHA(int, iscapture, D0),
         LIBBASETYPEPTR, SDL2Base, 108, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_GetNumAudioDevices_REAL(iscapture);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(const char*, SDL_GetAudioDeviceName,
         AROS_LHA(int, index, D0),
         AROS_LHA(int, iscapture, D1),
         LIBBASETYPEPTR, SDL2Base, 109, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_GetAudioDeviceName_REAL(index, iscapture);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(SDL_AudioDeviceID, SDL_OpenAudioDevice,
         AROS_LHA(const char *, device, A0),
         AROS_LHA(int, iscapture, D0),
         AROS_LHA(const SDL_AudioSpec *, desired, A1),
         AROS_LHA(SDL_AudioSpec *, obtained, A2),
         AROS_LHA(int, allowed_changes, D1),
         LIBBASETYPEPTR, SDL2Base, 110, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_OpenAudioDevice_REAL(device, iscapture, desired, obtained, allowed_changes);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(VOID, SDL_CloseAudioDevice,
         AROS_LHA(SDL_AudioDeviceID, dev, D0),
         LIBBASETYPEPTR, SDL2Base, 111, SDL2
)
{
    AROS_LIBFUNC_INIT

    SDL_CloseAudioDevice_REAL(dev);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(VOID, SDL_PauseAudioDevice,
         AROS_LHA(SDL_AudioDeviceID, dev, D0),
         AROS_LHA(int, pause_on, D1),
         LIBBASETYPEPTR, SDL2Base, 112, SDL2
)
{
    AROS_LIBFUNC_INIT

    SDL_PauseAudioDevice_REAL(dev, pause_on);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(Uint32, SDL_GetMouseState,
         AROS_LHA(int *, x, A0),
         AROS_LHA(int *, y, A1),
         LIBBASETYPEPTR, SDL2Base, 113, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_GetMouseState_REAL(x,y);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(Uint32, SDL_GetRelativeMouseState,
         AROS_LHA(int *, x, A0),
         AROS_LHA(int *, y, A1),
         LIBBASETYPEPTR, SDL2Base, 114, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_GetRelativeMouseState_REAL(x, y);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(VOID, SDL_WarpMouseInWindow,
         AROS_LHA(SDL_Window *, window, A0),
         AROS_LHA(int, x, D0),
         AROS_LHA(int, y, D1),
         LIBBASETYPEPTR, SDL2Base, 115, SDL2
)
{
    AROS_LIBFUNC_INIT

    SDL_WarpMouseInWindow_REAL(window, x, y);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(int, SDL_SetRelativeMouseMode,
         AROS_LHA(SDL_bool, enabled, D0),
         LIBBASETYPEPTR, SDL2Base, 116, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_SetRelativeMouseMode_REAL(enabled);

    AROS_LIBFUNC_EXIT
}

AROS_LH0(SDL_bool, SDL_GetRelativeMouseMode,
         LIBBASETYPEPTR, SDL2Base, 117, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_GetRelativeMouseMode_REAL();

    AROS_LIBFUNC_EXIT
}

AROS_LH1(int, SDL_ShowCursor,
         AROS_LHA(int, toggle, D0),
         LIBBASETYPEPTR, SDL2Base, 118, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_ShowCursor_REAL(toggle);

    AROS_LIBFUNC_EXIT
}

AROS_LH0(SDL_Cursor *, SDL_GetCursor,
         LIBBASETYPEPTR, SDL2Base, 119, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_GetCursor_REAL();

    AROS_LIBFUNC_EXIT
}

AROS_LH1(VOID, SDL_FreeCursor,
         AROS_LHA(SDL_Cursor *, cursor, A0),
         LIBBASETYPEPTR, SDL2Base, 120, SDL2
)
{
    AROS_LIBFUNC_INIT

    SDL_FreeCursor_REAL(cursor);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(SDL_Cursor*, SDL_CreateSystemCursor,
         AROS_LHA(SDL_SystemCursor, id, D0),
         LIBBASETYPEPTR, SDL2Base, 121, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_CreateSystemCursor_REAL(id);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(SDL_Cursor*, SDL_CreateColorCursor,
         AROS_LHA(SDL_Surface *, surface, A0),
         AROS_LHA(int, hot_x, D0),
         AROS_LHA(int, hot_y, D1),
         LIBBASETYPEPTR, SDL2Base, 122, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_CreateColorCursor_REAL(surface, hot_x, hot_y);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(VOID, SDL_SetCursor,
         AROS_LHA(SDL_Cursor *, cursor, A0),
         LIBBASETYPEPTR, SDL2Base, 123, SDL2
)
{
    AROS_LIBFUNC_INIT

    SDL_SetCursor_REAL(cursor);

    AROS_LIBFUNC_EXIT
}

AROS_LH0(int, SDL_GetNumVideoDisplays,
         LIBBASETYPEPTR, SDL2Base, 124, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_GetNumVideoDisplays_REAL();

    AROS_LIBFUNC_EXIT
}

AROS_LH1(const char*, SDL_GetDisplayName,
         AROS_LHA(int, displayIndex, D0),
         LIBBASETYPEPTR, SDL2Base, 125, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_GetDisplayName_REAL(displayIndex);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(int, SDL_GetDisplayBounds,
         AROS_LHA(int, displayIndex, D0),
         AROS_LHA(SDL_Rect *, rect, A0),
         LIBBASETYPEPTR, SDL2Base, 126, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_GetDisplayBounds_REAL(displayIndex, rect);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(int, SDL_GetDisplayDPI,
         AROS_LHA(int, displayIndex, D0),
         AROS_LHA(float *, ddpi, A0),
         AROS_LHA(float *, hdpi, A1),
         AROS_LHA(float *, vdpi, A2),
         LIBBASETYPEPTR, SDL2Base, 127, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_GetDisplayDPI_REAL(displayIndex, ddpi, hdpi, vdpi);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(int, SDL_SetWindowFullscreen,
         AROS_LHA(SDL_Window *, window, A0),
         AROS_LHA(Uint32, flags, D0),
         LIBBASETYPEPTR, SDL2Base, 128, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_SetWindowFullscreen_REAL(window, flags);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(int, SDL_SetRenderTarget,
         AROS_LHA(SDL_Renderer *, renderer, A0),
         AROS_LHA(SDL_Texture *, texture, A1),
         LIBBASETYPEPTR, SDL2Base, 129, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_SetRenderTarget_REAL(renderer, texture);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(int, SDL_RenderCopy,
         AROS_LHA(SDL_Renderer *, renderer, A0),
         AROS_LHA(SDL_Texture *, texture, A1),
         AROS_LHA(const SDL_Rect *, srcrect, A2),
         AROS_LHA(const SDL_Rect *, dstrect, A3),
         LIBBASETYPEPTR, SDL2Base, 130, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_RenderCopy_REAL(renderer, texture, srcrect, dstrect);

    AROS_LIBFUNC_EXIT
}

AROS_LH7(int, SDL_RenderCopyEx,
         AROS_LHA(SDL_Renderer *, renderer, A0),
         AROS_LHA(SDL_Texture *, texture, A1),
         AROS_LHA(const SDL_Rect *, srcrect, A2),
         AROS_LHA(const SDL_Rect *, dstrect, A3),
         AROS_LHA(const double, angle, D0),
         AROS_LHA(const SDL_Point *, center, A4),
         AROS_LHA(const SDL_RendererFlip, flip, D1),
         LIBBASETYPEPTR, SDL2Base, 131, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_RenderCopyEx_REAL(renderer, texture, srcrect, dstrect, angle, center, flip);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(int, SDL_RenderDrawPoint,
         AROS_LHA(SDL_Renderer *, renderer, A0),
         AROS_LHA(int, x, D0),
         AROS_LHA(int, y, D1),
         LIBBASETYPEPTR, SDL2Base, 132, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_RenderDrawPoint_REAL(renderer, x, y);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(int, SDL_RenderDrawPoints,
         AROS_LHA(SDL_Renderer *, renderer, A0),
         AROS_LHA(const SDL_Point *, points, A1),
         AROS_LHA(int, count, D0),
         LIBBASETYPEPTR, SDL2Base, 133, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_RenderDrawPoints_REAL(renderer, points, count);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(int, SDL_RenderDrawLines,
         AROS_LHA(SDL_Renderer *, renderer, A0),
         AROS_LHA(const SDL_Point *, points, A1),
         AROS_LHA(int, count, D0),
         LIBBASETYPEPTR, SDL2Base, 134, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_RenderDrawLines_REAL(renderer, points, count);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(SDL_Texture*, SDL_CreateTexture,
         AROS_LHA(SDL_Renderer *, renderer, A0),
         AROS_LHA(Uint32, format, D0),
         AROS_LHA(int, access, D1),
         AROS_LHA(int, w, D2),
         AROS_LHA(int, h, D3),
         LIBBASETYPEPTR, SDL2Base, 135, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_CreateTexture_REAL(renderer, format, access, w, h);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(VOID, SDL_DestroyTexture,
         AROS_LHA(SDL_Texture *, texture, A0),
         LIBBASETYPEPTR, SDL2Base, 136, SDL2
)
{
    AROS_LIBFUNC_INIT

    SDL_DestroyTexture_REAL(texture);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(int, SDL_SetRenderDrawBlendMode,
         AROS_LHA(SDL_Renderer *, renderer, A0),
         AROS_LHA(SDL_BlendMode, blendMode, D0),
         LIBBASETYPEPTR, SDL2Base, 137, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_SetRenderDrawBlendMode_REAL(renderer, blendMode);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(SDL_RWops*, SDL_RWFromMem,
         AROS_LHA(void *, mem, A0),
         AROS_LHA(int, size, D0),
         LIBBASETYPEPTR, SDL2Base, 138, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_RWFromMem_REAL(mem, size);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(int, SDL_RenderReadPixels,
         AROS_LHA(SDL_Renderer *, renderer, A0),
         AROS_LHA(const SDL_Rect *, rect, A1),
         AROS_LHA(Uint32, format, D0),
         AROS_LHA(void *, pixels, A2),
         AROS_LHA(int, pitch, D1),
         LIBBASETYPEPTR, SDL2Base, 138, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_RenderReadPixels_REAL(renderer, rect, format, pixels, pitch);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(VOID, SDL_LogSetAllPriority,
         AROS_LHA(SDL_LogPriority, priority, D0),
         LIBBASETYPEPTR, SDL2Base, 140, SDL2
)
{
    AROS_LIBFUNC_INIT

    SDL_LogSetAllPriority_REAL(priority);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(SDL_LogPriority, SDL_LogGetPriority,
         AROS_LHA(int, category, D0),
         LIBBASETYPEPTR, SDL2Base, 141, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_LogGetPriority_REAL(category);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(SDL_RWops*, SDL_RWFromConstMem,
         AROS_LHA(const void *, mem, A0),
         AROS_LHA(int, size, D0),
         LIBBASETYPEPTR, SDL2Base, 143, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_RWFromConstMem_REAL(mem, size);

    AROS_LIBFUNC_EXIT
}

AROS_LH0(VOID, SDL_ClearError,
         LIBBASETYPEPTR, SDL2Base, 144, SDL2
)
{
    AROS_LIBFUNC_INIT

    SDL_ClearError_REAL();

    AROS_LIBFUNC_EXIT
}

AROS_LH0(const char*, SDL_GetError,
         LIBBASETYPEPTR, SDL2Base, 145, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_GetError_REAL();

    AROS_LIBFUNC_EXIT
}

AROS_LH2(int, SDL_SetWindowOpacity,
         AROS_LHA(SDL_Window *, window, A0),
         AROS_LHA(float, opacity, D0),
         LIBBASETYPEPTR, SDL2Base, 147, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_SetWindowOpacity_REAL(window, opacity);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(int, SDL_GetWindowOpacity,
         AROS_LHA(SDL_Window *, window, A0),
         AROS_LHA(float *, opacity, A1),
         LIBBASETYPEPTR, SDL2Base, 148, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_GetWindowOpacity_REAL(window, opacity);

    AROS_LIBFUNC_EXIT
}

AROS_LH0(int, SDL_GetNumVideoDrivers,
         LIBBASETYPEPTR, SDL2Base, 149, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_GetNumVideoDrivers_REAL();

    AROS_LIBFUNC_EXIT
}

AROS_LH1(const char*, SDL_GetVideoDriver,
         AROS_LHA(int, index, D0),
         LIBBASETYPEPTR, SDL2Base, 150, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_GetVideoDriver_REAL(index);

    AROS_LIBFUNC_EXIT
}

AROS_LH0(int, SDL_GetNumRenderDrivers,
         LIBBASETYPEPTR, SDL2Base, 151, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_GetNumRenderDrivers_REAL();

    AROS_LIBFUNC_EXIT
}

AROS_LH2(int, SDL_GetRenderDriverInfo,
         AROS_LHA(int, index, D0),
         AROS_LHA(SDL_RendererInfo *, info, A0),
         LIBBASETYPEPTR, SDL2Base, 152, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_GetRenderDriverInfo_REAL(index, info);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(int, SDL_SetTextureColorMod,
         AROS_LHA(SDL_Texture *, texture, A0),
         AROS_LHA(Uint8, r, D0),
         AROS_LHA(Uint8, g, D1),
         AROS_LHA(Uint8, b, D2),
         LIBBASETYPEPTR, SDL2Base, 153, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_SetTextureColorMod_REAL(texture, r, g, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(int, SDL_SetTextureAlphaMod,
         AROS_LHA(SDL_Texture *, texture, A0),
         AROS_LHA(Uint8, alpha, D0),
         LIBBASETYPEPTR, SDL2Base, 154, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_SetTextureAlphaMod_REAL(texture, alpha);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(int, SDL_SetTextureBlendMode,
         AROS_LHA(SDL_Texture *, texture, A0),
         AROS_LHA(SDL_BlendMode, blendMode, D0),
         LIBBASETYPEPTR, SDL2Base, 155, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_SetTextureBlendMode_REAL(texture, blendMode);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(int, SDL_GetRenderDrawBlendMode,
         AROS_LHA(SDL_Renderer *, renderer, A0),
         AROS_LHA(SDL_BlendMode *, blendMode, A1),
         LIBBASETYPEPTR, SDL2Base, 156, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_GetRenderDrawBlendMode_REAL(renderer, blendMode);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(VOID, SDL_SetWindowBordered,
         AROS_LHA(SDL_Window *, window, A0),
         AROS_LHA(SDL_bool, bordered, D0),
         LIBBASETYPEPTR, SDL2Base, 157, SDL2
)
{
    AROS_LIBFUNC_INIT

    SDL_SetWindowBordered_REAL(window, bordered);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(SDL_GLContext, SDL_GL_CreateContext,
         AROS_LHA(SDL_Window *, window, A0),
         LIBBASETYPEPTR, SDL2Base, 158, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_GL_CreateContext_REAL(window);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(int, SDL_GL_MakeCurrent,
         AROS_LHA(SDL_Window *, window, A0),
         AROS_LHA(SDL_GLContext, context, D0),
         LIBBASETYPEPTR, SDL2Base, 159, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_GL_MakeCurrent_REAL(window, context);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(VOID, SDL_GL_DeleteContext,
         AROS_LHA(SDL_GLContext, context, D0),
         LIBBASETYPEPTR, SDL2Base, 160, SDL2
)
{
    AROS_LIBFUNC_INIT

    SDL_GL_DeleteContext_REAL(context);
    
    AROS_LIBFUNC_EXIT
}

AROS_LH1(int, SDL_GL_SetSwapInterval,
         AROS_LHA(int, interval, D0),
         LIBBASETYPEPTR, SDL2Base, 161, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_GL_SetSwapInterval_REAL(interval);

    AROS_LIBFUNC_EXIT
}

AROS_LH0(int, SDL_GL_GetSwapInterval,
         LIBBASETYPEPTR, SDL2Base, 162, SDL2
)
{
    AROS_LIBFUNC_INIT

    return SDL_GL_GetSwapInterval_REAL();

    AROS_LIBFUNC_EXIT
}

AROS_LH1(VOID, SDL_GL_SwapWindow,
         AROS_LHA(SDL_Window *, window, A0),
         LIBBASETYPEPTR, SDL2Base, 163, SDL2
)
{
    AROS_LIBFUNC_INIT

    SDL_GL_SwapWindow_REAL(window);

    AROS_LIBFUNC_EXIT
}


AROS_LH4(VOID, SDL_LogMessageV,
         AROS_LHA(int, category, D0),
         AROS_LHA(SDL_LogPriority, priority, D1),
         AROS_LHA(const char *, fmt, A0),
         AROS_LHA(va_list, ap, A1),
         LIBBASETYPEPTR, SDL2Base, 139, SDL2
)
{
    AROS_LIBFUNC_INIT

    SDL_LogMessageV_REAL(category, priority, fmt, ap);

    AROS_LIBFUNC_EXIT
}
