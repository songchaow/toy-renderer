#pragma once
#include "core/geometry.h"
inline Vector3f SampleSphere(const Vector2f& sample) {
      Float z = 1 - 2 * sample[0];
      Float r = 2 * std::sqrt(sample[0] * (1 - sample[0]));
      Float x = r * std::cos(2 * Pi*sample[1]);
      Float y = r * std::sin(2 * Pi*sample[1]);
      return { x,y,z };
}