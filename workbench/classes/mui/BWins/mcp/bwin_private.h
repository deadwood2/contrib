#ifndef BWIN_PRIVATE_H
#define BWIN_PRIVATE_H

#ifndef EXEC_LIBRARIES_H
#include <exec/libraries.h>
#endif

#include <intuition/classusr.h>

struct data
{
    Object *sg;

    Object *menu;

    Object *winShinePen;
    Object *winShadowPen;
    Object *over;

    Object *dragBarShinePen;
    Object *dragBarShadowPen;
    Object *dragBarBack;
    Object *useDragBarBack;
    Object *dragBarType;
    Object *nadragBarShinePen;
    Object *nadragBarShadowPen;
    Object *nadragBarBack;
    Object *nauseDragBarBack;
    Object *nadragBarType;

    Object *sizeShinePen;
    Object *sizeShadowPen;
    Object *sizeForegroundPen;
    Object *useSizeForeground;
    Object *sizeType;
    Object *nasizeShinePen;
    Object *nasizeShadowPen;
    Object *nasizeForegroundPen;
    Object *nauseSizeForeground;
    Object *nasizeType;

    Object *leftSp;
    Object *topSp;
    Object *rightSp;
    Object *bottomSp;
};

struct BWinBase
{
    struct Library          libNode;
    ULONG                   segList;
    struct MUI_CustomClass  *bw_mcp;
};

#endif
