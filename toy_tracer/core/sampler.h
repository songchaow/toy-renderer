#pragma once
#include <random>
#include <core/common.h>
#include "core/geometry.h"
#include <vector>
class Sampler {
      std::random_device r;
      std::mt19937 raw_random;
      std::uniform_real_distribution<Float> uniform_dist;
      std::uniform_real_distribution<Float> uniform_dist2;
public:
      Sampler() : raw_random(r()), uniform_dist(0.0, 1.0) {}
      bool SampleBool(Float p_true = 0.5);
      Float Sample1D() { return uniform_dist(raw_random); }
      Point2f Sample2D() {
            return Point2f(uniform_dist(raw_random), uniform_dist2(raw_random));
      }
      Point2f SamplePixel(Float startx, Float starty) {
            return { startx + uniform_dist(raw_random),starty + uniform_dist(raw_random) };
      }
      void SetUniformDist(Float xstart, Float xend, Float ystart, Float yend) { 
            uniform_dist = std::uniform_real_distribution<Float>(xstart, xend);
            uniform_dist2 = std::uniform_real_distribution<Float>(ystart, yend);
      }
      void ResetUniformDist() {
            uniform_dist = uniform_dist2 = std::uniform_real_distribution<Float>();
      }
};

extern Sampler generalSampler;

class StepSampler {
      std::vector<Float> prefix_sums;
public:
      StepSampler() = default;
      StepSampler(const std::vector<Float> &probabilities) {
            prefix_sums.push_back(0.f);
            Float sum = 0.f;
            for (Float f : probabilities) {
                  sum += f;
                  prefix_sums.push_back(sum);
            }
      }
      int sample(Float random);
};