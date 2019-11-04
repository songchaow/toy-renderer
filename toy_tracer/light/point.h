#pragma once
#include "core/light.h"

class PointLight : public Light {
      Spectrum irradiance = 1.f;
      Point3f _posWorld;
public:
      // without dividing r squared
      PointLight() = default;
      PointLight(const Spectrum& i, const Point3f p) : irradiance(i), _posWorld(p) {}
      virtual Spectrum Li(Vector3f &w) const override { return irradiance; }
      Spectrum radiance() const { return irradiance; }
      // fills i.wo
      bool Sample_wi(Point2f& sample, Interaction& i, Vector3f& wi, Float* pwi) const override;
      virtual bool isSingular() const override { return false; }
      Point3f pos() const { return _posWorld; }
      Point3f& rpos() { return _posWorld; }
      const Transform& obj2world() const { return Translate(_posWorld.x, _posWorld.y, _posWorld.z); }
      Transform world2obj() const { return Translate(-_posWorld.x, -_posWorld.y, -_posWorld.z); }
};