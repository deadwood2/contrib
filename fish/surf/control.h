typedef enum { FITBEZIER, DRAWPOLY, NOTACTIVE } CURMODE;

void SetFitBez();
void SetPolyDraw();

extern CURMODE CurMode;
extern void EditControl0(int,int);
extern void EditControl1(int,int);
