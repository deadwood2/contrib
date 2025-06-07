#ifndef READILBM_H_FILE

#define READILBM_H_FILE

#ifndef MYTYPES_H_FILE
#include "mytypes.h"
#endif /* MYTYPES_H_FILE */

#define DefRepV 1
#define DefRepH 1

/*
 * how many times the image should be replicated on screen
 */
extern short MapRepV, MapRepH;

/*
 * Size of image read in
 */
extern int MapImageV, MapImageH;
/*
 * ReadPixel returns an intensity between 0-255
 */
#define DefXYFlip false
extern void CloseImgPix();
 #if !defined(__AROS__)
extern short GetImgPix(/* short vert, hori */);
bool OpenImgPix( /* int sizex, sizey; short maxshade */);
void SetImgPix( /* int x, y; short val */ );
short GetImgPix( /* int x, y */);
extern void FlipImgPix( /* bool flip */);
extern void SetGreyModel( /* int */);
#else
extern short GetImgPix(int,int);
bool OpenImgPix(int, int, short);
void SetImgPix(int, int, int);
short GetImgPix(int, int);
extern void FlipImgPix(bool);
extern void SetGreyModel(int);
#endif
extern void PrepImgPix(/* void */);

#endif /* READILBM_H_FILE */

