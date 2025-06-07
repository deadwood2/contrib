#include "poly.h"

#define SingleTinyVal 1.0e-12
#define SingleLargeVal 1.0e12

#define MP_XMIN 0
#define MP_XMAX 1
#define MP_YMIN 0
#define MP_YMAX 2

bool InitMapping( /* void */);

#if !defined(__AROS__)
void CalcMapConsts();
void MapXYRatio( /* float px, py; float *outx, outy; */);
void DrawRhomMap( /* MapRhomboid  *mpr; */ );
#else
void CalcMapConsts(register ScrnPair *);
void MapXYRatio(float, float, float *, float *, short);
void DrawRhomMap(MapRhomboid *);
#endif
