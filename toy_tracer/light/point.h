#pragma once
#include "core/light.h"

class PointLight : public Light {
      Spectrum irradiance = 1.f;
      Point3f _posWorld;
      bool _spot = false;
      bool _directional = false;
      Float _angle;
      Vector3f _direction;
public:
      // without dividing r squared
      PointLight() = default;
      PointLight(const Spectrum& i, const Point3f p) : irradiance(i), _posWorld(p), _spot(false), _directional(false) {}
      PointLight(const Spectrum& i, const Point3f p, Float angle, const Vector3f& dir) : irradiance(i), _posWorld(p),
            _spot(true), _angle(angle), _directional(false), _direction(dir) {}
      PointLight(const Spectrum& i, const Vector3f& dir) : irradiance(i), _spot(false), _directional(true), _direction(dir) {}
      virtual Spectrum Li(Vector3f &w) const override { return irradiance; }
      Spectrum radiance() const { return irradiance; }
      Spectrum& rradiance() { return irradiance; }
      // fills i.wo
      bool Sample_wi(Point2f& sample, Interaction& i, Vector3f& wi, Float* pwi) const override;
      virtual bool isSingular() const override { return false; }
      Point3f pos() const { return _posWorld; }
      Point3f& rpos() { return _posWorld; }
      bool isSpotLight() const { return _spot; }
      bool isDirectionalLight() const { return _directional;}
      Vector3f& direction() { return _direction; }
      const Vector3f& direction() const { return _direction; }
      Float& HalfAngle() { return _angle; }
      const Transform& obj2world() const { return Translate(_posWorld.x, _posWorld.y, _posWorld.z); }
      Transform world2obj() const { return Translate(-_posWorld.x, -_posWorld.y, -_posWorld.z); }
};

class DirectionalLight {

};