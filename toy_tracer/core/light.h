#pragma once
#include "core/ray.h"
#include "core/spectrum.h"
#include "core/interaction.h"
#include "core/geometry.h"
class Light {
public:
      // assume there's no occlusion
      virtual Spectrum Li(Vector3f& w) const = 0;
      virtual Vector3f Sample_wi(Point2f& sample, Interaction& i, Float* pdf) const = 0;
};