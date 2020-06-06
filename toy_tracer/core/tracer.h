#pragma once
#include "core/scene.h"
#include "core/sampler.h"
#include "tracer/spectrum.h"
class PathTracer {
      Sampler sampler;
      Scene* scene;
      int maxSegment = 10;
public:
      PathTracer(Scene* scene) :scene(scene) {}
      PathTracer() = default;
      Spectrum Li(Ray& wo);
};