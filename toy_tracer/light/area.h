#pragma once
#include "core/light.h"
#include "core/shape.h"
class UniformAreaLight : public Light, public Shapeable {
      Spectrum emit;
public:
      virtual Spectrum Li(Vector3f &w) const override;
      virtual Vector3f Sample_wi(Point2f& sample, Interaction& i, Float* pArea) const override;
};

