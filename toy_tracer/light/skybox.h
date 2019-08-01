#pragma once
#include "core/light.h"
#include "core/texture.h"
#include <memory>

class Skybox : public Light {
      Spectrum scale = 1.f;
      std::unique_ptr<RGBSpectrumTexture> _texture;
public:
      Skybox(RGBSpectrumTexture* texture) : _texture(texture) {}
      virtual Spectrum Li(Vector3f& w) const override;
      virtual bool Sample_wi(Point2f& sample, Interaction& i, Vector3f& wi, Float* pdf) const override;
      virtual bool isSingular() const override { return false; }
};