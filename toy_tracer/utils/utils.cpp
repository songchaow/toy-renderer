#include "utils.h"

bool IsAscending(const Float* ar, int size)
{
    for(int i=0;i<=size-2;i++)
        if((ar[i]-ar[i+1]>0)) // there is 121 or 212
            return false;
    return true;
}

void Array1DScale(Float scale, const Float* ar, Float* res, int size)
{
    for(int i=0;i<size;i++)
        res[i] = ar[i] * scale;
}