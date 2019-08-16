#pragma once
#include "core/light.h"
#include "core/shape.h"
class UniformAreaLight : public Light, public Shapeable {
      Spectrum Le;
public:
      virtual Spectrum Li(Vector3f &w) const override;
      // fills i.wo
      bool Sample_wi(Point2f& sample, Interaction& i, Vector3f& wi, Float* pwi) const override;
      virtual bool isSingular() const override { return false; }
};

