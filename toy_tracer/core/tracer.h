#pragma once
#include "core/scene.h"
#include "core/spectrum.h"
class PathTracer {
      Scene* scene;
      int maxSegment = 10;
public:
      Spectrum Li(Ray& wo);
};