#pragma once
#include "core/scene.h"
#include "core/spectrum.h"
class PathTracer {
      Scene* scene;
      int maxSegment;
public:
      Spectrum Li(Ray& wo);
};