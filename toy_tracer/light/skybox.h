#pragma once
#include "core/light.h"

class Skybox : public Light {
      Spectrum reflection = 1.f;
public:
      virtual Spectrum Li(Ray& r) const override;
      virtual Vector3f Sample_wi(Vector2f& sample, Interaction& i, Float* pdf) override;

};