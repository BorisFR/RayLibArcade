#ifndef THE_DIRTY_H
#define THE_DIRTY_H

#include "TheMemory.h"

#ifdef USE_DIRTY

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct MyDirtyNode
{
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
    struct MyDirtyNode *nextNode;
} OneDirtyNode;

extern OneDirtyNode *DirtyNodeList;
extern OneDirtyNode *DirtyCreateNode(uint16_t x, uint16_t y);
extern void DirtyAddNode(uint16_t x, uint16_t y);
extern void DirtyClearNode();
extern void DirtyPrint();
extern void DirtySort();
extern void DirtyMerge();
extern void DirtyOptimize();
//extern void DirtyRefreshTile(uint16_t *screenGame, uint16_t x, uint16_t y);
#endif

//extern void DirtyAdd(THE_COLOR *screenGame, THE_COLOR color, uint8_t state, uint16_t x, uint16_t y);
extern void DirtyAdd(uint16_t *screenGame, uint16_t color, uint8_t state, uint16_t x, uint16_t y);

#ifdef USE_DIRTY
#ifdef __cplusplus
}
#endif
#endif


#endif