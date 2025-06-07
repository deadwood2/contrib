#ifndef FASTTRIG_H_FILE
#define FASTTRIG_H_FILE

#ifndef MYTYPES_H_FILE
#include "mytypes.h"
#endif /* !MYTYPES_H_FILE */

#define MaxTrigEntries 512
#ifndef PI
#define PI 3.1415927
#endif /* PI */
extern float *sintab;
extern float *costab;

#define fsin(sdeg) sintab[sdeg]
#define fcos(sdeg) costab[sdeg]

#if !defined(__AROS__)
bool InitFastTrig(/* float, int */);
#else
bool InitFastTrig(int,int,int);
#endif

#endif /* !FASTTRIG_H_FILE */
