#include "core/sampler.h"

bool Sampler::SampleBool(Float p_true) {
      std::bernoulli_distribution d(p_true);
      return d(raw_random);
}