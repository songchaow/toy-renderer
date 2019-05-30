#pragma once
#include <random>
#include <core/common.h>
#include "core/geometry.h"
class Sampler {
      std::random_device r;
      std::mt19937 raw_random;
      std::uniform_real_distribution<Float> uniform_dist;
public:
      Sampler() : raw_random(r()), uniform_dist(0.0, 1.0) {}
      bool SampleBool(Float p_true = 0.5);
      Point2f Sample2D() {
            return { uniform_dist(raw_random),uniform_dist(raw_random) };
      }
};