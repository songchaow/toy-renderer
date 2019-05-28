#pragma once
#include "core/ray.h"
#include "core/spectrum.h"
#include "core/interaction.h"
#include "core/geometry.h"
class Light {
public:
      virtual Spectrum Li(Ray& r) const = 0;
      virtual Vector3f Sample_wi(Vector2f& sample, Interaction& i, Float* pdf) = 0;
};