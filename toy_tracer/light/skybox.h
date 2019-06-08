#pragma once
#include "core/light.h"

class Skybox : public Light {
      Spectrum reflection = 1.f;
public:
      virtual Spectrum Li(Vector3f& w) const override;
      virtual Vector3f Sample_wi(Point2f& sample, Interaction& i, Float* pdf) const override;

};