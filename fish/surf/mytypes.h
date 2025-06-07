#ifndef MYTYPES_H_FILE
#define MYTYPES_H_FILE

#include <stdlib.h>
//extern char *calloc(), *malloc();

#if !defined(__AROS__)
#define true 1
#define false 0
typedef char bool;
#else
#include <stdbool.h>
#endif

#define null 0L

#ifndef MANX
#include "exec/types.h"
#include "libraries/mathffp.h"
#define ceil	SPCeil
#define floor	SPFloor
#define fabs	SPAbs
#endif


#endif /* MYTYPES_H_FILE */
