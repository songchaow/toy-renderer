#pragma once
#include "core/light.h"
#include "core/texture.h"
#include <memory>

// using a equirectangular map
class Skybox : public Light {
      Spectrum scale = 1.f;
      Float _radius = 1.f;
      std::unique_ptr<RGBSpectrumTexture2D> _texture;
public:
      Skybox(RGBSpectrumTexture2D* texture) : _texture(texture) {}
      Skybox(RGBSpectrumTexture2D* texture, Float radius) : _texture(texture), _radius(radius) {}
      virtual Spectrum Li(Vector3f& w) const override;
      virtual bool Sample_wi(Point2f& sample, Interaction& i, Vector3f& wi, Float* pdf) const override;
      virtual bool isSingular() const override { return false; }
};