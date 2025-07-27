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

extern int SDL_Init_REAL(Uint32 flags);
extern void SDL_GetVersion_REAL(SDL_version * ver);
extern SDL_version SDL_Linked_Version_REAL();
extern void SDL_Quit_REAL();
extern int SDL_InitSubSystem_REAL(Uint32 flags);
extern void SDL_QuitSubSystem_REAL(Uint32 flags);
extern Uint32 SDL_WasInit_REAL(Uint32 flags);
extern SDL_Window *SDL_CreateWindow_REAL(const char *title, int x, int y, int w, int h, Uint32 flags);
extern void SDL_DestroyWindow_REAL(SDL_Window *window);
extern void SDL_ShowWindow_REAL(SDL_Window *window);
extern void SDL_HideWindow_REAL(SDL_Window *window);
extern void SDL_RaiseWindow_REAL(SDL_Window *window);
extern void SDL_MaximizeWindow_REAL(SDL_Window *window);
extern void SDL_MinimizeWindow_REAL(SDL_Window *window);
extern void SDL_RestoreWindow_REAL(SDL_Window *window);
extern void SDL_SetWindowTitle_REAL(SDL_Window *window, const char *title);
extern const char *SDL_GetWindowTitle_REAL(SDL_Window *window);
extern int SDL_GetWindowDisplayIndex_REAL(SDL_Window *window);
extern void SDL_GetWindowPosition_REAL(SDL_Window *window, int *x, int *y);
extern void SDL_GetWindowSize_REAL(SDL_Window *window, int *w, int *h);
extern void SDL_SetWindowPosition_REAL(SDL_Window *window, int x, int y);
extern void SDL_SetWindowSize_REAL(SDL_Window *window, int w, int h);
extern SDL_Surface *SDL_GetWindowSurface_REAL(SDL_Window *window);
extern int SDL_UpdateWindowSurface_REAL(SDL_Window *window);
extern int SDL_UpdateWindowSurfaceRects_REAL(SDL_Window *window, const SDL_Rect *rects, int numrects);
extern int SDL_PollEvent_REAL(SDL_Event *event);
extern int SDL_WaitEvent_REAL(SDL_Event *event);
extern int SDL_PeepEvents_REAL(SDL_Event * events, int numevents, SDL_eventaction act, Uint32 minType, Uint32 maxType);
extern int SDL_PushEvent_REAL(SDL_Event *event);
extern void SDL_PumpEvents_REAL();
extern Uint32 SDL_GetTicks_REAL();
extern void SDL_Delay_REAL(Uint32 ms);
extern SDL_Renderer *SDL_CreateRenderer_REAL(SDL_Window *window, int index, Uint32 flags);
extern int SDL_RenderClear_REAL(SDL_Renderer *renderer);
extern void SDL_RenderPresent_REAL(SDL_Renderer *renderer);
extern void SDL_DestroyRenderer_REAL(SDL_Renderer *renderer);
extern int SDL_SetRenderDrawColor_REAL(SDL_Renderer *renderer, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
extern int SDL_RenderDrawLine_REAL(SDL_Renderer *renderer, int x1, int y1, int x2, int y2);
extern int SDL_RenderDrawRect_REAL(SDL_Renderer *renderer, const SDL_Rect *rect);
extern int SDL_RenderFillRect_REAL(SDL_Renderer *renderer, const SDL_Rect *rect);
extern int SDL_CreateWindowAndRenderer_REAL(int width, int height, Uint32 window_flags, SDL_Window **window, SDL_Renderer **renderer);
extern SDL_bool SDL_SetHint_REAL(const char *name, const char *value);
extern const char *SDL_GetHint_REAL(const char *name);
extern SDL_RWops *SDL_RWFromFile_REAL(const char *file, const char *mode);
extern SDL_RWops *SDL_RWFromMem_REAL(void *mem, int size);
extern SDL_RWops *SDL_AllocRW_REAL();
extern void SDL_FreeRW_REAL(SDL_RWops *area);
extern SDL_Surface *SDL_LoadBMP_RW_REAL(SDL_RWops *src, int freesrc);
extern int SDL_SaveBMP_RW_REAL(SDL_Surface *surface, SDL_RWops *dst, int freedst);
extern SDL_Surface *SDL_ConvertSurface_REAL(SDL_Surface * src, const SDL_PixelFormat * fmt, Uint32 flags);
extern SDL_Surface *SDL_DisplayFormat_REAL(SDL_Surface *surface);
extern SDL_Surface *SDL_DisplayFormatAlpha_REAL(SDL_Surface *surface);
extern void SDL_FreeSurface_REAL(SDL_Surface *surface);
extern int SDL_SetColorKey_REAL(SDL_Surface * surface, int flag, Uint32 key);
extern int SDL_SetAlpha_REAL(SDL_Surface *surface, Uint32 flag, Uint8 alpha);
extern int SDL_FillRect_REAL(SDL_Surface * dst, const SDL_Rect * rect, Uint32 color);
extern int SDL_UpperBlit_REAL(SDL_Surface * src, const SDL_Rect * srcrect, SDL_Surface * dst, SDL_Rect * dstrect);
extern int SDL_LowerBlit_REAL(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect);
extern int SDL_NumJoysticks_REAL();
extern const char *SDL_JoystickName_REAL(SDL_Joystick *joystick);
extern SDL_Joystick *SDL_JoystickOpen_REAL(int device_index);
extern int SDL_JoystickOpened_REAL(int device_index);
extern int SDL_JoystickIndex_REAL(SDL_Joystick *joystick);
extern int SDL_JoystickNumAxes_REAL(SDL_Joystick *joystick);
extern int SDL_JoystickNumBalls_REAL(SDL_Joystick *joystick);
extern int SDL_JoystickNumHats_REAL(SDL_Joystick *joystick);
extern int SDL_JoystickNumButtons_REAL(SDL_Joystick *joystick);
extern void SDL_JoystickUpdate_REAL();
extern int SDL_JoystickEventState_REAL(int state);
extern Sint16 SDL_JoystickGetAxis_REAL(SDL_Joystick *joystick, int axis);
extern Uint8 SDL_JoystickGetHat_REAL(SDL_Joystick *joystick, int hat);
extern int SDL_JoystickGetBall_REAL(SDL_Joystick *joystick, int ball, int *dx, int *dy);
extern Uint8 SDL_JoystickGetButton_REAL(SDL_Joystick *joystick, int button);
extern void SDL_JoystickClose_REAL(SDL_Joystick *joystick);
extern int SDL_SetClipboardText_REAL(const char *text);
extern char *SDL_GetClipboardText_REAL();
extern SDL_bool SDL_HasClipboardText_REAL();
extern SDL_AudioSpec *SDL_LoadWAV_RW_REAL(SDL_RWops *src, int freesrc, SDL_AudioSpec *spec, Uint8 **audio_buf, Uint32 *audio_len);
extern void SDL_FreeWAV_REAL(Uint8 *audio_buf);
extern int SDL_OpenAudio_REAL(SDL_AudioSpec *desired, SDL_AudioSpec *obtained);
extern void SDL_PauseAudio_REAL(int pause_on);
extern SDL_AudioStatus SDL_GetAudioStatus_REAL();
extern void SDL_CloseAudio_REAL();
extern SDL_TimerID SDL_AddTimer_REAL(Uint32 interval, SDL_TimerCallback callback, void *param);
extern SDL_bool SDL_RemoveTimer_REAL(SDL_TimerID t);
extern int SDL_SetTimer_REAL(Uint32 interval, SDL_TimerCallback callback);
extern SDL_sem *SDL_CreateSemaphore_REAL(Uint32 initial_value);
extern void SDL_DestroySemaphore_REAL(SDL_sem *sem);
extern int SDL_SemWait_REAL(SDL_sem *sem);
extern int SDL_SemTryWait_REAL(SDL_sem *sem);
extern int SDL_SemWaitTimeout_REAL(SDL_sem *sem, Uint32 toval);
extern int SDL_SemPost_REAL(SDL_sem *sem);
extern Uint32 SDL_SemValue_REAL(SDL_sem *sem);
extern SDL_mutex *SDL_CreateMutex_REAL();
extern void SDL_DestroyMutex_REAL(SDL_mutex *mutex);
extern int SDL_LockMutex_REAL(SDL_mutex *mutex);
extern int SDL_UnlockMutex_REAL(SDL_mutex *mutex);
extern SDL_cond *SDL_CreateCond_REAL();
extern void SDL_DestroyCond_REAL(SDL_cond *cond);
extern int SDL_CondSignal_REAL(SDL_cond *cond);
extern int SDL_CondBroadcast_REAL(SDL_cond *cond);
extern int SDL_CondWait_REAL(SDL_cond *cond, SDL_mutex *mutex);
extern int SDL_CondWaitTimeout_REAL(SDL_cond *cond, SDL_mutex *mutex, Uint32 toval);
extern int SDL_GetNumAudioDevices_REAL(int iscapture);
extern const char *SDL_GetAudioDeviceName_REAL(int index, int iscapture);
extern SDL_AudioDeviceID SDL_OpenAudioDevice_REAL(const char *device, int iscapture, const SDL_AudioSpec *desired, SDL_AudioSpec *obtained, int allowed_changes);
extern void SDL_CloseAudioDevice_REAL(SDL_AudioDeviceID dev);
extern void SDL_PauseAudioDevice_REAL(SDL_AudioDeviceID dev, int pause_on);
extern Uint32 SDL_GetMouseState_REAL(int *x, int *y);
extern Uint32 SDL_GetRelativeMouseState_REAL(int *x, int *y);
extern void SDL_WarpMouseInWindow_REAL(SDL_Window *window, int x, int y);
extern int SDL_SetRelativeMouseMode_REAL(SDL_bool enabled);
extern SDL_bool SDL_GetRelativeMouseMode_REAL();
extern int SDL_ShowCursor_REAL(int toggle);
extern SDL_Cursor *SDL_GetCursor_REAL();
extern void SDL_FreeCursor_REAL(SDL_Cursor *cursor);
extern SDL_Cursor *SDL_CreateSystemCursor_REAL(SDL_SystemCursor id);
extern SDL_Cursor *SDL_CreateColorCursor_REAL(SDL_Surface *surface, int hot_x, int hot_y);
extern void SDL_SetCursor_REAL(SDL_Cursor *cursor);
extern int SDL_GetNumVideoDisplays_REAL();
extern const char *SDL_GetDisplayName_REAL(int displayIndex);
extern int SDL_GetDisplayBounds_REAL(int displayIndex, SDL_Rect *rect);
extern int SDL_GetDisplayDPI_REAL(int displayIndex, float *ddpi, float *hdpi, float *vdpi);
extern int SDL_SetWindowFullscreen_REAL(SDL_Window *window, Uint32 flags);
extern int SDL_SetRenderTarget_REAL(SDL_Renderer *renderer, SDL_Texture *texture);
extern int SDL_RenderCopy_REAL(SDL_Renderer *renderer, SDL_Texture *texture, const SDL_Rect *srcrect, const SDL_Rect *dstrect);
extern int SDL_RenderCopyEx_REAL(SDL_Renderer *renderer, SDL_Texture *texture, const SDL_Rect *srcrect, const SDL_Rect *dstrect, const double angle, const SDL_Point *center, const SDL_RendererFlip flip);
extern int SDL_RenderDrawPoint_REAL(SDL_Renderer *renderer, int x, int y);
extern int SDL_RenderDrawPoints_REAL(SDL_Renderer *renderer, const SDL_Point *points, int count);
extern int SDL_RenderDrawLines_REAL(SDL_Renderer *renderer, const SDL_Point *points, int count);
extern SDL_Texture *SDL_CreateTexture_REAL(SDL_Renderer *renderer, Uint32 format, int access, int w, int h);
extern void SDL_DestroyTexture_REAL(SDL_Texture *texture);
extern int SDL_SetRenderDrawBlendMode_REAL(SDL_Renderer *renderer, SDL_BlendMode blendMode);
extern int SDL_RenderReadPixels_REAL(SDL_Renderer *renderer, const SDL_Rect *rect, Uint32 format, void *pixels, int pitch);
extern void SDL_LogSetAllPriority_REAL(SDL_LogPriority priority);
extern SDL_LogPriority SDL_LogGetPriority_REAL(int category);
extern SDL_RWops *SDL_RWFromConstMem_REAL(const void *mem, int size);
extern void SDL_ClearError_REAL();
extern const char *SDL_GetError_REAL();
extern int SDL_SetWindowOpacity_REAL(SDL_Window *window, float opacity);
extern int SDL_GetWindowOpacity_REAL(SDL_Window *window, float *opacity);
extern int SDL_GetNumVideoDrivers_REAL();
extern const char *SDL_GetVideoDriver_REAL(int index);
extern int SDL_GetNumRenderDrivers_REAL();
extern int SDL_GetRenderDriverInfo_REAL(int index, SDL_RendererInfo *info);
extern int SDL_SetTextureColorMod_REAL(SDL_Texture *texture, Uint8 r, Uint8 g, Uint8 b);
extern int SDL_SetTextureAlphaMod_REAL(SDL_Texture *texture, Uint8 alpha);
extern int SDL_SetTextureBlendMode_REAL(SDL_Texture *texture, SDL_BlendMode blendMode);
extern int SDL_GetRenderDrawBlendMode_REAL(SDL_Renderer *renderer, SDL_BlendMode *blendMode);
extern void SDL_SetWindowBordered_REAL(SDL_Window * window, SDL_bool bordered);
extern SDL_GLContext SDL_GL_CreateContext_REAL(SDL_Window *window);
extern int SDL_GL_MakeCurrent_REAL(SDL_Window *window, SDL_GLContext context);
extern void SDL_GL_DeleteContext_REAL(SDL_GLContext context);
extern int SDL_GL_SetSwapInterval_REAL(int interval);
extern int SDL_GL_GetSwapInterval_REAL();
extern void SDL_GL_SwapWindow_REAL(SDL_Window *window);
