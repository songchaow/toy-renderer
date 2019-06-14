#pragma once
#include "core/scene.h"
#include "core/sampler.h"
#include "core/spectrum.h"
class PathTracer {
      Sampler sampler;
      Scene* scene;
      int maxSegment = 10;
public:
      PathTracer(Scene* scene) :scene(scene) {}
      Spectrum Li(Ray& wo);
};