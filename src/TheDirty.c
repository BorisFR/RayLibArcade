#include "TheDirty.h"

OneDirtyNode *DirtyNodeList = NULL;

// *******************************************************************

#define DIRTY_MIN_WIDTH 1
#define DIRTY_MIN_HEIGHT 1

OneDirtyNode *DirtyCreateNode(uint16_t x, uint16_t y)
{
    OneDirtyNode *newNode = (OneDirtyNode *)malloc(sizeof(OneDirtyNode));
    newNode->x = x;
    newNode->y = y;
    newNode->width = DIRTY_MIN_WIDTH;
    newNode->height = DIRTY_MIN_HEIGHT;
    newNode->nextNode = NULL;
    return newNode;
}

// *******************************************************************

void DirtyAddNode(uint16_t x, uint16_t y)
{
    OneDirtyNode *temp = DirtyCreateNode(x, y);
    temp->nextNode = *(&DirtyNodeList);
    DirtyNodeList = temp;
}

// *******************************************************************

void DirtyClearNode()
{
    OneDirtyNode *current = DirtyNodeList;
    OneDirtyNode *previous;
    while (current)
    {
        previous = current;
        current = previous->nextNode;
        free(previous);
        previous = NULL;
    }
    current = NULL;
    DirtyNodeList = NULL;
}

void DirtyPrint()
{
    printf("*");
    OneDirtyNode *temp = DirtyNodeList;
    while (temp)
    {
        printf("%d/%d %d/%d\n", temp->x, temp->y, temp->width, temp->height);
        temp = temp->nextNode;
    }
}

void DirtySort()
{
    while (true)
    {
        OneDirtyNode *current = DirtyNodeList;
        OneDirtyNode *next;
        bool change = false;
        while (current)
        {
            next = current->nextNode;
            if (!next)
                break;
            if (next->x < current->x)
            {
                OneDirtyNode *temp = current;
                current->x = next->x;
                current->y = next->y;
                current->width = next->width;
                current->height = next->height;
                next->x = temp->x;
                next->y = temp->y;
                next->width = temp->width;
                next->height = temp->height;
                change = true;
            }
            else
            {
                if (next->x == current->x && next->y < current->y)
                {
                    OneDirtyNode *temp = current;
                    current->x = next->x;
                    current->y = next->y;
                    current->width = next->width;
                    current->height = next->height;
                    next->x = temp->x;
                    next->y = temp->y;
                    next->width = temp->width;
                    next->height = temp->height;
                    change = true;
                }
            }
            current = current->nextNode;
        } // while current
        if (!change)
            break;
    } // while true
}

void DirtyMerge()
{
    while (true)
    {
        OneDirtyNode *current = DirtyNodeList;
        OneDirtyNode *next;
        bool change = false;
        while (current)
        {
            next = current->nextNode;
            if (!next)
                break;

            if ((next->x >= current->x) && (next->x <= current->x + current->width) && (next->y >= current->y) & (next->y <= current->y + current->height))
            {
                current->width = next->x + next->width;
                current->height = next->y + next->height;
                current->nextNode = next->nextNode;
                free(next);
                next = NULL;
                change = true;
                if (current->x + current->width >= screenGameWidth)
                {
                    current->width = screenGameWidth - current->x;
                }
                if (current->y + current->height >= screenGameHeight)
                {
                    current->height = screenGameHeight - current->y;
                }
            }
            current = current->nextNode;
        }
        if (!change)
            break;
    } // while true
}

void DirtyOptimize()
{
    DirtySort();
    DirtyMerge();
    while (true)
    {
        OneDirtyNode *temp = DirtyNodeList;
        OneDirtyNode *next;
        bool change = false;
        while (temp)
        {
            next = temp->nextNode;
            if (!next)
                break;
            if ((next->x >= temp->x) && (next->x + next->width) <= (temp->x + temp->width) && ((next->y >= temp->y)) && ((next->y + next->height) <= (temp->y + temp->height)))
            {
                temp->nextNode = next->nextNode;
                free(next);
                next = NULL;
                change = true;
            }
            temp = temp->nextNode;
            if (!temp)
                break;
        } // while temp
        if (!change)
            break;
    }
}

// *******************************************************************

//void DirtyAdd(THE_COLOR *screenGame, THE_COLOR color, uint8_t state, uint16_t x, uint16_t y)
void DirtyAdd(uint16_t *screenGame, uint16_t color, uint8_t state, uint16_t x, uint16_t y)
{
    CHECK_IF_DIRTY_XY(x, y)
    uint32_t index = x + y * screenGameWidth;
    //if(screenGameOld[index] == color && screenGameDirty[index] == state) return;
    screenGame[index] = color;
    screenGameDirty[index] = state;
#ifdef USE_DIRTY
    OneDirtyNode *temp = DirtyNodeList;
    while (temp)
    {
        // same
        if ((x >= temp->x) && (x <= (temp->x + temp->width)) && (y >= temp->y) && (y <= (temp->y + temp->height)))
        {
            return;
        }
        // near bottom
        if ((temp->y + 1 == y) || (temp->y + temp->height + 1 == y))
        {
            if (temp->x == x)
            {
                temp->height += DIRTY_MIN_HEIGHT;
                if (temp->y + temp->height >= screenGameHeight)
                {
                    temp->height = screenGameHeight - temp->y;
                }
                return;
            }
        }
        // near up
        if ((temp->y - 1 == y) || (temp->y - temp->height - 1 == y))
        {
            if (temp->x == x)
            {
                temp->y--;
                temp->height += 1;
                if (temp->y + temp->height >= screenGameHeight)
                {
                    temp->height = screenGameHeight - temp->y;
                }
                return;
            }
        }
        // near right
        if ((temp->x + 1 == x) || (temp->x + temp->width + 1 == x))
        {
            if (temp->y == y)
            {
                temp->width += DIRTY_MIN_WIDTH;
                if (temp->x + temp->width >= screenGameWidth)
                {
                    temp->width = screenGameWidth - temp->x;
                }
                return;
            }
            // near bottom
            if ((temp->y + 1 == y) || (temp->y + temp->height + 1 == y))
            {
                temp->width += DIRTY_MIN_WIDTH;
                if (temp->x + temp->width >= screenGameWidth)
                {
                    temp->width = screenGameWidth - temp->x;
                }
                temp->height += DIRTY_MIN_HEIGHT;
                if (temp->y + temp->height >= screenGameHeight)
                {
                    temp->height = screenGameHeight - temp->y;
                }
                return;
            }
            // near up
            if ((temp->y - 1 == y) || (temp->y - temp->height - 1 == y))
            {
                temp->width += DIRTY_MIN_WIDTH;
                if (temp->x + temp->width >= screenGameWidth)
                {
                    temp->width = screenGameWidth - temp->x;
                }
                temp->y--;
                temp->height += 1;
                if (temp->y + temp->height >= screenGameHeight)
                {
                    temp->height = screenGameHeight - temp->y;
                }
                return;
            }
        }
        // near left
        if ((temp->x - 1 == x) || (temp->x - temp->width - 1 == x))
        {
            if (temp->y == y)
            {
                temp->x--;
                temp->width += 1;
                if (temp->x + temp->width >= screenGameWidth)
                {
                    temp->width = screenGameWidth - temp->x;
                }
                return;
            }
            // near bottom
            if ((temp->y + 1 == y) || (temp->y + temp->height + 1 == y))
            {
                temp->x--;
                temp->width += 1;
                if (temp->x + temp->width >= screenGameWidth)
                {
                    temp->width = screenGameWidth - temp->x;
                }
                temp->height += DIRTY_MIN_HEIGHT;
                if (temp->y + temp->height >= screenGameHeight)
                {
                    temp->height = screenGameHeight - temp->y;
                }
                return;
            }
            // near up
            if ((temp->y - 1 == y) || (temp->y - temp->height - 1 == y))
            {
                temp->x--;
                temp->width += DIRTY_MIN_WIDTH;
                if (temp->x + temp->width >= screenGameWidth)
                {
                    temp->width = screenGameWidth - temp->x;
                }
                temp->y--;
                temp->height += 1;
                if (temp->y + temp->height >= screenGameHeight)
                {
                    temp->height = screenGameHeight - temp->y;
                }
                return;
            }
        }
        temp = temp->nextNode;
    }
    DirtyAddNode(x, y);
#else
#endif
}