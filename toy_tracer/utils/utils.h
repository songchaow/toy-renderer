#include "../common.h"

bool IsAscending(const Float* ar, int size);

void Array1DScale(Float scale, const Float* ar, Float* res, int size);

inline Float Lerp(Float t, Float v1, Float v2) { return (1 - t) * v1 + t * v2; }