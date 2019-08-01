#include "core/sampler.h"

Sampler generalSampler;

bool Sampler::SampleBool(Float p_true) {
      std::bernoulli_distribution d(p_true);
      return d(raw_random);
}

int StepSampler::sample(Float random)
{
      for (int i = 0; i < prefix_sums.size(); i++) {
            if (prefix_sums[i] <= random && prefix_sums[i + 1] > random)
                  return i;
      }
      return 0;
}
