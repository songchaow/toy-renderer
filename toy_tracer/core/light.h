#pragma once
#include "core/ray.h"
#include "core/spectrum.h"
#include "core/interaction.h"
#include "core/geometry.h"
#include "core/primitive.h"

class Light {
      Primitive* _primitive = nullptr; // used in RTR
public:
      Light() = default;
      Light(Primitive* p) : _primitive(p) {}
      // assume there's no occlusion
      virtual Spectrum Li(Vector3f& w) const = 0;
      // return false if the random chosen sample is invalid(in the back side of the light)
      virtual bool Sample_wi(Point2f& sample, Interaction& i, Vector3f& wi, Float* pwi) const = 0;
      virtual bool isSingular() const = 0;
      Primitive* primitive() { return _primitive; }
      // TODO: remember to free shape
};