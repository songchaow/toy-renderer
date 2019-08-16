#pragma once
#include "core/common.h"

bool IsAscending(const Float* ar, int size);

void Array1DScale(Float scale, const Float* ar, Float* res, int size);

inline Float Lerp(Float t, Float v1, Float v2) { return (1 - t) * v1 + t * v2; }

template <typename T, typename U, typename V>
inline T Clamp(T val, U low, V high) {
    if (val < low)
        return low;
    else if (val > high)
        return high;
    else
        return val;
}

inline bool Quadratic(Float a, Float b, Float c, Float *t0, Float *t1) {
      // Find quadratic discriminant
      double discrim = (double)b * (double)b - 4 * (double)a * (double)c;
      if (discrim < 0) return false;
      double rootDiscrim = std::sqrt(discrim);

      // Compute quadratic _t_ values
      double q;
      if (b < 0)
            q = -.5 * (b - rootDiscrim);
      else
            q = -.5 * (b + rootDiscrim);
      *t0 = (Float)q / a;
      *t1 = c / (Float)q;
      if (*t0 > *t1) std::swap(*t0, *t1);
      return true;
}

#define GEN_VERBOSE_STRING_MAP(str) {str, #str}