#pragma once
#include "core/light.h"
#include "core/texture.h"
#include "shape/triangle.h"
#include <memory>

// uses a equirectangular map
class EquiRectSkybox : public Light {
      Spectrum scale = 1.f;
      Float _radius = 1.f;
      std::unique_ptr<RGBSpectrumTexture2D> _texture;
public:
      EquiRectSkybox(RGBSpectrumTexture2D* texture) : _texture(texture) {}
      EquiRectSkybox(RGBSpectrumTexture2D* texture, Float radius) : _texture(texture), _radius(radius) {}
      virtual Spectrum Li(Vector3f& w) const override;
      virtual bool Sample_wi(Point2f& sample, Interaction& i, Vector3f& wi, Float* pdf) const override;
      virtual bool isSingular() const override { return false; }
};

// uses a cube map for rt rendering
class Skybox {
      
      static TriangleMesh cube;
public:
      Skybox();
};